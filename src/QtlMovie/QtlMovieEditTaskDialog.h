//----------------------------------------------------------------------------
//
// Copyright (c) 201, Thierry Lelegard
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
//! @file QtlMovieEditTaskDialog.h
//!
//! Declare the class QtlMovieEditTaskDialog, an instance of the UI for editing a task.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEEDITTASKDIALOG_H
#define QTLMOVIEEDITTASKDIALOG_H

#include "QtlDialog.h"
#include "QtlMovieEditTask.h"

//!
//! A subclass of QtlDialog which implements the UI for the editing a task.
//! It is mainly a QtlDialog containing a QtlMovieEditTask.
//!
class QtlMovieEditTaskDialog : public QtlDialog
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] task The task to edit.
    //! @param [in] settings Application settings.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //!
    QtlMovieEditTaskDialog(QtlMovieTask* task, QtlMovieSettings* settings, QtlLogger* log, QWidget* parent = 0);

private:
    // Unaccessible operations.
    QtlMovieEditTaskDialog() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieEditTaskDialog)
};

#endif // QTLMOVIEEDITTASKDIALOG_H
