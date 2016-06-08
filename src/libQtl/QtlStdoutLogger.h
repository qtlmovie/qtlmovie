//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
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
//! @file QtlStdoutLogger.h
//!
//! Declare the class QtlStdoutLogger.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLSTDOUTLOGGER_H
#define QTLSTDOUTLOGGER_H

#include "QtlLogger.h"

//!
//! An implementation of QtlLogger which prints messages on standard output or other standard files.
//!
class QtlStdoutLogger: public QtlLogger
{
public:
    //!
    //! Constructor.
    //! @param [in] file Standard file to use, @c stdout by default.
    //! @param [in] debug Activate debug if true.
    //!
    QtlStdoutLogger(FILE* file = 0, bool debug = false) :
        _out(file == 0 ? stdout : file, QIODevice::WriteOnly),
        _debug(debug)
    {
    }

    //!
    //! Log text.
    //! @param [in] text Text to log.
    //!
    virtual void text(const QString& text)
    {
        _out << text;
    }

    //!
    //! Log a line of text.
    //! @param [in] line Line to log. No need to contain a trailing new-line character.
    //! @param [in] color When a valid color is passed, try to display the text in this color.
    //! Present by contract from QtlLogger but ignored.
    //!
    virtual void line(const QString& line, const QColor& color = QColor())
    {
        _out << line << endl;
    }

    //!
    //! Log a line of debug text.
    //! @param [in] line Line to log. No need to contain a trailing new-line character.
    //! @param [in] color When a valid color is passed, try to display the text in this color.
    //! Present by contract from QtlLogger but ignored.
    //!
    virtual void debug(const QString& line, const QColor& color = QColor())
    {
        if (_debug) {
            _out << "[debug] " << line << endl;
        }
    }

    //!
    //! Set / reset the debug mode.
    //! @param [in] on When true, the debug() lines are displayed. When false, they are discarded.
    //!
    void setDebugMode(bool on)
    {
        _debug = on;
    }

private:
    QTextStream _out;   //!< Output text stream.
    bool        _debug; //!< Debug mode.
};

#endif // QTLSTDOUTLOGGER_H
