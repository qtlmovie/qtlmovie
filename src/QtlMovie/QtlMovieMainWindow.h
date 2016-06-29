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
#include "QtlMovieMainWindowBase.h"
#include "QtlMovieInputFile.h"
#include "QtlMovieOutputFile.h"
#include "QtlMovieJob.h"

//!
//! A subclass of QMainWindow which implements the UI for the application.
//! The design of the UI is done using Qt Designer.
//!
class QtlMovieMainWindow : public QtlMovieMainWindowBase
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //! @param [in] initialFileNames Optional initial input files, typically from the command line.
    //! @param [in] logDebug If true, set initial log mode to debug.
    //!
    explicit QtlMovieMainWindow(QWidget *parent = 0, const QStringList& initialFileNames = QStringList(), bool logDebug = false);

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
    //! Abort the transcoding, using a slot to defer the abort when back in the event loop.
    //!
    void deferredAbort();
    //!
    //! Invoked by the "DVD Extraction..." button.
    //!
    void startDvdExtraction();
    //!
    //! Invoked by the "Switch to {Single File|Batch} Mode" button.
    //!
    void switchMode();
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

private:
    //!
    //! Check if an encoding is currently in progress and propose to abort it.
    //! If the user accept to abort it, start the cancelation. The completion
    //! of the cancelation will be notified later by transcodingStopped().
    //! @return Cancel status.
    //!
    virtual CancelStatus proposeToCancel();

    //!
    //! Apply the settings which affect the UI.
    //!
    virtual void applyUserInterfaceSettings() Q_DECL_OVERRIDE;

    //!
    //! Update the UI when transcoding starts or stops.
    //! @param [in] started True if transcoding starts, false if transcoding stops.
    //!
    void transcodingUpdateUi(bool started);

    //!
    //! Create and start a new job.
    //! @param [in] task The task to execute in the job.
    //! @return True on success, false on error.
    //!
    bool startNewJob(QtlMovieTask* task);

    Ui::QtlMovieMainWindow _ui;                 //!< UI from Qt Designer.
    QtlMovieJob*           _job;                //!< Current transcoding job.
    bool                   _batchMode;          //!< The UI is currently in batch mode (ie not single file mode).
    bool                   _restartRequested;   //!< A restart of the application is requested.

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlMovieMainWindow)
};

#endif // QTLMOVIEMAINWINDOW_H
