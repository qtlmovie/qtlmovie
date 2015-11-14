//----------------------------------------------------------------------------
//
// Copyright (c) 2014, Thierry Lelegard
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
// Define the class QtlMovieEditTask.
//
//----------------------------------------------------------------------------

#include "QtlMovieEditTask.h"
#include "QtlMovieInputFilePropertiesDialog.h"
#include "QtlMovieAudioTestDialog.h"
#include "QtlMovieJob.h"
#include "QtlMessageBoxUtils.h"
#include "QtlIncrement.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieEditTask::QtlMovieEditTask(QWidget* parent) :
    QWidget(parent),
    _settings(0),
    _log(0),
    _task(0),
    _updatingSelections(0)
{
    // Build the UI as defined in Qt Designer.
    _ui.setupUi(this);

    // Setup the radio buttons to select the output type.
    foreach (QtlMovieOutputFile::OutputType type, QtlMovieOutputFile::outputTypes()) {

        // Create the button.
        QRadioButton* button(new QRadioButton(_ui.boxOutputTypes));
        button->setText(QtlMovieOutputFile::outputTypeName(type));
        connect(button, &QRadioButton::toggled, this, &QtlMovieEditTask::setOutputFileType);

        // Associate the integer value of the output type to the radio button.
        _ui.boxOutputTypes->setButtonId(button, int(type));
    }

    // Clear input file stream info.
    _ui.boxSubtitleStreams->addExternalButton(_ui.radioButtonSubtitleFile);
    clearInputStreamInfo();

    // Say to the layout we need a minimum vertical size. Depending on the input
    // file characteristics, we may need more or less vertical space.
    QSizePolicy pol(QSizePolicy::Expanding, QSizePolicy::Minimum);
    pol.setHeightForWidth(true);
    setSizePolicy(pol);
}


