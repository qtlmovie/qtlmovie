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
//! @file QtlPlainTextEdit.h
//!
//! Declare the class QtlPlainTextEdit.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLPLAINTEXTEDIT_H
#define QTLPLAINTEXTEDIT_H

#include "QtlCore.h"

//!
//! A subclass of QPlainTextEdit with additional features.
//!
class QtlPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    //!
    //! Flags specifying the text format.
    //!
    enum FormatFlag {
        Monospace     = 0x0001,  //!< Use a monospace font.
        AllCharacters = 0x0002,  //!< Allow all characters.
        ToUpper       = 0x0004,  //!< Convert all letters to uppercase.
        ToLower       = 0x0008,  //!< Convert all letters to lowercase.
        Spaces        = 0x0010,  //!< Allow space characters.
        Ascii         = 0x0020,  //!< Allow 7-bit ASCII characters.
        Digits        = 0x0040,  //!< Allow decimal digits.
        Hexa          = 0x0080,  //!< Allow hexadecimal digits.
        Letters       = 0x0100   //!< Allow letters.
    };
    Q_DECLARE_FLAGS(FormatFlags, FormatFlag)

    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //! @param [in] format Format flags.
    //!
    explicit QtlPlainTextEdit(QWidget *parent = 0, FormatFlags format = AllCharacters);

    //!
    //! Constructor.
    //! @param [in] text Initial content.
    //! @param [in] parent Optional parent widget.
    //! @param [in] format Format flags.
    //!
    explicit QtlPlainTextEdit(const QString& text, QWidget *parent = 0, FormatFlags format = AllCharacters);

    //!
    //! Get the format flags.
    //! @return The format flags.
    //!
    FormatFlags format() const
    {
        return _format;
    }

    //!
    //! Set the format flags.
    //! @param [in] format The format flags.
    //!
    void setFormat(const FormatFlags& format);

    //!
    //! Set the plain text in the editor.
    //! Reimplemented from QPlainTextEdit.
    //! @param [in] text The text to set.
    //!
    void setPlainText(const QString &text);

protected:
    //!
    //! Invoked when a key is pressed.
    //! Reimplemented from superclass.
    //! @param [in,out] event The event to process.
    //!
    virtual void keyPressEvent(QKeyEvent* event);
    //!
    //! Insert MIME data into the widget.
    //! Reimplemented from QPlainTextEdit.
    //! @param [in] source Source data to check.
    //!
    virtual void insertFromMimeData(const QMimeData *source);

private:
    const QFont _standardFont;   //!< Standard font for QPlainTextEdit.
    const QFont _monospaceFont;  //!< Monospace font.
    FormatFlags _format;         //!< Format flags.

    //!
    //! Transform a text into an acceptable string for the current format.
    //! @param [in] text The text to transform.
    //! @return The transformed text.
    //!
    QString acceptableText(const QString& text) const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QtlPlainTextEdit::FormatFlags)

#endif // QTLPLAINTEXTEDIT_H
