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
#include "QtlIncrement.h"
#include "QtlFile.h"
#include <QtNetwork>


//----------------------------------------------------------------------------
// Constructor and destructor.
//----------------------------------------------------------------------------

QtlDataPull::QtlDataPull(int minBufferSize, QtlLogger* log, QObject* parent) :
    QObject(parent),
    _nullLog(),
    _log(log == 0 ? &_nullLog : log),
    _autoDelete(false),
    _minBufferSize(minBufferSize),
    _startTime(),
    _closed(false),
    _totalIn(0),
    _devices(),
    _processingState(0)
{
}

QtlDataPull::~QtlDataPull()
{
    // Report abort of all devices, if any remain.
    if (!_devices.isEmpty()) {
        foreach (const Context& ctx, _devices) {
            emit deviceCompleted(ctx.device, false);
        }
        _devices.clear();
        emit completed(false);
    }
}


//----------------------------------------------------------------------------
// Constructor of a device context
//----------------------------------------------------------------------------

QtlDataPull::Context::Context(QIODevice* dev) :
    device(dev),
    running(true),
    totalOut(0)
{
}


//----------------------------------------------------------------------------
// Start to transfer data into the specified devices.
//----------------------------------------------------------------------------

bool QtlDataPull::start(QIODevice* device)
{
    QList<QIODevice*> deviceList;
    deviceList.append(device);
    return start(deviceList);
}

bool QtlDataPull::start(const QList<QIODevice*>& devices)
{
    // Filter invalid state.
    if (!_devices.isEmpty()) {
        // A transfer is already in progress. Don't delete this object!
        return false;
    }

    // Initialize state.
    _devices.clear();
    _startTime.start();
    _closed = false;
    _totalIn = 0;

    // Detect duplicates.
    QSet<QIODevice*> alreadySeen;

    // Loop on all specified devices.
    foreach (QIODevice* dev, devices) {

        // Drop duplicates and invalid pointers.
        if (dev == 0 || alreadySeen.contains(dev)) {
            continue;
        }
        alreadySeen.insert(dev);

        // Add a device context.
        _devices.append(Context(dev));

        // Get notified of device events.
        connect(dev, &QIODevice::bytesWritten, this, &QtlDataPull::bytesWritten);
        connect(dev, &QObject::destroyed, this, &QtlDataPull::deviceDestroyed);
        deviceSpecificSetup(dev);
    }

    // If no valid device was found, this is an error.
    if (_devices.isEmpty()) {
        // If auto-delete is on, delete later when back in event loop.
        if (_autoDelete) {
            deleteLater();
        }
        return false;
    }

    // Let the subclass initialize the transfer.
    if (initializeTransfer()) {
        _log->debug(tr("DataPull: Data transfer started, %1 devices").arg(_devices.size()));
        emit started();
        processNewStateLater();
        return true;
    }
    else {
        _log->debug(tr("DataPull: Data transfer failed to start"));
        _devices.clear();
        // If auto-delete is on, delete later when back in event loop.
        if (_autoDelete) {
            deleteLater();
        }
        return false;
    }
}


//----------------------------------------------------------------------------
// Force a premature stop of all data transfers.
//----------------------------------------------------------------------------

void QtlDataPull::stop()
{
    _log->debug(tr("DataPull: stop requested, %1 devices").arg(_devices.size()));

    // Mark all devices as aborting.
    if (!_devices.isEmpty()) {
        for (QList<Context>::Iterator ctx = _devices.begin(); ctx != _devices.end(); ++ctx) {
            ctx->running = false;
        }
        processNewStateLater();
    }
}


//----------------------------------------------------------------------------
// Stop the transfer on the specified device.
//----------------------------------------------------------------------------

void QtlDataPull::stopCaller()
{
    // Stop the caller device. Can be zero if directly called (not signalled)
    // or if the caller is not a QIODevice.
    stopDevice(qobject_cast<QIODevice*>(sender()));
}

void QtlDataPull::stopDevice(QIODevice* device)
{
    if (device != 0) {
        _log->debug(tr("DataPull: stop requested on one device"));

        // Search the context for the device.
        for (QList<Context>::Iterator ctx = _devices.begin(); ctx != _devices.end(); ++ctx) {
            if (ctx->device == device) {
                ctx->running = false;
                processNewStateLater();
                break;
            }
        }
    }
}


