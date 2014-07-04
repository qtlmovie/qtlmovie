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
// Destructor.
//----------------------------------------------------------------------------

QtlNewVersionChecker::~QtlNewVersionChecker()
{
    // Delete the reply object.
    if (_reply != 0) {
        delete _reply;
        _reply = 0;
    }
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
    _completed(false),
    _currentVersion(currentVersion),
    _directoryUrl(directoryUrl),
    _filePrefix(filePrefix),
    _fileSuffix(fileSuffix),
    _urlSuffix(urlSuffix),
    _netwManager(),
    _currentUrl(),
    _reply(0),
    _text(),
    _redirected(0)
{
    // Start getting the content of the directory.
    startRequest(QUrl(_directoryUrl));
}


//----------------------------------------------------------------------------
// Start an HTTP request.
//----------------------------------------------------------------------------

void QtlNewVersionChecker::startRequest(const QUrl& url)
{
    // Keep track of current URL.
    _currentUrl = url;

    // If a list of proxies is defined in the system, use the first one.
    const QList<QNetworkProxy> proxies(QNetworkProxyFactory::systemProxyForQuery(QNetworkProxyQuery(url)));
    if (!proxies.empty()) {
        const QNetworkProxy& proxy(proxies.first());
        _log->debug(tr("Found %1 proxies, using proxy %2:%3").arg(proxies.size()).arg(proxy.hostName()).arg(proxy.port()));
        _netwManager.setProxy(proxy);
    }

    // If a previous reply was in progress, delete it asap.
    if (_reply != 0) {
        _reply->deleteLater();
    }

    // Build the HTTP request.
    // By default, QNetworkRequest sets User-Agent to "Mozilla/5.0".
    // For some unknown reason, this is a problem for some proxy servers which end up in "504 Gateway Timeout".
    // Simply setting any other User-Agent fixes the problem.
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Qtl");

    // Send the HTTP request, read the headers.
    _log->debug(tr("Searching new versions in %1").arg(url.toString()));
    _reply = _netwManager.get(request);
    if (_reply == 0) {
        // Cannot send the request.
        emitCompleted(tr("Error browsing %1").arg(url.toString()));
    }
    else {
        // Get notifications from the reply.
        connect(_reply, &QNetworkReply::readyRead, this, &QtlNewVersionChecker::httpReadyRead);
        connect(_reply, &QNetworkReply::finished,  this, &QtlNewVersionChecker::httpFinished);
    }
}


//----------------------------------------------------------------------------
// Emit the completed() signal and delete this instance.
//----------------------------------------------------------------------------

void QtlNewVersionChecker::emitCompleted(const QString& error)
{
    // Filter spurious invocations.
    if (_completed) {
        return;
    }

    // Do it only once.
    _completed = true;

    // Display error.
    if (!error.isEmpty()) {
        if (_silent) {
            _log->debug(error);
        }
        else {
            qtlError(this, error);
        }
    }

    // Emit the signal for clients.
    emit completed(error.isEmpty() && _latestVersion.isValid(), _latestVersion, _latestUrl);

    // If a reply was in progress, delete it asap.
    if (_reply != 0) {
        _reply->deleteLater();
        _reply = 0;
    }

    // Delete this instance when back in the event loop.
    deleteLater();
}


//----------------------------------------------------------------------------
// Invoked when data is available from HTTP.
//----------------------------------------------------------------------------

void QtlNewVersionChecker::httpReadyRead()
{
    // Filter spurious invocations.
    if (_reply == 0) {
        return;
    }

    // Append all data in internal text buffer.
    _text.append(_reply->readAll());

    // The link we search are {prefix}{version}{suffix}.
    const QString prefix(_directoryUrl + _filePrefix);
    const QString suffix(_fileSuffix + _urlSuffix);

    // Loop on all hyperlinks in the buffer. There is no real HTML parsing.
    int ref;
    while ((ref = _text.indexOf(QRegExp("<a +href=\""))) >= 0) {

        // Locate beginning and end of hyperlink.
        const int start = _text.indexOf("\"", ref);
        const int end = start < 0 ? -1 : _text.indexOf("\"", start + 1);

        // If not yet found, we need to read more.
        if (end < 0) {
            return;
        }

        // Extract the hyperlink from the buffer.
        QString link(_text.mid(start + 1, end - start - 1));

        // Delete beginning of the buffer, up to and including the extracted link.
        _text.remove(0, end + 1);

        // Resolve the linked URL relatively to current URL.
        const QUrl url(_currentUrl.resolved(QUrl(link)));
        link = url.toString();

        // Check if the resolved link matches what we search.
        if (link.startsWith(prefix) && link.endsWith(suffix)) {
            // Extract the version, ie the section between prefix and suffix.
            const QtlVersion version(link.mid(prefix.size(), link.size() - prefix.size() - suffix.size()));
            if (version.isValid()) {
                _log->debug(tr("Found version %1 at %2").arg(version.text()).arg(link));
                // Keep latest version.
                if (!_latestVersion.isValid() || version > _latestVersion) {
                    _latestVersion = version;
                    _latestUrl = url;
                }
            }
        }
    }
}


//----------------------------------------------------------------------------
// Invoked when HTTP request completes.
//----------------------------------------------------------------------------

void QtlNewVersionChecker::httpFinished()
{
    // Filter spurious invocations.
    if (_reply == 0) {
        return;
    }

    // Get error and optional redirection.
    const QNetworkReply::NetworkError error = _reply->error();
    const QUrl redirection(_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());

    if (error != QNetworkReply::NoError) {
        // Network error.
        emitCompleted(tr("Download failed: %1").arg(_reply->errorString()));
    }
    else if (redirection.isValid()) {
        // Redirect to a new URL.
        if (++_redirected > QTL_MAX_REDIRECT) {
            // Too many redirections, DoS attack?
            emitCompleted(tr("Too many redirections on %1").arg(_currentUrl.toString()));
        }
        else {
            // Start a new request on the redirected URL.
            startRequest(_currentUrl.resolved(redirection));
        }
    }
    else {
        // End of search.
        _log->debug(tr("End of search in %1").arg(_currentUrl.toString()));
        const QString app(qtlApplicationName());

        if (_latestVersion.isValid() && _latestVersion > _currentVersion) {
            // New version found.
            QMessageBox::information(qtlWidgetAncestor(this),
                                     app,
                                     tr("A new version of %1 is available<br/>Your version is %2<br/>Version %3 is available online at:<br/>%4")
                                     .arg(app)
                                     .arg(_currentVersion.text())
                                     .arg(_latestVersion.text())
                                     .arg(qtlHtmlLink(_latestUrl.toString())));
        }
        else if (_silent) {
            // No new version, silent mode.
            _log->debug(tr("No new version found, this: %1, latest: %2").arg(_currentVersion.text()).arg(_latestVersion.text()));
        }
        else if (!_latestVersion.isValid()){
            QMessageBox::information(qtlWidgetAncestor(this), app, tr("Unable to find any version of %1 online").arg(app));
        }
        else if (_latestVersion == _currentVersion){
            QMessageBox::information(qtlWidgetAncestor(this), app, tr("You are using the latest version of %1").arg(app));
        }
        else {
            QMessageBox::information(qtlWidgetAncestor(this),
                                     app,
                                     tr("Your version of %1 is %2<br/>More recent than %3, the latest available online")
                                     .arg(app)
                                     .arg(_currentVersion.text())
                                     .arg(_latestVersion.text()));
        }

        // End of processing.
        emitCompleted("");
    }
}
