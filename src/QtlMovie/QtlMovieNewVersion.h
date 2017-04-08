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
//! @file: QtlMovieNewVersion.h
//!
//! Declare the class QtlMovieNewVersion.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIENEWVERSION_H
#define QTLMOVIENEWVERSION_H

#include "QtlLogger.h"
#include "QtlVersion.h"
#include "QtlMovieSettings.h"
#include "QtlSimpleWebRequest.h"

//!
//! A class to check the existence of a new version of the application on the network.
//!
//! This class requires the cooperation of a server. An instance of this class sends a
//! Web request to a server using a cookie describing the current platform (a Base64-encoded
//! JSON structure). The server is responsible for finding a new version for this platform,
//! if available, and returns the response as a JSON structure.
//!
//! For debug purpose, the environment variable QTLMOVIE_NEWVERSION_URL can be set to an
//! alternate server. If this URL is defined, it overrides any application-defined URL.
//!
class QtlMovieNewVersion : public QObject
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! The search for a new version is automatically started.
    //! @param [in] silent If true, do not report the errors or absence of new version.
    //! @param [in] autoDelete If true, this object deletes itself after completion of all requests.
    //! @param [in] settings Application settings.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieNewVersion(bool silent,
                       bool autoDelete,
                       QtlMovieSettings* settings,
                       QtlLogger* log,
                       QObject* parent = 0);

private slots:
    //!
    //! Invoked when the Web request is completed.
    //! @param [in] success True on success, false on error (network error for instance).
    //! @param [in] url URL of the returned content. May not be the same as the original
    //! URL is some redirection occured.
    //! @param [in] response If @a success is true, contains the text of the response.
    //! If @a status is false, contains an error message.
    //! @param [in] mimeType If @a success is true, contains the MIME type of the response.
    //!
    void httpCompleted(bool success, const QUrl& url, const QString& response, const QString& mimeType);

    //!
    //! Invoked when the Web request for release notes is completed.
    //! @param [in] success True on success, false on error (network error for instance).
    //! @param [in] url URL of the returned content. May not be the same as the original
    //! URL is some redirection occured.
    //! @param [in] response If @a success is true, contains the text of the response.
    //! If @a status is false, contains an error message.
    //! @param [in] mimeType If @a success is true, contains the MIME type of the response.
    //!
    void releaseNotesCompleted(bool success, const QUrl& url, const QString& response, const QString& mimeType);

    //!
    //! Invoked when the user wants to download the last version.
    //!
    void downloadLatestVersion();

private:
    QtlMovieSettings*   _settings;             //!< Application settings.
    QtlLogger*          _log;                  //!< Message logger.
    bool                _silent;               //!< Do not report the errors or absence of new version.
    bool                _autoDelete;           //!< Delete itself after completion of all requests.
    QtlVersion          _currentVersion;       //!< Current version of the application.
    QtlSimpleWebRequest _newVersionRequest;    //!< Web request for new version.
    QtlSimpleWebRequest _releaseNotesRequest;  //!< Web request for the content of the release notes.
    QtlVersion          _latestVersion;        //!< Latest available version of the application.
    QString             _latestUrl;            //!< Download URL of the latest available version.
    bool                _newerVersion;         //!< Newer version found.
    QString             _statusMessage;        //!< Message to display about the new version.
    QString             _releaseNotes;         //!< Text of the release notes.

    //!
    //! Terminate the operation with error.
    //! @param [in] message Error message.
    //!
    void fail(const QString& message);

    //!
    //! Terminate the operation with success.
    //!
    void succeed();

    //!
    //! Display the new version message with release notes.
    //!
    void displayReleaseNotes();

    // Unaccessible operations.
    QtlMovieNewVersion() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieNewVersion)
};

#endif // QTLMOVIENEWVERSION_H
