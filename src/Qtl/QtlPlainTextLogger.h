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
//! @file QtlPlainTextLogger.h
//!
//! Declare the class QtlPlainTextLogger.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLPLAINTEXTLOGGER_H
#define QTLPLAINTEXTLOGGER_H

#include <QPlainTextEdit>
#include <QColor>
#include "QtlLogger.h"

//!
//! A subclass of QPlainTextEdit which implements QtlLogger interface.
//!
class QtlPlainTextLogger : public QPlainTextEdit, public QtlLogger
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlPlainTextLogger(QWidget *parent = 0);
    //!
    //! Constructor.
    //! @param [in] text Initial content.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlPlainTextLogger(const QString& text, QWidget *parent = 0);
    //!
    //! Log text.
    //! Reimplemented from QtlLogger.
    //! @param [in] text Text to log.
    //!
    virtual void text(const QString& text);
    //!
    //! Log a line of text.
    //! Reimplemented from QtlLogger.
    //! @param [in] line Line to log. No need to contain a trailing new-line character.
    //! @param [in] color When a valid color is passed, try to display the text in this color.
    //!
    virtual void line(const QString& line, const QColor& color = QColor());
    //!
    //! Log a line of debug text.
    //! Reimplemented from QtlLogger.
    //! @param [in] line Line to log. No need to contain a trailing new-line character.
    //! @param [in] color When a valid color is passed, try to display the text in this color.
    //!
    virtual void debug(const QString& line, const QColor& color = QColor());

public slots:
    //!
    //! Save the content of the log window in a file.
    //! Ask the user to select a file.
    //!
    void save();
    //!
    //! Save the content of the log window in a file.
    //! @param [in] fileName Name of the file to write the log window content into.
    //! If empty, ask the user to select a file.
    //!
    void saveToFile(const QString& fileName);
    //!
    //! Set / reset the debug mode.
    //! @param [in] on When true, the debug() lines are displayed. When false, they are discarded.
    //!
    void setDebugMode(bool on);

private:
    bool    _debug;        //!< Debug mode.
    QString _lastSavedLog; //!< Last saved log file name.
};

#endif // QTLPLAINTEXTLOGGER_H
