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
// Qtl, Qt utility library.
// Define the class QtlTextFileViewer.
//
//----------------------------------------------------------------------------

#include "QtlTextFileViewer.h"
#include "QtlFile.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlTextFileViewer::QtlTextFileViewer(QWidget* parent,
                                     const QString& fileName,
                                     const QString& title,
                                     const QString& icon) :
    QtlDialog(parent),
    _text(0)
{
    // Dialog layout.
    resize(650, 500);
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Text display.
    _text = new QPlainTextEdit(this);
    _text->setReadOnly(true);
    _text->setAcceptDrops(false);
    _text->setLineWrapMode(QPlainTextEdit::NoWrap);
    _text->setTextInteractionFlags(Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse);
    _text->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    layout->addWidget(_text);

    // Button box.
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QtlTextFileViewer::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QtlTextFileViewer::reject);

    // Set the user-defined properties.
    setWindowTitle(title);
    if (!icon.isEmpty()) {
        setWindowIcon(QIcon(icon));
    }
    if (!fileName.isEmpty()) {
        setTextFile(fileName);
    }
}


//----------------------------------------------------------------------------
// Manipulate text to display in the box.
//----------------------------------------------------------------------------

void QtlTextFileViewer::setTextFile(const QString& fileName)
{
    _text->setPlainText(QtlFile::readTextFile(fileName));
}

void QtlTextFileViewer::setText(const QString& text)
{
    _text->setPlainText(text);
}

QString QtlTextFileViewer::text() const
{
    return _text->toPlainText();
}

void QtlTextFileViewer::clear()
{
    _text->setPlainText(QString());
}
