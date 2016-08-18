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
// Define the class QtlMovieNewVersion.
//
//----------------------------------------------------------------------------

#include "QtlMovieNewVersion.h"
#include "QtlMovieVersion.h"
#include "QtlMovie.h"
#include "QtlDialog.h"
#include "QtlStringUtils.h"
#include "QtlMessageBoxUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieNewVersion::QtlMovieNewVersion(bool silent,
                                       bool autoDelete,
                                       QtlMovieSettings* settings,
                                       QtlLogger* log,
                                       QObject* parent) :
    QObject(parent),
    _settings(settings),
    _log(log),
    _silent(silent),
    _autoDelete(autoDelete),
    _currentVersion(QtlVersion(qtlMovieVersion())),
    _newVersionRequest(WEBREF_QTLMOVIE_NEWVERSION, 0, log),
    _releaseNotesRequest(QString(), 0, log),
    _latestVersion(),
    _latestUrl(),
    _newerVersion(false),
    _statusMessage(),
    _releaseNotes()
{
    Q_ASSERT(log != 0);
    Q_ASSERT(settings != 0);

    // For debug purpose, the environment variable QTLMOVIE_NEWVERSION_URL can be set to an alternate server.
    const QString alternateUrl(qgetenv("QTLMOVIE_NEWVERSION_URL"));
    if (!alternateUrl.isEmpty()) {
        _newVersionRequest.setUrl(alternateUrl);
    }

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
    _newVersionRequest.addCookie("target", systemDescription);

    // Start the Web request.
    connect(&_newVersionRequest, &QtlSimpleWebRequest::completed, this, &QtlMovieNewVersion::httpCompleted);
    _newVersionRequest.start();
}


//----------------------------------------------------------------------------
// Invoked when the Web request is completed.
//
// The returned content is a JSON object with the following fields:
// 'version' : lastest version
// 'url' : download URL of the latest version
// 'status' : one of 'new', 'same', 'old', describing the returned version.
// 'relnotes' : optional URL of the release notes
//
//----------------------------------------------------------------------------

void QtlMovieNewVersion::httpCompleted(bool success, const QUrl& url, const QString& response, const QString& mimeType)
{
    _log->debug(tr("End of search in %1").arg(url.toString()));

    // In case of error during Web request.
    if (!success) {
        fail(response);
        return;
    }

    // Analyze the returned text as a JSON object.
    _log->debug(tr("Server response: %1").arg(response));
    QJsonParseError jsonError;
    QJsonDocument jsonDocument(QJsonDocument::fromJson(response.toUtf8(), &jsonError));
    if (jsonError.error != QJsonParseError::NoError) {
        // Error while parsing the returned JSON.
        fail(jsonError.errorString());
        return;
    }

    // Get the required fields in the JSON structure.
    const QJsonObject json(jsonDocument.object());
    _latestVersion.setText(json.value("version").toString());
    _latestUrl = json.value("url").toString();
    const QString statusString(json.value("status").toString());
    const QString releaseNotesUrl(json.value("relnotes").toString());

    // Check validity of the returned values.
    const QString invalidResponse(tr("Invalid response from server"));
    if (_latestUrl.isEmpty() || !_latestVersion.isValid()) {
        fail(invalidResponse);
        return;
    }
    else if (statusString == "new") {
        _newerVersion = true;
        _statusMessage = QStringLiteral("<p style=\"font-weight: bold; font-size: 12pt;\">%1</p><p>%2</p>")
                .arg(tr("A new version of QtlMovie is available"))
                .arg(tr("Your version is %1, latest version is %2").arg(_currentVersion.text()).arg(_latestVersion.text()));
    }
    else if (statusString == "same") {
        _statusMessage = tr("You are using the latest version of QtlMovie");
    }
    else if (statusString == "old") {
        _statusMessage = tr("Your version of QtlMovie is %1<br/>More recent than %2, the latest available online")
                .arg(_currentVersion.text())
                .arg(_latestVersion.text());
    }
    else {
        fail(invalidResponse);
        return;
    }

    // Terminate now or more to fetch?
    if (_newerVersion && !releaseNotesUrl.isEmpty()) {
        // There is a new version and release notes are available.
        // Start getting the content of the release notes.
        connect(&_releaseNotesRequest, &QtlSimpleWebRequest::completed, this, &QtlMovieNewVersion::releaseNotesCompleted);
        _releaseNotesRequest.setUrl(releaseNotesUrl);
        _releaseNotesRequest.start();
    }
    else {
        // End of search.
        succeed();
    }
}


