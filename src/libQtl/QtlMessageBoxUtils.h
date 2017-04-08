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
//! @file QtlMessageBoxUtils.h
//!
//! Declare some utilities functions for QMessageBox.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLMESSAGEBOXUTILS_H
#define QTLMESSAGEBOXUTILS_H

#include "QtlCore.h"

//!
//! Ask a question with Yes/No buttons (default to No).
//! @param [in] parent Parent object/widget. If not a widget, find first widget in its hierarchy.
//! @param [in] question The question to display.
//! @param [in] title Dialog box title. Default to the application name.
//! @return True if answer is Yes, false otherwise.
//!
bool qtlConfirm(QObject* parent, const QString& question, const QString& title = qtlApplicationName());

//!
//! Report an error message.
//! @param [in] parent Parent object/widget. If not a widget, find first widget in its hierarchy.
//! @param [in] message The message to display.
//! @param [in] title Dialog box title. Default to the application name.
//!
void qtlError(QObject* parent, const QString& message, const QString& title = qtlApplicationName());

//!
//! Display a simple "about" box similar to QMessageBox::about() but add a specific icon.
//! @param [in] parent Parent object/widget. If not a widget, find first widget in its hierarchy.
//! @param [in] title Window title.
//! @param [in] iconPath Path to icon file, for instance ":/images/logo.png".
//! @param [in] text Description text. Can be HTML.
//!
void qtlAbout(QObject* parent, const QString& title, const QString& iconPath, const QString& text);

#endif // QTLMESSAGEBOXUTILS_H
