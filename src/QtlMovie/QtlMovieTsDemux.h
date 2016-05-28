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
//! @file QtlMovieTsDemux.h
//!
//! Declare the class QtlMovieTsDemux.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIETSDEMUX_H
#define QTLMOVIETSDEMUX_H

#include <QObject>
#include "QtlMovieAction.h"
#include "QtsTsFile.h"
#include "QtsDemux.h"

//!
//! Abstract base class to read an MPEG-TS file and demux its content.
//!
//! Reading a TS file is not a blocking operation. When start() is invoked,
//! the file is not synchronously read. The Qt event loop invokes the appropriate
//! slots when data is available from the file. The completion is notified using
//! the signal completed(), inherited from QtlMovieAction.
//!
class QtlMovieTsDemux : public QtlMovieAction
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] fileName MPEG-TS file name.
    //! @param [in] settings Application settings.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieTsDemux(const QString& fileName,
                    const QtlMovieSettings* settings,
                    QtlLogger* log,
                    QObject *parent = 0);

    //!
    //! Start the analysis.
    //! Reimplemented from QtlMovieAction.
    //! @return False if already started. True otherwise.
    //!
    virtual bool start();

    //!
    //! Abort analysis.
    //! Reimplemented from QtlMovieAction.
    //! If the analysis is in progress(), the signal completed() will be emitted.
    //!
    virtual void abort();

    //!
    //! Check if the input file has M2TS format.
    //! This information is available after reading at least one packet from the file.
    //! @return True if the input file has M2TS format.
    //!
    bool isM2tsFile() const
    {
        return _isM2ts;
    }

protected:
    //!
    //! Emit the completed() signal.
    //! Reimplemented from QtlMovieAction.
    //! @param [in] success True when the action completed successfully, false otherwise.
    //! @param [in] message Optional error message to log.
    //!
    virtual void emitCompleted(bool success, const QString& message = QString());

    //!
    //! Get the demux.
    //! Must be implemented by subclasses.
    //! @return The current demux. Cannot be null.
    //!
    virtual QtsDemux* demux() = 0;

    //!
    //! Event handler to handle timer.
    //! @param event Notified event.
    //!
    virtual void timerEvent(QTimerEvent* event);

private:
    QtsTsFile _file;            //!< TS file.
    bool      _isM2ts;          //!< File has M2TS format.
    int       _timerId;         //!< Repetitive timer.
    int       _totalPackets;    //!< File size in packets.
    int       _packetInterval;  //!< Min number of packets between two progress reports.
    int       _nextReport;      //!< Next packet index to indicate progress report.

    // Unaccessible operations.
    QtlMovieTsDemux() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieTsDemux)
};

#endif // QTLMOVIETSDEMUX_H
