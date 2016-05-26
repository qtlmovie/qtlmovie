//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2016, Thierry Lelegard
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
//! @file QtsTeletextFrame.h
//!
//! Declare the class QtsTeletextFrame.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSTELETEXTFRAME_H
#define QTSTELETEXTFRAME_H

#include "QtsCore.h"

//!
//! Description of one Teletext frame.
//!
class QtsTeletextFrame
{
public:
    QtsPid      pid;            //!< PID number.
    int         page;           //!< Teletext page number.
    int         frameCount;     //!< Frame counter in this page, starting at 1.
    quint64     showTimestamp;  //!< Show frame at this timestamp (in ms from start of stream)
    quint64     hideTimestamp;  //!< Hide frame at this timestamp (in ms from start of stream)
    QStringList lines;          //!< Text lines. May contain embedded HTML tags.

    //!
    //! Constructor.
    //! @param [in] pid PID number.
    //! @param [in] page Teletext page number.
    //! @param [in] frameCount Frame counter in this page, starting at 1.
    //! @param [in] showTimestamp Show frame at this timestamp (in ms from start of stream)
    //! @param [in] hideTimestamp Hide frame at this timestamp (in ms from start of stream)
    //! @param [in] lines Text lines.
    //!
    QtsTeletextFrame(
            QtsPid      pid_           = 0,
            int         page_          = 0,
            int         frameCount_    = 0,
            quint64     showTimestamp_ = 0,
            quint64     hideTimestamp_ = 0,
            QStringList lines_         = QStringList());

    //!
    //! Get the "show" timestamp in SRT format.
    //! @return The "show" timestamp in SRT format.
    //!
    QString srtShowTimestamp() const
    {
        return toSrtTime(showTimestamp);
    }

    //!
    //! Get the "hide" timestamp in SRT format.
    //! @return The "hide" timestamp in SRT format.
    //!
    QString srtHideTimestamp() const
    {
        return toSrtTime(hideTimestamp);
    }

    //!
    //! Get the frame duration in SRT format.
    //! @return The frame duration in SRT format.
    //!
    QString srtDuration() const;

    //!
    //! Get the complete frame in SRT format.
    //! @return The complete frame in SRT format.
    //!
    QString srtFrame() const;

    //!
    //! Format a timestamp as SRT time.
    //! @param [in] timestamp Timestamp in milliseconds.
    //! @return SRT formatted time.
    //!
    static QString toSrtTime(quint64 timestamp);
};

#endif // QTSTELETEXTFRAME_H
