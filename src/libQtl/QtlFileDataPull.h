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
//!
//! @file QtlFileDataPull.h
//!
//! Declare the class QtlFileDataPull.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLFILEDATAPULL_H
#define QTLFILEDATAPULL_H

#include "QtlDataPull.h"
#include "QtlByteBlock.h"
#include "QtlFileSlices.h"

//!
//! A class to pull data from a list of files into an asynchronous device such as QProcess.
//! @see QtlDataPull
//!
class QtlFileDataPull : public QtlDataPull
{
    Q_OBJECT

public:
    //!
    //! Default transfer size in bytes (1 MB).
    //!
    static const int DEFAULT_TRANSFER_SIZE = 1024 * 1024;

    //!
    //! Constructor using a list of files.
    //! Progress reporting is automatically enabled.
    //! @param [in] fileNames List of files to transfer. They are concatenated on the destination.
    //! @param [in] transferSize Data transfer size in bytes.
    //! @param [in] minBufferSize The minimum buffer size is the lower limit of the
    //! buffered data. When the amount of data not yet written to the device is lower
    //! than this size, new data is pulled from the DVD.
    //! @param [in] log Optional message logger.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtlFileDataPull(const QStringList& fileNames,
                             int transferSize = DEFAULT_TRANSFER_SIZE,
                             int minBufferSize = DEFAULT_MIN_BUFFER_SIZE,
                             QtlLogger* log = 0,
                             QObject* parent = 0);

    //!
    //! Constructor using a list of file slices.
    //! Progress reporting is automatically enabled.
    //! @param [in] files List of slices of files to transfer. They are concatenated on the destination.
    //! @param [in] transferSize Data transfer size in bytes.
    //! @param [in] minBufferSize The minimum buffer size is the lower limit of the
    //! buffered data. When the amount of data not yet written to the device is lower
    //! than this size, new data is pulled from the DVD.
    //! @param [in] log Optional message logger.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtlFileDataPull(const QtlFileSlicesPtrList& files = QtlFileSlicesPtrList(),
                             int transferSize = DEFAULT_TRANSFER_SIZE,
                             int minBufferSize = DEFAULT_MIN_BUFFER_SIZE,
                             QtlLogger* log = 0,
                             QObject* parent = 0);

protected:
    //!
    //! Initialize the transfer.
    //! Reimplemented from QtlDataPull.
    //! @return True on success, false on error.
    //!
    virtual bool initializeTransfer() Q_DECL_OVERRIDE;

    //!
    //! Invoked when more data is needed.
    //! Reimplemented from QtlDataPull.
    //! @param [in] maxSize Maximum size in bytes of the requested transfer.
    //! @return True on success, false on error.
    //!
    virtual bool needTransfer(qint64 maxSize) Q_DECL_OVERRIDE;

    //!
    //! Cleanup the transfer.
    //! Reimplemented from QtlDataPull.
    //! @param [in] clean If true, this is a clean termination.
    //!
    virtual void cleanupTransfer(bool clean) Q_DECL_OVERRIDE;

private:
    QtlFileSlicesPtrList                _files;    //!< Input files.
    QtlFileSlicesPtrList::ConstIterator _current;  //!< Current file.
    QtlByteBlock                        _buffer;   //!< Transfer buffer.

    //!
    //! Convert a list of file names into a list of file slices.
    //! @param [in] fileNames List of file names.
    //! @return A list of file slices.
    //!
    static QtlFileSlicesPtrList toFileSlicesList(const QStringList& fileNames);

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlFileDataPull)
};

#endif // QTLFILEDATAPULL_H
