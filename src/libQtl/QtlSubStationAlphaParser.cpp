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
// Define the class QtlSubRipGenerator.
//
//----------------------------------------------------------------------------

#include "QtlSubStationAlphaParser.h"

//
// Known SSA section names in normalized formats
//
#define QTL_SSA_SCRIPT_INFO "script info"
#define QTL_SSA_V4_STYLES   "v4 styles"
#define QTL_SSA_V4P_STYLES  "v4+ styles"
#define QTL_SSA_EVENTS      "events"
#define QTL_SSA_FONTS       "fonts"      // currently unsupported in this class
#define QTL_SSA_GRAPHICS    "graphics"   // currently unsupported in this class

//
// Known SSA line types in normalized formats
//
#define QTL_SSA_FORMAT      "format"
#define QTL_SSA_STYLE       "style"
#define QTL_SSA_DIALOGUE    "dialogue"


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlSubStationAlphaParser::QtlSubStationAlphaParser(QtlLogger* log, QObject *parent) :
    QObject(parent),
    _nullLog(),
    _log(log == 0 ? &_nullLog : log),
    _isAdvanced(false),
    _scriptInfoCompleted(false),
    _sectionName(),
    _sectionNormalized(),
    _formatList(),
    _infoMap(),
    _styles()
{
}


//----------------------------------------------------------------------------
// Convert an identifier to a normalized format.
//----------------------------------------------------------------------------

QString QtlSubStationAlphaParser::normalized(const QString& name)
{
    return name.toLower().simplified();
}


//----------------------------------------------------------------------------
// Get one field of the script info.
//----------------------------------------------------------------------------

QString QtlSubStationAlphaParser::getScriptInfo(const QString& name, const QString& defaultValue) const
{
    const QMap<QString,QString>::ConstIterator it = _infoMap.find(normalized(name));
    return it == _infoMap.end() ? defaultValue : it.value();
}


//----------------------------------------------------------------------------
// Provide a complete input file.
//----------------------------------------------------------------------------

bool QtlSubStationAlphaParser::addFile(const QString& fileName, const char* codecName)
{
    // Open the file. Note: the ~QFile() destructor will close it.
    QFile file(fileName);
    const bool ok = file.open(QFile::ReadOnly);
    if (ok) {
        // Read the file line by line.
        QTextStream text(&file);
        text.setCodec(codecName);
        QString line;
        while (text.readLineInto(&line)) {
            addLine(line);
        }
    }
    else {
        _log->line(tr("Error opening %1").arg(fileName));
    }
    return ok;
}


//----------------------------------------------------------------------------
// Provide input lines.
//----------------------------------------------------------------------------

void QtlSubStationAlphaParser::addLines(const QStringList& lines)
{
    foreach (const QString& line, lines) {
        addLine(line);
    }
}


//----------------------------------------------------------------------------
// Provide one input line.
//----------------------------------------------------------------------------

void QtlSubStationAlphaParser::addLine(const QString& line)
{
    QString trimmedLine(line.trimmed());

    // Detect start of section
    if (trimmedLine.startsWith(QLatin1Char('[')) && trimmedLine.endsWith(QLatin1Char(']'))) {
        _scriptInfoCompleted = _scriptInfoCompleted || _sectionNormalized == QTL_SSA_SCRIPT_INFO;
        _sectionName = trimmedLine.mid(1, trimmedLine.length() - 2).trimmed();
        _sectionNormalized = normalized(_sectionName);
        _isAdvanced = _isAdvanced || _sectionNormalized == QTL_SSA_V4P_STYLES;
        _formatList.clear(); // "Format:" applies to its enclosing section only.
        emit sectionChanged(_sectionName);
        return;
    }

    // Locate a colon, split between type and value.
    const int colon = trimmedLine.indexOf(QLatin1Char(':'));
    const QString type(colon < 0 ? "" : normalized(trimmedLine.left(colon)));
    const QString value(colon < 0 ? "" : trimmedLine.mid(colon + 1).trimmed());
    const bool validType = colon > 0 && !type.isEmpty() && type != QStringLiteral("!"); // "!:" is a comment

    // "Format:" lines define the layout of the subsequent lines in the current section.
    if (type == QTL_SSA_FORMAT) {
        _formatList.clear();
        foreach (const QString& name, value.split(QRegExp(",\\s*"))) {
            _formatList << normalized(name);
        }
    }

    // Process the content of each supported section.
    // Invalid or unsupported sections are ignored.
    if (_sectionNormalized == QTL_SSA_SCRIPT_INFO) {
        // [Script Info]
        if (validType) {
            _infoMap.insert(type, value);
        }
    }
    else if (_sectionNormalized == QTL_SSA_V4_STYLES || _sectionNormalized == QTL_SSA_V4P_STYLES) {
        // [V4 Styles] or [V4+ Styles]
        if (type == QTL_SSA_STYLE) {
            // This is a style definition.
            const QtlSubStationAlphaStylePtr style(new QtlSubStationAlphaStyle(value, _formatList));
            if (!style.isNull() && !style->name().isEmpty()) {
                _styles.insert(normalized(style->name()), style);
            }
        }
    }
    else if (_sectionNormalized == QTL_SSA_EVENTS) {
        // The [Events] section contains the actual subtitles.
        if (type == QTL_SSA_DIALOGUE) {
            // This is a subtitle frame.
            const QtlSubStationAlphaFramePtr frame(new QtlSubStationAlphaFrame(value, _formatList, _styles));
            if (!frame.isNull()) {
                emit subtitleFrame(frame);
            }
        }
    }
}
