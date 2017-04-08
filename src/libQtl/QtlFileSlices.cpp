//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2017, Thierry Lelegard
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
// Define the class QtlFileSlices.
//
//----------------------------------------------------------------------------

#include "QtlFileSlices.h"


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlFileSlices::QtlFileSlices(const QString& fileName, const QtlRangeList& slices, QObject* parent) :
    QIODevice(parent),
    _slices(slices),
    _size(0),
    _readSize(0),
    _file(fileName),
    _nextByte(-1),
    _currentSlice(_slices.begin())
{
    const qint64 fileSize = QFileInfo(fileName).size();

    if (_slices.empty()) {
        // If no slice is specified, read the complete file.
        _slices << QtlRange(0, fileSize - 1);
    }
    else {
        // Make sure that all slices are within file limits.
        _slices.clip(QtlRange(0, fileSize - 1));
        _slices.merge(Qtl::AdjacentOnly);
    }

    // Total number of bytes to read.
    _size = _slices.totalValueCount();
}


//----------------------------------------------------------------------------
// Open the device.
//----------------------------------------------------------------------------

bool QtlFileSlices::open(QIODevice::OpenMode mode)
{
    // Filter already open or invalid file mode.
    if (_file.isOpen() || (mode | ReadOnly) == 0 || (mode & ~(ReadOnly | Text)) != 0) {
        return false;
    }

    // Open actual file.
    if (!_file.open(mode)) {
        return false;
    }

    // Open superclass.
    if (!QIODevice::open(mode)) {
        _file.close();
        return false;
    }

    // Set position to first slice to read.
    _currentSlice = _slices.begin();
    _nextByte = -1;
    _readSize = 0;
    return true;
}


//----------------------------------------------------------------------------
// Close the device.
//----------------------------------------------------------------------------

void QtlFileSlices::close()
{
    _file.close();
    QIODevice::close();
}


//----------------------------------------------------------------------------
// Reads up to maxSize bytes from the device into data.
//----------------------------------------------------------------------------

qint64 QtlFileSlices::readData(char* data, qint64 maxSize)
{
    qint64 totalRead = 0;

    while (totalRead < maxSize) {

        // Move forward in slice list until we find something to read.
        while (_currentSlice != _slices.end()) {
            if (_currentSlice->isEmpty() || _nextByte > _currentSlice->last()) {
                // Empty slice or already completely read, look at next one.
                ++_currentSlice;
            }
            else if (_nextByte >= 0) {
                // Currently in the middle of this slice, continue reading.
                break;
            }
            else {
                // Current slice not yet started, need to seek here.
                _nextByte = _currentSlice->first();
                if (!_file.seek(_nextByte)) {
                    return totalRead > 0 ? totalRead : -1; // error
                }
                break;
            }
        }

        // Is the last slice completed?
        if (_currentSlice == _slices.end()) {
            return totalRead;
        }

        Q_ASSERT(_currentSlice->first() <= _currentSlice->last());
        Q_ASSERT(_nextByte >= _currentSlice->first());
        Q_ASSERT(_nextByte <= _currentSlice->last());

        // Compute maximum number of bytes to read.
        qint64 count = qMin(maxSize - totalRead, _currentSlice->last() - _nextByte + 1);
        if (count <= 0) {
            return totalRead;
        }

        // Read data.
        if ((count = _file.read(data, count)) <= 0) {
            return totalRead > 0 ? totalRead : -1; // error
        }

        _readSize += count;
        _nextByte += count;
        totalRead += count;
        data += count;
    }

    return totalRead;
}
