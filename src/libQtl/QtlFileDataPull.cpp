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
#include "dvdcss.h"


//----------------------------------------------------------------------------
// Constructor and destructor.
//----------------------------------------------------------------------------

QtlFileDataPull::QtlFileDataPull(const QStringList& fileNames,
                                 int transferSize,
                                 int minBufferSize,
                                 QtlLogger* log,
                                 QObject* parent) :
    QtlDataPull(minBufferSize, log, parent),
    _fileNames(fileNames),
    _input(),
    _currentIndex(0),
    _buffer(qMax(1024, transferSize))
{
}


//----------------------------------------------------------------------------
// Initialize the transfer.
//----------------------------------------------------------------------------

bool QtlFileDataPull::initializeTransfer()
{
    // Make sure the current file is closed.
    if (_input.isOpen()) {
        _input.close();
    }

    // Restart at first file.
    _currentIndex = 0;
    return true;
}


//----------------------------------------------------------------------------
// Invoked when more data is needed.
//----------------------------------------------------------------------------

bool QtlFileDataPull::needTransfer(qint64 maxSize)
{
    // Loop until something is read.
    for (;;) {

        // Close current file if at end of file.
        if (_input.isOpen() && _input.atEnd()) {
            _input.close();
            _currentIndex++;
        }

        // Transfer completed after last file.
        if (_currentIndex < 0 || _currentIndex >= _fileNames.size()) {
            close();
            return true;
        }

        // Open next file if none is open.
        if (!_input.isOpen()) {
            _input.setFileName(_fileNames.at(_currentIndex));
            if (!_input.open(QFile::ReadOnly)) {
                log()->line(tr("Error opening %1").arg(_fileNames.at(_currentIndex)));
                return false;
            }
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
        count = _input.read(reinterpret_cast<char*>(_buffer.data()), count);
        if (count == 0) {
            // At end of file, loop on next one.
            _input.close();
            _currentIndex++;
        }
        else if (count < 0) {
            // Read error.
            log()->line(tr("Error reading %1").arg(_fileNames.at(_currentIndex)));
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
    // Make sure the current file is closed.
    if (_input.isOpen()) {
        _input.close();
    }
}
