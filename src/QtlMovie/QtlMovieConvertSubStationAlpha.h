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
//! @file QtlMovieConvertSubStationAlpha.h
//!
//! Declare the class QtlMovieConvertSubStationAlpha.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIECONVERTSUBSTATIONALPHA_H
#define QTLMOVIECONVERTSUBSTATIONALPHA_H

#include "QtlMovieAction.h"
#include "QtlSubRipGenerator.h"
#include "QtlSubStationAlphaParser.h"

//!
//! A class to subtitles files from SSA/ASS to SRT format.
//!
//! The file processing is not a blocking operation. When start() is invoked,
//! the file is not synchronously read. The Qt event loop invokes the appropriate
//! slots when data is available from the file. The completion is notified using
//! the signal completed(), inherited from QtlMovieAction.
//!
class QtlMovieConvertSubStationAlpha : public QtlMovieAction
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] inputFileName Input file name.
    //! @param [in] outputFileName Output file name.
    //! @param [in] settings Application settings.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieConvertSubStationAlpha(const QString& inputFileName,
                                   const QString& outputFileName,
                                   const QtlMovieSettings* settings,
                                   QtlLogger* log,
                                   QObject *parent = 0);

    //!
    //! Start the processing.
    //! Reimplemented from QtlMovieAction.
    //! @return False if already started. True otherwise.
    //!
    virtual bool start() Q_DECL_OVERRIDE;

    //!
    //! Abort the processing.
    //! Reimplemented from QtlMovieAction.
    //! If the processing is in progress, the signal completed() will be emitted.
    //!
    virtual void abort() Q_DECL_OVERRIDE;

protected:
    //!
    //! Emit the completed() signal.
    //! Reimplemented from QtlMovieAction.
    //! @param [in] success True when the action completed successfully, false otherwise.
    //! @param [in] message Optional error message to log.
    //!
    virtual void emitCompleted(bool success, const QString& message = QString()) Q_DECL_OVERRIDE;

    //!
    //! Event handler to handle timer.
    //! @param event Notified event.
    //!
    virtual void timerEvent(QTimerEvent* event) Q_DECL_OVERRIDE;

private slots:
    //!
    //! Invoked when a subtitle frame is available.
    //! @param [in] frame SSA/ASS subtitle frame.
    //!
    void processSubtitleFrame(const QtlSubStationAlphaFramePtr& frame);

private:
    QFile                    _inputFile;       //!< Input file.
    QTextStream              _inputStream;     //!< Text stream for input file.
    QtlSubStationAlphaParser _inputParser;     //!< SSA/ASS parser.
    QtlSubRipGenerator       _outputFile;      //!< Output file.
    QString                  _outputFileName;  //!< Output file name.
    bool                     _useHtml;         //!< Add HTML tags in SRT subtitles.
    int                      _timerId;         //!< Repetitive timer.
    qint64                   _totalSize;       //!< Input file size.
    qint64                   _reportInterval;  //!< Min number of bytes between two progress reports.
    qint64                   _nextReport;      //!< Next byte index to indicate progress report.

    // Unaccessible operations.
    QtlMovieConvertSubStationAlpha() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieConvertSubStationAlpha)
};

#endif // QTLMOVIECONVERTSUBSTATIONALPHA_H
