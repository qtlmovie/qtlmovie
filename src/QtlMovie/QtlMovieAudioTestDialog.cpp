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
    _audioFormat(),
    _audio(0),
    _endTimer(),
    _state(Stopped),
    _startSecond(0),
    _processedUSecs(-1)
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
        const QtlMediaStreamInfoPtr stream(_file->streamInfo(si));
        if (!stream.isNull() && stream->streamType() == QtlMediaStreamInfo::Audio) {

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

    // Build the audio format used in the test.
    // Format: PCM, 16-bit, signed, little endian, mono, sampled at 44.1 kHz.
    _audioFormat.setSampleRate(44100);    // Sampled at 44.1 kHz.
    _audioFormat.setChannelCount(1);      // Mono.
    _audioFormat.setCodec("audio/pcm");   // PCM samples.
    _audioFormat.setSampleSize(16);       // 16-bit samples.
    _audioFormat.setByteOrder(QAudioFormat::LittleEndian); // Little endian samples.
    _audioFormat.setSampleType(QAudioFormat::SignedInt);   // Signed int samples.

    // Check that the audio format is supported by the output device.
    QAudioDeviceInfo deviceInfo(QAudioDeviceInfo::defaultOutputDevice());
    if (!deviceInfo.isFormatSupported(_audioFormat)) {
        _audioFormat = deviceInfo.nearestFormat(_audioFormat);
    }

    // Initialize the "end timer". This is a trick to cope with what seems to
    // be a bug in Qt audio. The problem : The QAudioOutput object is supposed
    // to switch to IdleState when its buffer is empty. This is true when
    // its input QIODevice is a plain file. But, when its input is a QProcess,
    // the IdleState is never notified. So, at the end of the FFmpeg output,
    // we are never notified. To cope with that, when the FFmpeg process
    // terminates, we start a periodic timer, this "end timer". Each time
    // the timer is triggered, we check if more audio was processed. When
    // no more audio is processed, we know that we can stop the audio processing.
    _endTimer.setSingleShot(false);
    connect(&_endTimer, &QTimer::timeout, this, &QtlMovieAudioTestDialog::audioCompletionCheck);
}


//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------

QtlMovieAudioTestDialog::~QtlMovieAudioTestDialog()
{
    // Early disconnect from all children to avoid being notified when the
    // children terminate. The potential problem is the following:
    // - This object is destroyed, the steps are:
    //   -- Destroy QtlMovieAudioTestDialog layer.
    //   -- Destroy intermediate subclasses.
    //   -- Destroy QObject layer, which includes:
    //      --- Destroy children objects.
    //      --- The destructions of these children emit signals.
    //      --- This object is notified by these signals but the
    //          QtlMovieAudioTestDialog layer is destroyed => crash.
    // So, we must not be notified by existing children objects.

    if (_audio != 0) {
        disconnect(_audio, 0, this, 0);
    }
    if (_process != 0) {
        disconnect(_process, 0, this, 0);
        disconnect(_process->outputDevice(), 0, this, 0);
    }
}


//-----------------------------------------------------------------------------
// Invoked by the Start/Stop button.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::startStop()
{
    switch (_state) {
        case Stopped:
            start();
            break;
        case Started:
            stop();
            break;
        case Stopping:
            // Can't do anything during this short period.
            break;
    }
}


//-----------------------------------------------------------------------------
// Start the audio play.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::start()
{
    Q_ASSERT(_process == 0);
    Q_ASSERT(_audio == 0);

    // Get selected audio stream.
    const QtlMediaStreamInfoPtr stream(_file->streamInfo(_ui.audioStreamsBox->checkedId()));
    if (stream.isNull()) {
        _log->line(tr("No audio stream in input file."));
        return;
    }

    // Remember we started.
    _state = Started;

    // The starting point in seconds is the slider value.
    _startSecond = _ui.playSlider->value();

    // Allocate the audio output object.
    _audio = new QAudioOutput(_audioFormat, this);

    // Get notifications from the audio output every 500 ms of audio content.
    _audio->setNotifyInterval(500); // milliseconds
    connect(_audio, &QAudioOutput::notify, this, &QtlMovieAudioTestDialog::updatePlaySlider);

    // Get notified when the state of the audio output changes.
    connect(_audio, &QAudioOutput::stateChanged, this, &QtlMovieAudioTestDialog::audioStateChanged);

    // Set the audio volume from the volume slider.
    _audio->setVolume(qreal(_ui.volumeSlider->value()) / qreal(_ui.volumeSlider->maximum()));

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
    _process = new QtlMovieFFmpegProcess(args, _file->durationInSeconds(), QString(), _settings, _log, this, _file->dataPull(this));

    // Attempt cleanup as soon as the process completes.
    connect(_process, &QtlMovieFFmpegProcess::completed, this, &QtlMovieAudioTestDialog::processCompleted);

    // Start/restart the audio device when audio data are available on ffmpeg output.
    connect(_process->outputDevice(), &QIODevice::readyRead, this, &QtlMovieAudioTestDialog::ffmpegDataReady);

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
    Q_ASSERT(_process != 0);
    Q_ASSERT(_audio != 0);

    // Remember we wanted to stop.
    _state = Stopping;

    // Stop the FFmpeg process.
    _process->abort();

    // Stop the audio engine.
    _audio->stop();

    // Cleanup will be triggered when audio processing is complete.
}


