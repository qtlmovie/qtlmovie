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
//!
//! @file QtlFileSlices.h
//!
//! Declare the class QtlFileSlices.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLFILESLICES_H
#define QTLFILESLICES_H

#include "QtlRangeList.h"
#include "QtlSmartPointer.h"

class QtlFileSlices;

//!
//! Smart pointer to QtlFileSlices, non thread-safe.
//!
typedef QtlSmartPointer<QtlFileSlices,QtlNullMutexLocker> QtlFileSlicesPtr;
Q_DECLARE_METATYPE(QtlFileSlicesPtr)

//!
//! List of smart pointers to QtlFileSlices, non thread-safe.
//!
typedef QList<QtlFileSlicesPtr> QtlFileSlicesPtrList;

//!
//! A read-only file device with sequential access on a list of data slices.
//! When reading the file, only specific portions of the physical files are read.
//!
class QtlFileSlices : public QIODevice
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] fileName Name of the file.
    //! @param [in] slices Slices of data to read in the files. If empty, the whole file is read.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtlFileSlices(const QString& fileName, const QtlRangeList& slices = QtlRangeList(), QObject* parent = 0);

    //!
    //! Get the file name.
    //! @return The file name.
    //!
    QString fileName() const
    {
        return _file.fileName();
    }

    //!
    //! Open the device.
    //! Reimplemented from QIODevice.
    //! @param [in] mode Open mode. The only allowed mode is ReadOnly, with optional flag Text. All other modes are forbidden.
    //! @return True on success, false on error.
    //!
    virtual bool open(OpenMode mode = ReadOnly) Q_DECL_OVERRIDE;

    //!
    //! Close the device.
    //! Reimplemented from QIODevice.
    //!
    virtual void close() Q_DECL_OVERRIDE;

    //!
    //! Get the size of the device, meaning the number of bytes to read.
    //! Reimplemented from QIODevice.
    //! @return The number of bytes to read, which is the sum of all slices to read.
    //! This is different from the size of the underlying file.
    //! If some slices overlap, the same data are counted several times.
    //!
    virtual qint64 size() const Q_DECL_OVERRIDE
    {
        return _size;
    }

    //!
    //! Get the position that data is read from.
    //! Reimplemented from QIODevice.
    //! @return The current position in the slices, which is also the number of bytes already read.
    //!
    virtual qint64 pos() const Q_DECL_OVERRIDE
    {
        return _readSize;
    }

    //!
    //! Returns true if the current read position is at the end of the device.
    //! Reimplemented from QIODevice.
    //! @return True if there is no more data to read, false otherwise.
    //!
    virtual bool atEnd() const Q_DECL_OVERRIDE
    {
        return _readSize >= _size;
    }

    //!
    //! Returns true if this device is sequential; otherwise returns false.
    //! Reimplemented from QIODevice.
    //! @return Always true.
    //!
    virtual bool isSequential() const Q_DECL_OVERRIDE
    {
        return true;
    }

protected:

    //!
    //! Reads up to maxSize bytes from the device into data.
    //! Implemented from QIODevice requirement.
    //! @param [out] data Buffer for returned data.
    //! @param [in] maxSize Maximum number of bytes to read.
    //! @return Number of bytes read or -1 if an error occurred.
    //!
    virtual qint64 readData(char *data, qint64 maxSize) Q_DECL_OVERRIDE;

    //!
    //! Writes up to maxSize bytes from data to the device.
    //! Implemented from QIODevice requirement.
    //! @param [in] data Buffer for data to write.
    //! @param [in] maxSize Maximum number of bytes to write.
    //! @return Always -1 (error) since the file is read-only.
    //!
    virtual qint64 writeData(const char *data, qint64 maxSize) Q_DECL_OVERRIDE
    {
        return -1;
    }

private:
    QtlRangeList _slices;     //!< Slices of file to read.
    qint64       _size;       //!< Number of bytes to read in the file.
    qint64       _readSize;   //!< Number of bytes already read in the file.
    QFile        _file;       //!< Actual device to read.
    int          _nextByte;   //!< Next byte in _currentSlice, -1 means at beginning.
    QtlRangeList::ConstIterator _currentSlice; //!< Current pointer in _slices.

    // Unaccessible operations.
    QtlFileSlices() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlFileSlices)
};

#endif // QTLFILESLICES_H
