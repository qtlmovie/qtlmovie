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
//! @file QtlMovieMainWindow.h
//!
//! Declare the class QtlMovieMainWindow, an instance of the UI for the
//! QtlMovie application.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEMAINWINDOW_H
#define QTLMOVIEMAINWINDOW_H

#include "ui_QtlMovieMainWindow.h"
#include "QtlLogger.h"
#include "QtlMovieJob.h"
#include "QtlMovieInputFile.h"
#include "QtlMovieOutputFile.h"
#include "QtlMovieSettings.h"
#include <QSoundEffect>

#if defined(QTL_WINEXTRAS)
#include <QtWinExtras>
#endif

//!
//! A subclass of QMainWindow which implements the UI for the application.
//! The design of the UI is done using Qt Designer.
//!
class QtlMovieMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //! @param [in] initialFileName Optional initial input file, typically from the command line.
    //!
    explicit QtlMovieMainWindow(QWidget *parent = 0, const QString& initialFileName = QString());

    //!
    //! Check if a restart of the application was requested by the user.
    //! @return True if the application shall be restarted.
    //!
    bool restartRequested() const
    {
        return _restartRequested;
    }

private slots:
    //!
    //! Invoked by the "Transcode ..." buttons.
    //!
    void startTranscoding();
    //!
    //! Invoked by the "Cancel ..." buttons.
    //!
    void cancelTranscoding();
    //!
    //! Invoked by the "DVD Decrypter..." button.
    //!
    void startDvdDecrypter();
    //!
    //! Invoked by the "Settings..." button.
    //!
    void editSettings();
    //!
    //! Invoked by the "Switch to {Single File|Batch} Mode" button.
    //!
    void switchMode();
    //!
    //! Invoked by the "About Media Tools" button.
    //!
    void aboutMediaTools();
    //!
    //! Invoked by the "About" button.
    //!
    void about();
    //!
    //! Invoked by the "Help" button.
    //!
    void showHelp();
    //!
    //! Invoked by the "Release Notes" button.
    //!
    void showReleaseNotes();
    //!
    //! Invoked by the "Search New Version" button.
    //! @param [in] silent Do not report errors or no new version.
    //!
    void searchNewVersion(bool silent = false);
    //!
    //! Invoked when transcoding starts.
    //!
    void transcodingStarted();
    //!
    //! Invoked when transcoding stops.
    //! @param [in] success Indicates whether the transcoding succeeded or failed.
    //!
    void transcodingStopped(bool success);
    //!
    //! Invoked when some progress is made in the transcoding process.
    //! @param [in] description The description of the current process.
    //! @param [in] current Current value (whatever unit it means).
    //! @param [in] maximum Value indicating full completion.
    //! If zero, we cannot evaluate the progress.
    //! @param [in] elapsedSeconds Elapsed seconds since the process started.
    //! @param [in] remainingSeconds Estimated remaining seconds to process.
    //! Negative if the remaining time cannot be estimated.
    //!
    void transcodingProgress(const QString& description, int current, int maximum, int elapsedSeconds, int remainingSeconds);

protected:
    //!
    //! Event handler to handle window close.
    //! @param event Notified event.
    //!
    virtual void closeEvent(QCloseEvent* event);

private:
    Ui::QtlMovieMainWindow _ui;                 //!< UI from Qt Designer.
    QtlMovieSettings*      _settings;           //!< Global settings.
#if defined(QTL_WINEXTRAS)
    QWinTaskbarButton*     _taskbarButton;      //!< The application button in the Windows task bar.
    QWinTaskbarProgress*   _taskbarProgress;    //!< The progress indicator in the Windows task bar.
#endif
    QtlMovieJob*           _job;                //!< Current transcoding job.
    QSoundEffect           _sound;              //!< Sound player for notification.
    bool                   _batchMode;          //!< The UI is currently in batch mode (ie not single file mode).
    bool                   _closePending;       //!< Close the application as soon as possible.
    bool                   _restartRequested;   //!< A restart of the application is requested.

    //!
    //! Start the first transcoding in the list.
    //! In case of failure, clear the complete list of transcodings.
    //! @return True if the transcoding started successfully, false otherwise.
    //!
    bool startFirstTranscoding();

    //!
    //! The type of transcoding cancelation.
    //!
    enum CancelStatus {
        NothingToCancel,  //!< No transcoding in progress, nothing to cancel.
        CancelInProgress, //!< Cancel accepted and in progress.
        CancelRefused     //!< Transcoding in progress, refuse to cancel it.
    };

    //!
    //! Check if an encoding is currently in progress and propose to abort it.
    //! If the user accept to abort it, start the cancelation. The completion
    //! of the cancelation will be notified later by transcodingStopped().
    //! @return Cancel status.
    //!
    CancelStatus proposeToCancelTranscoding();

    //!
    //! Apply the settings which affect the UI.
    //!
    void applyUiSettings();

    //!
    //! Update the UI when transcoding starts or stops.
    //! @param [in] started True if transcoding starts, false if transcoding stops.
    //!
    void transcodingUpdateUi(bool started);

    //!
    //! Create and start a new job.
    //! @param [in] task The task to execute in the job.
    //! @return The new job or zero on error.
    //!
    QtlMovieJob* newJob(QtlMovieTask* task);

    //!
    //! Start to play the notification sound.
    //!
    void playNotificationSound();

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlMovieMainWindow)
};

#endif // QTLMOVIEMAINWINDOW_H
