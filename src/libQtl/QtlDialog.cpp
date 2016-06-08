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
// Qtl, Qt utility library.
// Define the class QtlDialog.
//
//----------------------------------------------------------------------------

#include "QtlDialog.h"
#include "QtlIncrement.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlDialog::QtlDialog(QWidget* parent, Qt::WindowFlags flags, QtlGeometrySettingsInterface* geometrySettings) :
    QDialog(parent, flags),
    _geometrySettings(geometrySettings),
    _eventRecursion(0),
    _geometrySaved(false),
    _lastSavedGeometry()
{
}


//----------------------------------------------------------------------------
// Set the object where to save and restore the dialog geometry.
//----------------------------------------------------------------------------

void QtlDialog::setGeometrySettings(QtlGeometrySettingsInterface* geometrySettings, bool restoreImmediately)
{
    _geometrySettings = geometrySettings;
    _geometrySaved = false;
    if (restoreImmediately) {
        restoreGeometryFromSettings();
    }
}


//----------------------------------------------------------------------------
// Restore the geometry from _geometrySettings.
//----------------------------------------------------------------------------

void QtlDialog::restoreGeometryFromSettings()
{
    if (_geometrySettings != 0) {
        _geometrySettings->restoreGeometry(this);
        _geometrySaved = true;
        _lastSavedGeometry = geometry();
    }
}


//----------------------------------------------------------------------------
// Save the geometry to _geometrySettings.
//----------------------------------------------------------------------------

void QtlDialog::saveGeometryToSettings()
{
    if (_geometrySettings != 0 && (!_geometrySaved || _lastSavedGeometry != geometry())) {
        _geometrySettings->saveGeometry(this);
        _geometrySaved = true;
        _lastSavedGeometry = geometry();
    }
}


//----------------------------------------------------------------------------
// Process an event.
// Reimplemented from QObject and QWidget.
//----------------------------------------------------------------------------

bool QtlDialog::event(QEvent* event)
{
    QtlIncrement<int> recursionGuard(&_eventRecursion);

    switch (event->type()) {
    case QEvent::Show:
        // Dialog is shown on screen, restore previous geometry.
        if (_eventRecursion == 1) {
            restoreGeometryFromSettings();
        }
        break;

    case QEvent::Move:
    case QEvent::Resize:
    case QEvent::Quit:
    case QEvent::Close:
    case QEvent::Hide:
        // Dialog is closed or its geometry changed, save the geometry of the dialog.
        if (_eventRecursion == 1) {
            saveGeometryToSettings();
        }
        break;

    default:
        // Other events are irrelevant.
        break;
    }

    // Finally let the superclass handle the event.
    return QDialog::event(event);
}
