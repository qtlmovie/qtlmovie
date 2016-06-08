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
//! @file QtlNullLogger.h
//!
//! Declare the class QtlNullLogger.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLNULLLOGGER_H
#define QTLNULLLOGGER_H

#include "QtlLogger.h"

//!
//! An implementation of QtlLogger which drops all messages.
//!
class QtlNullLogger: public QtlLogger
{
public:
    //!
    //! Log text.
    //! @param [in] text Text to log.
    //!
    virtual void text(const QString& text) {}
    //!
    //! Log a line of text.
    //! @param [in] line Line to log. No need to contain a trailing new-line character.
    //! @param [in] color When a valid color is passed, try to display the text in this color.
    //!
    virtual void line(const QString& line, const QColor& color = QColor()) {}
    //!
    //! Log a line of debug text.
    //! @param [in] line Line to log. No need to contain a trailing new-line character.
    //! @param [in] color When a valid color is passed, try to display the text in this color.
    //!
    virtual void debug(const QString& line, const QColor& color = QColor()) {}
};

#endif // QTLNULLLOGGER_H
