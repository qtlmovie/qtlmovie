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
//
// Define the class QtlBrowserDialog.
//
//----------------------------------------------------------------------------

#include "QtlBrowserDialog.h"
#include "ui_QtlBrowserDialog.h"
#include "images/browser-left.h"
#include "images/browser-right.h"
#include "images/browser-home.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlBrowserDialog::QtlBrowserDialog(QWidget *parent, const QString& url, const QString& title, const QString& icon ) :
    QDialog(parent),
    _ui(new Ui::QtlBrowserDialog)
{
    // Build the UI as defined in Qt Designer.
    _ui->setupUi(this);

    // Set the built-in browser button images.
    _ui->buttonBackward->setIcon(QIcon(QPixmap(browser_left_xpm)));
    _ui->buttonForward->setIcon(QIcon(QPixmap(browser_right_xpm)));
    _ui->buttonHome->setIcon(QIcon(QPixmap(browser_home_xpm)));

    // Set the user-defined properties.
    setWindowTitle(title);
    if (!icon.isEmpty()) {
        setWindowIcon(QIcon(icon));
    }
    setUrl(url);
}


//----------------------------------------------------------------------------
// Destructor.
//----------------------------------------------------------------------------

QtlBrowserDialog::~QtlBrowserDialog()
{
    delete _ui;
    _ui = 0;
}


//----------------------------------------------------------------------------
// Set the home URL.
//----------------------------------------------------------------------------

void QtlBrowserDialog::setUrl(const QString& url)
{
    if (url.isEmpty()) {
        _ui->text->clear();
    }
    else {
        _ui->text->setSource(QUrl(url));
    }
}
