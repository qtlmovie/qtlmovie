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
// Define the class QtlPasswordDialog.
//
//----------------------------------------------------------------------------

#include "QtlPasswordDialog.h"


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------

QtlPasswordDialog::QtlPasswordDialog(const QString& target, bool createPassword, bool emptyAllowed, QWidget* parent) :
    QDialog(parent),
    _emptyAllowed(emptyAllowed),
    _editPassword(0),
    _editVerify(0),
    _password()
{
    setWindowTitle(tr("Password"));

    // Initial dialog size.
    resize(400, 100);

    // Setup a bold font.
    QFont boldFont;
    boldFont.setBold(true);
    boldFont.setWeight(75);

    // Layout for this dialog. There will be two columns.
    QGridLayout* layout = new QGridLayout(this);
    layout->setVerticalSpacing(12);
    int currentRow = 0;

    // Header label in first row, two columns.
    QLabel* label = new QLabel(target, this);
    label->setFont(boldFont);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label, currentRow++, 0, 1, 2);

    // Password edit box.
    label = new QLabel(tr("Password :"), this);
    layout->addWidget(label, currentRow, 0, 1, 1);
    _editPassword = new QLineEdit(this);
    _editPassword->setEchoMode(QLineEdit::Password);
    layout->addWidget(_editPassword, currentRow++, 1, 1, 1);

    // Optional password verification box.
    if (createPassword) {
        label = new QLabel(tr("Verify :"), this);
        layout->addWidget(label, currentRow, 0, 1, 1);
        _editVerify = new QLineEdit(this);
        _editVerify->setEchoMode(QLineEdit::Password);
        layout->addWidget(_editVerify, currentRow++, 1, 1, 1);
    }

    // Vertical spacer before button box.
    QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(spacer, currentRow++, 0, 1, 2);

    // Bottom button box.
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok, Qt::Horizontal, this);
    layout->addWidget(buttonBox, currentRow++, 0, 1, 2);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QtlPasswordDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QtlPasswordDialog::reject);
}


//-----------------------------------------------------------------------------
// Invoked when the dialog is shown.
//-----------------------------------------------------------------------------

void QtlPasswordDialog::showEvent(QShowEvent* event)
{
    // Ensure that the first password box get the focus.
    _editPassword->setFocus();
}


//-----------------------------------------------------------------------------
// Invoked when a key is pressed.
//-----------------------------------------------------------------------------

void QtlPasswordDialog::keyPressEvent(QKeyEvent* event)
{
    // In "create" mode, the enter key in the "password" edit moves to the "verify" edit instead of closing the dialog.
    if (_editVerify != 0 && _editPassword->hasFocus() && (event->key () == Qt::Key_Enter || event->key () == Qt::Key_Return)) {
        // Move to the verify edit.
        _editVerify->setFocus();
    }
    else {
        // Invoke the superclass for normal event processing.
        QDialog::keyPressEvent(event);
    }
}


//-----------------------------------------------------------------------------
// Invoked when OK is clicked.
//-----------------------------------------------------------------------------

void QtlPasswordDialog::accept()
{
    // Main password.
    const QString password1(_editPassword->text());

    // Check password validity.
    if (!_emptyAllowed && password1.isEmpty()) {
        QMessageBox::warning(this, tr("Password error"), tr("Please enter a password"));
        return;
    }

    // Optional password verification.
    if (_editVerify != 0 && _editVerify->text() != password1) {
        QMessageBox::warning(this, tr("Password error"), tr("Verified password does not match"));
        return;
    }

    // At this stage, we are happy with the password.
    _password = password1;
    QDialog::accept();
}
