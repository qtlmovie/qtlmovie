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
//
// Base class for main windows classes for the QtlMovie application.
//
//----------------------------------------------------------------------------

#include "QtlMovieMainWindowBase.h"
#include "QtlMovieVersion.h"
#include "QtlMovie.h"
#include "QtlMovieEditSettings.h"
#include "QtlMovieAboutMediaTools.h"
#include "QtlTextFileViewer.h"
#include "QtlTranslator.h"
#include "QtlSysInfo.h"
#include "QtlStringUtils.h"
#include "QtlMessageBoxUtils.h"


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------

QtlMovieMainWindowBase::QtlMovieMainWindowBase(QWidget *parent, bool logDebug) :
    QMainWindow(parent),
#if defined(QTL_WINEXTRAS)
    _taskbarButton(0),
    _taskbarProgress(0),
#endif
    _settings(0),
    _defaultLog(stderr, logDebug),
    _log(&_defaultLog),
    _sound(),
    _help(),
    _closePending(false)
{
}


//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------

QtlMovieMainWindowBase::~QtlMovieMainWindowBase()
{
    // Any log from the subclass is now invalid.
    _log = &_defaultLog;
}


//-----------------------------------------------------------------------------
// Finalize the setup of the user interface.
//-----------------------------------------------------------------------------