//----------------------------------------------------------------------------
// Write data to the devices.
//----------------------------------------------------------------------------

bool QtlDataPull::write(const void* data, int dataSize)
{
    // Filter invalid parameters.
    if (data == 0 || dataSize <= 0) {
        return dataSize == 0;
    }

    // Accumulate input data size.
    _totalIn += dataSize;

    // Do we succeed on at least one device?
    bool success = false;

    // Track aborted devices.
    bool aborted = false;

    // Write the data on all devices.
    for (QList<Context>::Iterator ctx = _devices.begin(); ctx != _devices.end(); ++ctx) {
        if (ctx->running) {
            // Write data on device and process all events before each write.
            if (QtlFile::writeBinary(*(ctx->device), data, dataSize, true)) {
                // At least one device succeeded.
                success = true;
            }
            else {
                // Abort this device.
                _log->line(tr("Error writing on %1").arg(ctx->device->objectName()));
                ctx->running = false;
                aborted = true;
            }
        }
    }

    // If one device aborted or still underflow, do it later in the event loop.
    if (aborted || needMoreData()) {
        processNewStateLater();
    }
    return success;
}


//----------------------------------------------------------------------------
// Properly terminate the transfer.
//----------------------------------------------------------------------------

void QtlDataPull::close()
{
    _closed = true;
    processNewStateLater();
}


//----------------------------------------------------------------------------
// Invoked when a device has written data.
//----------------------------------------------------------------------------

void QtlDataPull::bytesWritten(qint64 bytes)
{
    // Get the sender device.
    QIODevice* dev = qobject_cast<QIODevice*>(sender());
    if (dev != 0) {
        // Search the sender device.
        for (QList<Context>::Iterator ctx = _devices.begin(); ctx != _devices.end(); ++ctx) {
            if (ctx->device == dev) {
                // Accumulate total number of bytes on this device.
                ctx->totalOut += bytes;
                //@@@@@@@@@@@@@@@@@@@@@
                static quint64 counter = 0;//@@
                if (++counter % 100 == 0) _log->debug(tr("QtlDataPull::bytesWritten(), called %1 times, buffered data: %2 bytes").arg(counter).arg(_totalIn - ctx->totalOut));//@@@@
                //@@@@@@@@@@@@@@@@@@@@@
                // Detect underflow.
                if (_totalIn - ctx->totalOut <= _minBufferSize) {
                    processNewStateLater();
                }
                break;
            }
        }
    }
}


//----------------------------------------------------------------------------
// Invoked when a device object is destroyed.
//----------------------------------------------------------------------------

void QtlDataPull::deviceDestroyed(QObject* object)
{
    // Search the sender device.
    for (QList<Context>::Iterator ctx = _devices.begin(); ctx != _devices.end(); ++ctx) {
        if (ctx->device == object) {
            // Make sure we no longer reference the device.
            ctx->device = 0;
            // Abort the transfer if was in progress.
            if (ctx->running) {
                _log->debug(tr("DataPull: Data transfer destination destroyed"));
                ctx->running = false;
                processNewStateLater();
            }
            break;
        }
    }
}


//----------------------------------------------------------------------------
// Check if more data need to be pulled from the subclass.
//----------------------------------------------------------------------------

bool QtlDataPull::needMoreData() const
{
    // If the transfer was properly closed, we do not need more data.
    if (_closed) {
        return false;
    }

    // Look for underflow conditions
    bool underflow = false;
    for (QList<Context>::ConstIterator ctx = _devices.begin(); ctx != _devices.end(); ++ctx) {
        if (ctx->running) {
            if (_totalIn - ctx->totalOut > _minBufferSize) {
                // This device is full, we can't get more data.
                return false;
            }
            else {
                // This device needs more data.
                underflow = true;
            }
        }
    }
    return underflow;
}


//----------------------------------------------------------------------------
// Invoked from the event loop to process a new state.
//----------------------------------------------------------------------------

