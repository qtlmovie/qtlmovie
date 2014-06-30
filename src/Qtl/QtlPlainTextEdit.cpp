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
// Define the class QtlPlainTextEdit.
//
//----------------------------------------------------------------------------

#include "QtlPlainTextEdit.h"
#include "QtlFontUtils.h"


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtlPlainTextEdit::QtlPlainTextEdit(QWidget *parent, FormatFlags format) :
    QPlainTextEdit(parent),
    _standardFont(font()),
    _monospaceFont(qtlMonospaceFont()),
    _format(AllCharacters)
{
    setFormat(format);
}

QtlPlainTextEdit::QtlPlainTextEdit(const QString& text, QWidget* parent, FormatFlags format) :
    QPlainTextEdit(text, parent),
    _standardFont(font()),
    _monospaceFont(qtlMonospaceFont()),
    _format(AllCharacters)
{
    setFormat(format);
}


//----------------------------------------------------------------------------
// Set the format flags.
//----------------------------------------------------------------------------

void QtlPlainTextEdit::setFormat(const FormatFlags& format)
{
    // Change font if required.
    if (format.testFlag(Monospace) && !_format.testFlag(Monospace)) {
        // Switch to monospace font.
        setFont(_monospaceFont);
    }
    else if (!format.testFlag(Monospace) && _format.testFlag(Monospace)) {
        // Switch back to standard font.
        setFont(_standardFont);
    }

    // Keep new format.
    _format = format;
}


//----------------------------------------------------------------------------
// Set the plain text in the editor. Reimplemented from QPlainTextEdit.
//----------------------------------------------------------------------------

void QtlPlainTextEdit::setPlainText(const QString& text)
{
    // Invoke superclass with filtered text.
    QPlainTextEdit::setPlainText(acceptableText(text));
}


//----------------------------------------------------------------------------
// Transform a text into an acceptable string for the current format.
//----------------------------------------------------------------------------

QString QtlPlainTextEdit::acceptableText(const QString& text) const
{
    // Copy text for later transformation.
    QString s(text);

    // Remove forbidden characters.
    if (!_format.testFlag(AllCharacters)) {

        // Build a list of permitted characters in a regular expression.
        QString allowed;
        if (_format.testFlag(Spaces)) {
            allowed.append("\\s");
        }
        if (_format.testFlag(Digits)) {
            allowed.append("0-9");
        }
        if (_format.testFlag(Hexa)) {
            allowed.append("0-9a-fA-F");
        }
        if (_format.testFlag(Letters)) {
            allowed.append("a-zA-Z");
        }
        if (_format.testFlag(Ascii)) {
            allowed.append(" -~");
        }

        // If nothing is allowed, return an empty string.
        if (allowed.isEmpty()) {
            return "";
        }

        // Remove non-allowed characters.
        s.remove(QRegExp("[^" + allowed + "]"));
    }

    // Finally, perform case conversion.
    if (_format.testFlag(ToLower)) {
        return s.toLower();
    }
    else if (_format.testFlag(ToUpper)) {
        return s.toUpper();
    }
    else {
        return s;
    }
}


//----------------------------------------------------------------------------
// Handle key press event. Reimplemented from superclass.
//----------------------------------------------------------------------------

void QtlPlainTextEdit::keyPressEvent(QKeyEvent* event)
{
    // Transform the typed text into acceptable text.
    const QString text(event->text());
    const QString acceptable(acceptableText(text));

    // Get current key and modifiers.
    const int key = event->key();
    Qt::KeyboardModifiers mods(event->modifiers());

    // "Command keys" shall be passed since they do not insert anything.
    // There is no guaranteed way of detecting a command key.
    // We assume here that keys giving empty text are command keys.
    // We also assume that control characters other than spaces are command keys.
    const bool isCommand = text.isEmpty() ||
            key == Qt::Key_Delete ||
            (text.size() == 1 && text.at(0) < QChar(' ') && !text.at(0).isSpace());

    if (isCommand || text == acceptable) {
        // Pass unmodified event to superclass.
        QPlainTextEdit::keyPressEvent(event);
    }
    else if (!acceptable.isEmpty() &&                 // the transformed text is non-empty,
             (mods & ~Qt::ShiftModifier) == 0 &&      // and there is no other modifier than Shift,
             key >= Qt::Key_A && key <= Qt::Key_Z) {  // and the key is a letter.
        // This must be a letter to set to upper or lower case.
        // Build a new acceptable key event.
        if (_format.testFlag(ToLower)) {
            // Clear the "Shift" key to get a lower case
            mods &= ~Qt::ShiftModifier;
        }
        else if (_format.testFlag(ToUpper)) {
            // Set the "Shift" key to get an upper case
            mods |= Qt::ShiftModifier;
        }
        QKeyEvent newEvent(event->type(), event->key(), mods, acceptable, event->isAutoRepeat());
        // Pass acceptable event to superclass.
        QPlainTextEdit::keyPressEvent(&newEvent);
    }
    else {
        // Ignore the event, pass to parent widget.
        event->ignore();
    }
}


//----------------------------------------------------------------------------
// Insert MIME data into the widget.
// Reimplemented from QPlainTextEdit.
//----------------------------------------------------------------------------

void QtlPlainTextEdit::insertFromMimeData(const QMimeData* source)
{
    // Transform the source data into acceptable text.
    const QString text(source == 0 ? QString() : source->text());
    const QString acceptable(acceptableText(text));

    if (!text.isEmpty() && text == acceptable) {
        // The text is acceptable as is. Pass to superclass.
        QPlainTextEdit::insertFromMimeData(source);
    }
    else if (!acceptable.isEmpty()) {
        // Build a MIME source with the acceptable text.
        QMimeData newSource;
        newSource.setText(acceptable);
        // Pass the transformed MIME source to superclass.
        QPlainTextEdit::insertFromMimeData(&newSource);
    }
}
