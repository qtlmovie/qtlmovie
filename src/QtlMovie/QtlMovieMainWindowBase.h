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
//! @file QtlMovieMainWindowBase.h
//!
//! Declare the class QtlMovieMainWindowBase, the base class for UI's.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEMAINWINDOWBASE_H
#define QTLMOVIEMAINWINDOWBASE_H

#include "QtlStdoutLogger.h"
#include "QtlMovieSettings.h"
#include <QSoundEffect>

#if defined(QTL_WINEXTRAS)
#include <QtWinExtras>
#endif

//!
//! A subclass of QMainWindow, used as intermediate base for the main UI's in the application.
//!
class QtlMovieMainWindowBase : public QMainWindow
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //! @param [in] logDebug If true, set initial log mode to debug.
    //!
    explicit QtlMovieMainWindowBase(QWidget *parent = 0, bool logDebug = false);

    //!
    //! Destructor.
    //!
    virtual ~QtlMovieMainWindowBase();

    //!
    //! Get the global logger of the application.
    //! @return The logger, never null.
    //!
    QtlLogger* log() const
    {
        return _log;
    }

    //!
    //! Check if the application is about to close.
    //! Typically, this is after a close action on the window, the subclass
    //! started to abort the processing and we are waiting for the abort
    //! processing to complete.
    //! @return True if the application is about to close.
    //!
    bool closePending() const
    {
        return _closePending;
    }

    //!
    //! Get the global settings of the application.
    //! @return The global settings of the application.
    //! Return a null pointer before the subclass calls setupUserInterface().
    //!
    QtlMovieSettings* settings() const
    {
        return _settings;
    }

    //!
    //! Start to play the notification sound.
    //!
    void playNotificationSound();

    //!
    //! Set the task bar in the application's icon visible (or not).
    //! @param [in] visible Whether or not set task bar should be visible.
    //!
    void setIconTaskBarVisible(bool visible);

    //!
    //! Set the value of the task bar in the application's icon.
    //! @param [in] value The value to set, between zero and @a maximum.
    //! @param [in] maximum Maximum value.
    //!
    void setIconTaskBarValue(int value, int maximum);

public slots:
    //!
    //! Invoked by the "Settings..." button.
    //!
    void editSettings();
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
    //! Invoked by the "Home Page" button.
    //!
    void openHomePage();
    //!
    //! Invoked by the "Support & Bug Report" button.
    //!
    void openSupportPage();

protected:
    //!
    //! Finalize the setup of the user interface.
    //! Should be called by the constructor of a subclass after the UI is created.
    //! @param [in] log Optional log window in the subclass application.
    //! @param [in] aboutQt Optional menu action "About Qt" in the application.
    //!
    void setupUserInterface(QtlLogger* log, QAction* aboutQt);

    //!
    //! Apply the settings which affect the UI.
    //! Invoked when the settings are loaded or changed.
    //! Should be reimplemented by subclasses.
    //! @param [in] initial If true, the object is being constructed.
    //! If false, this is typically following the edition of settings.
    //!
    virtual void applyUserInterfaceSettings(bool initial)
    {
    }

    //!
    //! The type of cancelation.
    //!
    enum CancelStatus {
        NothingToCancel,  //!< Nothing in progress, nothing to cancel.
        CancelInProgress, //!< Cancel accepted and in progress.
        CancelRefused     //!< Something in progress, refuse to cancel it.
    };

    //!
    //! Check if some work is currently in progress and propose to abort it.
    //! Must be reimplemented by subclasses.
    //! @return Cancel status.
    //!
    virtual CancelStatus proposeToCancel() = 0;

    //!
    //! Event handler to handle window close.
    //! @param event Notified event.
    //!
    virtual void closeEvent(QCloseEvent* event);

private:
#if defined(QTL_WINEXTRAS)
    QWinTaskbarButton*   _taskbarButton;   //!< The application button in the Windows task bar.
    QWinTaskbarProgress* _taskbarProgress; //!< The progress indicator in the Windows task bar.
#endif
    QtlMovieSettings*    _settings;        //!< Global settings.
    QtlStdoutLogger      _defaultLog;      //!< Default logger, until set by application.
    QtlLogger*           _log;             //!< Application logger, typically a text window.
    QSoundEffect         _sound;           //!< Sound player for notification.
    bool                 _closePending;    //!< Close the application as soon as possible.

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlMovieMainWindowBase)
};

#endif // QTLMOVIEMAINWINDOWBASE_H
