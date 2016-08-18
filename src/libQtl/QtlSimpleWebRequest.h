//----------------------------------------------------------------------------
//
// Copyright (c) 2015, Thierry Lelegard
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
//! @file: QtlSimpleWebRequest.h
//!
//! Declare the class QtlSimpleWebRequest.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLSIMPLEWEBREQUEST_H
#define QTLSIMPLEWEBREQUEST_H

#include "QtlCore.h"
#include <QtNetwork>
#include "QtlNullLogger.h"

//!
//! A class which performs one simple HTTP request, expecting a text response
//! (HTML, XML, whatever, but not binary data).
//!
//! Automatically handle proxies and HTTP redirections.
//!
class QtlSimpleWebRequest : public QObject
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //!
    //! @param [in] url URL to get. If unspecified, you must call setUrl() before start().
    //! @param [in] parent Optional parent object.
    //! @param [in] log Optional message logger.
    //! @param [in] maxResponseSize Maximum size in bytes of response
    //! (to avoid memory saturation). Default: 1 MB.
    //!
    QtlSimpleWebRequest(const QString& url = QString(), QObject* parent = 0, QtlLogger* log = 0, int maxResponseSize = 1000000);

    //!
    //! Destructor.
    //!
    virtual ~QtlSimpleWebRequest();

    //!
    //! Set the URL of the request.
    //! Must be called before start(). Ignored otherwise.
    //! @param [in] url URL of the request.
    //!
    void setUrl(const QString& url);

    //!
    //! Get the URL of the request.
    //! @return URL of the request.
    //!
    QUrl url() const
    {
        return _url;
    }

    //!
    //! Add a cookie for the request.
    //! Must be called before start(). Ignored otherwise.
    //! @param [in] name Cookie name.
    //! @param [in] value Cookie value.
    //!
    void addCookie(const QString& name, const QString& value);

    //!
    //! Start the Web request.
    //! The completion will be notified later using the completed() signal.
    //! Can be called only once. Subsequent calls are ignored.
    //!
    void start();

    //!
    //! Check if the request was already started.
    //! @return True if the request was already started, false otherwise.
    //!
    bool isStarted() const
    {
        return _started;
    }

    //!
    //! Check if the request is completed.
    //! @return True if the request is completed, false otherwise.
    //!
    bool isCompleted() const
    {
        return _completed;
    }

signals:
    //!
    //! Emitted when the request is completed.
    //! @param [in] success True on success, false on error (network error for instance).
    //! @param [in] url URL of the returned content. May not be the same as the original
    //! URL is some redirection occured.
    //! @param [in] response If @a success is true, contains the text of the response.
    //! If @a status is false, contains an error message.
    //! @param [in] mimeType If @a success is true, contains the MIME type of the response.
    //! Examples: "text/html", "text/plain", "application/json".
    //!
    void completed(bool success, const QUrl& url, const QString& response, const QString& mimeType);

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
    QtlNullLogger         _nullLog;         //!< Null logger.
    QtlLogger*            _log;             //!< Message logger.
    bool                  _started;         //!< The request is started.
    bool                  _completed;       //!< Signal completed() was emitted.
    QUrl                  _url;             //!< URL to get.
    QList<QNetworkCookie> _cookies;         //!< List of cookies to get.
    const int             _maxResponseSize; //!< Maximum size in bytes of response.
    QNetworkAccessManager _netwManager;     //!< Network manager for all requests.
    QNetworkReply*        _reply;           //!< HTTP reply.
    QString               _text;            //!< Returned text.
    QString               _contentType;     //!< MIME content type.
    int                   _redirected;      //!< Number of redirections.

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
    QtlSimpleWebRequest() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlSimpleWebRequest)
};

#endif // QTLSIMPLEWEBREQUEST_H
