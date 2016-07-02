//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2016, Thierry Lelegard
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
#include "QtlMovie.h"
#include "QtlProcess.h"
#include "QtlStringUtils.h"
#include "QtlMessageBoxUtils.h"


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------

QtlMovieMainWindow::QtlMovieMainWindow(QWidget *parent, const QStringList& initialFileNames, bool logDebug) :
    QtlMovieMainWindowBase(parent, logDebug),
    _job(0),
    _batchMode(false),
    _restartRequested(false)
{
    // Build the UI as defined in Qt Designer.
    _ui.setupUi(this);

    // Set log debug mode.
    _ui.actionLogDebug->setChecked(logDebug);

    // Load settings and finish up base class part of user interface.
    setupUserInterface(_ui.log, _ui.actionAboutQt);

    // Adjust UI for single file mode or batch mode.
    _batchMode = settings()->useBatchMode();
    if (_batchMode) {
        // Batch mode: Hide unused widgets.
        _ui.actionOpen->setVisible(false);
        _ui.actionInputFileProperties->setVisible(false);
        _ui.actionTestAudio->setVisible(false);
        _ui.singleTask->setVisible(false);

        // Update the "switch mode" button.
        _ui.actionSwitchMode->setText(tr("Switch to Single File Mode"));

        // Initialize task list.
        _ui.taskList->initialize(settings(), log());
    }
    else {
        // Single file mode: Hide unused widgets.
        _ui.actionNewTask->setVisible(false);
        _ui.actionPurgeCompleted->setVisible(false);
        _ui.taskBox->setVisible(false);

        // Update the "switch mode" button.
        _ui.actionSwitchMode->setText(tr("Switch to Batch Mode"));

        // Initialize the task editor.
        _ui.singleTask->initialize(new QtlMovieTask(settings(), log(), this), settings(), log());
    }

    // Report missing tools (FFmpeg, DvdAuthor, etc.)
    settings()->reportMissingTools();

    // Transcoding is initially stopped.
    transcodingUpdateUi(false);

    // Use input files from command line arguments.
    if (!initialFileNames.isEmpty()) {
        if (_batchMode) {
            // Batch mode, create a new task for each file.
            foreach (const QString& fileName, initialFileNames) {
                _ui.taskList->addTask(fileName, false);
            }
        }
        else {
            // Single file mode, simply set the input file name.
            _ui.singleTask->task()->inputFile()->setFileName(initialFileNames.first());
        }
    }
}


//-----------------------------------------------------------------------------
// Update the UI when transcoding starts or stops.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::transcodingUpdateUi(bool started)
{
    // Enable / disable widgets which must be inactive during transcoding.
    _ui.actionSwitchMode->setEnabled(!started);
    _ui.actionOpen->setEnabled(!started);
    _ui.singleTask->setEnabled(!started);

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
    setIconTaskBarVisible(started);
}


//-----------------------------------------------------------------------------
// Create and start a new job.
//-----------------------------------------------------------------------------