//-----------------------------------------------------------------------------
// Clear input file stream information.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::clearInputStreamInfo()
{
    // List of group boxes to clean up.
    QList<QtlButtonGrid*> boxes;
    boxes << _ui.boxVideoStreams<< _ui.boxAudioStreams << _ui.boxSubtitleStreams;

    // Cleanup all group boxes.
    foreach (QtlButtonGrid* box, boxes) {
        // Delete all children radio buttons with an id in the box.
        foreach (QRadioButton* button, box->findChildren<QRadioButton*>()) {
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


//----------------------------------------------------------------------------
// Initialize the editor.
//----------------------------------------------------------------------------

void QtlMovieEditTask::initialize(QtlMovieTask* task, QtlMovieSettings* settings, QtlLogger* log)
{
    // Can be called only once.
    Q_ASSERT(_task == 0);
    Q_ASSERT(_settings == 0);
    Q_ASSERT(_log == 0);

    Q_ASSERT(task != 0);
    Q_ASSERT(settings != 0);
    Q_ASSERT(log != 0);

    // Store the settings for this task editor.
    _task = task;
    _settings = settings;
    _log = log;

    // Initialize the UI.
    inputFileNameChanged(_task->inputFile()->fileName());
    inputFileFormatChanged();
    _ui.boxOutputTypes->checkId(int(_task->outputFile()->outputType()));
    outputFileNameChanged(_task->outputFile()->fileName());

    // Get notified of file modifications.
    connect(_task->inputFile(),  &QtlMovieInputFile::fileNameChanged,  this, &QtlMovieEditTask::inputFileNameChanged);
    connect(_task->inputFile(),  &QtlMovieInputFile::mediaInfoChanged, this, &QtlMovieEditTask::inputFileFormatChanged);
    connect(_task->outputFile(), &QtlMovieOutputFile::fileNameChanged, this, &QtlMovieEditTask::outputFileNameChanged);
}


//-----------------------------------------------------------------------------
// Update the UI when transcoding starts or stops.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::transcodingUpdateUi(bool started)
{
    // Enable / disable widgets which must be inactive during transcoding.
    _ui.groupInputFile->setEnabled(!started);
    _ui.groupOutputFile->setEnabled(!started);
}


//-----------------------------------------------------------------------------
// Invoked to select input file.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::selectInputFile()
{
    if (_task != 0) {
        // Ask the user to select an input file.
        const QString previous(_task->inputFile()->isSet() ? _task->inputFile()->fileName() : _settings->initialInputDir());
        const QString name(QFileDialog::getOpenFileName(this, tr("Open Movie File"), previous));

        // Set the new file name.
        if (!name.isEmpty()) {
            _task->inputFile()->setFileName(name);
        }
    }
}


//-----------------------------------------------------------------------------
// Invoked to select output file.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::selectOutputFile()
{
    if (_task != 0) {
        // Ask the user to select an output file.
        const QString previous(_task->outputFile()->isSet() ? _task->outputFile()->fileName() : _task->outputFile()->defaultOutputDirectory(_task->inputFile()->fileName()));
        const QString name(QFileDialog::getSaveFileName(this, tr("Output Movie File"), previous));

        // Set the new file name.
        if (!name.isEmpty()) {
            _task->outputFile()->setFileName(name);
        }
    }
}


//-----------------------------------------------------------------------------
// Invoked to select subtitle file.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::selectSubtitleFile()
{
    // Previous content of edit box.
    QString previous(_ui.editSubtitleFile->text());

    // If no previous content, use the input file directory.
    if (previous.isEmpty()) {
        previous = _task != 0 && _task->inputFile()->isSet() ? QFileInfo(_task->inputFile()->fileName()).absolutePath() : _settings->initialInputDir();
    }

    // Select the subtitle file.
    const QString filters(tr("Subtitle files (*.srt *.ssa *.ass)"));
    const QString name(QFileDialog::getOpenFileName(this, tr("Select Subtitle File"), previous, filters));
    if (!name.isEmpty()) {
        _ui.editSubtitleFile->setText(QtlFile::absoluteNativeFilePath(name));
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Show Input File Properties..." button.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::showInputFileProperties()
{
    if (_task != 0) {
        QtlMovieInputFilePropertiesDialog dialog(_task->inputFile(), _settings, this);
        dialog.exec();
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Audio Test..." button.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::showAudioTest()
{
    if (_task != 0 && _task->inputFile()->streamCount(QtlMovieStreamInfo::Audio) > 0) {
        QtlMovieAudioTestDialog dialog(_task->inputFile(), _ui.boxAudioStreams->checkedId(), _settings, _log, this);
        dialog.exec();
    }
    else {
        qtlError(this, tr("No audio stream in file"));
    }
}


//-----------------------------------------------------------------------------
// Set the output file type based on selected radio button.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::setOutputFileType()
{
    // Get the output type from the state of the radio buttons.
    const int id = _ui.boxOutputTypes->checkedId();
    if (id < 0) {
        _log->line(tr("Internal error, cannot get output file type"));
    }
    else if (_task != 0 && _task->outputFile() != 0) {
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
            _task->outputFile()->setOutputType(type);
            _task->outputFile()->setDefaultFileName(_task->inputFile()->fileName());
        }
    }
}


//-----------------------------------------------------------------------------
// Invoked when the selections of input streams have changed.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::inputStreamSelectionUpdated()
{
    // Prevent recursion when updating widgets.
    if (_updatingSelections > 0) {
        return;
    }
    QtlIncrement<int> updating(&_updatingSelections);

    // Check if the external subtitle file is selected.
    const bool subtitleFileSelected = _ui.radioButtonSubtitleFile->isChecked();

    // Enable subtitle file edits only when the subtitle file is active.
    _ui.editSubtitleFile->setEnabled(subtitleFileSelected);
    _ui.buttonSubtitleFile->setEnabled(subtitleFileSelected);

    // Select the streams to transcode.
    if (_task != 0) {
        _task->inputFile()->setSelectedVideoStreamIndex(_ui.boxVideoStreams->checkedId());
        _task->inputFile()->setSelectedAudioStreamIndex(_ui.boxAudioStreams->checkedId());
        _task->inputFile()->setSelectedSubtitleStreamIndex(_ui.boxSubtitleStreams->checkedId());
    }

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
        else if (_task != 0) {
            _task->inputFile()->setExternalSubtitleFileName(subtitleFile);
        }
    }

    // Enable / disable output types based on what is possible.
    enableOutputTypes();
}


//-----------------------------------------------------------------------------
// Invoked when new media information is available on the input file.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::inputFileFormatChanged()
{
    // We update streams and widget ourselves, prevent inputStreamSelectionUpdated() from doing it in parallel.
    QtlIncrement<int> updating(&_updatingSelections);

    // Clear previous stream info.
    clearInputStreamInfo();

    // Loop on all input streams.
    const int streamCount = _task == 0 ? 0 : _task->inputFile()->streamCount();
    for (int si = 0; si < streamCount; ++si) {

        const QtlMovieStreamInfoPtr stream(_task->inputFile()->streamInfo(si));
        Q_ASSERT(!stream.isNull());
        _log->debug(tr("Found stream: %1").arg(stream->description(false)));

        QtlButtonGrid* box = 0;
        bool selected = false;

        // Select group box and widget type based on stream type.
        switch (stream->streamType()) {
        case QtlMovieStreamInfo::Video:
            box = _ui.boxVideoStreams;
            selected = si == _task->inputFile()->selectedVideoStreamIndex();
            break;
        case QtlMovieStreamInfo::Audio:
            box = _ui.boxAudioStreams;
            selected = si == _task->inputFile()->selectedAudioStreamIndex();
            break;
        case QtlMovieStreamInfo::Subtitle:
            box = _ui.boxSubtitleStreams;
            selected = si == _task->inputFile()->selectedSubtitleStreamIndex();
            break;
        case QtlMovieStreamInfo::Other:
            // Ignore these streams.
            break;
        default:
            _log->line(tr("Internal error, invalid stream type"));
            break;
        }

        if (box != 0) {
            // This is an audio/video/subtitle stream, create a radio button.
            QRadioButton* radio = new QRadioButton(stream->description(true), box);
            box->setButtonId(radio, si);
            radio->setToolTip(stream->description(false));
            radio->setChecked(selected);

            // Update the input file object when the stream is selected.
            connect(radio, &QRadioButton::toggled, this, &QtlMovieEditTask::inputStreamSelectionUpdated);
        }
    }

    // Enable / disable output types based on what is possible.
    enableOutputTypes();

    // Reset the forced DAR.
    resetForcedDisplayAspectRatio();
}


//-----------------------------------------------------------------------------
// Invoked when the edition of the output file name changed in the edit box.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::outputFileNameEdited()
{
    if (_task != 0) {
        // Set output file name from its edit box.
        _task->outputFile()->setFileName(_ui.editOutputFile->text());
    }
}


//-----------------------------------------------------------------------------
// Invoked when the output file name has changed.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::outputFileNameChanged(const QString& fileName)
{
    // Set the output file name in the edit box.
    if (_ui.editOutputFile->text() != fileName) {
        _ui.editOutputFile->setText(fileName);
    }
}


//-----------------------------------------------------------------------------
// Invoked when the edition of the input file name changed in the edit box.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::inputFileNameEdited()
{
    if (_task != 0) {
        // Set input file name from its edit box.
        _task->inputFile()->setFileName(_ui.editInputFile->text());
    }
}


//-----------------------------------------------------------------------------
// Invoked when the input file name has changed.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::inputFileNameChanged(const QString& fileName)
{
    // Set the input file name in the edit box.
    if (_ui.editInputFile->text() != fileName) {
        _ui.editInputFile->setText(fileName);
    }

    // Clear stream information in input file.
    clearInputStreamInfo();

    // Select default output file name and window title.
    if (fileName.isEmpty()) {
        setWindowTitle("QtlMovie");
        if (_task != 0) {
            _task->outputFile()->setFileName("");
        }
    }
    else {
        setWindowTitle(QFileInfo(fileName).fileName() + " - QtlMovie");
        if (_task != 0) {
            _task->outputFile()->setDefaultFileName(fileName);
        }
    }

    // Clear subtitle file name.
    _ui.editSubtitleFile->clear();
}


//-----------------------------------------------------------------------------
// Enable or disable the various output types based on the input file.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::enableOutputTypes()
{
    // Keep track of first allowed output type and currently selected button.
    QAbstractButton* firstAllowed = 0;
    QAbstractButton* current = 0;

    // List of output types in the "display order".
    foreach (QtlMovieOutputFile::OutputType type, QtlMovieOutputFile::outputTypes()) {
        // Get the corresponding button.
        QAbstractButton* button = _ui.boxOutputTypes->buttonOf(int(type));
        if (button != 0) {
            // Is this output type allowed for the input file?
            const bool allowed = _task != 0 && QtlMovieJob::canTranscode(_task->inputFile(), type);
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
// Reset the content of the "forced display aspect ratio" with the
// actual DAR of the selected video stream.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::resetForcedDisplayAspectRatio()
{
    _ui.editForceDAR->clear();

    if (_task != 0) {
        // Update the "Forced DAR" editable value with actual video DAR.
        const QtlMovieStreamInfoPtr video(_task->inputFile()->selectedVideoStreamInfo());
        if (!video.isNull()) {
            _ui.editForceDAR->setText(video->displayAspectRatioString(true, true));
        }

        // Do not change video DAR in output file.
        _task->outputFile()->setForcedDisplayAspectRatio(0.0);
    }
}


//-----------------------------------------------------------------------------
// Invoked when the "forced display aspect ratio" has been changed.
//-----------------------------------------------------------------------------

void QtlMovieEditTask::forcedDarChanged()
{
    if (_task != 0) {
        _task->outputFile()->setForcedDisplayAspectRatio(_ui.checkForceDAR->isChecked() ? qtlToFloat(_ui.editForceDAR->text()) : 0.0);
    }
}
