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
#include "QtlMovieAudioTestDialog.h"
#include "QtlMovieAboutMediaTools.h"
#include "QtlMovieInputFilePropertiesDialog.h"
#include "QtlNewVersionChecker.h"
#include "QtlBrowserDialog.h"
#include "QtlTextFileViewer.h"
#include "QtlTranslator.h"
#include "QtlIncrement.h"
#include "QtlProcess.h"
#include "QtlUtils.h"
#include <QtWidgets>


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
    _inFile(0),
    _outFile(0),
    _job(0),
    _closePending(false),
    _updatingSelections(0)
{
    // Build the UI as defined in Qt Designer.
    _ui.setupUi(this);

    // Setup the Windows taskbar button.
#if defined(QTL_WINEXTRAS)
    _taskbarButton = new QWinTaskbarButton(this);
    _taskbarButton->setWindow(windowHandle());
    _taskbarProgress = _taskbarButton->progress();
    _taskbarProgress->setRange(0, 1000);
#endif

    // Connect the "About Qt" action.
    connect(_ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // Clear input file stream info.
    _ui.boxSubtitleStreams->addExternalButton(_ui.radioButtonSubtitleFile);
    clearInputStreamInfo();

    // Create one radio button per output type (output file type selection).
    setupOutputTypes();

    // Create and load settings. Ignore errors (typically default settings file not yet created).
    _settings = new QtlMovieSettings(_ui.log, this);
    _settings->load();
    applyUiSettings();

    // Report missing tools (FFmpeg, DvdAuthor, etc.)
    _settings->reportMissingTools();

    // Transcoding is initially stopped.
    transcodingUpdateUi(false);

    // Create input and output file objects.
    _inFile = new QtlMovieInputFile("", _settings, _ui.log, this);
    _outFile = new QtlMovieOutputFile("", _settings, _ui.log, this);

    // Create the various connections around file modifications.
    connect(_inFile, SIGNAL(fileNameChanged(QString)), this, SLOT(inputFileNameChanged(QString)));
    connect(_inFile, SIGNAL(mediaInfoChanged()), this, SLOT(inputFileFormatChanged()));
    connect(_outFile, SIGNAL(fileNameChanged(QString)), this, SLOT(outputFileNameChanged(QString)));

    // If a command line argument is provided, use it as input file.
    if (!initialFileName.isEmpty()) {
        _inFile->setFileName(initialFileName);
    }

    // Start a search for a new version online.
    if (_settings->newVersionCheck()) {
        searchNewVersion(true);
    }
}


//-----------------------------------------------------------------------------
// Setup the radio buttons to select the output type.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::setupOutputTypes()
{
    // List of output types in the "display order".
    const QList<QtlMovieOutputFile::OutputType> outputTypes(QtlMovieOutputFile::outputTypes());

    // Add all buttons.
    foreach (QtlMovieOutputFile::OutputType type, outputTypes) {

        // Create the button.
        QRadioButton* button(new QRadioButton(_ui.boxOutputTypes));
        button->setText(QtlMovieOutputFile::outputTypeName(type));
        connect(button, SIGNAL(toggled(bool)), this, SLOT(setOutputFileType()));

        // Associate the integer value of the output type to the radio button.
        _ui.boxOutputTypes->setButtonId(button, int(type));
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Open ..." button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::openFile()
{
    // Ask the user to select an input file.
    const QString previous(_inFile->isSet() ? _inFile->fileName() : _settings->initialInputDir());
    const QString name(QFileDialog::getOpenFileName(this, tr("Open Movie File"), previous));

    // Set the new file name.
    if (!name.isEmpty()) {
        _inFile->setFileName(name);
    }
}


//-----------------------------------------------------------------------------
// Invoked to select output file.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::selectOutputFile()
{
    // Ask the user to select an output file.
    const QString previous(_outFile->isSet() ? _outFile->fileName() : _outFile->defaultOutputDirectory(_inFile->fileName()));
    const QString name(QFileDialog::getSaveFileName(this, tr("Output Movie File"), previous));

    // Set the new file name.
    if (!name.isEmpty()) {
        _outFile->setFileName(name);
    }
}


//-----------------------------------------------------------------------------
// Invoked to select subtitle file.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::selectSubtitleFile()
{
    // Previous content of edit box.
    QString previous(_ui.editSubtitleFile->text());

    // If no previous content, use the input file directory.
    if (previous.isEmpty()) {
        previous = _inFile->isSet() ? QFileInfo(_inFile->fileName()).absolutePath() : _settings->initialInputDir();
    }

    // Select the subtitle file.
    const QString filters(tr("Subtitle files (*.srt *.ssa *.ass)"));
    const QString name(QFileDialog::getOpenFileName(this, tr("Select Subtitle File"), previous, filters));
    if (!name.isEmpty()) {
        _ui.editSubtitleFile->setText(QtlFile::absoluteNativeFilePath(name));
    }
}


//-----------------------------------------------------------------------------
// Invoked when the edition of the input file name changed in the edit box.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::inputFileNameEdited()
{
    // Set input file name from its edit box.
    _inFile->setFileName(_ui.editInputFile->text());
}


//-----------------------------------------------------------------------------
// Invoked when the input file name has changed.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::inputFileNameChanged(const QString& fileName)
{
    // Set the input file name in the edit box.
    if (_ui.editInputFile->text() != fileName) {
        _ui.editInputFile->setText(fileName);
    }

    // Clear stream information in input file.
    clearInputStreamInfo();

    // Select default output file name and window title.
    if (fileName.isEmpty()) {
        _outFile->setFileName("");
        setWindowTitle("QtlMovie");
    }
    else {
        _outFile->setDefaultFileName(fileName);
        setWindowTitle(QFileInfo(fileName).fileName() + " - QtlMovie");
    }

    // Clear subtitle file name.
    _ui.editSubtitleFile->clear();
}


//-----------------------------------------------------------------------------
// Clear input file stream information.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::clearInputStreamInfo()
{
    // List of group boxes to clean up.
    QList<QtlButtonGrid*> boxes;
    boxes << _ui.boxVideoStreams<< _ui.boxAudioStreams << _ui.boxSubtitleStreams;

    // Cleanup all group boxes.
    foreach (QtlButtonGrid* box, boxes) {
        // Delete all children radio buttons with an id in the box.
        const QList<QRadioButton*> buttons(box->findChildren<QRadioButton*>());
        foreach (QRadioButton* button, buttons) {
            if (box->buttonId(button) >= 0) {
                delete button;
            }
        }
    }

    // No subtitle stream is selected, pre-select "no subtitle".
    _ui.radioButtonNoSubtitle->setChecked(true);
    _ui.buttonSubtitleFile->setEnabled(false);
    _ui.editSubtitleFile->setEnabled(false);

    // Make sure the "None" and "Subtitle File" are in the same radio group as subtitle streams.
    _ui.boxSubtitleStreams->addExternalButton(_ui.radioButtonNoSubtitle);
    _ui.boxSubtitleStreams->addExternalButton(_ui.radioButtonSubtitleFile);
}


//-----------------------------------------------------------------------------
// Invoked when the selections of input streams have changed.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::inputStreamSelectionUpdated()
{
    // Prevent recursion when updating widgets.
    if (_updatingSelections > 0) {
        return;
    }
    QtlIncrement<int> updating(&_updatingSelections);

    // Select the streams to transcode.
    if (_inFile != 0) {
        _inFile->setSelectedVideoStreamIndex(_ui.boxVideoStreams->checkedId());
        _inFile->setSelectedAudioStreamIndex(_ui.boxAudioStreams->checkedId());
        _inFile->setSelectedSubtitleStreamIndex(_ui.boxSubtitleStreams->checkedId());
    }

    // Check if the external subtitle file is selected.
    const bool subtitleFileSelected = _ui.radioButtonSubtitleFile->isChecked();

    // Enable subtitle file edits only when the subtitle file is active.
    _ui.editSubtitleFile->setEnabled(subtitleFileSelected);
    _ui.buttonSubtitleFile->setEnabled(subtitleFileSelected);

    // Associate the subtitle file to the input file.
    if (subtitleFileSelected) {
        const QString subtitleFile(_ui.editSubtitleFile->text());
        const bool allowed = subtitleFile.isEmpty() || QtlMovieJob::canInsertSubtitles(subtitleFile);

        // If the file is invalid, report an error, clear file and select it again.
        if (!allowed) {
            qtlError(this, tr("File not found or invalid subtitle file:\n%1").arg(subtitleFile));
            _ui.editSubtitleFile->clear();
            selectSubtitleFile();
        }
        else if (_inFile != 0) {
            _inFile->setExternalSubtitleFileName(subtitleFile);
        }
    }

    // Enable / disable output types based on what is possible.
    enableOutputTypes();
}


//-----------------------------------------------------------------------------
// Enable or disable the various output types based on the input file.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::enableOutputTypes()
{
    // Keep track of first allowed output type and currently selected button.
    QAbstractButton* firstAllowed = 0;
    QAbstractButton* current = 0;

    // List of output types in the "display order".
    const QList<QtlMovieOutputFile::OutputType> outputTypes(QtlMovieOutputFile::outputTypes());

    foreach (QtlMovieOutputFile::OutputType type, outputTypes) {
        // Get the corresponding button.
        QAbstractButton* button = _ui.boxOutputTypes->buttonOf(int(type));
        if (button != 0) {
            // Is this output type allowed for the input file?
            const bool allowed = QtlMovieJob::canTranscode(_inFile, type);
            // Keep track of currently checked button.
            if (button->isChecked()) {
                current = button;
            }
            // Keep track of first allowed button.
            if (allowed && firstAllowed == 0) {
                firstAllowed = button;
            }
            // Disable buttons for output types which are not allowed.
            button->setEnabled(allowed);
        }
    }

    // If the currently selected output type becomes disallowed, switch to first allowed.
    if (current != 0 && firstAllowed != 0 && !current->isEnabled()) {
        firstAllowed->setChecked(true);
    }
}


//-----------------------------------------------------------------------------
// Invoked when new media information is available on the input file.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::inputFileFormatChanged()
{
    // We update streams and widget ourselves, prevent inputStreamSelectionUpdated() from doing it in parallel.
    QtlIncrement<int> updating(&_updatingSelections);

    // Clear previous stream info.
    clearInputStreamInfo();

    // Loop on all input streams.
    const int streamCount = _inFile->streamCount();
    for (int si = 0; si < streamCount; ++si) {

        const QtlMovieStreamInfoPtr stream(_inFile->streamInfo(si));
        Q_ASSERT(!stream.isNull());
        _ui.log->debug(tr("Found stream: %1").arg(stream->description(false)));

        QtlButtonGrid* box = 0;
        bool selected = false;

        // Select group box and widget type based on stream type.
        switch (stream->streamType()) {
        case QtlMovieStreamInfo::Video:
            box = _ui.boxVideoStreams;
            selected = si == _inFile->selectedVideoStreamIndex();
            break;
        case QtlMovieStreamInfo::Audio:
            box = _ui.boxAudioStreams;
            selected = si == _inFile->selectedAudioStreamIndex();
            break;
        case QtlMovieStreamInfo::Subtitle:
            box = _ui.boxSubtitleStreams;
            selected = si == _inFile->selectedSubtitleStreamIndex();
            break;
        case QtlMovieStreamInfo::Other:
            // Ignore these streams.
            break;
        default:
            _ui.log->line(tr("Internal error, invalid stream type"));
            break;
        }

        if (box != 0) {
            // This is an audio/video/subtitle stream, create a radio button.
            QRadioButton* radio = new QRadioButton(stream->description(true), box);
            box->setButtonId(radio, si);
            radio->setToolTip(stream->description(false));
            radio->setChecked(selected);

            // Update the input file object when the stream is selected.
            connect(radio, SIGNAL(toggled(bool)), this, SLOT(inputStreamSelectionUpdated()));
        }
    }

    // Enable / disable output types based on what is possible.
    enableOutputTypes();
}


//-----------------------------------------------------------------------------
// Invoked when the edition of the output file name changed in the edit box.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::outputFileNameEdited()
{
    // Set output file name from its edit box.
    _outFile->setFileName(_ui.editOutputFile->text());
}


//-----------------------------------------------------------------------------
// Invoked when the output file name has changed.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::outputFileNameChanged(const QString& fileName)
{
    // Set the output file name in the edit box.
    if (_ui.editOutputFile->text() != fileName) {
        _ui.editOutputFile->setText(fileName);
    }
}


//-----------------------------------------------------------------------------
// Set the output file type based on selected radio button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::setOutputFileType()
{
    // Get the output type from the state of the radio buttons.
    const int id = _ui.boxOutputTypes->checkedId();
    if (id < 0) {
        _ui.log->line(tr("Internal error, cannot get output file type"));
    }
    else if (_outFile != 0) {
        // Get the output file type.
        QtlMovieOutputFile::OutputType type = QtlMovieOutputFile::OutputType(id);
        // Cannot select "Burn DVD" if the DVD burner device is undefined
        if (type == QtlMovieOutputFile::DvdBurn && _settings->dvdBurner().isEmpty()) {
            qtlError(this, tr("Cannot burn DVD, specify the burning device in settings"));
            // Revert to DVD ISO Image (will trigger this slot again).
            _ui.boxOutputTypes->checkId(int(QtlMovieOutputFile::DvdImage));
        }
        else {
            // Apply the output file type.
            _outFile->setOutputType(type);
            _outFile->setDefaultFileName(_inFile->fileName());
        }
    }
}


//-----------------------------------------------------------------------------
// Update the UI when transcoding starts or stops.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::transcodingUpdateUi(bool started)
{
    // Enable / disable widgets which must be inactive during transcoding.
    _ui.actionOpen->setEnabled(!started);
    _ui.groupInputFile->setEnabled(!started);
    _ui.groupOutputFile->setEnabled(!started);

    // Toggle transcode/cancel labels in buttons.
    // Change their target slot to start/cancel transcoding.
    if (started) {
        _ui.actionTranscode->setText(tr("Cancel ..."));
        disconnect(_ui.actionTranscode, SIGNAL(triggered()), this, SLOT(startTranscoding()));
        connect(_ui.actionTranscode, SIGNAL(triggered()), this, SLOT(cancelTranscoding()));

        _ui.buttonStart->setText(tr("Cancel ..."));
        disconnect(_ui.buttonStart, SIGNAL(clicked()), this, SLOT(startTranscoding()));
        connect(_ui.buttonStart, SIGNAL(clicked()), this, SLOT(cancelTranscoding()));
    }
    else {
        _ui.actionTranscode->setText(tr("Transcode ..."));
        disconnect(_ui.actionTranscode, SIGNAL(triggered()), this, SLOT(cancelTranscoding()));
        connect(_ui.actionTranscode, SIGNAL(triggered()), this, SLOT(startTranscoding()));

        _ui.buttonStart->setText(tr("Start ..."));
        disconnect(_ui.buttonStart, SIGNAL(clicked()), this, SLOT(cancelTranscoding()));
        connect(_ui.buttonStart, SIGNAL(clicked()), this, SLOT(startTranscoding()));
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
    _taskbarProgress->setVisible(started);
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

    // Setup the transcoding job.
    _job = new QtlMovieJob(_inFile, _outFile, _settings, _ui.log, this);
    connect(_job, SIGNAL(started()), this, SLOT(transcodingStarted()));
    connect(_job, SIGNAL(progress(QString,int,int,int,int)), this, SLOT(transcodingProgress(QString,int,int,int,int)));
    connect(_job, SIGNAL(completed(bool)), this, SLOT(transcodingStopped(bool)));

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
    // Update UI.
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
    // Compute a value between 0 and 1000 reflecting the global progress.
    // First, compute duration of one action. This assumes that all actions
    // have the same duration, which is of course wrong. But never mind.
    const int actionCount = _job == 0 ? 0 : _job->totalActionCount();
    const int actionDuration = actionCount == 0 ? 1000 : 1000 / actionCount;
    // Then, compute initial progress of current action.
    int progressValue = actionDuration * (_job == 0 ? 0 : _job->currentActionCount());
    // Finally, add progress within current action, if available.
    if (maximum > 0) {
        progressValue += (actionDuration * current) / maximum;
    }
    // Display the progress.
    _taskbarProgress->setValue(progressValue);
#endif
}


//-----------------------------------------------------------------------------
// Invoked when transcoding stops.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::transcodingStopped(bool success)
{
    // Delete transcoding job.
    if (_job != 0) {
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
    // If _closePending is already true, either this is a close following the completion
    // of the transcoding cancelation or the cancelation does not work and the user tries
    // to close again. In both cases, we accept the close.
    if (_closePending) {
        event->accept();
        return;
    }

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


//-----------------------------------------------------------------------------
// Invoked by the "Show Input File Properties..." button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::showInputFileProperties()
{
    QtlMovieInputFilePropertiesDialog dialog(_inFile, this);
    dialog.exec();
}


//-----------------------------------------------------------------------------
// Invoked by the "Audio Test..." button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindow::showAudioTest()
{
    if (_inFile->streamCount(QtlMovieStreamInfo::Audio) > 0) {
        QtlMovieAudioTestDialog dialog(_inFile, _ui.boxAudioStreams->checkedId(), _settings, _ui.log, this);
        dialog.exec();
    }
    else {
        qtlError(this, tr("No audio stream in file"));
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
        if (_settings->isModified()) {
            // Apply the settings which affect the UI.
            applyUiSettings();
            // Save settings.
            if (!_settings->save()) {
                QMessageBox::warning(this, tr("Save Error"), _settings->lastFileError());
            }
        }
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
    if (qtlRunningOnWin64()) {
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