bool QtlMovieMainWindow::startNewJob(QtlMovieTask* task)
{
    _job = new QtlMovieJob(task, settings(), log(), this);

    // Connect the job's signals to the UI.
    connect(_job, &QtlMovieJob::started,   this, &QtlMovieMainWindow::transcodingStarted);
    connect(_job, &QtlMovieJob::progress,  this, &QtlMovieMainWindow::transcodingProgress);
    connect(_job, &QtlMovieJob::completed, this, &QtlMovieMainWindow::transcodingStopped);

    // Start the job.
    if (_job->start()) {
        // Note that the job may fail in the meantime and _job is now back to zero.
        return true;
    }
    else {
       // Error starting the job, delete it now.
        delete _job;
        _job = 0;
        return false;
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Transcode ..." buttons.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::startTranscoding()
{
    // Fool-proof check.
    if (_job != 0) {
        log()->line(tr("Internal error, transcoding job already created."));
        return;
    }

    // The start processing depends on the single task vs. batch mode.
    if (_batchMode) {
        // Batch mode. Loop until a job is successfully started.
        // We exit the loop either when there is no more job to start (task == 0)
        // or a job was successfully started.
        QtlMovieTask* task = 0;
        do {
            task = _ui.taskList->nextTask();
        } while (task != 0 && !startNewJob(task));
    }
    else {
        // Single task mode. Get the task to execute.
        QtlMovieTask* const task = _ui.singleTask->task();
        if (task != 0) {
            // If the output file already exists...
            if (!task->askOverwriteOutput()) {
                // Don't overwrite, give up.
                log()->line(tr("Overwritting output file denied"));
                task->setCompleted(false);
            }
            else {
                // Setup the transcoding job.
                startNewJob(task);
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Cancel ..." buttons.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::cancelTranscoding()
{
    // Ask the user to confirm the cancelation of the current transcoding.
    proposeToCancel();
}


//-----------------------------------------------------------------------------
// Invoked when transcoding starts.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::transcodingStarted()
{
    // Clear the log if required.
    if (settings()->clearLogBeforeTranscode()) {
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
    if (_job != 0) {
        setIconTaskBarValue(_job->currentProgress(current, maximum, 1000), 1000);
    }
}


//-----------------------------------------------------------------------------
// Invoked when transcoding stops.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::transcodingStopped(bool success)
{
    // Check if a transcoding job was actually in progress.
    if (_job != 0) {

        // Play notification sound at complete end of all jobs.
        if (settings()->playSoundOnCompletion() && (!_batchMode || !_ui.taskList->hasQueuedTasks())) {
            playNotificationSound();
        }

        // Save log file if required.
        if (settings()->saveLogAfterTranscode()) {
            const QString logFile(_job->task()->outputFile()->fileName() + settings()->logFileExtension());
            _ui.log->saveToFile(logFile);
            log()->line(tr("Saved log to %1").arg(logFile));
        }

        // Delete transcoding job.
        _job->deleteLater();
        _job = 0;
    }

    // Update UI.
    transcodingUpdateUi(false);

    // If the application needs to be closed, close it now.
    if (closePending()) {
        close();
    }
    else if (_batchMode) {
        // In batch mode, start the next task, if any.
        startTranscoding();
    }
}


//-----------------------------------------------------------------------------
// Check if an encoding is currently in progress. Propose to abort.
//-----------------------------------------------------------------------------

QtlMovieMainWindow::CancelStatus QtlMovieMainWindow::proposeToCancel()
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
        // Defer the actual abort() when back in event loop.
        QMetaObject::invokeMethod(this, "deferredAbort", Qt::QueuedConnection);
        return CancelInProgress;
    }
}


//-----------------------------------------------------------------------------
// Abort the extraction, using a slot to defer the abort when back in the event loop.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::deferredAbort()
{
    if (_job != 0) {
        _job->abort();
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "DVD Extraction..." button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::startDvdExtraction()
{
    // Use the same executable with option "-e" (DVD "e"xtraction).
    // Also propagate debug mode to child process.
    QStringList args("-e");
    if (_ui.log->debugMode()) {
        args << "-d";
    }

    // Start the process.
    QProcess::startDetached(QApplication::applicationFilePath(), args);
}


//-----------------------------------------------------------------------------
// Invoked by the "Switch to {Single File|Batch} Mode" button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::switchMode()
{
    // We need to restart the application to change the UI. Ask the user.
    if (qtlConfirm(this, tr("Switching mode needs the application to restart.\nRestart now?"))) {
        // Update the settings with the reverse of the current mode (single file vs. batch).
        settings()->setUseBatchMode(!_batchMode);
        // Close the main window and let main() restart a new instance.
        _restartRequested = true;
        close();
    }
}


//-----------------------------------------------------------------------------
// Apply the settings which affect the UI.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::applyUserInterfaceSettings()
{
    _ui.log->setMaximumBlockCount(settings()->maxLogLines());

    // Check if the application needs a restart.
    // Currently, the single file vs. batch mode option is the only cause for a restart.
    _restartRequested = int(_batchMode) ^ int(settings()->useBatchMode());

    if (_restartRequested && qtlConfirm(this, tr("These settings will be applied when the application restarts.\nRestart now?"))) {
        close();
    }
}
