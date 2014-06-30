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
//! @file QtlCore.h
//!
//! Qtl core declarations.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLCORE_H
#define QTLCORE_H

#include <QtCore>
#include <QtWidgets>

//!
//! The symbol QTL_WINEXTRAS is defined when the Qt module winextras is available.
//! This means on Windows with Qt version >= 5.2.0.
//!
#if defined(DOXYGEN) || (defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(5,2,0))
#define QTL_WINEXTRAS 1
#endif

//!
//! Return the runtime version of Qt as an integer, 0xMMNNPP (MM = major, NN = minor, PP = patch).
//! Similar to the macro QT_VERSION but applies to the runtime version, not to the compile-time version.
//! @return Qt version as an integer, 0xMMNNPP.
//!
int qtlQtVersion();

//!
//! Get a suitable value for application name.
//! @return Application name.
//!
QString qtlApplicationName();

//!
//! Get the first QWidget in the line of ancestors, starting at (and including) @a object.
//! @param [in] object Starting object. Can be null.
//! @return Return the first QWidget in @a object hierarchy.
//! Return @a object it is a QWidget.
//! Return zero is @a object is zero or no QWidget is found in its hierarchy.
//!
QWidget* qtlWidgetAncestor(QObject* object);

//!
//! Dump an object hierarchy.
//! Multiple lines are separated by new lines.
//! The last line is not terminated by a new line.
//! @param [in,out] output Output text stream.
//! @param [in] object Root object of the hierarchy to dump.
//! @param [in] prefix Optional prefix to add at beginning of each line, except the first line.
//! @param [in] maxDepth Maximum depth of children to explore. A negative number means unlimited.
//!
void qtlDumpObjectHierarchy(QTextStream& output, QObject* object, const QString& prefix = QString(), int maxDepth = -1);

//!
//! Dump an object hierarchy in a string.
//! @param [in] object Root object of the hierarchy to dump.
//! @param [in] prefix Optional prefix to add at beginning of each line, except the first line.
//! @param [in] maxDepth Maximum depth of children to explore. A negative number means unlimited.
//! @return Object hierarchy.
//!
QString qtlObjectHierarchy(QObject* object, const QString& prefix = QString(), int maxDepth = -1);

#endif // QTLCORE_H
