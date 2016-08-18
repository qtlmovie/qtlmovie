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
#include "images/browser-left.h"
#include "images/browser-right.h"
#include "images/browser-home.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlBrowserDialog::QtlBrowserDialog(QWidget *parent, const QString& url, const QString& title, const QString& icon) :
    QtlDialog(parent),
    _text(0)
{
    // Dialog layout.
    resize(750, 580);
    QGridLayout* gridLayout = new QGridLayout(this);

    // Top row: buttons.
    QPushButton* buttonBackward = new QPushButton(this);
    buttonBackward->setStyleSheet(QStringLiteral("border: none;"));
    buttonBackward->setIcon(QIcon(QPixmap(browser_left_xpm)));
    gridLayout->addWidget(buttonBackward, 0, 0);

    QPushButton* buttonForward = new QPushButton(this);
    buttonForward->setStyleSheet(QStringLiteral("border: none;"));
    buttonForward->setIcon(QIcon(QPixmap(browser_right_xpm)));
    gridLayout->addWidget(buttonForward, 0, 1);

    QPushButton* buttonHome = new QPushButton(this);
    buttonHome->setStyleSheet(QStringLiteral("border: none;"));
    buttonHome->setIcon(QIcon(QPixmap(browser_home_xpm)));
    gridLayout->addWidget(buttonHome, 0, 2);

    QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayout->addItem(horizontalSpacer, 0, 3);

    QPushButton* buttonClose = new QPushButton(tr("Close"), this);
    gridLayout->addWidget(buttonClose, 0, 4);

    // Second row: text window.
    _text = new QTextBrowser(this);
    _text->setOpenExternalLinks(true);
    _text->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard |
                                   Qt::LinksAccessibleByMouse |
                                   Qt::TextBrowserInteraction |
                                   Qt::TextSelectableByKeyboard |
                                   Qt::TextSelectableByMouse);
    gridLayout->addWidget(_text, 1, 0, 1, 5);

    // Enable / disable navigation buttons when content is available.
    connect(_text, &QTextBrowser::backwardAvailable, buttonBackward, &QPushButton::setEnabled);
    connect(_text, &QTextBrowser::forwardAvailable,  buttonForward,  &QPushButton::setEnabled);

    // Navigation actions.
    connect(buttonBackward, &QPushButton::clicked, _text, &QTextBrowser::backward);
    connect(buttonForward,  &QPushButton::clicked, _text, &QTextBrowser::forward);
    connect(buttonHome,     &QPushButton::clicked, _text, &QTextBrowser::home);
    connect(buttonClose,    &QPushButton::clicked, this,  &QtlBrowserDialog::accept);

    // Set the user-defined properties.
    setWindowTitle(title);
    if (!icon.isEmpty()) {
        setWindowIcon(QIcon(icon));
    }
    setUrl(url);
}


//----------------------------------------------------------------------------
// Set the home URL.
//----------------------------------------------------------------------------

void QtlBrowserDialog::setUrl(const QString& url)
{
    if (url.isEmpty()) {
        _text->clear();
    }
    else {
        _text->setSource(QUrl(url));
    }
}
