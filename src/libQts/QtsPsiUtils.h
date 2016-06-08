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
//! @file QtsPsiUtils.h
//!
//! Declare some utilities for PSI/SI analysis.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSPSIUTILS_H
#define QTSPSIUTILS_H

#include <QtCore>
#include "QtsCore.h"
#include "QtlByteBlock.h"

//!
//! Deserialize a 3-letter ISO-639 language code.
//! @param [in] data Address of a 3-byte area.
//! @return The ISO-639 language code as a string.
//!
QString qtsGetIso639Language(const void* data);

//!
//! Append a 3-letter ISO-639 language code into a byte array.
//! @param [in] language The ISO-639 language code as a string.
//! @param [in,out] data The data byte into which the code is append.
//! @return True if success, false if @a language is too long.
//!
bool qtsAppendIso639Language(const QString& language, QtlByteBlock& data);

#endif // QTSPSIUTILS_H
