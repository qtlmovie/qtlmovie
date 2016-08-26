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
#include "QtlByteBlock.h"
#include "QtlDvdMedia.h"

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
    //! Progress reporting is automatically enabled.
    //! @param [in] deviceName DVD device name.
    //! @param [in] startSector First sector to read.
    //! @param [in] sectorCount Total number of sectors to read.
    //! @param [in] badSectorPolicy How to handle bad sectors.
    //! @param [in] transferSize Data transfer size in bytes.
    //! @param [in] minBufferSize The minimum buffer size is the lower limit of the
    //! buffered data. When the amount of data not yet written to the device is lower
    //! than this size, new data is pulled from the DVD.
    //! @param [in] log Optional message logger.
    //! @param [in] parent Optional parent object.
    //! @param [in] useMaxReadSpeed If true, try to set the DVD reader to maximum speed.
    //! @see QtlDvdMedia::BadSectorPolicy
    //!
    explicit QtlDvdDataPull(const QString& deviceName,
                            int startSector,
                            int sectorCount,
                            Qtl::BadSectorPolicy badSectorPolicy = Qtl::SkipBadSectors,
                            int transferSize = DEFAULT_TRANSFER_SIZE,
                            int minBufferSize = DEFAULT_MIN_BUFFER_SIZE,
                            QtlLogger* log = 0,
                            QObject* parent = 0,
                            bool useMaxReadSpeed = false);

    //!
    //! Constructor.
    //! Progress reporting is automatically enabled.
    //! @param [in] deviceName DVD device name.
    //! @param [in] sectorList List of sectors to read.
    //! @param [in] badSectorPolicy How to handle bad sectors.
    //! @param [in] transferSize Data transfer size in bytes.
    //! @param [in] minBufferSize The minimum buffer size is the lower limit of the
    //! buffered data. When the amount of data not yet written to the device is lower
    //! than this size, new data is pulled from the DVD.
    //! @param [in] log Optional message logger.
    //! @param [in] parent Optional parent object.
    //! @param [in] useMaxReadSpeed If true, try to set the DVD reader to maximum speed.
    //! @see QtlDvdMedia::BadSectorPolicy
    //!
    explicit QtlDvdDataPull(const QString& deviceName,
                            const QtlRangeList sectorList,
                            Qtl::BadSectorPolicy badSectorPolicy = Qtl::SkipBadSectors,
                            int transferSize = DEFAULT_TRANSFER_SIZE,
                            int minBufferSize = DEFAULT_MIN_BUFFER_SIZE,
                            QtlLogger* log = 0,
                            QObject* parent = 0,
                            bool useMaxReadSpeed = false);

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

    //!
    //! Report bandwidth.
    //!
    void reportBandwidth();

private:
    const QString               _deviceName;      //!< DVD device name.
    const QtlRangeList          _sectorList;      //!< List of sectors to read.
    const Qtl::BadSectorPolicy  _badSectorPolicy; //!< How to handle bad sectors.
    const int                   _sectorChunk;     //!< Number of sectors per transfer.
    const bool                  _maxReadSpeed;    //!< Set the DVD reader to maximum speed.
    QtlRangeList::ConstIterator _currentRange;    //!< Current pointer in _sectorList.
    int           _nextSector;     //!< Next sector in _currentRange, -1 means at beginning.
    QtlByteBlock  _buffer;         //!< Transfer buffer.
    QtlDvdMedia   _dvd;            //!< Access to DVD media.
    QTime         _timeAverage;    //!< Timer for average bandwidth reporting.
    QTime         _timeInstant;    //!< Timer for instant bandwidth reporting.
    int           _countAverage;   //!< Count sectors for average bandwidth reporting.
    int           _countInstant;   //!< Count sectors for instant bandwidth reporting.
    const int     _reportInterval; //!< Interval in milli-seconds between bandwidth reporting.

    // Unaccessible operations.
    QtlDvdDataPull() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlDvdDataPull)
};

#endif // QTLDVDDATAPULL_H
