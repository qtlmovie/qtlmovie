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
//
// Qtl, Qt utility library.
// Define the class QtlNewVersionCheckerJson.
//
//----------------------------------------------------------------------------

#include "QtlNewVersionCheckerJson.h"
#include "QtlMessageBoxUtils.h"
#include "QtlStringUtils.h"


//----------------------------------------------------------------------------
// Create and start a new instance. Enforce object creation on the heap.
//----------------------------------------------------------------------------

QtlNewVersionCheckerJson* QtlNewVersionCheckerJson::newInstance(const QtlVersion& currentVersion,
                                                                const QString& url,
                                                                bool silent,
                                                                QtlLogger* log,
                                                                QObject* parent)
{
    return new QtlNewVersionCheckerJson(currentVersion, url, silent, log, parent);
}


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlNewVersionCheckerJson::QtlNewVersionCheckerJson(const QtlVersion& currentVersion,
                                                   const QString& url,
                                                   bool silent,
                                                   QtlLogger* log,
                                                   QObject* parent) :
    QObject(parent),
    _silent(silent),
    _nullLog(),
    _log(log == 0 ? &_nullLog : log), // enforce a non-null logger
    _currentVersion(currentVersion),
    _request(url, 0, log)
{
    // Build the system description as a JSON object.
    QJsonObject json;
    json.insert("appName", qtlApplicationName());
    json.insert("appVersion", _currentVersion.text());
    json.insert("qtVersion", qVersion());
    json.insert("cpuArch", QSysInfo::currentCpuArchitecture());
    json.insert("kernelName", QSysInfo::kernelType());
    json.insert("kernelVersion", QSysInfo::kernelVersion());
    json.insert("osName", QSysInfo::productType());
    json.insert("osVersion", QSysInfo::productVersion());
    json.insert("osDescription", QSysInfo::prettyProductName());
    json.insert("locale", QLocale::system().name());
    json.insert("timeZone", QString(QTimeZone::systemTimeZoneId()));

    // Build a Base-64 encoding of the serialized JSON object.
    const QString systemDescription(QJsonDocument(json).toJson(QJsonDocument::Compact).toBase64());

    // Use it as cookie named "target".
    _request.addCookie("target", systemDescription);

    // Start getting the content of the directory.
    connect(&_request, &QtlSimpleWebRequest::completed, this, &QtlNewVersionCheckerJson::httpCompleted);
    _request.start();
}


//----------------------------------------------------------------------------
// Invoked when the Web request is completed.
//----------------------------------------------------------------------------

void QtlNewVersionCheckerJson::httpCompleted(bool success, const QUrl& url, const QString& response)
{
    // The returned content is expected to be a JSON object with the following fields:
    // 'version' : lastest version
    // 'url' : download URL of the latest version
    // 'status' : one of 'new', 'same', 'old', describing the returned version.

    // Expected status of returned latest version:
    enum Status {New, Same, Old};

    QJsonDocument jsonDocument;
    QString error;
    QtlVersion latestVersion;
    QString latestUrl;
    Status status(New);

    if (!success) {
        // Error during Web request.
        error = response;
    }
    else {
        // Analyze the returned text as a JSON object.
        _log->debug(tr("Server response: %1").arg(response));
        QJsonParseError jsonError;
        jsonDocument = QJsonDocument::fromJson(response.toUtf8(), &jsonError);
        if (jsonError.error != QJsonParseError::NoError) {
            // Error while parsing the returned JSON.
            error = jsonError.errorString();
        }
    }

    if (error.isEmpty()) {
        // Get the required fields in the JSON structure.
        const QJsonObject json(jsonDocument.object());
        latestUrl = json.value("url").toString();
        latestVersion.setText(json.value("version").toString());
        const QString statusString(json.value("status").toString());

        // Check validity of the returned value.
        const QString invalidResponse(tr("Invalid response from server"));
        if (latestUrl.isEmpty() || !latestVersion.isValid()) {
            error = invalidResponse;
        }
        else if (statusString == "new") {
            status = New;
        }
        else if (statusString == "same") {
            status = Same;
        }
        else if (statusString == "old") {
            status = Old;
        }
        else {
            error = invalidResponse;
        }
    }

    // End of search.
    _log->debug(tr("End of search in %1").arg(url.toString()));
    const QString app(qtlApplicationName());

    if (error.isEmpty()) {
        // No error.
        if (status == New) {
            // New version found.
            QMessageBox::information(qtlWidgetAncestor(this),
                                     app,
                                     tr("A new version of %1 is available<br/>Your version is %2<br/>Version %3 is available online at:<br/>%4")
                                     .arg(app)
                                     .arg(_currentVersion.text())
                                     .arg(latestVersion.text())
                                     .arg(qtlHtmlLink(latestUrl)));
        }
        else if (_silent) {
            // No new version, silent mode.
            _log->debug(tr("No new version found, this: %1, latest: %2").arg(_currentVersion.text()).arg(latestVersion.text()));
        }
        else if (status == Same) {
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
        emit completed(true, latestVersion, QUrl(latestUrl));
    }
    else {
        // Error while retrieving Web content. Parameter "response" is an error message.
        if (_silent) {
            _log->debug(error);
        }
        else {
            qtlError(this, error);
        }

        // Emit the signal for clients.
        emit completed(false, QtlVersion(), QUrl());
    }

    // Delete this instance when back in the event loop.
    deleteLater();
}
