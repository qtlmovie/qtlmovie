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
//! @file QtlBrowserDialog.h
//!
//! Declare the class QtlBrowserDialog.
//!
//----------------------------------------------------------------------------

#ifndef QTLBROWSERDIALOG_H
#define QTLBROWSERDIALOG_H

#include "QtlCore.h"
#include "QtlDialog.h"

// UI from Qt Designer.
namespace Ui {
    class QtlBrowserDialog;
}

//!
//! A subclass of QtlDialog which implements a browser.
//! The design of the UI is done using Qt Designer.
//!
class QtlBrowserDialog : public QtlDialog
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //! @param [in] url Optional home URL. When the HTML file is in the
    //! application's resources, use a syntax like "qrc:/help/index.html".
    //! @param [in] title Optional dialog window title.
    //! @param [in] icon Optional icon file name. When the icon file is in the
    //! application's resources, use a syntax like ":/images/logo.png".
    //!
    explicit QtlBrowserDialog(QWidget *parent = 0, const QString& url = QString(), const QString& title = QString(), const QString& icon = QString());

    //!
    //! Destructor.
    //!
    virtual ~QtlBrowserDialog();

    //!
    //! Set the home URL.
    //! @param [in] url Source URL. When the HTML file is in the
    //! application's resources, use a syntax like "qrc:/help/index.html".
    //!
    void setUrl(const QString& url);

private:
    Ui::QtlBrowserDialog* _ui; //!< UI from Qt Designer.

    // Unaccessible operations.
    QtlBrowserDialog() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlBrowserDialog)
};

#endif // QTLBROWSERDIALOG_H
