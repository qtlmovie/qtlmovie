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
//! @file QtlMovieInputFilePropertiesDialog.h
//!
//! Declare the class QtlMovieInputFilePropertiesDialog.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEINPUTFILEPROPERTIESDIALOG_H
#define QTLMOVIEINPUTFILEPROPERTIESDIALOG_H

#include "ui_QtlMovieInputFilePropertiesDialog.h"
#include "QtlMovieInputFile.h"

//!
//! A subclass of QDialog which implements the UI for the "View Input File Properties" action.
//! The design of the UI is done using Qt Designer.
//!
class QtlMovieInputFilePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] inputFile Input file to view.
    //! @param [in] settings Application settings.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtlMovieInputFilePropertiesDialog(const QtlMovieInputFile* inputFile, QtlMovieSettings* settings, QWidget *parent = 0);

protected:
    //!
    //! Event handler to handle window close.
    //! @param event Notified event.
    //!
    virtual void closeEvent(QCloseEvent* event);

private:
    Ui::QtlMovieInputFilePropertiesDialog _ui;        //!< UI from Qt Designer.
    QtlMovieSettings*                     _settings;  //!< Application settings.
};

#endif // QTLMOVIEINPUTFILEPROPERTIESDIALOG_H
