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
//! @file QtlStringUtils.h
//!
//! Declare some utilities functions for QString.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLSTRINGUTILS_H
#define QTLSTRINGUTILS_H

#include "QtlCore.h"

//! @cond nodoxygen
#define QTL_STRINGIFY1(x) #x
//! @endcond
//!
//! @hideinitializer
//! This macro transforms the @e value of a macro parameter into the equivalent string.
//!
//! This is a very specific macro. It is typically used only inside the definition of
//! another macro. It is similar to the @# token in the preprocessor but has a slightly
//! different effect. The @# token transforms the @e text of a macro parameter into a
//! string while QTL_STRINGIFY transforms the @e value of a macro parameter into a
//! string, after all preprocessing substitutions.
//!
//! The following example illustrates the difference between the @# token and V_SYS_STRINGIFY:
//!
//! @code
//! #define P1(v) printf ("#parameter:    %s = %d\n", #v, v)
//! #define P2(v) printf ("QTL_STRINGIFY: %s = %d\n", QTL_STRINGIFY(v), v)
//! ....
//! #define X 1
//! ....
//! P1 (X);
//! P2 (X);
//! @endcode
//!
//! This will print:
//!
//! @code
//! #parameter:    X = 1
//! QTL_STRINGIFY: 1 = 1
//! @endcode
//!
#define QTL_STRINGIFY(x) QTL_STRINGIFY1(x)

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
//! Convert a string into a 32-bit unsigned integer.
//! @param [in] str String to convert.
//! @param [in] def Value to return if @a str is not a valid int in the range @a min to @a max.
//! @param [in] min Minimum allowed value.
//! @param [in] max Maximum allowed value.
//! @param [in] base Base for conversion, 10 by default, must be between 2 and 36, or 0.
//! If base is 0, the C language convention is used: If the string begins with "0x", base 16 is used;
//! if the string begins with "0", base 8 is used; otherwise, base 10 is used.
//! @return The decoded value or @a def if out of range.
//!
quint32 qtlToUInt32(const QString& str,
                    quint32 def = 0xFFFFFFFFU,
                    quint32 min = 0,
                    quint32 max = 0xFFFFFFFFU,
                    int base = 10);

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
//! Convert a string into a 64-bit unsigned integer.
//! @param [in] str String to convert.
//! @param [in] def Value to return if @a str is not a valid int in the range @a min to @a max.
//! @param [in] min Minimum allowed value.
//! @param [in] max Maximum allowed value.
//! @param [in] base Base for conversion, 10 by default, must be between 2 and 36, or 0.
//! If base is 0, the C language convention is used: If the string begins with "0x", base 16 is used;
//! if the string begins with "0", base 8 is used; otherwise, base 10 is used.
//! @return The decoded value or @a def if out of range.
//!
quint64 qtlToUInt64(const QString& str,
                    quint64 def = Q_UINT64_C(0xFFFFFFFFFFFFFFFF),
                    quint64 min = 0,
                    quint64 max = Q_INT64_C(0xFFFFFFFFFFFFFFFF),
                    int base = 10);

//!
//! Convert a string into a float.
//! Syntax like "x/y" and "x:y" are interpreted as rational numbers.
//! @param [in] str String to convert.
//! @param [in] def Value to return if @a str is not a valid float.
//! @return The decoded value or @a def if invalid.
//!
float qtlToFloat(const QString& str, float def = 0.0);

//!
//! Insert spaces at regular intervals in a string.
//! @param [in] str The string to update.
//! @param [in] groupSize The number of characters between spaces. If negative or zero, do not insert spaces.
//! @param [in] direction The direction from which the characters are counted.
//! @param [in] space The space pattern to use.
//! @return The modified string.
//!
QString qtlStringSpace(const QString& str,
                       int groupSize,
                       Qt::LayoutDirection direction = Qt::LeftToRight,
                       const QString& space = QStringLiteral(" "));

//!
//! Format a duration into a string.
//! @param [in] seconds Duration in seconds.
//! @return Formatted string.
//!
QString qtlSecondsToString(int seconds);

//!
//! Format a size into a human-friendly string.
//! The resulting string is approximated into the closest "big unit" such as "kB", MB", etc.
//! The default unit is "B" for bytes but it is replaceable.
//! @param [in] value The size value to format.
//! @param [in] maxDecimalDigits Maximum number of decimal digits. Valid values are 0 to 3.
//! @param [in] useBinaryUnits If true, use "binary" units, meaning chunks of 1024 units instead of 1000.
//! The abbreviations are updated accordingly. For instance, "1 MB" for "MegaByte" becomes "0.954 MiB"
//! for "MibiByte". The default is false, meaning use standard decimal units or chunks of 1000 units.
//! @param [in] unitName Name of the size unit to display. The default is "B" for "byte".
//! @return The formatted string.
//!
QString qtlSizeToString(qlonglong value,
                        int maxDecimalDigits = 3,
                        bool useBinaryUnits = false,
                        const QString& unitName = "B");

//!
//! Format a fraction @a numerator / @a divisor as a packed string.
//! @tparam INT Any numerical type.
//! @param [in] numerator Numerator of the fraction.
//! @param [in] divisor Divisor of the fraction.
//! @param [in] maxDecimalDigits Maximum number of decimal digits. Valid values are 0 to 3.
//! In the result, useless trailing zeroes and decimal dot are removed.
//! @return The formatted string.
//!
template<typename INT>
QString qtlFractionToString(INT numerator, INT divisor = INT(1), int maxDecimalDigits = 3)
{
    const double div = double(divisor);
    if (div == 0.0) {
        return QObject::tr("infinite");
    }
    else {
        // Format the number, remove useless trailing zeroes and dots.
        QString number(QStringLiteral("%1").arg(double(numerator) / div, 0, 'f', qMax(0, maxDecimalDigits)));
        number.remove(QRegExp("0*$"));
        number.remove(QRegExp("\\.$"));
        return number;
    }
}

//!
//! Build an HTML string containing a link to a URL.
//! @param [in] link Target URL.
//! @param [in] text Text of the link. If empty (the default), @a link is also used as text.
//! @return An HTML fragment.
//!
QString qtlHtmlLink(const QString& link, const QString& text = QString());

//!
//! Convert a QString into a null-terminated vector of wchar_t.
//! Can be useful to call native C functions using wide characters.
//! @param [in] str Qt string to convert.
//! @return Corresponding null-terminated vector of wchar_t.
//!
QVector<wchar_t> qtlToWCharVector(const QString& str);

#endif // QTLSTRINGUTILS_H
