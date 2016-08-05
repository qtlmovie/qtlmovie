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
//
// Qtl, Qt utility library.
// Define the class QtlSubStationAlphaStyle.
//
//----------------------------------------------------------------------------

#include "QtlSubStationAlphaStyle.h"
#include "QtlSubStationAlphaParser.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlSubStationAlphaStyle::QtlSubStationAlphaStyle(const QString& definition, const QStringList& format) :
    _name(),
    _isBold(false),
    _isItalic(false),
    _isUnderline(false),
    _color(0x00FFFFFF), // white in RGB format
    _info()
{
    // Split the definition in fields.
    const QStringList values(definition.split(QRegExp(",\\s*")));
    const int count = qMin(format.size(), values.size());

    // Register all fields by name.
    for (int i = 0; i < count; ++i) {
        const QString& type(format[i]);
        const QString value(values[i].trimmed());
        _info.insert(type, value);
        bool ok = false;
        if (type == "name") {
            _name = value;
        }
        else if (type == "bold") {
            _isBold = value.toInt(&ok) != 0 && ok;
        }
        else if (type == "italic") {
            _isItalic = value.toInt(&ok) != 0 && ok;
        }
        else if (type == "underline") {
            _isUnderline = value.toInt(&ok) != 0 && ok;
        }
        else if (type == "primarycolor") {
            _color = toRgbColor(value);
        }
    }
}


//----------------------------------------------------------------------------
// Get one field of the style.
//----------------------------------------------------------------------------

QString QtlSubStationAlphaStyle::getInfo(const QString& name, const QString& defaultValue) const
{
    const QMap<QString,QString>::ConstIterator it = _info.find(QtlSubStationAlphaParser::normalized(name));
    return it == _info.end() ? defaultValue : it.value();
}


//----------------------------------------------------------------------------
// Interpret a string as an RGB color specification.
//----------------------------------------------------------------------------

quint32 QtlSubStationAlphaStyle::toRgbColor(const QString& value, quint32 defaultColor)
{
    quint32 decoded = 0;
    bool ok = false;

    // As a specificity of the SSA format, the "&H" prefix means hexadecimal.
    if (value.startsWith(QStringLiteral("&H"))) {
        decoded = value.mid(2).toInt(&ok, 16);
    }
    else {
        decoded = value.toInt(&ok, 10);
    }

    // In SSA format, the color is encoded as AABBGGRR. We need to reverse the RGB order.
    if (ok) {
        return (decoded & 0xFF00FF00) | ((decoded & 0x000000FF) << 16) | ((decoded & 0x00FF0000) >> 16);
    }
    else {
        return defaultColor;
    }
}
