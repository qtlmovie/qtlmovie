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
// Define the class QtlPlainTextLogger.
//
//----------------------------------------------------------------------------

#include "QtlPlainTextLogger.h"
#include <QTextCharFormat>
#include <QTextCursor>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QDir>


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtlPlainTextLogger::QtlPlainTextLogger(QWidget *parent) :
    QPlainTextEdit(parent),
    _debug(false),
    _lastSavedLog()
{
}

QtlPlainTextLogger::QtlPlainTextLogger(const QString& text, QWidget* parent) :
    QPlainTextEdit(text, parent),
    _debug(false),
    _lastSavedLog()
{
}


//-----------------------------------------------------------------------------
// Log text. Reimplemented from QtlLogger.
//-----------------------------------------------------------------------------

void QtlPlainTextLogger::text(const QString& text)
{
    // Get a copy of current cursor position in text edit.
    QTextCursor cursor(textCursor());
    const bool wasAtEnd = cursor.atEnd();

    // Move this cursor at end of document.
    // This does not change the cursor in the text edit.
    cursor.movePosition(QTextCursor::End);

    // Insert the text at end of document.
    cursor.insertText(text);

    // If the cursor was at the end of text, we assume that the user is following
    // the text and we need to ensure the cursor is still visible after insertion.
    // Otherwise, we assume the user was inspecting some other part of the log and
    // we should not disrupt what he was looking at (do not force scroll).
    if (wasAtEnd) {
        ensureCursorVisible();
    }
}


//-----------------------------------------------------------------------------
// Log a line of text. Reimplemented from QtlLogger.
//-----------------------------------------------------------------------------

void QtlPlainTextLogger::line(const QString& line, const QColor& color)
{
    // Check if the cursor is as end of the text.
    const bool wasAtEnd = textCursor().atEnd();

    // Save current character format.
    const QTextCharFormat previousFormat (currentCharFormat());

    // If a valid color was passed, apply it.
    if (color.isValid()) {
        QTextCharFormat newFormat(previousFormat);
        newFormat.setForeground(QBrush(color));
        setCurrentCharFormat(newFormat);
    }

    // Add the line as one paragraph in the text.
    appendPlainText(line);

    // Restore previous character format if we changed it.
    if (color.isValid()) {
        setCurrentCharFormat (previousFormat);
    }

    // If the cursor was at the end of text, we assume that the user is following
    // the text and we need to ensure the cursor is still visible after insertion.
    // Otherwise, we assume the user was inspecting some other part of the log and
    // we should not disrupt what he was looking at (do not force scroll).
    if (wasAtEnd) {
        ensureCursorVisible();
    }
}


//-----------------------------------------------------------------------------
// Log a line of debug text. Reimplemented from QtlLogger.
//-----------------------------------------------------------------------------

void QtlPlainTextLogger::debug(const QString& text, const QColor& color)
{
    if (_debug) {
        line(QStringLiteral("[debug] ") + text, color.isValid() ? color : QColor("brown"));
    }
}


//-----------------------------------------------------------------------------
// Set / reset the debug mode.
//-----------------------------------------------------------------------------

void QtlPlainTextLogger::setDebugMode(bool on)
{
    _debug = on;
}


//-----------------------------------------------------------------------------
// Save the content of the log window in a file.
//-----------------------------------------------------------------------------

void QtlPlainTextLogger::saveToFile(const QString& fileName)
{
    QString name(fileName);

    // If no file specified, ask the user to select an output file.
    if (name.isEmpty()) {
        name = QFileDialog::getSaveFileName(this,
                                            tr("Save Log"),
                                            _lastSavedLog.isEmpty() ? QDir::homePath() : _lastSavedLog,
                                            tr("Log files (*.log);;Text files (*.txt)"));
    }

    // If no file is selected, nothing to do.
    if (name.isEmpty()) {
        return;
    }

    // Keep the last saved location.
    _lastSavedLog = name;

    // Save the file
    QFile file(name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Error opening the file.
        QMessageBox::warning(this, tr("Save Error"), tr("Error creating file %1").arg(name));
        return;
    }

    // Write the content of the log window.
    QTextStream stream (&file);
    stream << toPlainText() << '\n';
    file.close();
}
