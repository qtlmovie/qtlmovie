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
//! A subclass of QDialog which implements the UI for the "Audio Test" action.
//! The design of the UI is done using Qt Designer.
//!
class QtlMovieAudioTestDialog : public QDialog
{
#if !defined (DOXYGEN)
    Q_OBJECT
#endif

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
                            const QtlMovieSettings* settings,
                            QtlLogger* log,
                            QWidget *parent = 0);

    //!
    //! Check if the audio test is currently playing.
    //! @return True if the audio test is currently playing.
    //!
    bool playing() const
    {
        return _process != 0 || _audio != 0;
    }

public slots:
    //!
    //! Start the audio play.
    //!
    void start();
    //!
    //! Stop the audio play.
    //!
    void stop();

private slots:
    //!
    //! Invoked by the Start/Stop button.
    //!
    void startStop();
    //!
    //! Invoked by the volume slider when it is changed.
    //!
    void updateVolume(int value);
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
    //! Invoked when the FFmpeg process completes.
    //! @param [in] success True when the process completed successfully, false otherwise.
    //!
    void ffmpegCompleted(bool success);

protected:
    //!
    //! Event handler to handle window close.
    //! @param event Notified event.
    //!
    virtual void closeEvent(QCloseEvent* event);
    //!
    //! Event handler to handle timer.
    //! @param event Notified event.
    //!
    virtual void timerEvent(QTimerEvent* event);

private:
    Ui::QtlMovieAudioTestDialog _ui;           //!< UI from Qt Designer.
    const QtlMovieInputFile*    _file;         //!< Input file containing audio streams to test.
    const QtlMovieSettings*     _settings;     //!< Application settings.
    QtlLogger*                  _log;          //!< Message logger.
    QtlMovieFFmpegProcess*      _process;      //!< FFmpeg process.
    QAudioOutput*               _audio;        //!< Audio output device.
    int                         _startSecond;  //!< Starting time of current audio play.
    bool                        _closePending; //!< When true, close the dialog asap.
    int                         _pollTimerId;  //!< Polling timer id.

    //!
    //! Perform termination operations if the process and the audio output engine are completed.
    //!
    void cleanup();

    // Unaccessible operations.
    QtlMovieAudioTestDialog() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieAudioTestDialog)
};

#endif // QTLMOVIEAUDIOTESTDIALOG_H
