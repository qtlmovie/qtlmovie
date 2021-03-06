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
//! @file QtlMovieClosedCaptionsSearch.h
//!
//! Declare the class QtlMovieClosedCaptionsSearch.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIECLOSEDCAPTIONSSEARCH_H
#define QTLMOVIECLOSEDCAPTIONSSEARCH_H

#include <QtCore>
#include "QtlMovieProcess.h"
#include "QtlMovieSettings.h"
#include "QtlMediaStreamInfo.h"

//!
//! This file scans a multimedia file and searches Closed Captions streams.
//!
class QtlMovieClosedCaptionsSearch : public QtlMovieProcess
{
    Q_OBJECT

public:
    //!
    //! Create a new instance.
    //! Enforce object creation on the heap.
    //! @param [in] fileName Input file name.
    //! @param [in] settings Application settings.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //!
    static QtlMovieClosedCaptionsSearch* newInstance(const QString& fileName,
                                                     const QtlMovieSettings* settings,
                                                     QtlLogger* log,
                                                     QObject *parent = 0);

signals:
    //!
    //! Emitted when a Closed Captions stream is found.
    //! @param [in] stream A smart pointer to the stream info data.
    //!
    void foundClosedCaptions(QtlMediaStreamInfoPtr stream);

protected:
    //!
    //! Process one text line from standard error or standard output.
    //! Reimplemented from QtlMovieProcess.
    //! @param [in] channel Origin of the line (QProcess::StandardOutput or QProcess::StandardError).
    //! @param [in] textLine Text line.
    //!
    virtual void processOutputLine(QProcess::ProcessChannel channel, const QString& textLine) Q_DECL_OVERRIDE;

    //!
    //! Emit the completed() signal.
    //! Reimplemented from QtlMovieProcess.
    //! @param [in] success True when the action completed successfully, false otherwise.
    //! @param [in] message Optional error message to log.
    //!
    virtual void emitCompleted(bool success, const QString& message = QString()) Q_DECL_OVERRIDE;

private:
    QString _fileName;  //!< Input file name.

    //!
    //! Private constructor.
    //! @param [in] fileName Input file name.
    //! @param [in] settings Application settings.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //!
    QtlMovieClosedCaptionsSearch(const QString& fileName,
                                 const QtlMovieSettings* settings,
                                 QtlLogger* log,
                                 QObject *parent = 0);

    //!
    //! Emit the foundClosedCaptions() signal.
    //! @param ccNumber CC number (see QtlMediaStreamInfo::ccNumber()).
    //!
    void emitFoundClosedCaptions(int ccNumber);

    // Unaccessible operations.
    QtlMovieClosedCaptionsSearch() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieClosedCaptionsSearch)
};

#endif // QTLMOVIECLOSEDCAPTIONSSEARCH_H
