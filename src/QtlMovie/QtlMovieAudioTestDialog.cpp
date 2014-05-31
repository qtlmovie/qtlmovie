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
// Define the class QtlMovieAudioTestDialog.
//
//----------------------------------------------------------------------------

#include "QtlMovieAudioTestDialog.h"
#include <QtWidgets>
#include <QAudioFormat>
#include <QAudioDeviceInfo>


//----------------------------------------------------------------------------
// Return the audio format used in the test.
// Format: PCM, 16-bit, signed, little endian, mono, sampled at 44.1 kHz.
//----------------------------------------------------------------------------

QAudioFormat QtlMovieAudioTestDialog::audioFormat()
{
    QAudioFormat format;
    format.setSampleRate(44100);    // Sampled at 44.1 kHz.
    format.setChannelCount(1);      // Mono.
    format.setCodec("audio/pcm");   // PCM samples.
    format.setSampleSize(16);       // 16-bit samples.
    format.setByteOrder(QAudioFormat::LittleEndian);  // Little endian samples.
    format.setSampleType(QAudioFormat::SignedInt);    // Signed int samples.

    // Check that the audio format is supported by the output device.
    QAudioDeviceInfo deviceInfo(QAudioDeviceInfo::defaultOutputDevice());
    if (!deviceInfo.isFormatSupported(format)) {
        format = deviceInfo.nearestFormat(format);
    }

    return format;
}


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieAudioTestDialog::QtlMovieAudioTestDialog(const QtlMovieInputFile* inputFile,
                                                 int selectedStream,
                                                 QtlMovieSettings* settings,
                                                 QtlLogger* log,
                                                 QWidget* parent) :
    QtlDialog(parent),
    _file(inputFile),
    _settings(settings),
    _log(log),
    _process(0),
    _audio(audioFormat()),
    _startSecond(0),
    _started(false),
    _closePending(false)
{
    Q_ASSERT(inputFile != 0);
    Q_ASSERT(log != 0);
    Q_ASSERT(settings != 0);

    // Build the UI as defined in Qt Designer.
    _ui.setupUi(this);

    // Restore the window geometry from the saved settings.
    setGeometrySettings(_settings, true);

    // Loop on all audio streams in input files and display thems.
    const int streamCount = _file->streamCount();
    for (int si = 0; si < streamCount; ++si) {

        // Stream description.
        const QtlMovieStreamInfoPtr stream(_file->streamInfo(si));
        if (!stream.isNull() && stream->streamType() == QtlMovieStreamInfo::Audio) {

            // Create a radio button for the audio stream.
            QRadioButton* radio = new QRadioButton(stream->description(true), _ui.audioStreamsBox);
            radio->setToolTip(stream->description(false));
            _ui.audioStreamsBox->setButtonId(radio, si);

            // Preselect the radio button for the same audio stream as the main window.
            radio->setChecked(si == selectedStream);
        }
    }

    // The play slider values are seconds within the input file.
    _ui.playSlider->setMinimum(0);
    _ui.playSlider->setMaximum(int(_file->durationInSeconds()));

    // Get notifications from the audio output.
    _audio.setNotifyInterval(500); // milliseconds
    connect(&_audio, SIGNAL(notify()), this, SLOT(updatePlaySlider()));
    connect(&_audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(audioStateChanged(QAudio::State)));
}


//-----------------------------------------------------------------------------
// Check if the audio test is currently playing.
//-----------------------------------------------------------------------------

bool QtlMovieAudioTestDialog::playing() const
{
    return _process != 0 || _audio.state() == QAudio::ActiveState;
}


//-----------------------------------------------------------------------------
// Invoked by the Start/Stop button.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::startStop()
{
    if (playing()) {
        stop();
    }
    else {
        start();
    }
}


