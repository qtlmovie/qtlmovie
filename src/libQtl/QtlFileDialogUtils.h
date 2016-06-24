//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2016, Thierry Lelegard
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
//! @file QtlFileDialogUtils.h
//!
//! Declare some utilities functions for QFileDialog.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLFILEDIALOGUTILS_H
#define QTLFILEDIALOGUTILS_H

#include "QtlCore.h"

//!
//! Select a directory and set a QLineEdit with the selected value.
//! Typically invoked by "Browse..." buttons for a directory.
//!
//! @param [in,out] edit Line edit to read and write.
//! @param [in] title Browse window title.
//! @param [in] title Default directory to start from if @a edit is empty.
//! The user's home directory by default.
//! @param [in] parent Optional parent widget.
//! If not 0, the dialog will be shown centered over the parent widget.
//!
void qtlBrowseDirectory(QWidget* parent,
                        QLineEdit* edit,
                        const QString& title,
                        const QString& defaultInitial = QDir::homePath());

//!
//! Select an existing file and set a QLineEdit with the selected value.
//! Typically invoked by "Browse..." buttons for a file.
//! @param [in,out] edit Line edit to read and write.
//! @param [in] title Browse window title.
//! @param [in] title Default file path to start from if @a edit is empty.
//! The user's home directory by default.
//! @param [in] filters File filters, see the documentation of QFileDialog::getOpenFileName().
//! @param [in] parent Optional parent widget.
//! If not 0, the dialog will be shown centered over the parent widget.
//!
void qtlBrowseFile(QWidget* parent,
                   QLineEdit* edit,
                   const QString& title,
                   const QString& defaultInitial = QDir::homePath(),
                   const QString& filters = QString());

#endif // QTLFILEDIALOGUTILS_H
