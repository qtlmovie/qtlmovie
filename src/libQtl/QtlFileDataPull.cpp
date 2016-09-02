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
// Define the class QtlFileDataPull.
//
//----------------------------------------------------------------------------

#include "QtlFileDataPull.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlFileDataPull::QtlFileDataPull(const QStringList& fileNames,
                                 int transferSize,
                                 int minBufferSize,
                                 QtlLogger* log,
                                 QObject* parent) :
    QtlFileDataPull(toFileSlicesList(fileNames), transferSize, minBufferSize, log, parent)
{
}


QtlFileDataPull::QtlFileDataPull(const QtlFileSlicesPtrList& files,
                                 int transferSize,
                                 int minBufferSize,
                                 QtlLogger* log,
                                 QObject* parent) :
    QtlDataPull(minBufferSize, log, parent),
    _files(files),
    _current(_files.begin()),
    _buffer(qMax(1024, transferSize))
{
    // Set total transfer size in bytes.
    qint64 total = 0;
    foreach (const QtlFileSlicesPtr& file, _files) {
        if (!file.isNull()) {
            const qint64 size = file->size();
            if (size > 0) {
                total += size;
            }
        }
    }
    setProgressMaxHint(total);

    // Set progress interval: every 5% below 100 MB, every 1% above.
    setProgressIntervalInBytes(total < 100000000 ? total / 20 : total / 100);
}


//----------------------------------------------------------------------------
// Initialize the transfer.
//----------------------------------------------------------------------------

bool QtlFileDataPull::initializeTransfer()
{
    // Make sure all files are closed.
    foreach (const QtlFileSlicesPtr& file, _files) {
        if (!file.isNull() && file->isOpen()) {
            file->close();
        }
    }

    // Restart at first file.
    _current = _files.begin();
    return true;
}


//----------------------------------------------------------------------------
// Invoked when more data is needed.
//----------------------------------------------------------------------------

bool QtlFileDataPull::needTransfer(qint64 maxSize)
{
    // Loop until something is read.
    for (;;) {

        // Transfer completed after last file.
        if (_current == _files.end()) {
            return false;
        }

        // Close current file if at end of file.
        if (!_current->isNull() && (*_current)->isOpen() && (*_current)->atEnd()) {
            (*_current)->close();
            ++_current;
            continue;
        }

        // Open next file if none is open.
        if (!(*_current)->isOpen() && !(*_current)->open()) {
            log()->line(tr("Error opening %1").arg((*_current)->fileName()));
            return false;
        }

        // Maximum size of data to read.
        qint64 count = _buffer.size();
        if (maxSize >= 0 && maxSize < count) {
            count = maxSize;
        }
        if (count <= 0) {
            return true;
        }

        // Read some data from the input file.
        count = (*_current)->read(reinterpret_cast<char*>(_buffer.data()), count);
        if (count == 0) {
            // At end of file, loop on next one.
            (*_current)->close();
            ++_current;
        }
        else if (count < 0) {
            // Read error.
            log()->line(tr("Error reading %1").arg((*_current)->fileName()));
            return false;
        }
        else {
            // Write data.
            return write(_buffer.data(), count);
        }
    }
}


//----------------------------------------------------------------------------
// Cleanup the transfer.
//----------------------------------------------------------------------------

void QtlFileDataPull::cleanupTransfer(bool clean)
{
    // Make sure all files are closed.
    foreach (const QtlFileSlicesPtr& file, _files) {
        if (!file.isNull() && file->isOpen()) {
            file->close();
        }
    }
}


//----------------------------------------------------------------------------
// Convert a list of file names into a list of file slices.
//----------------------------------------------------------------------------

QtlFileSlicesPtrList QtlFileDataPull::toFileSlicesList(const QStringList& fileNames)
{
    QtlFileSlicesPtrList result;
    foreach (const QString& name, fileNames) {
        result << QtlFileSlicesPtr(new QtlFileSlices(name));
    }
    return result;
}
