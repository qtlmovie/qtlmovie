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
//! @file QtlMovieExecFile.h
//!
//! Declare the QtlMovieExecFile class.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEEXECFILE_H
#define QTLMOVIEEXECFILE_H

#include "QtlFile.h"
#include "QtlLogger.h"
#include "QtlBoundProcessResult.h"

//!
//! Describe an executable file such as ffmpeg, ffprobe, etc.
//!
class QtlMovieExecFile : public QtlFile
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] name Common name (for instance "FFmpeg").
    //! @param [in] homePage Product home page URL. Empty if none available.
    //! @param [in] windowsBuilds Binary builds for Windows home page URL. Empty if none available.
    //! @param [in] fileName File name of executable (for instance "ffmpeg").
    //! Do not add system-specific extension such as ".exe".
    //! If no directory is specified, the file is searched in movieExecSearchPath().
    //! @param [in] versionOptions If non empty, these options can be used to get the
    //! version of the executable (for instance "-version").
    //! @param [in] versionStart If non-empty, used to locate the start of the version string
    //! into the output of execution with @a versionOptions. The text of @a versionStart is
    //! included into the version text.
    //! @param [in] versionEnd If non-empty, used to locate the end of the version string
    //! into the output of execution with @a versionOptions. The text of @a versionEnd is
    //! excluded from the version text.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieExecFile(const QString& name,
                     const QString& homePage,
                     const QString& windowsBuilds,
                     const QString& fileName,
                     const QStringList& versionOptions,
                     const QString& versionStart,
                     const QString& versionEnd,
                     QtlLogger* log,
                     QObject *parent = 0);

    //!
    //! Constructor.
    //! @param [in] other Other instance to get all fields from, except file name and parent.
    //! @param [in] fileName File name of executable (for instance "ffmpeg").
    //! Do not add system-specific extension such as ".exe".
    //! If no directory is specified, the file is searched in movieExecSearchPath().
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieExecFile(const QtlMovieExecFile& other,
                     const QString& fileName,
                     QObject *parent = 0);

    //!
    //! Get the associated error logger.
    //! @return The associated error logger.
    //!
    QtlLogger* log() const
    {
        return _log;
    }

    //!
    //! Get the commn name (for display).
    //! @return Common name.
    //!
    QString name() const
    {
        return _name;
    }

    //!
    //! Get the product home page URL.
    //! @return Product home page URL.
    //!
    QString homePage() const
    {
        return _homePage;
    }

    //!
    //! Get the binary builds for Windows home page URL.
    //! @return Binary builds for Windows home page URL.
    //!
    QString windowsBuilds() const
    {
        return _windowsBuilds;
    }

    //!
    //! Get the version string obtained from executable.
    //! @return Version string obtained from executable.
    //!
    QString version() const
    {
        return _version;
    }

    //!
    //! A description of the executable in HTML format.
    //! @return Description in HTML format.
    //!
    QString htmlDescription() const;

    //!
    //! Set the file name.
    //! Reimplemented from QtlFile.
    //! @param [in] fileName File path.
    //! @return True if the file name was modified, false if it was the same file name.
    //!
    virtual bool setFileName(const QString& fileName) Q_DECL_OVERRIDE;

    //!
    //! Build the search path of QtlMovie utilities.
    //! @return A list of directories to search.
    //!
    static QStringList movieExecSearchPath();

private slots:
    //!
    //! Invoked when the "get version" process completes.
    //! @param [in] result Process execution results.
    //!
    void getVersionTerminated(const QtlBoundProcessResult& result);

private:
    QtlLogger*  _log;            //!< Where to log errors.
    QString     _name;           //!< Common name.
    QString     _homePage;       //!< Product home page URL.
    QString     _windowsBuilds;  //!< Binary builds for Windows home page URL.
    QStringList _versionOptions; //!< Options to get version of executable.
    QString     _versionStart;   //!< Start marker for version text.
    QString     _versionEnd;     //!< End marker for version text.
    QString     _version;        //!< Version string obtained from executable.

    //!
    //! Start determining the executable version.
    //!
    void startGetVersion();

    //!
    //! Locate a marker into a string.
    //! @param [in] marker The marker to search, case insensitive.
    //! @param [in] str The string into which we search.
    //! @return -1 is @a marker is empty or not found in @a str.
    //! Otherwise, return the position of @a marker in @a str.
    //!
    static int markerPosition(const QString& marker, const QString& str);

    // Unaccessible operations.
    QtlMovieExecFile() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieExecFile)
};

#endif // QTLMOVIEEXECFILE_H
