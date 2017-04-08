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
// Define the class QtlNamedIntSet.
// Qtl, Qt utility library.
//
//----------------------------------------------------------------------------

#include "QtlNamedIntSet.h"
#include "QtlFile.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlNamedIntSet::QtlNamedIntSet(QtlLogger* log) :
    _log(log),
    _mapByName(),
    _defaultValue(0xFFFFFFFFL),
    _defaultBase(10),
    _defaultWidth(0)
{
}


//----------------------------------------------------------------------------
// Copy constructor.
//----------------------------------------------------------------------------

QtlNamedIntSet::QtlNamedIntSet(const QtlNamedIntSet& other) :
    _log(other._log),
    _mapByName(other._mapByName),
    _defaultValue(other._defaultValue),
    _defaultBase(other._defaultBase),
    _defaultWidth(other._defaultWidth)
{
}


//----------------------------------------------------------------------------
// Assignment.
//----------------------------------------------------------------------------

const QtlNamedIntSet& QtlNamedIntSet::operator=(const QtlNamedIntSet& other)
{
    if (&other != this) {
        _log = other._log;
        _mapByName = other._mapByName;
        _defaultValue = other._defaultValue;
        _defaultBase = other._defaultBase;
        _defaultWidth = other._defaultWidth;
    }
    return *this;
}


//----------------------------------------------------------------------------
// Add a value.
//----------------------------------------------------------------------------

void QtlNamedIntSet::addValue(quint32 value, const QString& name)
{
    _mapByName.insert(name, value);
}


//----------------------------------------------------------------------------
// Add a definition of the form "value = name".
//----------------------------------------------------------------------------

bool QtlNamedIntSet::addDefinition(const QString& definition)
{
    // Filter out empty and command lines.
    const QString def(definition.trimmed());
    if (def.isEmpty() || def[0] == '#') {
        return true;
    }

    // Split the value=name
    const QStringList fields(def.trimmed().split(QRegExp("\\s*=\\s*")));
    if (fields.size() == 2 && !fields[1].isEmpty()) {
        // Decode integer value.
        bool ok = false;
        const quint32 value = fields[0].toULong(&ok, 0);
        if (ok) {
            // Found a value.
            _mapByName.insert(fields[1], value);
            return true;
        }
    }

    // At this point, the value=name is invalid
    if (_log != 0) {
        _log->line(QObject::tr("Invalid \"value=name\": %1").arg(definition));
    }
    return false;
}


//----------------------------------------------------------------------------
// Add a list of definitions of the form "value = name".
//----------------------------------------------------------------------------

bool QtlNamedIntSet::addDefinitions(const QStringList& definitions)
{
    bool ok = true;
    foreach (const QString& def, definitions) {
        ok = addDefinition(def) && ok;
    }
    return ok;
}


//----------------------------------------------------------------------------
// Load a list of definitions of the form "value = name" from a file.
//----------------------------------------------------------------------------

bool QtlNamedIntSet::loadDefinitions(const QString& fileName)
{
    return addDefinitions(QtlFile::readTextLinesFile(fileName));
}


//----------------------------------------------------------------------------
// Get a list of all names.
//----------------------------------------------------------------------------

QStringList QtlNamedIntSet::names() const
{
    return QStringList(_mapByName.keys());
}


//----------------------------------------------------------------------------
// Get a value from a name.
//----------------------------------------------------------------------------

quint32 QtlNamedIntSet::value(const QString& name, quint32 defaultValue) const
{
    // Is this a real name.
    const MapByName::ConstIterator it(_mapByName.find(name));
    if (it != _mapByName.end()) {
        return *it;
    }

    // Is this a representation of an integer.
    bool ok = false;
    const quint32 i = name.toULong(&ok, 0);
    return ok ? i : defaultValue;
}


//----------------------------------------------------------------------------
// Get the name of an integer.
//----------------------------------------------------------------------------

QString QtlNamedIntSet::name(quint32 value, int defaultBase, bool addBasePrefix, int width) const
{
    // Loop through the map to search the value.
    for (MapByName::ConstIterator it = _mapByName.begin(); it != _mapByName.end(); ++it) {
        if (it.value() == value) {
            return it.key();
        }
    }

    // Value not found, format a string representation of the value.
    if (defaultBase < 0) {
        defaultBase = _defaultBase;
    }
    if (width < 0) {
        width = _defaultWidth;
    }
    switch (defaultBase) {
    case 10:
        return QStringLiteral("%1").arg(ulong(value), width, 10, QChar(' '));
    case 16:
        return (addBasePrefix ? "0x" : "") + QStringLiteral("%1").arg(ulong(value), width, 16, QChar('0')).toUpper();
    case 8:
        return (addBasePrefix ? "0" : "") + QStringLiteral("%1").arg(ulong(value), width, 8, QChar('0'));
    default:
        return QString();
    }
}
