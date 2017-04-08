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
//! @file QtsTimeStamper.h
//!
//! Declare the class QtsTimeStamper.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSTIMESTAMPER_H
#define QTSTIMESTAMPER_H

#include "QtsDemux.h"
#include "QtsPesPacket.h"

//!
//! Time stamp management per PID.
//!
//! Return a current time reference in millisecond for a given PID.
//! The first time reference found is zero.
//! Use the PTS from the PID if some are found.
//! Otherwise, use the global PCR from the TS.
//!
class QtsTimeStamper
{
public:
    //!
    //! Constructor.
    //! @param [in] demux Optional demux which will be used to collect PCR.
    //! If zero, no PCR will be used, only PTS from the PES packets in the PID.
    //!
    QtsTimeStamper(const QtsDemux* demux = 0);

    //!
    //! Set a new demux.
    //! @param [in] demux New demux. Ignored if the time source is already PTS on the PID.
    //!
    void setDemux(const QtsDemux* demux = 0);

    //!
    //! Get the last timestamp in milliseconds, starting with zero.
    //! @return The last timestamp in milliseconds.
    //!
    quint64 lastTimeStamp();

    //!
    //! Process one PES packet from the reference PID.
    //! @param [in] packet The PES packet. The first packet defines the PID.
    //! All subsequent packets must belong to the same PID.
    //!
    void processPesPacket(const QtsPesPacket& packet);

    //!
    //! Reset the stamper, back to constructor state.
    //!
    void reset();

    //!
    //! Get the reference PID.
    //! @return The reference PID or QTS_PID_NULL if unknown.
    //!
    QtsPid getPid() const
    {
        return _pid;
    }

private:
    //!
    //! Our source of time reference.
    //!
    enum TimeSource {PTS, PCR, UNDEFINED};

    //!
    //! Process a new clock value in millisecond.
    //! @param [in] clock New clock value in millisecond.
    //!
    void processClock(qint64 clock);

    const QtsDemux* _demux;          //!< Demux providing the PCR.
    QtsPid          _pid;            //!< Reference PID for timestamps.
    TimeSource      _source;         //!< Where do we get the time reference from.
    quint64         _lastTimeStamp;  //!< Last known timestamp.
    qint64          _previousClock;  //!< Previous time clock value.
    qint64          _delta;          //!< Adjustment value between time clock and time stamps.
};

#endif // QTSTIMESTAMPER_H
