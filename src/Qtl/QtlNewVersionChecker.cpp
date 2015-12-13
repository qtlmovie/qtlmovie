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
//
// Qtl, Qt utility library.
// Define the class QtlNewVersionChecker.
//
//----------------------------------------------------------------------------

#include "QtlNewVersionChecker.h"
#include "QtlMessageBoxUtils.h"
#include "QtlStringUtils.h"


//!
//! Maximum number of HTTP redirects (to avoid infinite loops).
//!
#define QTL_MAX_REDIRECT 10


//----------------------------------------------------------------------------
// Create and start a new instance. Enforce object creation on the heap.
//----------------------------------------------------------------------------

QtlNewVersionChecker* QtlNewVersionChecker::newInstance(const QtlVersion& currentVersion,
                                                        const QString& directoryUrl,
                                                        const QString& filePrefix,
                                                        const QString& fileSuffix,
                                                        const QString& urlSuffix,
                                                        bool silent,
                                                        QtlLogger* log,
                                                        QObject *parent)
{
    return new QtlNewVersionChecker(currentVersion, directoryUrl, filePrefix, fileSuffix, urlSuffix, silent, log, parent);
}


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlNewVersionChecker::QtlNewVersionChecker(const QtlVersion& currentVersion,
                                           const QString& directoryUrl,
                                           const QString& filePrefix,
                                           const QString& fileSuffix,
                                           const QString& urlSuffix,
                                           bool silent,
                                           QtlLogger* log,
                                           QObject* parent) :
    QObject(parent),
    _silent(silent),
    _nullLog(),
    _log(log == 0 ? &_nullLog : log), // enforce a non-null logger
    _currentVersion(currentVersion),
    _directoryUrl(directoryUrl),
    _filePrefix(filePrefix),
    _fileSuffix(fileSuffix),
    _urlSuffix(urlSuffix),
    _request(_directoryUrl, 0, log)
{
    // Start getting the content of the directory.
    connect(&_request, &QtlSimpleWebRequest::completed, this, &QtlNewVersionChecker::httpCompleted);
    _request.start();
}


//----------------------------------------------------------------------------
// Invoked when the Web request is completed.
//----------------------------------------------------------------------------

void QtlNewVersionChecker::httpCompleted(bool success, const QUrl& url, const QString& response)
{
    if (success) {
        // Succesful Web request

        // The link we search are {prefix}{version}{suffix}.
        const QString prefix(_directoryUrl + _filePrefix);
        const QString suffix(_fileSuffix + _urlSuffix);

        // Description of latest version.
        QtlVersion latestVersion;
        QUrl latestUrl;

        // Loop on all hyperlinks in the buffer. There is no real HTML parsing.
        int ref;
        int start = 0;
        while (start < response.length() && (ref = response.indexOf(QRegExp("<a +href=\""), start)) >= 0) {

            // Locate beginning and end of hyperlink.
            start = response.indexOf("\"", ref);
            const int end = start < 0 ? -1 : response.indexOf("\"", start + 1);

            // If no final '"' found, invalid reference, end of search.
            if (end < 0) {
                break;
            }

            // Extract the hyperlink from the buffer.
            QString link(response.mid(start + 1, end - start - 1));

            // Will search next reference after this one.
            start = end + 1;

            // Resolve the linked URL relatively to current URL.
            const QUrl linkUrl(url.resolved(QUrl(link)));
            link = linkUrl.toString();

            // Check if the resolved link matches what we search.
            if (link.startsWith(prefix) && link.endsWith(suffix)) {
                // Extract the version, ie the section between prefix and suffix.
                const QtlVersion version(link.mid(prefix.size(), link.size() - prefix.size() - suffix.size()));
                if (version.isValid()) {
                    _log->debug(tr("Found version %1 at %2").arg(version.text()).arg(link));
                    // Keep latest version.
                    if (!latestVersion.isValid() || version > latestVersion) {
                        latestVersion = version;
                        latestUrl = linkUrl;
                    }
                }
            }
        }

        // End of search.
        _log->debug(tr("End of search in %1").arg(url.toString()));
        const QString app(qtlApplicationName());

        if (latestVersion.isValid() && latestVersion > _currentVersion) {
            // New version found.
            QMessageBox::information(qtlWidgetAncestor(this),
                                     app,
                                     tr("A new version of %1 is available<br/>Your version is %2<br/>Version %3 is available online at:<br/>%4")
                                     .arg(app)
                                     .arg(_currentVersion.text())
                                     .arg(latestVersion.text())
                                     .arg(qtlHtmlLink(latestUrl.toString())));
        }
        else if (_silent) {
            // No new version, silent mode.
            _log->debug(tr("No new version found, this: %1, latest: %2").arg(_currentVersion.text()).arg(latestVersion.text()));
        }
        else if (!latestVersion.isValid()){
            QMessageBox::information(qtlWidgetAncestor(this), app, tr("Unable to find any version of %1 online").arg(app));
        }
        else if (latestVersion == _currentVersion){
            QMessageBox::information(qtlWidgetAncestor(this), app, tr("You are using the latest version of %1").arg(app));
        }
        else {
            QMessageBox::information(qtlWidgetAncestor(this),
                                     app,
                                     tr("Your version of %1 is %2<br/>More recent than %3, the latest available online")
                                     .arg(app)
                                     .arg(_currentVersion.text())
                                     .arg(latestVersion.text()));
        }

        // Emit the signal for clients.
        emit completed(latestVersion.isValid(), latestVersion, latestUrl);
    }
    else {
        // Error while retrieving Web content. Parameter "response" is an error message.
        if (_silent) {
            _log->debug(response);
        }
        else {
            qtlError(this, response);
        }

        // Emit the signal for clients.
        emit completed(false, QtlVersion(), QUrl());
    }

    // Delete this instance when back in the event loop.
    deleteLater();
}
