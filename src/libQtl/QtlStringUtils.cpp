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
//
// Qtl, Qt utility library.
// Declare some utilities functions for QString.
//
//----------------------------------------------------------------------------

#include "QtlStringUtils.h"


//-----------------------------------------------------------------------------
// Convert a string into an integer.
//-----------------------------------------------------------------------------

int qtlToInt(const QString& str, int def, int min, int max, int base)
{
    QString s(str);
    s.remove(QRegExp(QStringLiteral("[\\s_]")));
    bool ok = false;
    int value = s.toInt(&ok, base);
    return ok && value >= min && value <= max ? value : def;
}

quint32 qtlToUInt32(const QString& str, quint32 def, quint32 min, quint32 max, int base)
{
    QString s(str);
    s.remove(QRegExp(QStringLiteral("[\\s_]")));
    bool ok = false;
    quint32 value = s.toULong(&ok, base);
    return ok && value >= min && value <= max ? value : def;
}

qint64 qtlToInt64(const QString& str, qint64 def, qint64 min, qint64 max, int base)
{
    QString s(str);
    s.remove(QRegExp(QStringLiteral("[\\s_]")));
    bool ok = false;
    qint64 value = s.toLongLong(&ok, base);
    return ok && value >= min && value <= max ? value : def;
}

quint64 qtlToUInt64(const QString& str, quint64 def, quint64 min, quint64 max, int base)
{
    QString s(str);
    s.remove(QRegExp(QStringLiteral("[\\s_]")));
    bool ok = false;
    quint64 value = s.toULongLong(&ok, base);
    return ok && value >= min && value <= max ? value : def;
}


//-----------------------------------------------------------------------------
// Convert a string into a float.
//-----------------------------------------------------------------------------

float qtlToFloat(const QString& str, float def)
{
    // Try as an integer value.
    bool ok = false;
    const int iValue = str.toInt(&ok, 0);
    if (ok) {
        return float(iValue);
    }

    // Try as a float value.
    const float fValue = str.toFloat(&ok);
    if (ok) {
        return fValue;
    }

    // Try as a fraction: "x:y" or "x/y".
    int sep = str.indexOf(QChar(':'));
    if (sep < 0) {
        sep = str.indexOf(QChar('/'));
    }
    if (sep >= 0) {
        const int x = str.left(sep).toInt(&ok);
        if (ok) {
            const int y = str.mid(sep + 1).toInt(&ok);
            if (ok && y != 0) {
                return float(x) / float(y);
            }
        }
    }

    // Cannot find a format.
    return def;
}


//-----------------------------------------------------------------------------
// Format a duration into a string.
//-----------------------------------------------------------------------------

QString qtlSecondsToString(int seconds)
{
    const int sec = seconds % 60;
    const int min = (seconds / 60) % 60;
    const int hrs = seconds / 3600;

    if (hrs > 0) {
        return QObject::tr("%1 h %2 mn %3 sec.").arg(hrs).arg(min).arg(sec);
    }
    else if (min > 0) {
        return QObject::tr("%1 mn %2 sec.").arg(min).arg(sec);
    }
    else {
        return QObject::tr("%1 sec.").arg(sec);
    }
}


//----------------------------------------------------------------------------
// Insert spaces at regular intervals in a string.
//----------------------------------------------------------------------------

QString qtlStringSpace(const QString& str, int groupSize, Qt::LayoutDirection direction, const QString& space)
{
    // Filter trivial cases where there is nothing to do.
    if (groupSize <= 0 || space.isEmpty() || str.size() < groupSize) {
        return str;
    }

    // Work on this string.
    QString s(str);
    switch (direction) {
    case Qt::LeftToRight: {
        for (int i = groupSize; i < s.size(); i += groupSize + space.size()) {
            s.insert(i, space);
        }
        break;
    }
    case Qt::RightToLeft: {
        for (int i = s.size() - groupSize; i > 0; i -= groupSize) {
            s.insert(i, space);
        }
        break;
    }
    default:
        // Don't do anything.
        break;
    }

    return s;
}


//-----------------------------------------------------------------------------
// Build an HTML string containing a link to a URL.
//-----------------------------------------------------------------------------

QString qtlHtmlLink(const QString& link, const QString& text)
{
    return QStringLiteral("<a href=\"%1\">%2</a>").arg(link).arg((text.isEmpty() ? link : text).toHtmlEscaped());
}


//-----------------------------------------------------------------------------
// Format a size into a human-friendly string.
//-----------------------------------------------------------------------------

QString qtlSizeToString(qlonglong value, int maxDecimalDigits, bool useBinaryUnits, const QString& unitName)
{
    // Treat sign separately.
    QString sign;
    if (value < 0) {
        sign = "-";
        value = -value;
    }

    // Binary units means using chunks of 1024 units instead of 1000.
    const qlonglong chunk = useBinaryUnits ? 1024 : 1000;

    // List of big units, in increasing order.
    // Note: kilo, mega, giga, tera, peta, exa, zeta, yotta.
    static const char* const prefixes[] = {"", "k", "M", "G", "T", "P", "E", "Z", "Y", 0};

    // Actual value for useBinaryUnits. Initially, do not use binary units for values less than chunk.
    bool actualBinary = false;

    // Loop on all big units in increasing order.
    qlonglong divisor = 1;
    for (const char* const* pref = prefixes; *pref != 0; ++pref) {

        // Stop loop when we find a number of
        const qlonglong units = value / divisor;
        if (units < chunk || pref[1] == 0) {

            // Format the number, remove useless trailing zeroes.
            QString number(QStringLiteral("%1").arg(double(value) / double(divisor), 0, 'f', qMax(0, maxDecimalDigits)));
            number.remove(QRegExp("0*$"));
            number.remove(QRegExp("\\.$"));

            // Now we have everything we need, return the full string.
            return QStringLiteral("%1%2 %3%4%5")
                    .arg(sign)
                    .arg(qtlFractionToString(value, divisor, maxDecimalDigits))
                    .arg(*pref)
                    .arg(actualBinary ? "i" : "")
                    .arg(unitName);
        }

        // Now try next larger unit.
        divisor *= chunk;

        // After the initial unit, always use the requested decimal/binary unit mode.
        actualBinary = useBinaryUnits;
    }

    // Should not get there, the loop should have returned from inside.
    Q_ASSERT(false);
    return QString();
}


//-----------------------------------------------------------------------------
// Convert a QString into a null-terminated vector of wchar_t.
//-----------------------------------------------------------------------------

QVector<wchar_t> qtlToWCharVector(const QString& str)
{
    const int size = str.size();
    QVector<wchar_t> wstr(size + 1);
    const int returnedSize = str.toWCharArray(&wstr[0]);
    wstr[qMax(0, qMin(returnedSize, size))] = L'\0';
    return wstr;
}
