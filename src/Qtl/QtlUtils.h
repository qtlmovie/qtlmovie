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
//! @file QtlUtils.h
//!
//! Declare some utilities functions.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLUTILS_H
#define QTLUTILS_H

#include <QtCore>
#include <QtWidgets>
#include <numeric>

//!
//! Return the runtime version of Qt as an integer, 0xMMNNPP (MM = major, NN = minor, PP = patch).
//! Similar to the macro QT_VERSION but applies to the runtime version, not to the compile-time version.
//! @return Qt version as an integer, 0xMMNNPP.
//!
int qtlQtVersion();

//!
//! Check if the application is running on a 64-bit version of Windows.
//! This cannot be detected by conditional compilation since a 32-bit
//! Windows application may run on a 64-bit system.
//! @return True if the application is running on a 64-bit version of Windows.
//!
bool qtlRunningOnWin64();

//!
//! Construct a QString from a raw memory area containing UTF-8 characters.
//! @param [in] addr Starting address.
//! @param [in] size Size in bytes.
//! @return A QString.
//!
inline QString qtlQString(const void* addr, int size)
{
    return QString(QByteArray::fromRawData(reinterpret_cast<const char*>(addr), size));
}

//!
//! Convert a string into an integer.
//! @param [in] str String to convert.
//! @param [in] def Value to return if @a str is not a valid int in the range @a min to @a max.
//! @param [in] min Minimum allowed value.
//! @param [in] max Maximum allowed value.
//! @param [in] base Base for conversion, 10 by default, must be between 2 and 36, or 0.
//! If base is 0, the C language convention is used: If the string begins with "0x", base 16 is used;
//! if the string begins with "0", base 8 is used; otherwise, base 10 is used.
//! @return The decoded value or @a def if out of range.
//!
int qtlToInt(const QString& str, int def = -1, int min = 0, int max = INT_MAX, int base = 10);

//!
//! Convert a string into a 64-bit integer.
//! @param [in] str String to convert.
//! @param [in] def Value to return if @a str is not a valid int in the range @a min to @a max.
//! @param [in] min Minimum allowed value.
//! @param [in] max Maximum allowed value.
//! @param [in] base Base for conversion, 10 by default, must be between 2 and 36, or 0.
//! If base is 0, the C language convention is used: If the string begins with "0x", base 16 is used;
//! if the string begins with "0", base 8 is used; otherwise, base 10 is used.
//! @return The decoded value or @a def if out of range.
//!
qint64 qtlToInt64(const QString& str,
                  qint64 def = -1,
                  qint64 min = 0,
                  qint64 max = Q_INT64_C(0x7FFFFFFFFFFFFFFF),
                  int base = 10);

//!
//! Format a duration into a string.
//! @param [in] seconds Duration in seconds.
//! @return Formatted string.
//!
QString qtlSecondsToString(int seconds);

//!
//! Build an HTML string containing a link to a URL.
//! @param [in] link Target URL.
//! @param [in] text Text of the link. If empty (the default), @a link is also used as text.
//! @return An HTML fragment.
//!
QString qtlHtmlLink(const QString& link, const QString& text = QString());

//!
//! Get the first QWidget in the line of ancestors, starting at (and including) @a object.
//! @param [in] object Starting object. Can be null.
//! @return Return the first QWidget in @a object hierarchy.
//! Return @a object it is a QWidget.
//! Return zero is @a object is zero or no QWidget is found in its hierarchy.
//!
QWidget* qtlWidgetAncestor(QObject* object);

//!
//! Get a suitable value for application name.
//! @return Application name.
//!
QString qtlApplicationName();

//!
//! Get all items in a QListWidget as a list of strings.
//! @param [in] list The QListWidget.
//! @return A list of all items in the QListWidget, in the same order.
//!
QStringList qtlListItems(const QListWidget* list);

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

#endif // QTLUTILS_H
