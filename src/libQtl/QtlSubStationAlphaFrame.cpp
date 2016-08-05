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
// Define the class QtlSubStationAlphaFrame.
//
//----------------------------------------------------------------------------

#include "QtlSubStationAlphaFrame.h"
#include "QtlSubStationAlphaParser.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlSubStationAlphaFrame::QtlSubStationAlphaFrame(const QString& definition, const QStringList& format) :
    _info()
{
    // Here, we cannot just split the string using commas as separator since the text,
    // normally the last field, can contain commas.

    int previousColon = -1;
    for (int i = 0; previousColon < definition.length() && i < format.size(); ++i) {

        // Locate next colon.
        int nextColon = definition.indexOf(QLatin1Char(':'), previousColon + 1);
        if (nextColon < 0 || i == format.size() - 1) {
            // This is the last field.
            nextColon = definition.length();
        }

        // Locate and store field.
        const QString& type(format[i]);
        const QString value(definition.mid(previousColon + 1, nextColon - previousColon - 1).trimmed());
        _info.insert(type, value);

        // Prepare for next field.
        previousColon = nextColon;
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
