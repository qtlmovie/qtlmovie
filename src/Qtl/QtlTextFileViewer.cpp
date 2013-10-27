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
//
//
//----------------------------------------------------------------------------

#include "QtlTextFileViewer.h"
#include "ui_QtlTextFileViewer.h"
#include "QtlFile.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlTextFileViewer::QtlTextFileViewer(QWidget* parent,
                                     const QString& fileName,
                                     const QString& title,
                                     const QString& icon) :
    QDialog(parent),
    _ui(new Ui::QtlTextFileViewer)
{
    // Build the UI as defined in Qt Designer.
    _ui->setupUi(this);

    // The defaut font for the text window is console-like.
    // Select "Monospace" family. When not found (Windows), use a typewriter style font.
    // This double specification is necessary since 1) "Monospace" is known on X11 but
    // not on Windows, 2) QFont::setStyleHint() is ineffective on X11.
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    _ui->text->setFont(font);

    // Set the user-defined properties.
    setWindowTitle(title);
    if (!icon.isEmpty()) {
        setWindowIcon(QIcon(icon));
    }
    setTextFile(fileName);
}


//----------------------------------------------------------------------------
// Destructor.
//----------------------------------------------------------------------------

QtlTextFileViewer::~QtlTextFileViewer()
{
    delete _ui;
    _ui = 0;
}


//----------------------------------------------------------------------------
// Manipulate text to display in the box.
//----------------------------------------------------------------------------

void QtlTextFileViewer::setTextFile(const QString& fileName)
{
    _ui->text->setPlainText(QtlFile::readTextFile(fileName));
}

QString QtlTextFileViewer::text() const
{
    return _ui->text->toPlainText();
}

void QtlTextFileViewer::clear()
{
    _ui->text->setPlainText(QString());
}
