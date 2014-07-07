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
// Main window class for the QtlMovie application.
//
//----------------------------------------------------------------------------

#include "QtlMovieMainWindow.h"
#include "QtlMovieVersion.h"
#include "QtlMovie.h"
#include "QtlMovieEditSettings.h"
#include "QtlMovieAboutMediaTools.h"
#include "QtlNewVersionChecker.h"
#include "QtlBrowserDialog.h"
#include "QtlTextFileViewer.h"
#include "QtlTranslator.h"
#include "QtlProcess.h"
#include "QtlSysInfo.h"
#include "QtlStringUtils.h"
#include "QtlMessageBoxUtils.h"


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------

QtlMovieMainWindow::QtlMovieMainWindow(QWidget *parent, const QString& initialFileName) :
    QMainWindow(parent),
    _settings(0),
#if defined(QTL_WINEXTRAS)
    _taskbarButton(0),
    _taskbarProgress(0),
#endif
    _job(0),
    _sound(),
    _closePending(false)
{
    // Build the UI as defined in Qt Designer.
    _ui.setupUi(this);

#if defined(QTL_WINEXTRAS)
    // On Windows Vista, it has been reported that QtlMovie crashes on exit
    // when using a progress bar on icon. To avoid that, we use it only on
    // Windows 7 and higher.
    if ((QSysInfo::WindowsVersion & QSysInfo::WV_NT_based) != 0 && QSysInfo::WindowsVersion >= QSysInfo::WV_WINDOWS7) {

        // The following sequence is a mystery. If not present, the taskbar progress does not work.
        // But the Qt documentation is not really clear on what this does.
        if (QtWin::isCompositionEnabled()) {
            QtWin::enableBlurBehindWindow(this);
        }
        else {
            QtWin::disableBlurBehindWindow(this);
        }

        // Setup the Windows taskbar button.
        _taskbarButton = new QWinTaskbarButton(this);
        _taskbarButton->setWindow(windowHandle());
        _taskbarProgress = _taskbarButton->progress();
        _taskbarProgress->setRange(0, 1000);
    }
#endif

    // Connect the "About Qt" action.
    connect(_ui.actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    // Create and load settings. Ignore errors (typically default settings file not yet created).
    _settings = new QtlMovieSettings(_ui.log, this);
    applyUiSettings();

    // Adjust UI for single file mode or batch mode.
    if (_settings->useBatchMode()) {
        // Batch mode: Delete unused widgets.
        delete _ui.actionOpen;
        delete _ui.actionInputFileProperties;
        delete _ui.actionTestAudio;
        delete _ui.singleTask;
        _ui.singleTask = 0;

        // Initialize task list.
        _ui.taskList->initialize(_settings, _ui.log);
    }
    else {
        // Single file mode: Delete unused widgets.
        delete _ui.actionNewTask;
        delete _ui.taskBox;
        _ui.taskBox = _ui.taskList = 0;

        // Initialize the task editor.
        _ui.singleTask->initialize(new QtlMovieTask(_settings, _ui.log, this), _settings, _ui.log);
    }

    // Restore the window geometry from the saved settings.
    _settings->restoreGeometry(this);

    // Report missing tools (FFmpeg, DvdAuthor, etc.)
    _settings->reportMissingTools();

    // Transcoding is initially stopped.
    transcodingUpdateUi(false);

    // If a command line argument is provided, use it as input file.
    if (!initialFileName.isEmpty()) {
        if (_ui.singleTask != 0) {
            // Single file mode, simply set the input file name.
            _ui.singleTask->task()->inputFile()->setFileName(initialFileName);
        }
        else {
            // Batch mode, create a new task.
            QtlMovieTask* task = new QtlMovieTask(_settings, _ui.log, this);
            task->inputFile()->setFileName(initialFileName);
            _ui.taskList->addTask(task, true);
        }
    }

    // Start a search for a new version online.
    if (_settings->newVersionCheck()) {
        searchNewVersion(true);
    }
}


//-----------------------------------------------------------------------------
// Update the UI when transcoding starts or stops.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::transcodingUpdateUi(bool started)
{
    // Enable / disable widgets which must be inactive during transcoding.
    if (_ui.singleTask != 0) {
        _ui.actionOpen->setEnabled(!started);
    }

    // Toggle transcode/cancel labels in buttons.
    // Change their target slot to start/cancel transcoding.
    if (started) {
        _ui.actionTranscode->setText(tr("Cancel ..."));
        disconnect(_ui.actionTranscode, &QAction::triggered, this, &QtlMovieMainWindow::startTranscoding);
        connect(_ui.actionTranscode, &QAction::triggered, this, &QtlMovieMainWindow::cancelTranscoding);

        _ui.buttonStart->setText(tr("Cancel ..."));
        disconnect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieMainWindow::startTranscoding);
        connect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieMainWindow::cancelTranscoding);
    }
    else {
        _ui.actionTranscode->setText(tr("Transcode ..."));
        disconnect(_ui.actionTranscode, &QAction::triggered, this, &QtlMovieMainWindow::cancelTranscoding);
        connect(_ui.actionTranscode, &QAction::triggered, this, &QtlMovieMainWindow::startTranscoding);

        _ui.buttonStart->setText(tr("Start ..."));
        disconnect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieMainWindow::cancelTranscoding);
        connect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieMainWindow::startTranscoding);
    }

    // Set the progress bar.
    // When transcoding starts, we set the maximum to zero to force a "busy" bar.
    // If a more precise progression indicator is set later, an exact percentage will be displayed.
    _ui.progressBarEncode->setTextVisible(false);
    _ui.progressBarEncode->setRange(0, started ? 0 : 100);
    _ui.progressBarEncode->reset();
    _ui.labelRemainingTime->setVisible(false);

    // Set the Windows task bar button.
