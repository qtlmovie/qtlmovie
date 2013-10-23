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
//! @file: QtlNewVersionChecker.h
//!
//! Declare the class QtlNewVersionChecker.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLNEWVERSIONCHECKER_H
#define QTLNEWVERSIONCHECKER_H

#include <QtCore>
#include <QtNetwork>
#include "QtlNullLogger.h"
#include "QtlVersion.h"

//!
//! A class to check the existence of a new version of the application on the network.
//!
class QtlNewVersionChecker : public QObject
{
#if !defined (DOXYGEN)
    Q_OBJECT
#endif

public:
    //!
    //! Create and start a new instance.
    //! Enforce object creation on the heap.
    //! The object deletes itself upon processing completion.
    //! @param [in] currentVersion Current version of the application.
    //! @param [in] directoryUrl URL of directory containing update packages.
    //! @param [in] filePrefix Package file name prefix.
    //! @param [in] fileSuffix Package file name suffix.
    //! @param [in] urlSuffix Suffix to add to package files URL.
    //! @param [in] silent If true, do not report the errors or absence of new version.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    static QtlNewVersionChecker* newInstance(const QtlVersion& currentVersion,
                                             const QString& directoryUrl,
                                             const QString& filePrefix,
                                             const QString& fileSuffix,
                                             const QString& urlSuffix = "",
                                             bool silent = false,
                                             QtlLogger* log = 0,
                                             QObject *parent = 0);

    //!
    //! Destructor.
    //!
    virtual ~QtlNewVersionChecker();

signals:
    //!
    //! Emitted when the search is completed.
    //! @param [in] success True on success, false on error (network error for instance).
    //! @param [in] latestVersion If @a success is true, contains the latest available version.
    //! @param [in] latestUrl If @a success is true, contains URL of the latest available version.
    //!
    void completed(bool success, const QtlVersion& latestVersion, const QUrl& lastestUrl);

private slots:
    //!
    //! Invoked when data is available from HTTP.
    //!
    void httpReadyRead();
    //!
    //! Invoked when HTTP request completes.
    //!
    void httpFinished();

private:
    bool                  _silent;         //!< Do not report the errors or absence of new version.
    QtlNullLogger         _nullLog;        //!< Null logger.
    QtlLogger*            _log;            //!< Message logger.
    bool                  _completed;      //!< Signal completed() was emitted.
    QtlVersion            _currentVersion; //!< Current version of the application.
    QString               _directoryUrl;   //!< URL of directory containing update packages.
    QString               _filePrefix;     //!< Package file name prefix.
    QString               _fileSuffix;     //!< Package file name suffix.
    QString               _urlSuffix;      //!< Suffix to add to package files URL.
    QNetworkAccessManager _netwManager;    //!< Network manager for all requests.
    QUrl                  _currentUrl;     //!< Current URL.
    QNetworkReply*        _reply;          //!< HTTP reply.
    QString               _text;           //!< Returned text.
    int                   _redirected;     //!< Number of redirections.
    QtlVersion            _latestVersion;  //!< Latest version found.
    QUrl                  _latestUrl;      //!< URL of latest version.

    //!
    //! Private constructor.
    //! Avoid the creator of static or local instances.
    //! The object deletes itself upon processing completion.
    //! @param [in] currentVersion Current version of the application.
    //! @param [in] directoryUrl URL of directory containing update packages.
    //! @param [in] filePrefix Package file name prefix.
    //! @param [in] fileSuffix Package file name suffix.
    //! @param [in] urlSuffix Suffix to add to package files URL.
    //! @param [in] silent If true, do not report the errors or absence of new version.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    QtlNewVersionChecker(const QtlVersion& currentVersion,
                         const QString& directoryUrl,
                         const QString& filePrefix,
                         const QString& fileSuffix,
                         const QString& urlSuffix = "",
                         bool silent = false,
                         QtlLogger* log = 0,
                         QObject *parent = 0);

    //!
    //! Start an HTTP request.
    //! Used in original request and all redirections.
    //! @param [in] url URL to get.
    //!
    void startRequest(const QUrl& url);

    //!
    //! Emit the completed() signal and delete this instance.
    //! @param [in] error Error message, empty on success.
    //!
    void emitCompleted(const QString& error);

    // Unaccessible operations.
    QtlNewVersionChecker() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlNewVersionChecker)
};

#endif // QTLNEWVERSIONCHECKER_H
