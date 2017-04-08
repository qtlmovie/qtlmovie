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
// Define the class QtlSubStationAlphaFrame.
//
//----------------------------------------------------------------------------

#include "QtlSubStationAlphaFrame.h"
#include "QtlSubStationAlphaParser.h"
#include "QtlStringUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlSubStationAlphaFrame::QtlSubStationAlphaFrame(const QString& definition,
                                                 const QStringList& format,
                                                 const QtlSubStationAlphaStyleMap& styles) :
    _text(),
    _style(),
    _start(0),
    _end(0),
    _info()
{
    // Here, we cannot just split the string using commas as separator since the text,
    // normally the last field, can contain commas.

    int previousComma = -1;
    for (int i = 0; previousComma < definition.length() && i < format.size(); ++i) {

        // Locate next colon.
        int nextComma = definition.indexOf(QLatin1Char(','), previousComma + 1);
        if (nextComma < 0 || i == format.size() - 1) {
            // This is the last field.
            nextComma = definition.length();
        }

        // Locate and store field.
        const QString& type(format[i]);
        const QString value(definition.mid(previousComma + 1, nextComma - previousComma - 1).trimmed());
        _info.insert(type, value);

        // Prepare for next field.
        previousComma = nextComma;

        // Filter known fields.
        if (type == "style") {
            const QtlSubStationAlphaStyleMap::ConstIterator it = styles.find(QtlSubStationAlphaParser::normalized(value));
            if (it != styles.end()) {
                _style = it.value();
            }
        }
        else if (type == "text") {
            _text = value;
        }
        else if (type == "start") {
            _start = toTimestamp(value);
        }
        else if (type == "end") {
            _end = toTimestamp(value);
        }
    }
}


//----------------------------------------------------------------------------
// Get one field of the style.
//----------------------------------------------------------------------------

QString QtlSubStationAlphaFrame::getInfo(const QString& name, const QString& defaultValue) const
{
    const QMap<QString,QString>::ConstIterator it = _info.find(QtlSubStationAlphaParser::normalized(name));
    return it == _info.end() ? defaultValue : it.value();
}


//----------------------------------------------------------------------------
// Convert an SSA/ASS timestamp specification to a value in milliseconds.
//----------------------------------------------------------------------------

quint64 QtlSubStationAlphaFrame::toTimestamp(const QString& value)
{
    // Expected format: "H:MM:SS:hh" (as in SSA/ASS spec) or "H:MM:SS.hh" (as seen in real files).
    // We are very permissive on format here.
    const QStringList strFields = value.split(QRegExp("[:\\.]"));
    if (strFields.size() != 4) {
        return 0; // invalid
    }

    QList<int> intFields;
    foreach (const QString& s, strFields) {
        const int i = qtlToInt(s);
        if (i < 0) {
            return 0; // invalid
        }
        intFields << i;
    }

    return  quint64(intFields[0]) * 3600000 +  // hours
            quint64(intFields[1]) * 60000 +    // minutes
            quint64(intFields[2]) * 1000 +     // seconds
            quint64(intFields[3]) * 10;        // hundredth
}

//----------------------------------------------------------------------------
// Convert a color code in RGB format into a SubRip color specification.
//----------------------------------------------------------------------------

QString QtlSubStationAlphaFrame::rgbToSubRip(quint32 color)
{
    return QStringLiteral("#%1%2%3")
            .arg((color >> 16) & 0xFF, 2, 16, QLatin1Char('0'))
            .arg((color >> 8) & 0xFF, 2, 16, QLatin1Char('0'))
            .arg(color & 0xFF, 2, 16, QLatin1Char('0'));
}


//----------------------------------------------------------------------------
// Convert the text of this subtitle frame in SubRip format.
//----------------------------------------------------------------------------

QStringList QtlSubStationAlphaFrame::toSubRip(bool useHtmlTags) const
{
    QString text(_text);

    // Transform supported "style override codes".
    text.replace("{\\b1}", useHtmlTags ? "<b>" : "", Qt::CaseInsensitive);
    text.replace("{\\b0}", useHtmlTags ? "</b>" : "", Qt::CaseInsensitive);
    text.replace("{\\i1}", useHtmlTags ? "<i>" : "", Qt::CaseInsensitive);
    text.replace("{\\i0}", useHtmlTags ? "</i>" : "", Qt::CaseInsensitive);
    text.replace("{\\u1}", useHtmlTags ? "<u>" : "", Qt::CaseInsensitive);
    text.replace("{\\u0}", useHtmlTags ? "</u>" : "", Qt::CaseInsensitive);

    // Remove unsupported "style override codes".
    text.remove(QRegExp("{\\\\[^}]*}"));

    // Apply style.
    if (useHtmlTags && !_style.isNull()) {
        if (_style->isBold()) {
            text.prepend("<b>");
            text.append("</b>");
        }
        if (_style->isItalic()) {
            text.prepend("<i>");
            text.append("</i>");
        }
        if (_style->isUnderline()) {
            text.prepend("<u>");
            text.append("</u>");
        }
        text.prepend(QStringLiteral("<font color=\"%1\">").arg(rgbToSubRip(_style->color())));
        text.append("</font>");
    }

    // Split using new lines.
    return text.split(QRegExp("\\\\[nN]"));
}
