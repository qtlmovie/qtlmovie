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
//! @file QtlDvdDataPull.h
//!
//! Declare the class QtlDvdDataPull.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLDVDDATAPULL_H
#define QTLDVDDATAPULL_H

#include "QtlDataPull.h"
#include "QtlDvdTitleSet.h"

//!
//! A class to pull data from an encrypted DVD into an asynchronous device such as QProcess.
//! @see QtlDataPull
//!
class QtlDvdDataPull : public QtlDataPull
{
    Q_OBJECT

public:
    //!
    //! Default transfer size in bytes (512 kB, 256 DVD sectors).
    //!
    static const int DEFAULT_TRANSFER_SIZE = 512 * 1024;

    //!
    //! Constructor.
    //! @param [in] deviceName DVD device name.
    //! @param [in] startSector First sector to read.
    //! @param [in] sectorCount Total number of sectors to read.
    //! @param [in] transferSize Data transfer size in bytes.
    //! @param [in] minBufferSize The minimum buffer size is the lower limit of the
    //! buffered data. When the amount of data not yet written to the device is lower
    //! than this size, new data is pulled from the DVD.
    //! @param [in] log Optional message logger.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtlDvdDataPull(const QString& deviceName,
                            int startSector,
                            int sectorCount,
                            int transferSize = DEFAULT_TRANSFER_SIZE,
                            int minBufferSize = DEFAULT_MIN_BUFFER_SIZE,
                            QtlLogger* log = 0,
                            QObject* parent = 0);

    //!
    //! Destructor.
    //!
    virtual ~QtlDvdDataPull();

protected:
    //!
    //! Initialize the transfer.
    //! Reimplemented from QtlDataPull.
    //! @param [in] device Data destination. Never null.
    //! @return True on success, false on error.
    //!
    virtual bool initializeTransfer(QIODevice* device);

    //!
    //! Invoked when more data is needed.
    //! Reimplemented from QtlDataPull.
    //! @return True on success, false on error.
    //!
    virtual bool needTransfer();

    //!
    //! Cleanup the transfer.
    //! Reimplemented from QtlDataPull.
    //! @param [in] device Data destination. It can be null.
    //! @param [in] clean If true, this is a clean termination.
    //!
    virtual void cleanupTransfer(QIODevice* device, bool clean);

private:
    QString          _deviceName;   //!< DVD device name.
    int              _startSector;  //!< First sector to transfer.
    int              _sectorCount;  //!< Total number of sectors to transfer.
    int              _sectorChunk;  //!< Number of sectors per transfer.
    int              _sectorRemain; //!< Remaining number of sectors to transfer.
    QtlByteBlock     _buffer;       //!< Transfer buffer.
    struct dvdcss_s* _dvdcss;       //!< Handle to libdvdcss (don't include dvdcss.h in this .h).
};

#endif // QTLDVDDATAPULL_H
