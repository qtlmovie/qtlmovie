//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//
// Qtl, Qt utility library.
// Define the class QtlDataPull.
//
//----------------------------------------------------------------------------

#include "QtlDataPull.h"
#include "QtlFile.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlDataPull::QtlDataPull(int minBufferSize, QtlLogger* log, QObject* parent) :
    QObject(parent),
    _nullLog(),
    _log(log == 0 ? &_nullLog : log),
    _autoDelete(false),
    _minBufferSize(minBufferSize),
    _totalIn(0),
    _totalOut(0),
    _state(Inactive),
    _device(0),
    _error()
{
}

QtlDataPull::~QtlDataPull()
{
    if (_state != Inactive) {
        if (_error.isEmpty()) {
            _error = tr("%1 destroyed during data transfer").arg(objectName());
        }
        _state = Inactive;
        emit completed(false, _error);
    }
}


//----------------------------------------------------------------------------
// Start to transfer data into the specified device.
//----------------------------------------------------------------------------

bool QtlDataPull::start(QIODevice* device)
{
    // Filter invalid state.
    if (_state != Inactive || device == 0) {
        return false;
    }

    // Initialize state.
    _device = device;
    _totalIn = _totalOut = 0;
    _error.clear();

    // Get notified of device events.
    connect(_device, &QIODevice::bytesWritten, this, &QtlDataPull::bytesWritten);
    connect(_device, &QObject::destroyed, this, &QtlDataPull::deviceDestroyed);

    // If the device is a process, abort the transfer if the process unexpectedly terminates.
    QProcess* process = qobject_cast<QProcess*>(device);
    if (process != 0) {
        // Note: In Qt 5, the signal QProcess::finished is overloaded and we must explicitly
        // select the one to use in order to resolve the template instantiation.
        connect(process, static_cast<void(QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished), this, &QtlDataPull::stop);
    }

    // Let the subclass initialize the transfer.
    if (initializeTransfer(_device)) {
        _state = Running;
        _log->debug(tr("Data transfer started"));
        emit started();
        processNewStateLater();
        return true;
    }
    else {
        _log->debug(tr("Data failed to start"));
        cleanupTransfer(_device, false);
        autoDelete();
        return false;
    }
}


//----------------------------------------------------------------------------
// Force a premature stop of the data transfer.
//----------------------------------------------------------------------------

void QtlDataPull::stop()
{
    if (_state == Running) {
        _state = Aborting;
        _error = tr("Data transfer aborted");
        processNewStateLater();
    }
}


//----------------------------------------------------------------------------
// Write data to the device.
//----------------------------------------------------------------------------

bool QtlDataPull::write(const void* data, int dataSize)
{
    if (_state != Running) {
        return false;
    }

    if (!QtlFile::writeBinary(*_device, data, dataSize)) {
        _state = Aborting;
        _error = tr("Error writing on %1").arg(_device->objectName());
        processNewStateLater();
        return false;
    }

    _totalIn += dataSize;

    if (bufferUnderflow()) {
        processNewStateLater();
    }
    return true;
}


//----------------------------------------------------------------------------
// Properly terminate the transfer.
//----------------------------------------------------------------------------

void QtlDataPull::close()
{
    if (_state == Running) {
        _state = Closing;
        processNewStateLater();
    }
}


//----------------------------------------------------------------------------
// Invoked when the device has written data.
//----------------------------------------------------------------------------

void QtlDataPull::bytesWritten(qint64 bytes)
{
    if (_state == Running) {
        _totalOut += bytes;
        if (bufferUnderflow()) {
            processNewStateLater();
        }
    }
}


//----------------------------------------------------------------------------
// Invoked when the device object is destroyed.
//----------------------------------------------------------------------------

void QtlDataPull::deviceDestroyed(QObject* object)
{
    // Make sure the destroyed object is the current device.
    if (object != 0 && object == _device) {
        // Make sure we no longer reference the device.
        _device = 0;
        if (_state == Running) {
            _state = Aborting;
            _error = tr("Data transfer destination destroyed");
            processNewStateLater();
        }
    }
}


//----------------------------------------------------------------------------
// Invoked from the event loop to process a new state.
//----------------------------------------------------------------------------

void QtlDataPull::processNewState()
{
    switch (_state) {
        case Inactive: {
            // Nothing to do.
            break;
        }
        case Aborting: {
            // Transfer aborted.
            cleanupTransfer(_device, false);
            _state = Inactive;
            _log->debug(tr("Data transfer aborted, read %1 bytes, written %2 bytes, error: %3").arg(_totalIn).arg(_totalOut).arg(_error));
            emit completed(false, _error);
            autoDelete();
            break;
        }
        case Closing: {
            // Proper close was requested.
            cleanupTransfer(_device, true);
            _state = Inactive;
            _log->debug(tr("Data transfer completed, read %1 bytes, written %2 bytes").arg(_totalIn).arg(_totalOut));
            emit completed(true, _error);
            autoDelete();
            break;
        }
        case Running: {
            // Ask for more data to the subclass.
            const bool ok = needTransfer();
            // If no longer running after this, we closed or aborted.
            if (_state == Running) {
                if (!ok) {
                    // The subclass reported an error and we are still running.
                    // This is a subclass error, not a write error, not a close, etc.
                    if (_error.isEmpty()) {
                        _error = tr("Error reading from %1").arg(objectName());
                    }
                    _state = Aborting;
                    processNewStateLater();
                }
                else if (bufferUnderflow()) {
                    // Still buffer underflow, retry after all event processing.
                    processNewStateLater();
                }
            }
            break;
        }
    }
}
