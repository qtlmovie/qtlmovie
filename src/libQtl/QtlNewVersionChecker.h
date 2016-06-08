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

#include "QtlCore.h"
#include "QtlNullLogger.h"
#include "QtlSimpleWebRequest.h"
#include "QtlVersion.h"

//!
//! A class to check the existence of a new version of the application on the network.
//!
//! An instance of this class get the content of a Web page and parses the returned
//! HTML content, searching for a link to a file matching a new version of the application.
//!
class QtlNewVersionChecker : public QObject
{
    Q_OBJECT

public:
    //!
    //! Create and start a new instance.
    //! Enforce object creation on the heap.
    //! The object deletes itself upon processing completion.
    //!
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
                                             QObject* parent = 0);

signals:
    //!
    //! Emitted when the search is completed.
    //! @param [in] success True on success, false on error (network error for instance).
    //! @param [in] latestVersion If @a success is true, contains the latest available version.
    //! @param [in] latestUrl If @a success is true, contains URL of the latest available version.
    //!
    void completed(bool success, const QtlVersion& latestVersion, const QUrl& latestUrl);

private slots:
    //!
    //! Invoked when the Web request is completed.
    //! @param [in] success True on success, false on error (network error for instance).
    //! @param [in] url URL of the returned content. May not be the same as the original
    //! URL is some redirection occured.
    //! @param [in] response If @a success is true, contains the text of the response.
    //! If @a status is false, contains an error message.
    //!
    void httpCompleted(bool success, const QUrl& url, const QString& response);

private:
    bool                  _silent;         //!< Do not report the errors or absence of new version.
    QtlNullLogger         _nullLog;        //!< Null logger.
    QtlLogger*            _log;            //!< Message logger.
    QtlVersion            _currentVersion; //!< Current version of the application.
    QString               _directoryUrl;   //!< URL of directory containing update packages.
    QString               _filePrefix;     //!< Package file name prefix.
    QString               _fileSuffix;     //!< Package file name suffix.
    QString               _urlSuffix;      //!< Suffix to add to package files URL.
    QtlSimpleWebRequest   _request;        //!< Web request.

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

    // Unaccessible operations.
    QtlNewVersionChecker() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlNewVersionChecker)
};

#endif // QTLNEWVERSIONCHECKER_H