//----------------------------------------------------------------------------
// Invoked when the Web request for release notes is completed.
//----------------------------------------------------------------------------

void QtlMovieNewVersion::releaseNotesCompleted(bool success, const QUrl& url, const QString& response, const QString& mimeType)
{
    // In case of error during Web request.
    if (!success) {
        fail(response);
        return;
    }

    // The release notes are formatted as HTML.
    QString html;
    const char* ulOpen  = "<ul>";
    const char* ulClose = "</ul>";
    const char* liOpen  = "<li>";
    const char* liClose = "</li>";
    const char* hOpen   = "<h3>";
    const char* hClose  = "</h3>";
    bool ulIsOpen = false;
    bool liIsOpen = false;

    // Analyze the text of the release notes.
    // Prefix for lines introducing a version.
    const QString versionPrefix("Version ");

    // Analyze line by line. Keep only release notes from current version to new version.
    bool keep = false;
    foreach (const QString& line, response.split(QRegExp("\\s*\\n\\s*"))) {

        // Check if this is the start of a version description.
        if (line.startsWith(versionPrefix, Qt::CaseInsensitive)) {
            // Get the associated version.
            const QtlVersion version(line.mid(versionPrefix.length()).trimmed());
            if (version.isValid()) {
                if (version <= _currentVersion) {
                    // This is the start of current and former versions, not new, not interesting.
                    break;
                }
                // Close previous lists.
                if (liIsOpen) {
                    html.append(liClose);
                    liIsOpen = false;
                }
                if (ulIsOpen) {
                    html.append(ulClose);
                    ulIsOpen = false;
                }
                // Do not keep header and beta versions, more recent than latest official.
                keep = version <= _latestVersion;
                // Transform the line in a header.
                if (keep) {
                    html.append(hOpen).append(line.toHtmlEscaped().append(hClose));
                }
            }
        }
        else if (keep) {
            if (line.startsWith('-') || line.startsWith('*')) {
                if (!ulIsOpen) {
                    html.append(ulOpen);
                    ulIsOpen = true;
                }
                else if (liIsOpen) {
                    html.append(liClose);
                }
                html.append(liOpen);
                liIsOpen = true;
                html.append(line.mid(1).trimmed().toHtmlEscaped());
            }
            else if (!line.isEmpty()) {
                html.append(" ");
                html.append(line.toHtmlEscaped());
            }
        }
    }

    // Close previous lists.
    if (liIsOpen) {
        html.append(liClose);
    }
    if (ulIsOpen) {
        html.append(ulClose);
    }

    // Final text of the release notes.
    if (!html.isEmpty()) {
        _releaseNotes = "<html><body>" + html + "</body></html>";
    }

    // Now, this is the end.
    succeed();
}


//----------------------------------------------------------------------------
// Invoked when the user wants to download the last version.
//----------------------------------------------------------------------------

void QtlMovieNewVersion::downloadLatestVersion()
{
    // Open the URL of the new version in an external browser.
    if (!_latestUrl.isEmpty()) {
        QDesktopServices::openUrl(QUrl(_latestUrl));
    }
}


//----------------------------------------------------------------------------
// Terminate the operation with error.
//----------------------------------------------------------------------------

