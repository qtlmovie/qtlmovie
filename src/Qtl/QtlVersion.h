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
//! @file: QtlVersion.h
//!
//! Declare the class QtlVersion.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLVERSION_H
#define QTLVERSION_H

#include <QtCore>

//!
//! Description of an application version.
//!
//! A version is typically a list of integers.
//! The text representation is the list of integers, separated by dots or dashes.
//! Examples: "1", "1.3.0", "1.2-45".
//!
class QtlVersion
{
public:
    //!
    //! Default constructor.
    //! The version is invalid.
    //!
    QtlVersion()
    {
    }

    //!
    //! Constructor.
    //! @param [in] text Text representation of the version.
    //! The version is invalid if the syntax of @a text is incorrect.
    //!
    QtlVersion(const QString& text)
    {
        setText(text);
    }

    //!
    //! Constructor.
    //! @param [in] values List of integer values.
    //! The version is invalid if the list is empty or any value is negative.
    //!
    QtlVersion(const QList<int> values)
    {
        setValues(values);
    }

    //!
    //! Get the version as a list of integer values.
    //! @return The list of integer values (empty if the version is invalid).
    //!
    QList<int> values() const
    {
        return _values;
    }

    //!
    //! Set the version from a list of integer values.
    //! @param [in] values List of integer values.
    //! The version is invalid if the list is empty or any value is negative.
    //!
    void setValues(const QList<int>& values);

    //!
    //! Get the textual representation of the version.
    //! @return The textual representation of the version.
    //! If the version was built from a valid textual representation,
    //! return the same string. Otherwise, return a dotted representation.
    //!
    QString text() const;

    //!
    //! Set the version from a text representation.
    //! @param [in] text Text representation of the version.
    //! The version is invalid if the syntax of @a text is incorrect.
    //!
    void setText(const QString& text);

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is equal to @a other.
    //! False otherwise.
    //!
    bool operator==(const QtlVersion& other) const
    {
        return _values == other._values;
    }

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is different from @a other.
    //! False otherwise.
    //!
    bool operator!=(const QtlVersion& other) const
    {
        return _values != other._values;
    }

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is less than @a other.
    //! False otherwise.
    //!
    bool operator<(const QtlVersion& other) const;

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is less than or equal to @a other.
    //! False otherwise.
    //!
    bool operator<=(const QtlVersion& other) const
    {
        return *this < other || *this == other;
    }

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is greater than @a other.
    //! False otherwise.
    //!
    bool operator>(const QtlVersion& other) const
    {
        return !(*this <= other);
    }

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is greater than or equal to @a other.
    //! False otherwise.
    //!
    bool operator>=(const QtlVersion& other) const
    {
        return !(*this < other);
    }


    //!
    //! Check if the version is valid.
    //! @return True if this instance is valid.
    //! False otherwise.
    //!
    bool isValid() const
    {
        return !_values.isEmpty();
    }

    //!
    //! Get the number of integer elements in the version.
    //! @return The number of integer elements in the version.
    //!
    int elementCount() const
    {
        return _values.size();
    }

    //!
    //! Invalidate the version content.
    //!
    void clear()
    {
        _values.clear();
        _text.clear();
    }

private:
    QList<int> _values; //!< The list of integer values.
    QString    _text;   //!< Original textual representation.
};

#endif // QTLVERSION_H
