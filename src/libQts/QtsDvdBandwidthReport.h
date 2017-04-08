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
//! @file QtsDvdBandwidthReport.h
//!
//! Declare the class QtsDvdBandwidthReport.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDVDBANDWIDTHREPORT_H
#define QTSDVDBANDWIDTHREPORT_H

#include "QtlLogger.h"
#include "QtsDvd.h"

//!
//! A utility class to report transfer bandwidth on DVD media.
//!
class QtsDvdBandwidthReport
{
public:
    //!
    //! Constructor.
    //! @param [in] reportInterval Interval in milli-seconds between bandwidth reporting.
    //! @param [in] log Where to report messages.
    //!
    QtsDvdBandwidthReport(int reportInterval, QtlLogger* log);

    //!
    //! Start the transfer.
    //!
    void start();

    //!
    //! Declare the transfer of sectors from DVD media.
    //! If it is time to report the bandwidth, a message is logged.
    //! @param [in] sectorCount Number of sectors transfered.
    //!
    void transfered(int sectorCount);

    //!
    //! Report bandwidth immediately.
    //!
    void reportBandwidth();

    //!
    //! Build a human-readable string for DVD transfer rate.
    //! @param [in] bytes Number of bytes transfered.
    //! @param [in] milliSeconds Number of milliseconds to transfer @a bytes.
    //! @param [in] flags A set of flags to specify the format.
    //! @return A human readable string.
    //!
    static QString transferRateToString(qint64 bytes, qint64 milliSeconds, Qts::TransferRateFlags flags = Qts::TransferDvdBase);

private:
    QtlLogger* _log;            //!< Where to log the messages.
    bool       _started;        //!< If the report was actually started.
    QTime      _timeAverage;    //!< Timer for average bandwidth reporting.
    QTime      _timeInstant;    //!< Timer for instant bandwidth reporting.
    int        _countAverage;   //!< Count sectors for average bandwidth reporting.
    int        _countInstant;   //!< Count sectors for instant bandwidth reporting.
    int        _reportInterval; //!< Interval in milli-seconds between bandwidth reporting.
};

#endif // QTSDVDBANDWIDTHREPORT_H
