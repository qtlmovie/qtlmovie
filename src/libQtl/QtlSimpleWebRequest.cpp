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
//
// Qtl, Qt utility library.
// Define the class QtlSimpleWebRequest.
//
//----------------------------------------------------------------------------

#include "QtlSimpleWebRequest.h"
#include "QtlStringUtils.h"


//!
//! Maximum number of HTTP redirects (to avoid infinite loops).
//!
#define QTL_MAX_REDIRECT 10


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlSimpleWebRequest::QtlSimpleWebRequest(const QString& url, QObject* parent, QtlLogger* log, int maxResponseSize) :
    QObject(parent),
    _nullLog(),
    _log(log == 0 ? &_nullLog : log), // enforce a non-null logger
    _started(false),
    _completed(false),
    _url(url),
    _cookies(),
    _maxResponseSize(maxResponseSize),
    _netwManager(),
    _reply(0),
    _text(),
    _contentType(),
    _redirected(0)
{
}


//----------------------------------------------------------------------------
// Destructor.
//----------------------------------------------------------------------------

QtlSimpleWebRequest::~QtlSimpleWebRequest()
{
    // Delete the reply object.
    if (_reply != 0) {
        delete _reply;
        _reply = 0;
    }
}


//----------------------------------------------------------------------------
// Set the URL of the request.
//----------------------------------------------------------------------------

void QtlSimpleWebRequest::setUrl(const QString& url)
{
    if (!_started) {
        _url = QUrl(url);
    }
}


//----------------------------------------------------------------------------
// Add a cookie for the request.
//----------------------------------------------------------------------------

void QtlSimpleWebRequest::addCookie(const QString& name, const QString& value)
{
    if (!_started) {
        _cookies << QNetworkCookie(name.toUtf8(), value.toUtf8());
    }
}


//----------------------------------------------------------------------------
// Start the Web request.
//----------------------------------------------------------------------------

void QtlSimpleWebRequest::start()
{
    if (!_started) {
        _started = true;
        startRequest(_url);
    }
}


//----------------------------------------------------------------------------
// Start an HTTP request.
//----------------------------------------------------------------------------

void QtlSimpleWebRequest::startRequest(const QUrl& url)
{
    // Keep track of current URL.
    _url = url;

    // Reset previous content (in case of redirection).
    _text.clear();

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
        _reply = 0;
    }

    // Build the HTTP request.
    // By default, QNetworkRequest sets User-Agent to "Mozilla/5.0".
    // For some unknown reason, this is a problem for some proxy servers which end up in "504 Gateway Timeout".
    // Simply setting any other User-Agent fixes the problem.
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Qtl");

    // Add the list of cookies. First modify the domain name of the cookie to
    // match the current URL, then add the cookies.
    if (!_cookies.empty()) {
        for (QList<QNetworkCookie>::iterator it = _cookies.begin(); it != _cookies.end(); ++it) {
            it->setDomain(url.host());
        }
        request.setHeader(QNetworkRequest::CookieHeader, QVariant::fromValue(_cookies));
    }

    // Send the HTTP request, read the headers.
    _log->debug(tr("Fetching %1").arg(url.toString()));
    _reply = _netwManager.get(request);
    if (_reply == 0) {
        // Cannot send the request.
        emitCompleted(tr("Error browsing %1").arg(url.toString()));
    }
    else {
        // Get notifications from the reply.
        connect(_reply, &QNetworkReply::readyRead, this, &QtlSimpleWebRequest::httpReadyRead);
        connect(_reply, &QNetworkReply::finished,  this, &QtlSimpleWebRequest::httpFinished);
    }
}


//----------------------------------------------------------------------------
// Emit the completed() signal and delete this instance.
//----------------------------------------------------------------------------

void QtlSimpleWebRequest::emitCompleted(const QString& error)
{
    // Do it only once.
    if (!_completed) {
        _completed = true;

        // Emit the signal for clients.
        const bool success = error.isEmpty();
        if (success) {
            emit completed(success, _url, _text, _contentType);
        }
        else {
            emit completed(success, _url, error, QString());
        }

        // If a reply was in progress, delete it asap.
        if (_reply != 0) {
            _reply->deleteLater();
            _reply = 0;
        }
    }
}


//----------------------------------------------------------------------------
// Invoked when data is available from HTTP.
//----------------------------------------------------------------------------

void QtlSimpleWebRequest::httpReadyRead()
{
    if (_reply != 0) {
        // Append all data in internal text buffer.
        _text.append(_reply->readAll());

        // Limit the request size to avoid saturation.
        if (_text.size() > _maxResponseSize) {
            disconnect(_reply, 0, this, 0);
            _reply->abort();
            emitCompleted(tr("Maximum response size exceeded from %1").arg(_url.toString()));
        }
    }
}


//----------------------------------------------------------------------------
// Invoked when HTTP request completes.
//----------------------------------------------------------------------------

void QtlSimpleWebRequest::httpFinished()
{
    // Filter spurious invocations.
    if (_reply == 0) {
        return;
    }

    // Get error and optional redirection.
    const QNetworkReply::NetworkError error = _reply->error();
    const QUrl redirection(_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());

    // Get response mime type.
    _contentType = _reply->header(QNetworkRequest::ContentTypeHeader).toString();

    // Keep only the actual mime type.
    _contentType.remove(QRegExp(";.*"));
    _contentType = _contentType.trimmed();

    if (error != QNetworkReply::NoError) {
        // Network error.
        emitCompleted(tr("Download failed: %1").arg(_reply->errorString()));
    }
    else if (redirection.isValid()) {
        // Redirect to a new URL.
        if (++_redirected > QTL_MAX_REDIRECT) {
            // Too many redirections, DoS attack?
            emitCompleted(tr("Too many redirections on %1").arg(_url.toString()));
        }
        else {
            // Start a new request on the redirected URL.
            startRequest(_url.resolved(redirection));
        }
    }
    else {
        // End of search.
        _log->debug(tr("End of download of %1, content type: %2").arg(_url.toString()).arg(_contentType));
        emitCompleted("");
    }
}