//-----------------------------------------------------------------------------
// Start the audio play.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::start()
{
    // Skip spurious invocation.
    if (_started || playing()) {
        _log->line(tr("Internal error: test audio decoding already started"));
        return;
    }

    // Get selected audio stream.
    const QtlMovieStreamInfoPtr stream(_file->streamInfo(_ui.audioStreamsBox->checkedId()));
    if (stream.isNull()) {
        _log->line(tr("No audio stream in input file."));
        return;
    }

    // Remember we started.
    _started = true;

    // The starting point in seconds is the slider value.
    _startSecond = _ui.playSlider->value();

    // Set the audio volume from the volume slider.
    _audio.setVolume(qreal(_ui.volumeSlider->value()) / qreal(_ui.volumeSlider->maximum()));

    // Build the FFmpeg command line arguments to extract the selected audio channel
    // and convert it to the above format.
    QStringList args;
    args << "-nostdin"                  // Do not attempt to read from standard input.
         << "-fflags" << "+genpts"      // Make FFmpeg generate PTS (time stamps).
         << "-ss" << QString::number(_startSecond)
         << "-i" << _file->ffmpegInputFileSpecification()
         << "-vn"                       // Suppress video streams.
         << "-map" << stream->ffSpecifier()
         << "-codec:a" << "pcm_s16le"   // Audio format: PCM 16 bits little endian.
         << "-ar" << "44100"            // Resample to 44.1 kHz.
         << "-ac" << "1"                // Remix to one channel (mono).
         << "-f" << "s16le"             // Output file format is raw PCM.
         << "-";                        // Output file is standard output.

    // Create the process object.
    Q_ASSERT(_process == 0);
    _process = new QtlMovieFFmpegProcess(args, _file->durationInSeconds(), QString(), _settings, _log, this);

    // Attempt cleanup as soon as the process completes.
    connect(_process, SIGNAL(completed(bool)), this, SLOT(cleanup()));

    // Start/restart the audio device when audio data are available on ffmpeg output.
    connect(_process->outputDevice(), SIGNAL(readyRead()), this, SLOT(ffmpegDataReady()));

    // Start the process.
    _process->start();

    // The Play button becomes a Stop one.
    _ui.playStopButton->setText(tr("Stop"));

    // Disable user action on the audio stream selection and the play slider.
    _ui.audioStreamsBox->setEnabled(false);
    _ui.playSlider->setEnabled(false);
}


//-----------------------------------------------------------------------------
// Stop the audio play.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::stop()
{
    // Remember we wanted to stop.
    _started = false;

    // Stop the FFmpeg process.
    if (_process != 0) {
        _process->abort();
    }

    // Stop the audio engine.
    _audio.stop();

    // Cleanup when possible. If cleanup is not possible now, it will be
    // when both the process and the audio output signal their completion.
    cleanup();
}


//-----------------------------------------------------------------------------
// Perform termination operations if the process and the audio output engine
// are completed.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::cleanup()
{
    // If not plying or the process or the audio engine are active, cannot cleanup now.
    if (_process == 0 || !_process->isCompleted() || _audio.state() == QAudio::ActiveState) {
        return;
    }

    // Deallocate the process upon return to event loop.
    if (_process != 0) {
        _process->deleteLater();
        _process = 0;
    }

    // The Stop button becomes a Play one again.
    _ui.playStopButton->setText(tr("Play"));

    // Re-enable user action on the audio stream selection and the play slider.
    _ui.audioStreamsBox->setEnabled(true);
    _ui.playSlider->setEnabled(true);

    // If a close request is pending, close this dialog.
    if (_closePending) {
        accept();
    }
}


//-----------------------------------------------------------------------------
// Invoked by the volume slider when it is changed.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::updateVolume(int value)
{
    _audio.setVolume(qreal(value) / qreal(_ui.volumeSlider->maximum()));
}


//-----------------------------------------------------------------------------
// Periodically invoked to update the play slider.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::updatePlaySlider()
{
    // Get the number of seconds from the start of audio processing.
    const int seconds = int(_audio.processedUSecs() / 1000000);

    // Update the slider.
    _ui.playSlider->setValue(_startSecond + seconds);
}


//-----------------------------------------------------------------------------
// Invoked by the audio output engine when it changes its state.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::audioStateChanged(QAudio::State audioState)
{
    switch (audioState) {

    case QAudio::ActiveState:
        // Audio starts playing. nothing to do.
        break;

    case QAudio::IdleState:
    case QAudio::SuspendedState:
        // If the process is completed, stop now.
        if (_process == 0 || _process->isCompleted()) {
            _audio.stop();
        }
        break;

    case QAudio::StoppedState:
        // Report decoding error.
        if (_audio.error() != QAudio::NoError) {
            _log->line(tr("Audio decoding error %1").arg(_audio.error()));
        }
        // Cleanup if necessary.
        cleanup();
        break;

    default:
        _log->line(tr("Invalid audio output state: %1").arg(audioState));
        break;
    }
}


//-----------------------------------------------------------------------------
// Invoked when audio data is available from ffmpeg output.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::ffmpegDataReady()
{
    // Start the audio processing using the standard output of the process as source.
    if (_started && _process != 0 && _audio.state() != QAudio::ActiveState) {
        _audio.start(_process->outputDevice());
    }
}


//-----------------------------------------------------------------------------
// Event handler to handle window close.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::closeEvent(QCloseEvent* event)
{
    // Request to terminate the audio processing.
    stop();

    // If still playing, wait for play termination.
    // It this is the second time we try to close (_closePending is true), force a close anyway.
    if (playing() && !_closePending) {
        _closePending = true;  // Will close asap.
        event->ignore();       // Refuse to close now.
    }
    else {
        // Accept to close the window.
        event->accept();
    }
}