void QtlDataPull::processNewState()
{
    // Prevent recursion during nested event processing. When we call needTransfer(),
    // there will be calls to write(). During write(), we process events to make the
    // application more responsive. If there are queued calls to processNewState(),
    // they could be called. Such recursion could lead to output data in the wrong
    // compared to input data.
    QtlIncrement<int> updating(&_processingState);
    if (_processingState > 1) {
        return;
    }

    // Filter outdated calls.
    if (_devices.isEmpty()) {
        return;
    }

    // If some devices need data and none are busy, ask for more data to the subclass.
    if (needMoreData()) {
        if (!needTransfer()) {
            // The subclass returned false, abort everything.
            _log->debug(tr("DataPull: subclass returned an error on needTransfer"));
            for (QList<Context>::Iterator ctx = _devices.begin(); ctx != _devices.end(); ++ctx) {
                ctx->running = false;
            }
        }
    }

    // Process closing or aborting devices.
    // Use an index instead of "foreach" since we may remove some of them.
    for (int index = 0; index < _devices.size(); ++index) {
        Context& ctx(_devices[index]);
        if (_closed || !ctx.running) {
            // Found a terminating device.
            // The device is zero when the device object has been destroyed.
            if (ctx.device != 0) {
                // Notify application.
                emit deviceCompleted(ctx.device, _closed);

                // Disconnect from this device's signals.
                disconnect(ctx.device, 0, this, 0);

                // Device-specific operations to notify the termination.
                deviceSpecificCleanup(ctx.device, _closed && ctx.running);
            }
            // Remove this device from the list.
            _devices.removeAt(index);
            --index;
        }
    }

    // If the list becomes empty, this is the end of the transfer.
    if (_devices.isEmpty()) {

        // Report a full debug message.
        const int ms = _startTime.elapsed();
        const qint64 bps = ms <= 0 ? 0 : (qint64(_totalIn) * 8 * 1000) / ms;
        _log->debug(tr("DataPull: Data transfer %1, read %2 bytes, time: %3 ms, bandwidth: %4 b/s, %5 B/s")
                    .arg(_closed ? "completed" : "aborted")
                    .arg(_totalIn)
                    .arg(ms)
                    .arg(bps)
                    .arg(bps / 8));

        // Let the subclass do its cleanup.
        cleanupTransfer(_closed);

        // Notify clients
        emit completed(_closed);
    }

    // If some devices need data and none are busy, ask for more data to the subclass.
    if (needMoreData()) {
        processNewStateLater();
    }
}


//----------------------------------------------------------------------------
// Perform specific setup on a device, depending on its class.
//----------------------------------------------------------------------------

void QtlDataPull::deviceSpecificSetup(QIODevice* dev)
{
    // If the device is a process, abort the transfer if the process unexpectedly terminates.
    QProcess* process = qobject_cast<QProcess*>(dev);
    if (process != 0) {
        // Note: In Qt 5, the signal QProcess::finished is overloaded and we must explicitly
        // select the one to use in order to resolve the template instantiation. Quite dirty...
        connect(process, static_cast<void(QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished), this, &QtlDataPull::stopCaller);
    }

    // If the device is a socket, abort the transfer if the socket is unexpectedly disconnected.
    QAbstractSocket* socket = qobject_cast<QAbstractSocket*>(dev);
    if (socket != 0) {
        connect(socket, &QAbstractSocket::disconnected, this, &QtlDataPull::stopCaller);
    }
}

void QtlDataPull::deviceSpecificCleanup(QIODevice* dev, bool closed)
{
    // Don't do this on abort, we assume that the device is in error state.
    // In case of close(), the device is fine but we decide to stop the transfer.
    // In this case, we need to notify the device that the transfer is complete.
    if (closed) {

        // If the device is a process, close the write channel to notify the termination.
        QProcess* process = qobject_cast<QProcess*>(dev);
        if (process != 0) {
            process->closeWriteChannel();
        }

        // If the device is a socket, disconnect.
        QAbstractSocket* socket = qobject_cast<QAbstractSocket*>(dev);
        if (socket != 0) {
            socket->disconnectFromHost();
        }
    }
}
