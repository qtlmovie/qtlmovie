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
//! @file QtlEnumUtils.h
//!
//! Declare some utilities functions for enumeration types.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLENUMUTILS_H
#define QTLENUMUTILS_H

#include "QtlCore.h"

//!
//! Get the name of an enumeration value.
//! @tparam ENUM An enumeration type with Qt meta data (macro Q_ENUM).
//! @param [in] e Enumeration value.
//! @return Identifier for @a e as a string.
//!
template<typename ENUM>
QString qtlNameOf(ENUM e)
{
    const char* name = QMetaEnum::fromType<ENUM>().valueToKey(e);
    return name != 0 && name[0] != '\0' ? QString(name) : QString::number(int(e));
}

//
// Specializations of qtlNameOf() for enumeration types without Q_ENUM in older versions of Qt.
// In that case, we simply return the integer value of the enumeration value.
// Was fixed in Qt 5.6.0 (https://bugreports.qt.io/browse/QTBUG-10777).
//
#if QT_VERSION < QT_VERSION_CHECK(5,6,0) && !defined(DOXYGEN)
template<> inline QString qtlNameOf(QProcess::ProcessError e) {return QString::number(int(e));}
template<> inline QString qtlNameOf(QProcess::ProcessState e) {return QString::number(int(e));}
#endif

#endif // QTLENUMUTILS_H