void QtlMovieNewVersion::fail(const QString& message)
{
    // Report error.
    if (_silent) {
        _log->debug(message);
    }
    else {
        qtlError(this, message);
    }

    // Delete this instance when back in the event loop.
    if (_autoDelete) {
        deleteLater();
    }
}


//----------------------------------------------------------------------------
// Terminate the operation with success.
//----------------------------------------------------------------------------

void QtlMovieNewVersion::succeed()
{
    if (_silent && !_newerVersion) {
        // In silent mode, do not report anything if no new version is available.
        _log->debug(tr("No new version found, this: %1, latest: %2").arg(_currentVersion.text()).arg(_latestVersion.text()));
    }
    else if (_releaseNotes.isEmpty()) {
        // No release notes to display, use a simple message box.
        if (_newerVersion) {
            _statusMessage.append(tr("<p>Download latest version %1</p>").arg(qtlHtmlLink(_latestUrl, tr("here"))));
        }
        QMessageBox::information(qtlWidgetAncestor(this), "QtlMovie", _statusMessage);
    }
    else {
        // Display message and release notes.
        displayReleaseNotes();
    }

    // Delete this instance when back in the event loop.
    if (_autoDelete) {
        deleteLater();
    }
}


//----------------------------------------------------------------------------
// Display the new version message with release notes.
//----------------------------------------------------------------------------

void QtlMovieNewVersion::displayReleaseNotes()
{
    // Create a modal dialog.
    QtlDialog dialog(qtlWidgetAncestor(this));
    dialog.setWindowTitle(tr("QtlMovie New Version"));
    dialog.setWindowIcon(QIcon(":/images/qtlmovie-64.png"));

    // Dialog layout.
    dialog.resize(400, 400);
    dialog.setObjectName("QtlMovieNewFeaturesViewer");
    dialog.setGeometrySettings(_settings, true);
    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // Top widget: the message.
    QLabel* message = new QLabel(_statusMessage, &dialog);
    message->setStyleSheet(QStringLiteral("padding-bottom: 15px;"));
    layout->addWidget(message);

    // Title of the text window.
    QLabel* header = new QLabel(tr("New features :"), &dialog);
    header->setStyleSheet(QStringLiteral("font-weight: bold;"));
    layout->addWidget(header);

    // The text window containing the release notes.
    QTextEdit* text = new QTextEdit(_releaseNotes, &dialog);
    text->setReadOnly(true);
    text->setTextInteractionFlags(Qt::TextBrowserInteraction);
    layout->addWidget(text);

    // Button box. Use a custom widget instead of a QButtonBox to better control the buttons identity and behavious.
    QWidget* box = new QWidget(&dialog);
    layout->addWidget(box);

    // Horizontal layout inside the button box. The inner margins are set to zero since the margin of the dialog's
    // layout already apply. Keep a no-zero top margin to keep some space from the text box.
    QHBoxLayout* boxLayout = new QHBoxLayout(box);
    boxLayout->setSpacing(6);
    boxLayout->setContentsMargins(0, 10, 0, 0);

    // The download button is the default one.
    // Use MinimumExpanding horizontal policy, otherwise the margins are too narrow.
    QPushButton* downloadButton = new QPushButton(tr("Download QtlMovie %1").arg(_latestVersion.text()), box);
    boxLayout->addWidget(downloadButton);
    downloadButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    downloadButton->setDefault(true);
    connect(downloadButton, &QAbstractButton::clicked, this, &QtlMovieNewVersion::downloadLatestVersion);

    // An horizontal spacer is user to keep the two buttons on opposite sizes.
    QSpacerItem* boxSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    boxLayout->addItem(boxSpacer);

    // Close button.
    // Use Minimum horizontal policy, otherwise the margins are too wide.
    QPushButton* closeButton = new QPushButton(tr("Close"), box);
    boxLayout->addWidget(closeButton);
    closeButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    connect(closeButton, &QPushButton::clicked, &dialog, &QtlDialog::accept);

    // Wait until the user close the dialog.
    dialog.exec();
}