#if defined(QTL_WINEXTRAS)
    if (_taskbarProgress != 0) {
        _taskbarProgress->setVisible(started);
    }
#endif
}


//-----------------------------------------------------------------------------
// Invoked by the "Transcode ..." buttons.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::startTranscoding()
{
    // Fool-proof check.
    if (_job != 0) {
        _ui.log->line(tr("Internal error, transcoding job already created."));
        return;
    }

    //@@@@@@@@@@ TO BE REFACTORED TO SUPPORT BATCH MODE.

    if (_ui.singleTask == 0) {
        return;
    }

    // Setup the transcoding job.
    _job = new QtlMovieJob(_ui.singleTask->task(), _settings, _ui.log, this);
    connect(_job, &QtlMovieJob::started,   this, &QtlMovieMainWindow::transcodingStarted);
    connect(_job, &QtlMovieJob::progress,  this, &QtlMovieMainWindow::transcodingProgress);
    connect(_job, &QtlMovieJob::completed, this, &QtlMovieMainWindow::transcodingStopped);

    //@@@@@@@@@@

    // Start the job.
    if (!_job->start()) {
        // Error starting the job, delete it now.
        delete _job;
        _job = 0;
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Cancel ..." buttons.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::cancelTranscoding()
{
    // Ask the user to confirm the cancelation of the current transcoding.
    proposeToCancelTranscoding();
}


//-----------------------------------------------------------------------------
// Invoked when transcoding starts.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::transcodingStarted()
{
    // Clear the log if required.
    if (_settings->clearLogBeforeTranscode()) {
        _ui.log->clear();
    }

    // Update user interface.
    transcodingUpdateUi(true);
}


//-----------------------------------------------------------------------------
// Invoked when some progress is made in the transcoding process.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::transcodingProgress(const QString& description, int current, int maximum, int elapsedSeconds, int remainingSeconds)
{
    Q_UNUSED(elapsedSeconds);

    // Restrict current within range.
    current = qBound(0, current, maximum);

    // Progress bar.
    if (maximum <= 0) {
        // Cannot evaluate the progression, display a "busy" progress bar without percentage.
        _ui.progressBarEncode->setRange(0, 0);
        _ui.progressBarEncode->setTextVisible(false);
    }
    else {
        // Display the progression.
        _ui.progressBarEncode->setRange(0, maximum);
        _ui.progressBarEncode->setValue(current);
        _ui.progressBarEncode->setTextVisible(true);
    }

    // Job description and remaining time.
    if (remainingSeconds >= 0) {
        _ui.labelRemainingTime->setText(tr("%1%2Remaining time: %3")
                                        .arg(description)
                                        .arg(description.isEmpty() ? "" : " - ")
                                        .arg(qtlSecondsToString(int(remainingSeconds))));
        _ui.labelRemainingTime->setVisible(true);
    }
    else if (!description.isEmpty()) {
        _ui.labelRemainingTime->setText(description);
        _ui.labelRemainingTime->setVisible(true);
    }
    else {
        _ui.labelRemainingTime->setVisible(false);
    }

    // Update the Windows task bar button.
#if defined(QTL_WINEXTRAS)
    if (_taskbarProgress != 0 && _job != 0) {
        _taskbarProgress->setValue(_job->currentProgress(current, maximum, _taskbarProgress->maximum()));
    }
#endif
}


//-----------------------------------------------------------------------------
// Invoked when transcoding stops.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::transcodingStopped(bool success)
{
    // Check if a transcoding job was actually in progress.
    if (_job != 0) {

        // Play notification sound if required.
        if (_settings->playSoundOnCompletion()) {
            // Stop current play (if any).
            _sound.stop();
            // Set now sound source.
            _sound.setSource(QUrl("qrc:/sounds/completion.wav"));
            // Play the sound.
            _sound.setVolume(1.0);
            _sound.play();
        }

        // Save log file if required.
        if (_settings->saveLogAfterTranscode()) {
            const QString logFile(_job->task()->outputFile()->fileName() + _settings->logFileExtension());
            _ui.log->saveToFile(logFile);
            _ui.log->line(tr("Saved log to %1").arg(logFile));
        }

        // Delete transcoding job.
        _job->deleteLater();
        _job = 0;
    }

    // Update UI.
    transcodingUpdateUi(false);

    // If the application needs to be closed, close it now.
    if (_closePending) {
        close();
    }
}


//-----------------------------------------------------------------------------
// Check if an encoding is currently in progress. Propose to abort.
//-----------------------------------------------------------------------------

QtlMovieMainWindow::CancelStatus QtlMovieMainWindow::proposeToCancelTranscoding()
{
    // If there is nothing to cancel now, no need to ask.
    if (_job == 0) {
        return NothingToCancel;
    }

    // Ask the user if we should cancel the current transcoding?
    const bool confirm = qtlConfirm(this, tr("Do you want to abort the current transcoding ?"));

    if (_job == 0) {
        // If all transcoding completed in the meantime, all clear...
        return NothingToCancel;
    }
    else if (!confirm) {
        // The user refused to cancel and the transcoding is still in progress, refuse.
        return CancelRefused;
    }
    else {
        // A transcoding is in progress and the user decided to abort it.
        _job->abort();
        return CancelInProgress;
    }
}


//-----------------------------------------------------------------------------
// Event handler to handle window close.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::closeEvent(QCloseEvent* event)
{
    if (_closePending) {
        // If _closePending is already true, either this is a close following the completion
        // of the transcoding cancelation or the cancelation does not work and the user tries
        // to close again. In both cases, we accept the close.
        event->accept();
    }
    else {
        // Check if should abort any transcoding.
        switch (proposeToCancelTranscoding()) {
        case NothingToCancel:
            // It is always safe to close when no transcoding is in progress.
            event->accept();
            break;
        case CancelRefused:
            // The user refused to cancel the transcoding. Do not close.
            event->ignore();
            break;
        case CancelInProgress:
            // The user accepted to cancel the transcoding. However, we wait for the
            // cancelation to complete before closing. Will close later.
            _closePending = true;
            event->ignore();
            break;
        default:
            _ui.log->line(tr("Internal error, invalid cancel state"));
            event->ignore();
            break;
        }
    }

    // If the event is accepted, save the geometry of the window.
    if (event->isAccepted()) {
        _settings->saveGeometry(this);
        _settings->sync();
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "DVD Decrypter..." button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::startDvdDecrypter()
{
    // DVD Decrypter executable file.
    const QString exec(_settings->dvddecrypter()->fileName());
    if (exec.isEmpty()) {
        qtlError(this, tr("DVD Decrypter not found, install it or set explicit path in settings."));
    }
    else {
        QtlProcess::newInstance(exec, QStringList(), 0, 0, this)->start();
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Settings..." button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::editSettings()
{
    // Open an edition dialog with the current settings.
    QtlMovieEditSettings edit(_settings, this);
    if (edit.exec() == QDialog::Accepted) {
        // Button "OK" has been selected, update settings from the values in the dialog box.
        edit.applySettings();
        applyUiSettings();
    }
}


//-----------------------------------------------------------------------------
// Apply the settings which affect the UI.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::applyUiSettings()
{
    _ui.log->setMaximumBlockCount(_settings->maxLogLines());
}


//-----------------------------------------------------------------------------
// Invoked by the "About Media Tools" button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::aboutMediaTools()
{
    QtlMovieAboutMediaTools box(_settings, this);
    box.exec();
}


//-----------------------------------------------------------------------------
// Invoked by the "About" button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::about()
{
    QMessageBox::about(this,
                       tr("About QtlMovie"),
                       QStringLiteral("<p><b>QtlMovie</b>: %1</p>"
                          "<p>%2 " QTLMOVIE_VERSION "</p>"
                          "<p>%3 " __DATE__ "</p>"
                          "<p>Copyright (c) 2013, Thierry Lel&eacute;gard</p>")
                       .arg(tr("A specialized Qt front-end for<br/>FFmpeg and other free media tools"))
                       .arg(tr("Version"))
                       .arg(tr("Built")));
}


//-----------------------------------------------------------------------------
// Invoked by the "Help" button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::showHelp()
{
    QtlBrowserDialog box(this,
                         "qrc" + QtlTranslator::searchLocaleFile(":/help/qtlmovie.html"),
                         tr("QtlMovie Help"),
                         ":/images/qtlmovie-logo.png");
    box.setObjectName("QtlMovieHelpViewer");
    box.setGeometrySettings(_settings, true);
    box.exec();
}


//-----------------------------------------------------------------------------
// Invoked by the "Release Notes" button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::showReleaseNotes()
{
    QtlTextFileViewer box(this,
                          QtlTranslator::searchLocaleFile(":/changelog.txt"),
                          tr("QtlMovie Release Notes"),
                          ":/images/qtlmovie-logo.png");
    box.setObjectName("QtlMovieReleaseNotesViewer");
    box.setGeometrySettings(_settings, true);
    box.exec();
}


//-----------------------------------------------------------------------------
// Invoked by the "Search New Version" button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::searchNewVersion(bool silent)
{
    // Build URL and file name template.
    QString directoryUrl("http://sourceforge.net/projects/qtlmovie/files/");
    QString filePrefix;
    QString fileSuffix;

#if defined (Q_OS_WIN)
    // On Windows, fetch the binary installer.
    if (QtlSysInfo::runningOnWin64()) {
        directoryUrl.append("win64/");
        filePrefix = "QtlMovie-Win64-";
        fileSuffix = ".exe";
    }
    else {
        directoryUrl.append("win32/");
        filePrefix = "QtlMovie-Win32-";
        fileSuffix = ".exe";
    }
#else
    // On other OS, fetch the source archive.
    directoryUrl.append("src/");
    filePrefix = "QtlMovie-";
    fileSuffix = "-src.zip";
#endif

    // Start searching for new versions.
    QtlNewVersionChecker::newInstance(QtlVersion(QTLMOVIE_VERSION),
                                      directoryUrl,
                                      filePrefix,
                                      fileSuffix,
                                      "/download", // The download suffix for SourceForge URL's.
                                      silent,
                                      _ui.log,
                                      this);
}
