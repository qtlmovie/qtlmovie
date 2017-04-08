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
//!
//! @file QtlTextFileViewer.h
//!
//! Declare the class QtlTextFileViewer.
//!
//----------------------------------------------------------------------------

#ifndef QTLTEXTFILEVIEWER_H
#define QTLTEXTFILEVIEWER_H

#include "QtlCore.h"
#include "QtlDialog.h"

//!
//! A subclass of QtlDialog which displays the content of a text file.
//!
class QtlTextFileViewer : public QtlDialog
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //! @param [in] fileName Optional text file name.
    //! @param [in] title Optional dialog window title.
    //! @param [in] icon Optional icon file name.
    //!
    explicit QtlTextFileViewer(QWidget *parent = 0,
                               const QString& fileName = QString(),
                               const QString& title = QString(),
                               const QString& icon = QString());

    //!
    //! Load a new text file to display in the box.
    //! @param [in] fileName Text file name.
    //!
    void setTextFile(const QString& fileName);

    //!
    //! Set a new text to display in the box.
    //! @param [in] text Text content.
    //!
    void setText(const QString& text);

    //!
    //! Get the text content of the viewer.
    //! @return The text content of the viewer.
    //!
    QString text() const;

    //!
    //! Clear the text content.
    //!
    void clear();

private:
    QPlainTextEdit* _text;  //!< Text viewer.

    // Unaccessible operations.
    QtlTextFileViewer() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlTextFileViewer)
};

#endif // QTLTEXTFILEVIEWER_H
