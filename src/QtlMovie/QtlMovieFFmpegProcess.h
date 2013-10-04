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
//! @file QtlMovieFFmpegProcess.h
//!
//! Declare the class QtlMovieFFmpegProcess.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEFFMPEGPROCESS_H
#define QTLMOVIEFFMPEGPROCESS_H

#include <QtCore>
#include "QtlLogger.h"
#include "QtlMovieProcess.h"

//!
//! An execution of FFmpeg.
//!
//! Availability of standard output data is signaled by readyReadOutputData().
//! Standard output data can be read from outputDevice().
//!
//! Standard error lines, except "progress report" ones, are logged.
//! All "progress report" lines (starting with "frame=") are analyzed.
//! The progress report is signaled by progress().
//!
class QtlMovieFFmpegProcess : public QtlMovieProcess
{
#if !defined (DOXYGEN)
    Q_OBJECT
#endif

public:
    //!
    //! Constructor.
    //! @param [in] ffmpegArguments FFmpeg command line arguments.
    //! @param [in] inputDurationInSeconds Input file play duration in seconds
    //! or zero if unknown. Used as a hint to indicate progress.
    //! @param [in] temporaryDirectory Directory of temporary files.
    //! @param [in] settings Application settings.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieFFmpegProcess(const QStringList& ffmpegArguments,
                          int inputDurationInSeconds,
                          const QString& temporaryDirectory,
                          const QtlMovieSettings* settings,
                          QtlLogger* log,
                          QObject *parent = 0);

protected:
    //!
    //! Process one text line from standard error.
    //! @param [in] channel Origin of the line (QProcess::StandardOutput or QProcess::StandardError).
    //! @param [in] line Text line.
    //!
    virtual void processOutputLine(QProcess::ProcessChannel channel, const QString& line);

    //!
    //! Update the QProcessEnvironment for the target process before it starts.
    //! @param [in] env A set of environment variables to update.
    //!
    virtual void updateEnvironment(QProcessEnvironment& env);

private:
    int     _inputDurationInSeconds; //!< Input file duration.
    QString _tempDir;                //!< Directory of temporary files.

    //!
    //! Create the temporary fontconfig configuration file.
    //! @param [in] templateFile Absolute path of template file.
    //! @param [in] actualFile Absolute path of actual file to create.
    //! @return True on success, false on error.
    //!
    bool createFontConfig(const QString& templateFile, const QString& actualFile);

    // Unaccessible operations.
    QtlMovieFFmpegProcess() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieFFmpegProcess)
};

#endif // QTLMOVIEFFMPEGPROCESS_H
