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
//!
//! @file QtlMovieAudioTestDialog.h
//!
//! Declare the class QtlMovieAudioTestDialog.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEAUDIOTESTDIALOG_H
#define QTLMOVIEAUDIOTESTDIALOG_H

#include "ui_QtlMovieAudioTestDialog.h"
#include "QtlMovieInputFile.h"
#include "QtlLogger.h"
#include "QtlMovieSettings.h"
#include "QtlMovieFFmpegProcess.h"
#include <QAudio>
#include <QAudioOutput>

//!
//! A subclass of QtlDialog which implements the UI for the "Audio Test" action.
//! The design of the UI is done using Qt Designer.
//!
class QtlMovieAudioTestDialog : public QtlDialog
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] inputFile Input file containing audio streams to test.
    //! @param [in] selectedStream Stream index of initially selected stream.
    //! @param [in] settings Application settings.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieAudioTestDialog(const QtlMovieInputFile* inputFile,
                            int selectedStream,
                            QtlMovieSettings* settings,
                            QtlLogger* log,
                            QWidget* parent = 0);

    //!
    //! Destructor.
    //!
    virtual ~QtlMovieAudioTestDialog();

public slots:
    //!
    //! Invoked by the Start/Stop button.
    //!
    void startStop();
    //!
    //! Invoked by the volume slider when it is changed.
    //!
    void updateVolume(int value);

private slots:
    //!
    //! Periodically invoked by the audio output engine to update the play slider.
    //!
    void updatePlaySlider();
    //!
    //! Invoked by the audio output engine when it changes its state.
    //! @param [in] audioState New state of the audio output engine.
    //!
    void audioStateChanged(QAudio::State audioState);
    //!
    //! Invoked when audio data is available from ffmpeg output.
    //!
    void ffmpegDataReady();
    //!
    //! Invoked when the FFmpeg process terminates.
    //!
    void processCompleted();
    //!
    //! Periodically invoked after the process completion to check if the audio
    //! processing is complete.
    //!
    void audioCompletionCheck();

private:
    //!
    //! Define the three states of the audio play.
    //!
    enum AudioTestState {
        Started,   //!< Audio play in progress.
        Stopping,  //!< Audio stop requested ("Stop" button), waiting for processing completion.
        Stopped    //!< Audio fully stopped.
    };

    Ui::QtlMovieAudioTestDialog _ui;             //!< UI from Qt Designer.
    const QtlMovieInputFile*    _file;           //!< Input file containing audio streams to test.
    QtlMovieSettings*           _settings;       //!< Application settings.
    QtlLogger*                  _log;            //!< Message logger.
    QtlMovieFFmpegProcess*      _process;        //!< FFmpeg process.
    QAudioFormat                _audioFormat;    //!< Audio format used in the test.
    QAudioOutput*               _audio;          //!< Audio output device.
    QTimer                      _endTimer;       //!< A timer used after FFmpeg process completion.
    AudioTestState              _state;          //!< The state of the audio play.
    int                         _startSecond;    //!< Starting time of current audio play.
    qint64                      _processedUSecs; //!< Processed audio time at last timer.

    //!
    //! Start the audio play.
    //!
    void start();

    //!
    //! Stop the audio play.
    //!
    void stop();

    //!
    //! Perform termination operations when the process and the audio output engine are completed.
    //!
    void cleanup();

    // Unaccessible operations.
    QtlMovieAudioTestDialog() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieAudioTestDialog)
};

#endif // QTLMOVIEAUDIOTESTDIALOG_H
