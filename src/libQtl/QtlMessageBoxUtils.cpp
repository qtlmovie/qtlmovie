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
// Definition of utilities for QMessageBox.
//
//----------------------------------------------------------------------------

#include "QtlMessageBoxUtils.h"


//-----------------------------------------------------------------------------
// Ask a question with Yes/No buttons (default to No).
//-----------------------------------------------------------------------------

bool qtlConfirm(QObject* parent, const QString& question, const QString& title)
{
    return QMessageBox::Yes ==
        QMessageBox::warning(qtlWidgetAncestor(parent),
                             title,
                             question,
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No);
}


//-----------------------------------------------------------------------------
// Report an error message.
//-----------------------------------------------------------------------------

void qtlError(QObject* parent, const QString& message, const QString& title)
{
    QMessageBox::critical(qtlWidgetAncestor(parent), title, message);
}


//-----------------------------------------------------------------------------
// Display a simple "about" box similar to QMessageBox::about() but add a specific icon.
//-----------------------------------------------------------------------------

void qtlAbout(QObject* parent, const QString& title, const QString& iconPath, const QString& text)
{
    // Create a message box. We cannot use QMessageBox::about() since we want a specific icon.
    QMessageBox* box = new QMessageBox(qtlWidgetAncestor(parent));
    box->setAttribute(Qt::WA_DeleteOnClose); // automatic delete when closed.
    box->setStandardButtons(QMessageBox::Ok);
    box->setWindowTitle(title);
    box->setText(text);

    // Get and set icon.
    if (!iconPath.isEmpty()) {
        QPixmap pixmap(iconPath);
        if (!pixmap.isNull()) {
            box->setIconPixmap(pixmap);
        }
    }

    // Display the message box.
    box->exec();
}
