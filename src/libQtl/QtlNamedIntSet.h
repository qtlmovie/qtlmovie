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
//! @file QtlNamedIntSet.h
//!
//! Declare the class QtlNamedIntSet.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLNAMEDINTSET_H
#define QTLNAMEDINTSET_H

#include "QtlCore.h"
#include "QtlLogger.h"

//!
//! A class containing a set of integer values with names.
//! The values are 32-bit unsigned integers (quint32).
//!
class QtlNamedIntSet
{
public:
    //!
    //! Constructor.
    //! @param [in] log Where to log errors. Ignored if zero.
    //!
    explicit QtlNamedIntSet(QtlLogger* log = 0);

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy.
    //!
    QtlNamedIntSet(const QtlNamedIntSet& other);

    //!
    //! Assignment.
    //! @param [in] other Other instance to copy.
    //!
    const QtlNamedIntSet& operator=(const QtlNamedIntSet& other);

    //!
    //! Add a value.
    //! If a value with the same name already exists, replace it.
    //! @param [in] value Integer value to add.
    //! @param [in] name Name of the value.
    //!
    void addValue(quint32 value, const QString& name);

    //!
    //! Add a definition of the form "value = name".
    //! Empty lines or lines starting with '#' are ignored.
    //! If a value with the same name already exists, replace it.
    //! Errors are logged if a logger was specified.
    //! @param [in] definition A string of the form "value = name".
    //! Base prefix like "0x" or "0" are recognized in the value.
    //! @return True on success, false on error.
    //!
    bool addDefinition(const QString& definition);

    //!
    //! Add a list of definitions of the form "value = name".
    //! Empty lines or lines starting with '#' are ignored.
    //! If a value with the same name already exists, replace it.
    //! Errors are logged if a logger was specified.
    //! @param [in] definitions A list of strings of the form "value = name".
    //! Base prefix like "0x" or "0" are recognized in the value.
    //! @return True on success, false on error.
    //!
    bool addDefinitions(const QStringList& definitions);

    //!
    //! Load a list of definitions of the form "value = name" from a file.
    //! Empty lines or lines starting with '#' are ignored.
    //! If a value with the same name already exists, replace it.
    //! Errors are logged if a logger was specified.
    //! @param [in] fileName A text file containing lines of the form "value = name".
    //! Base prefix like "0x" or "0" are recognized in the value.
    //! @return True on success, false on error.
    //!
    bool loadDefinitions(const QString& fileName);

    //!
    //! Get a list of all names.
    //! @return A list of al names.
    //!
    QStringList names() const;

    //!
    //! Get a value from a name.
    //! @param [in] name Name of the value or a string representation of an integer.
    //! @return The corresponding value or defaultValue() if the value has no name.
    //!
    quint32 value(const QString& name) const
    {
        return value(name, _defaultValue);
    }

    //!
    //! Get a value from a name.
    //! @param [in] name Name of the value or a string representation of an integer.
    //! @param [in] defaultValue Default value to return if @a name is unknown.
    //! @return The corresponding value or @a defaultValue if the value has no name.
    //!
    quint32 value(const QString& name, quint32 defaultValue) const;

    //!
    //! Get the name of an integer.
    //! If several names have the same value, return one of these names.
    //! If no name exists, a string representation of the value is returned.
    //! @param [in] value The value to get the name of.
    //! @param [in] defaultBase If @a value has no name, base of the string representation.
    //! Valid values are 10, 16 and 8. If a negative value is specified, use defaultBase().
    //! For any other value, return an empty string.
    //! @param [in] addBasePrefix If @a defaultBase is 16 or 8, add the standard prefix "0x" or "0".
    //! @param [in] width If @a value has no name, gives the width of the value representation,
    //! not including the prefix. If a negative value is specified, use defaultWidth().
    //! If zero is specified, do not justify.
    //! @return The name for @a value or a string representation.
    //!
    QString name(quint32 value, int defaultBase = -1, bool addBasePrefix = true, int width = -1) const;

    //!
    //! Check if an integer has a name.
    //! @param [in] value The value to check the name of.
    //! @return True if @a value has a name, false otherwise.
    //!
    bool hasName(quint32 value) const
    {
        return _mapByName.values().contains(value);
    }

    //!
    //! Get the default value for value().
    //! @return The value for value().
    //! @see value(const QString&)
    //!
    quint32 defaultValue() const
    {
        return _defaultValue;
    }

    //!
    //! Set the default value for value().
    //! @param [in] defaultValue The value for value().
    //! @see value(const QString&)
    //!
    void setDefaultValue(const quint32& defaultValue)
    {
        _defaultValue = defaultValue;
    }

    //!
    //! Get the default base for name().
    //! @return The default base for name().
    //! @see name()
    //!
    int defaultBase() const
    {
        return _defaultBase;
    }

    //!
    //! Set the default base for name().
    //! @param [in] defaultBase The default base for name().
    //! @see name()
    //!
    void setDefaultBase(int defaultBase)
    {
        _defaultBase = defaultBase;
    }

    //!
    //! Get the default width for name().
    //! @return The default width for name().
    //! @see name()
    //!
    int defaultWidth() const
    {
        return _defaultWidth;
    }

    //!
    //! Set the default width for name().
    //! @param [in] defaultWidth The default width for name().
    //! @see name()
    //!
    void setDefaultWidth(int defaultWidth)
    {
        _defaultWidth = defaultWidth;
    }

private:
    typedef QMap<QString,quint32> MapByName;   //!< Translate names to values.

    QtlLogger*    _log;           //!< Where to log errors.
    MapByName     _mapByName;     //!< Translate names to values.
    quint32       _defaultValue;  //!< Default value for value().
    int           _defaultBase;   //!< Default base for name().
    int           _defaultWidth;  //!< Default width for name().
};

#endif // QTLNAMEDINTSET_H
