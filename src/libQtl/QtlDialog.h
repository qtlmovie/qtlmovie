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
//! @file QtlDialog.h
//!
//! Declare the class QtlDialog.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLDIALOG_H
#define QTLDIALOG_H

#include "QtlCore.h"
#include "QtlGeometrySettingsInterface.h"

//!
//! A subclass of QDialog with additional features.
//!
class QtlDialog : public QDialog
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //! @param [in] flags Widget flags.
    //! @param [in] geometrySettings Where to save and restore the dialog geometry.
    //!
    explicit QtlDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0, QtlGeometrySettingsInterface* geometrySettings = 0);

    //!
    //! Get the object where to save and restore the dialog geometry.
    //! @return The object where to save and restore the dialog geometry. Can be null.
    //!
    QtlGeometrySettingsInterface* geometrySettings() const
    {
        return _geometrySettings;
    }

    //!
    //! Set the object where to save and restore the dialog geometry.
    //! @param [in] geometrySettings The object where to save and restore the dialog geometry. Can be null.
    //! @param [in] restoreImmediately If true and @a geometrySettings is not null, immediately restore the geometry.
    //!
    void setGeometrySettings(QtlGeometrySettingsInterface* geometrySettings, bool restoreImmediately = false);

protected:
    //!
    //! Process an event.
    //! Reimplemented from QObject and QWidget.
    //! @param [in,out] event The event to process.
    //! @return True if the event was recognized and processed.
    //!
    virtual bool event(QEvent* event) Q_DECL_OVERRIDE;

private:
    QtlGeometrySettingsInterface* _geometrySettings;  //! Where to save and restore the dialog geometry.
    int                           _eventRecursion;    //! Recursion counter in event().
    bool                          _geometrySaved;     //! If true, last geometry was saved as _lastSavedGeometry.
    QRect                         _lastSavedGeometry; //! Last saved geometry.

    //!
    //! Restore the geometry from _geometrySettings.
    //!
    void restoreGeometryFromSettings();

    //!
    //! Save the geometry to _geometrySettings.
    //!
    void saveGeometryToSettings();

    // Unaccessible operations.
    QtlDialog() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlDialog)
};

#endif // QTLDIALOG_H
