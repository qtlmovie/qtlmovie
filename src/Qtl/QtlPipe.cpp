//----------------------------------------------------------------------------
//
// Copyright (c) 2013, Thierry Lelegard
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
// Define the class QtlPipe.
//
//----------------------------------------------------------------------------

#include "QtlPipe.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlPipe::QtlPipe(QObject* parent, int bufferSize) :
    QIODevice(parent),
    _mutex(QMutex::Recursive),
    _buffer(qMax(1, bufferSize)),
    _start(0),
    _size(0),
    _source(0)
{
}


//----------------------------------------------------------------------------
// State enquiry methods.
//----------------------------------------------------------------------------

qint64 QtlPipe::bytesAvailable() const
{
    QMutexLocker locker(&_mutex);
    return _size + QIODevice::bytesAvailable();
}

qint64 QtlPipe::bytesToWrite() const
{
    QMutexLocker locker(&_mutex);
    return _buffer.size() - _size;
}

bool QtlPipe::atEnd() const
{
    QMutexLocker locker(&_mutex);
    return _size <= 0;
}


//----------------------------------------------------------------------------
// Clear the buffer.
//----------------------------------------------------------------------------

void QtlPipe::clear()
{
    QMutexLocker locker(&_mutex);
    _start = _size = 0;
}


//----------------------------------------------------------------------------
// Automatically pull data from a QIODevice.
//----------------------------------------------------------------------------

void QtlPipe::pullFrom(QIODevice* source)
{
    QMutexLocker locker(&_mutex);

    // Close previous source.
    if (_source != 0) {
        disconnect(_source, 0, this, 0);
        _source = 0;
    }

    // Attach new source.
    if (source != 0) {
        _source = source;
        connect(_source, SIGNAL(destroyed(QObject*)), this, SLOT(sourceDestroyed(QObject*)));

        // Get read notifications if there is some room in the buffer.
        if (_size < _buffer.size()) {
            connect(_source, SIGNAL(readyRead()), this, SLOT(readFromSource()));
        }
    }
}


//----------------------------------------------------------------------------
// Invoked when a source object is destroyed.
//----------------------------------------------------------------------------

void QtlPipe::sourceDestroyed(QObject* object)
{
    QMutexLocker locker(&_mutex);
    if (object != 0 && object == _source) {
        disconnect(_source, 0, this, 0);
        _source = 0;
    }
}


//----------------------------------------------------------------------------
// Read data from the pipe device.
//----------------------------------------------------------------------------

qint64 QtlPipe::readData(char* data, qint64 maxSize)
{
    QMutexLocker locker(&_mutex);

    Q_ASSERT(_size >= 0);
    Q_ASSERT(_size <= _buffer.size());

    // Check if buffer was initially full.
    const bool wasFull = _size >= _buffer.size();

    // Number of returned bytes.
    qint64 returned = 0;

    // Since the buffer is circular, we may need to read at most twice:
    // - first part between the first used byte and end of buffer,
    // - second part, between the start of buffer and the last used byte.
    for (int count = 0; maxSize > 0 && _size > 0 && count < 2; ++count) {

        // Locate the readable part of the buffer: from _start to end.
        const qint64 end = qMin(_start + _size, qint64(_buffer.size()));

        // Read user data from the buffer.
        const qint64 cnt = qMin(maxSize, end - _start);
        ::memcpy(data, _buffer.data() + _start, size_t(cnt));

        // Update pointers and sizes.
        _size -= cnt;
        returned += cnt;
        data += cnt;
        maxSize -= cnt;
    }

    // If some room was made in the buffer, try to read from the source.
    readFromSource();

    // Restore read notifications from the source if required.
    if (_source != 0 && wasFull && _size < _buffer.size()) {
        connect(_source, SIGNAL(readyRead()), this, SLOT(readFromSource()));
    }

    return returned;
}


//----------------------------------------------------------------------------
// Write data into the pipe device.
//----------------------------------------------------------------------------

qint64 QtlPipe::writeData(const char* data, qint64 maxSize)
{
    QMutexLocker locker(&_mutex);

    Q_ASSERT(_size >= 0);
    Q_ASSERT(_size <= _buffer.size());

    // Number of written bytes.
    qint64 written = 0;

    // Since the buffer is circular, we may need to write at most twice:
    // - first part between last used byte and end of buffer,
    // - second part, between the last used byte and the start index.
    for (int count = 0; maxSize > 0 && _size < _buffer.size() && count < 2; ++count) {

        // Locate the writable part of the buffer.
        const qint64 pos = (_start + _size) % _buffer.size();
        const qint64 end = pos < _start ? _start : _buffer.size();

        // Write user data in the buffer.
        const qint64 cnt = qMin(maxSize, end - pos);
        ::memcpy(_buffer.data() + pos, data, size_t(cnt));

        // Update pointers and sizes.
        _size += cnt;
        written += cnt;
        data += cnt;
        maxSize -= cnt;
    }

    // Signal the new data.
    if (written > 0) {
        emit bytesWritten(written);
        emit readyRead();
    }

    // Stop read notifications from the source if the buffer is full.
    if (_source != 0 && _size >= _buffer.size()) {
        disconnect(_source, SIGNAL(readyRead()), this, SLOT(readFromSource()));
    }

    return written;
}


//----------------------------------------------------------------------------
// Invoked when data is available from the source.
//----------------------------------------------------------------------------

void QtlPipe::readFromSource()
{
    QMutexLocker locker(&_mutex);

    Q_ASSERT(_size >= 0);
    Q_ASSERT(_size <= _buffer.size());

    // Without source, nothing to do.
    if (_source == 0) {
        return;
    }

    // Number of written bytes.
    qint64 written = 0;

    // Since the buffer is circular, we may need to write at most twice:
    // - first part between last used byte and end of buffer,
    // - second part, between the last used byte and the start index.
    for (int count = 0; _size < _buffer.size() && count < 2; ++count) {
        // Locate the writable part of the buffer.
        const qint64 pos = (_start + _size) % _buffer.size();
        const qint64 end = pos < _start ? _start : _buffer.size();
        if (pos < end) {
            // Read data from source and write it in the buffer.
            const qint64 cnt = _source->read(_buffer.data() + pos, end - pos);
            if (cnt <= 0) {
                break;
            }
            // Update pointers and sizes.
            _size += cnt;
            written += cnt;
        }
    }

    // Signal the new data.
    if (written > 0) {
        emit bytesWritten(written);
        emit readyRead();
    }

    // Stop read notifications from the source if the buffer is full.
    if (_size >= _buffer.size()) {
        disconnect(_source, SIGNAL(readyRead()), this, SLOT(readFromSource()));
    }
}
