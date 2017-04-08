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
//
// Qtl, Qt utility library.
// Definition of utilities for QFileDialog.
//
//----------------------------------------------------------------------------

#include "QtlFileDialogUtils.h"
#include "QtlFile.h"


//----------------------------------------------------------------------------
// Select a directory and set a QLineEdit with the selected value.
//----------------------------------------------------------------------------

void qtlBrowseDirectory(QWidget* parent, QLineEdit* edit, const QString& title, const QString& defaultInitial)
{
    // Ask the user to select a directory.
    const QString current(edit->text());
    const QString initial(current.isEmpty()? defaultInitial : current);
    const QString selected(QFileDialog::getExistingDirectory(parent, title, initial));

    // Set the new file name.
    if (!selected.isEmpty()) {
        edit->setText(QtlFile::absoluteNativeFilePath(selected));
    }
}


//----------------------------------------------------------------------------
// Select an existing file and set a QLineEdit with the selected value.
//----------------------------------------------------------------------------

void qtlBrowseFile(QWidget* parent, QLineEdit* edit, const QString& title, const QString& defaultInitial, const QString& filters)
{
    // Ask the user to select an input file.
    const QString current(edit->text());
    const QString initial(current.isEmpty()? defaultInitial : current);
    const QString selected(QFileDialog::getOpenFileName(parent, title, initial, filters));

    // Set the new file name.
    if (!selected.isEmpty()) {
        edit->setText(QtlFile::absoluteNativeFilePath(selected));
    }
}