void QtlMovieMainWindowBase::setupUserInterface(QtlLogger* log, QAction* aboutQt)
{
    // Redirect the log to the subclass.
    if (log != 0) {
        _log = log;
    }

    // Connect the "About Qt" action.
    if (aboutQt != 0) {
        connect(aboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    }

    // Create and load settings. Ignore errors (typically default settings file not yet created).
    _settings = new QtlMovieSettings(_log, this);

    // Restore the window geometry from the saved settings.
    _settings->restoreGeometry(this);
    _settings->restoreState(this);

    // Let the subclass finish up settings-dependent configuration of the UI.
    applyUserInterfaceSettings(true);

    // Start a search for a new version online.
    if (_settings->newVersionCheck()) {
        searchNewVersion(true);
    }

#if defined(QTL_WINEXTRAS)
    // On Windows Vista, it has been reported that QtlMovie crashes on exit when using
    // a progress bar on icon. To avoid that, we use it only on Windows 7 and higher.
    if ((QSysInfo::WindowsVersion & QSysInfo::WV_NT_based) != 0 && QSysInfo::WindowsVersion >= QSysInfo::WV_WINDOWS7) {

        // The following sequence is a mystery. If not present, the taskbar progress does not work.
        // But the Qt documentation is not really clear on what this does.
        if (QtWin::isCompositionEnabled()) {
            QtWin::enableBlurBehindWindow(this);
        }
        else {
            QtWin::disableBlurBehindWindow(this);
        }

        // Setup the Windows taskbar button.
        _taskbarButton = new QWinTaskbarButton(this);
        _taskbarButton->setWindow(windowHandle());
        _taskbarProgress = _taskbarButton->progress();
        _taskbarProgress->setRange(0, 1000);
    }
#endif
}


//-----------------------------------------------------------------------------
// Manage the task bar in the application's icon.
//-----------------------------------------------------------------------------

void QtlMovieMainWindowBase::setIconTaskBarVisible(bool visible)
{
#if defined(QTL_WINEXTRAS)
    if (_taskbarProgress != 0) {
        _taskbarProgress->setVisible(visible);
    }
#endif
}

void QtlMovieMainWindowBase::setIconTaskBarValue(int value, int maximum)
{
#if defined(QTL_WINEXTRAS)
    if (_taskbarProgress != 0) {
        _taskbarProgress->setMaximum(maximum);
        _taskbarProgress->setValue(value);
    }
#endif
}


//-----------------------------------------------------------------------------
// Start to play the notification sound.
//-----------------------------------------------------------------------------

void QtlMovieMainWindowBase::playNotificationSound()
{
    // Stop current play (if any).
    _sound.stop();

    // Set new sound source.
    _sound.setSource(QUrl("qrc:/sounds/completion.wav"));

    // Play the sound.
    _sound.setVolume(1.0);
    _sound.play();
}


//-----------------------------------------------------------------------------
// Event handler to handle window close.
//-----------------------------------------------------------------------------

void QtlMovieMainWindowBase::closeEvent(QCloseEvent* event)
{
    if (_closePending) {
        // If _closePending is already true, either this is an explicit close() following the completion
        // of the transcoding cancelation or the cancelation does not work and the user tries to close
        // the window again. In both cases, we accept the close.
        event->accept();
    }
    else {
        // Check if should abort any transcoding.
        switch (proposeToCancel()) {
            case NothingToCancel:
                // It is always safe to close when nothing is in progress.
                event->accept();
                break;
            case CancelRefused:
                // The user refused to cancel. Do not close.
                event->ignore();
                break;
            case CancelInProgress:
                // The user accepted to cancel. However, we wait for the
                // cancelation to complete before closing. Will close later.
                _closePending = true;
                event->ignore();
                break;
            default:
                _log->line(tr("Internal error, invalid cancel state"));
                event->ignore();
                break;
        }
    }

    // If the event is accepted, save the geometry of the window.
    if (event->isAccepted()) {
        _settings->saveGeometry(this);
        _settings->saveState(this);
        _settings->sync();
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Settings..." button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindowBase::editSettings()
{
    // Open an edition dialog with the current settings.
    QtlMovieEditSettings edit(_settings, &_help, this);
    if (edit.exec() == QDialog::Accepted) {
        // Button "OK" has been selected, update settings from the values in the dialog box.
        edit.applySettings();
        applyUserInterfaceSettings(false);
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "About Media Tools" button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindowBase::aboutMediaTools()
{
    QtlMovieAboutMediaTools box(_settings, this);
    box.exec();
}


//-----------------------------------------------------------------------------
// Invoked by the "About" button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindowBase::about()
{
    QMessageBox::about(this,
                       tr("About QtlMovie"),
                       QStringLiteral("<p><b>%1</b>: %2</p>"
                           "<p>%3 %4 (%5)</p>"
                           "<p>Copyright (c) 2013-2016, Thierry Lel&eacute;gard</p>"
                           "<hr/>"
                           "<p><b>Contributions:</b></p>"
                           "<p>Teletext code from Petr Kutalek's Telxcc.<br/>"
                           "Libdvdcss by St&eacute;phane Borel, Sam Hocevar et al.<br/>"
                           "QtlMovie logo by %6.</p>")
                       .arg(qtlHtmlLink(WEBREF_QTLMOVIE_HOME, "QtlMovie"))
                       .arg(tr("A specialized Qt front-end for<br/>FFmpeg and other free media tools"))
                       .arg(tr("Version"))
                       .arg(qtlMovieVersion())
                       .arg(__DATE__)
                       .arg(qtlHtmlLink(WEBREF_DESIGNBOLT, "DesignBolts")));
}


//-----------------------------------------------------------------------------
// Invoked by the "Help" button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindowBase::showHelp()
{
    _help.showHelp(this);
}


//-----------------------------------------------------------------------------
// Invoked by the "Release Notes" button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindowBase::showReleaseNotes()
{
    QtlTextFileViewer box(this,
                          QtlTranslator::searchLocaleFile(":/changelog.txt"),
                          tr("QtlMovie Release Notes"),
                          ":/images/qtlmovie-64.png");
    box.setObjectName("QtlMovieReleaseNotesViewer");
    box.setGeometrySettings(_settings, true);
    box.exec();
}


//-----------------------------------------------------------------------------
// Invoked by the "Search New Version" button.
//-----------------------------------------------------------------------------

void QtlMovieMainWindowBase::searchNewVersion(bool silent)
{
    new QtlMovieNewVersion(silent, true, _settings, _log, this);
}


//-----------------------------------------------------------------------------
// Open external links.
//-----------------------------------------------------------------------------

void QtlMovieMainWindowBase::openHomePage()
{
    QDesktopServices::openUrl(QUrl(WEBREF_QTLMOVIE_HOME));
}

void QtlMovieMainWindowBase::openSupportPage()
{
    QDesktopServices::openUrl(QUrl(WEBREF_QTLMOVIE_SUPPORT));
}