//-----------------------------------------------------------------------------
// Perform termination operations if the process and the audio output engine
// have both completed.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::cleanup()
{
    // Filter recursive invocation.
    if (_state == Stopped) {
        _log->debug(tr("cleanup, recursive call, skipped"));
        return;
    }

    Q_ASSERT(_process != 0);
    Q_ASSERT(_audio != 0);

    // Now effectively stopped.
    _log->debug(tr("cleanup, now stopped"));
    _state = Stopped;

    // Free process and audio engine.
    _process->deleteLater();
    _process = 0;
    _audio->deleteLater();
    _audio = 0;

    // Terminate timer, if enabled.
    _endTimer.stop();
    _processedUSecs = -1;

    // The Stop button becomes a Play one again.
    _ui.playStopButton->setText(tr("Play"));

    // Re-enable user action on the audio stream selection and the play slider.
    _ui.audioStreamsBox->setEnabled(true);
    _ui.playSlider->setEnabled(true);
}



//-----------------------------------------------------------------------------
// Invoked when audio data is available from ffmpeg output.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::ffmpegDataReady()
{
    // We can only process a chunk of audio when data is available.
    if (_state == Started &&
        _process != 0 &&
        _audio != 0 &&
        (!_process->isCompleted() || _process->outputDevice()->bytesAvailable() > 0) &&
        _audio->state() != QAudio::ActiveState) {

        // Start the audio processing using the standard output of the process as source.
        _audio->start(_process->outputDevice());
    }
}


//-----------------------------------------------------------------------------
// Invoked when the FFmpeg process terminates.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::processCompleted()
{
    _log->debug(tr("QtlMovieAudioTestDialog::processCompleted"));
    if (_audio != 0) {
        // Start a periodic timer to detect the end of audio rendering.
        // See comments at end of method start().
        _processedUSecs = _audio->processedUSecs();
        _endTimer.start(500); // milliseconds
    }
}


//-----------------------------------------------------------------------------
// Periodically invoked after the process completion to check if the audio
// processing is complete.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::audioCompletionCheck()
{
    // Get total amount of processed audio microseconds.
    const qint64 currentUSecs = _audio != 0 ? _audio->processedUSecs() : _processedUSecs;
    _log->debug(tr("_processedUSecs = %1, _audio->processedUSecs() = %2").arg(_processedUSecs).arg(currentUSecs));

    // Check if there is more than during last timer interrupt.
    if (currentUSecs <= _processedUSecs) {
        // No audio progression since last timer, assume audio completed.
        _endTimer.stop();
        if (_audio != 0) {
            _audio->stop();
        }
        cleanup();
    }

    // Keep current amount of processed audio for next interrupt.
    _processedUSecs = currentUSecs;
}


//-----------------------------------------------------------------------------
// Invoked by the audio output engine when it changes its state.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::audioStateChanged(QAudio::State audioState)
{
    _log->debug(tr("Audio state changed to %1").arg(audioState));

    switch (audioState) {
        case QAudio::ActiveState: {
            // Audio starts playing. nothing to do.
            break;
        }
        case QAudio::IdleState:
        case QAudio::SuspendedState: {
            // If the process is completed, stop now.
            if (_process == 0 || _process->isCompleted()) {
                _audio->stop();
            }
            break;
        }
        case QAudio::StoppedState: {
            // Report decoding error and cleanup (if not already done).
            if (_audio != 0 && _audio->error() != QAudio::NoError) {
                _log->line(tr("Audio decoding error %1").arg(_audio->error()));
            }
            cleanup();
            break;
        }
        default: {
            _log->line(tr("Invalid audio output state: %1").arg(audioState));
            break;
        }
    }
}


//-----------------------------------------------------------------------------
// Invoked by the volume slider when it is changed.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::updateVolume(int value)
{
    // Update the audio volume accordingly.
    if (_audio != 0) {
        _audio->setVolume(qreal(value) / qreal(_ui.volumeSlider->maximum()));
    }
}


//-----------------------------------------------------------------------------
// Periodically invoked by the audio engine to update the play slider.
//-----------------------------------------------------------------------------

void QtlMovieAudioTestDialog::updatePlaySlider()
{
    if (_audio != 0) {
        // Get the number of seconds from the start of audio processing.
        const int seconds = int(_audio->processedUSecs() / 1000000);

        // Update the slider.
        _ui.playSlider->setValue(_startSecond + seconds);
    }
}
