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
//! @file QtlMovieAboutMediaTools.h
//!
//! Declare the class QtlMovieAboutMediaTools, an instance of the UI for the
//! "About Media Tools" action.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEABOUTMEDIATOOLS_H
#define QTLMOVIEABOUTMEDIATOOLS_H

#include "ui_QtlMovieAboutMediaTools.h"
#include "QtlMovieSettings.h"

//!
//! A subclass of QDialog which implements the UI for the "About Media Tools" action.
//! The design of the UI is done using Qt Designer.
//!
class QtlMovieAboutMediaTools : public QDialog
{
#if !defined (DOXYGEN)
    Q_OBJECT
#endif

public:
    //!
    //! Constructor.
    //! @param [in] settings The settings from which to load the initial values.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlMovieAboutMediaTools(const QtlMovieSettings* settings, QWidget* parent = 0);

private:
    Ui::QtlMovieAboutMediaTools _ui;   //!< UI from Qt Designer.

    //!
    //! Build an HTML title label for a media tool.
    //! @param [in] file Executable file for the media tool.
    //! @return HTML string.
    //!
    static QString title(const QtlMovieExecFile* file);

    // Unaccessible operations.
    QtlMovieAboutMediaTools() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieAboutMediaTools)
};

#endif // QTLMOVIEABOUTMEDIATOOLS_H
