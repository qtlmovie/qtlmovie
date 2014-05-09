//----------------------------------------------------------------------------
//
// Copyright(c) 2013, Thierry Lelegard
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
// CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//!
//! @file QtlPasswordDialog.h
//!
//! Declare the class QtlPasswordDialog.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLPASSWORDDIALOG_H
#define QTLPASSWORDDIALOG_H

#include <QtWidgets>

//!
//! A subclass of QDialog which implements the UI for entering a password.
//!
class QtlPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] target The name of the target object for which a password is entered.
    //! @param [in] createPassword If true, this creates a new passord with verification.
    //! @param [in] emptyAllowed If true, an empty password is allowed.
    //! @param [in] parent Optional parent widget.
    //!
    QtlPasswordDialog(const QString& target, bool createPassword, bool emptyAllowed, QWidget* parent = 0);

    //!
    //! Get the entered password.
    //! @return The entered password.
    //!
    QString password() const
    {
        return _password;
    }

public slots:
    //!
    //! Invoked when OK is clicked.
    //! Reimplemented from QDialog.
    //!
    virtual void accept();

protected:
    //!
    //! Invoked when the dialog is shown.
    //! Reimplemented from superclass.
    //! @param [in] event The show event.
    //!
    virtual void showEvent(QShowEvent* event);
    //!
    //! Invoked when a key is pressed.
    //! Reimplemented from superclass.
    //! @param [in] event The key event.
    //!
    virtual void keyPressEvent(QKeyEvent* event);

private:
    bool       _emptyAllowed;  //!< If true, an empty password is allowed.
    QLineEdit* _editPassword;  //!< Edit box for password.
    QLineEdit* _editVerify;    //!< Optional password verification edit box.
    QString    _password;      //!< The entered password.

    // Unaccessible operations.
    QtlPasswordDialog() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlPasswordDialog)
};

#endif // QTLPASSWORDDIALOG_H
