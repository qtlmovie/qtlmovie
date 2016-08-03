//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
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
//! @file QtlMovieDvdExtractionWindow.h
//!
//! Declare the class QtlMovieDvdExtractionWindow, an instance of
//! the UI for the DVD Ripper in QtlMovie application.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEDVDEXTRACTIONWINDOW_H
#define QTLMOVIEDVDEXTRACTIONWINDOW_H

#include "ui_QtlMovieDvdExtractionWindow.h"
#include "QtlMovieMainWindowBase.h"
#include "QtlMovieDvdExtractionSession.h"
#include "QtlFileDialogUtils.h"

//!
//! A subclass of QMainWindow which implements the UI for the DVD Ripper application.
//! The design of the UI is done using Qt Designer.
//!
class QtlMovieDvdExtractionWindow : public QtlMovieMainWindowBase
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //! @param [in] logDebug If true, set initial log mode to debug.
    //!
    explicit QtlMovieDvdExtractionWindow(QWidget *parent = 0, bool logDebug = false);

private slots:
    //!
    //! Invoked by the "Refresh ..." buttons.
    //!
    void refresh();
    //!
    //! Invoked by the "Start ..." buttons.
    //!
    void startExtraction();
    //!
    //! Invoked by the "Cancel ..." buttons.
    //!
    void cancelExtraction();
    //!
    //! Abort the extraction, using a slot to defer the abort when back in the event loop.
    //!
    void deferredAbort();
    //!
    //! Invoked when extraction starts.
    //!
    void extractionStarted();
    //!
    //! Invoked when extraction stops.
    //! @param [in] success Indicates whether the extraction succeeded or failed.
    //!
    void extractionStopped(bool success);
    //!
    //! Invoked when some progress is made in the extraction process.
    //! @param [in] description The description of the current process.
    //! @param [in] current Current value (whatever unit it means).
    //! @param [in] maximum Value indicating full completion.
    //! If zero, we cannot evaluate the progress.
    //! @param [in] elapsedSeconds Elapsed seconds since the process started.
    //! @param [in] remainingSeconds Estimated remaining seconds to process.
    //! Negative if the remaining time cannot be estimated.
    //!
    void extractionProgress(const QString& description, int current, int maximum, int elapsedSeconds, int remainingSeconds);
    //!
    //! Update the label containing the ISO full path.
    //!
    void updateIsoFullPath();
    //!
    //! Invoked by the "Browse..." button for the output directory for DVD extraction.
    //!
    void browseDestination()
    {
        qtlBrowseDirectory(this, _ui.editDestination, tr("DVD extraction directory"));
    }

protected:
    //!
    //! Event handler to handle window close.
    //! @param event Notified event.
    //!
    virtual void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;

private:
    //!
    //! Check if an extraction is currently in progress and propose to abort it.
    //! If the user accept to abort it, start the cancelation. The completion
    //! of the cancelation will be notified later by extractionStopped().
    //! @return Cancel status.
    //!
    virtual CancelStatus proposeToCancel() Q_DECL_OVERRIDE;

    //!
    //! Common setup for the VTS and Files tables.
    //! @param [in] table The table to setup.
    //!
    void setupTable(QTableWidget* table);

    //!
    //! Apply the settings which affect the UI.
    //! @param [in] initial If true, the object is being constructed.
    //!
    virtual void applyUserInterfaceSettings(bool initial) Q_DECL_OVERRIDE;

    //!
    //! Update the UI when extraction starts or stops.
    //! @param [in] started True if extraction starts, false if it stops.
    //!
    void extractionUpdateUi(bool started);

    //!
    //! Get the currently selected DVD.
    //! @return A pointer to the currently selected DVD or a null pointer if none is selected.
    //!
    QtlDvdMediaPtr currentDvd() const;

    //!
    //! Refresh the list of DVD's.
    //!
    void refreshDvdList();

    //!
    //! Refresh the list of video title sets.
    //!
    void refreshVtsList();

    //!
    //! Refresh the list of files.
    //!
    void refreshFilesList();

    //!
    //! Add a file in the current extraction.
    //! @param [in] file File description.
    //!
    void addFileForExtraction(const QtlDvdFile& file);

    //!
    //! Add a tree of files and directories in the table of files.
    //! @param [in] dir Directory description.
    //!
    void addDirectoryTree(const QtlDvdDirectory& dir);

    Ui::QtlMovieDvdExtractionWindow _ui;          //!< UI from Qt Designer.
    QList<QtlDvdMediaPtr>           _dvdList;     //!< List of detected DVD's.
    QtlMovieDvdExtractionSession*   _extraction;  //!< Current extraction.

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlMovieDvdExtractionWindow)
};

#endif // QTLMOVIEDVDEXTRACTIONWINDOW_H
