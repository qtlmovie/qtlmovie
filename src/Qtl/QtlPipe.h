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
//!
//! @file QtlPipe.h
//!
//! Declare the class QtlPipe.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLPIPE_H
#define QTLPIPE_H

#include <QtCore>

//!
//! A subclass of QIODevice which buffers data between a producer and a consumer.
//!
class QtlPipe : public QIODevice
{
#if !defined (DOXYGEN)
    Q_OBJECT
#endif

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent object.
    //! @param [in] bufferSize Internal buffer size.
    //!
    explicit QtlPipe(QObject *parent = 0, int bufferSize = 8192);

    //!
    //! Return the number of available bytes for reading.
    //! Reimplemented from QIODevice.
    //! @return The number of available bytes for reading.
    //!
    virtual qint64 bytesAvailable() const;

    //!
    //! Return the number of available bytes to write.
    //! Reimplemented from QIODevice.
    //! @return The number of available bytes for writing.
    //!
    virtual qint64 bytesToWrite() const;

    //!
    //! Get for end of current input.
    //! Reimplemented from QIODevice.
    //! @return Returns true if the current read and write position is at
    //! the end of the device (i.e. there is no more data available for reading
    //! on the device); otherwise returns false.
    //!
    virtual bool atEnd() const;

    //!
    //! Clear the buffer.
    //!
    void clear();

    //!
    //! Automatically pull data from a QIODevice.
    //!
    //! When data is ready from @a source and there is some space in the
    //! buffer, automatically pull all available data.
    //!
    //! If a previous source was specified, forget it. Do forget the
    //! previous source without using a new one, use zero.
    //!
    //! @param [in] source Where to pull the data from. Can be null.
    //!
    void pullFrom(QIODevice* source);

protected:
    //!
    //! Read data from the pipe device.
    //! Reimplemented from QIODevice.
    //! @param [out] data Data buffer.
    //! @param [in] maxSize Maximum data size to read.
    //! @return Returned data size or -1 on error.
    //!
    virtual qint64 readData(char* data, qint64 maxSize);

    //!
    //! Write data into the pipe device.
    //! Reimplemented from QIODevice.
    //! @param [in] data Data buffer.
    //! @param [in] maxSize Maximum data size to write.
    //! @return Written data size or -1 on error.
    //!
    virtual qint64 writeData(const char* data, qint64 maxSize);

private slots:
    //!
    //! Invoked when a source object is destroyed.
    //! @param [in] object The destroyed object.
    //!
    void sourceDestroyed(QObject* object);

    //!
    //! Invoked when data is available from the source.
    //!
    void readFromSource();

private:
    mutable QMutex _mutex;  //!< Buffer exclusive access.
    QVector<char>  _buffer; //!< Internal circular data buffer.
    qint64         _start;  //!< Current start index of data inside _buffer.
    qint64         _size;   //!< Current data size inside _buffer.
    QIODevice*     _source; //!< External data source.

    // Prevent object instance copy.
    Q_DISABLE_COPY(QtlPipe)
};

#endif // QTLPIPE_H
