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
//
// Qtl, Qt utility library.
// Define the class QtlVersion.
//
//----------------------------------------------------------------------------

#include "QtlVersion.h"
#include "QtlStringUtils.h"


//----------------------------------------------------------------------------
// Set the version from a list of integer values.
//----------------------------------------------------------------------------

void QtlVersion::setValues(const QList<int>& values)
{
    // There is not original text representation.
    _text.clear();

    // Check that no value is negative.
    foreach (int i, values) {
        if (i < 0) {
            _values.clear();
            return;
        }
    }

    // All values are correct.
    _values = values;
}


//----------------------------------------------------------------------------
// Get the textual representation of the version.
//----------------------------------------------------------------------------

QString QtlVersion::text() const
{
    if (_text.isEmpty()) {
        // No original text representation. Build a dotted representation.
        QString text;
        foreach (int i, _values) {
            if (!text.isEmpty()) {
                text.append(".");
            }
            text.append(QString::number(i));
        }
        return text;
    }
    else {
        // There was an original text representation.
        return _text;
    }
}


//----------------------------------------------------------------------------
// Set the version from a text representation.
//----------------------------------------------------------------------------

void QtlVersion::setText(const QString& text)
{
    // Clear the content.
    _values.clear();
    _text.clear();

    // Cut the version string.
    const QStringList fields(text.split(QRegExp("[-\\.]"), QString::SkipEmptyParts));

    // Build the list of integers.
    foreach (const QString& s, fields) {
        const int i = qtlToInt(s, -1, 0);
        if (i < 0) {
            _values.clear();
            return;
        }
        _values << i;
    }

    // Keep the original representation.
    _text = text;
}


//----------------------------------------------------------------------------
// Comparison operator.
//----------------------------------------------------------------------------

bool QtlVersion::operator <(const QtlVersion& other) const
{
    // Compare field by field.
    for (int i = 0; i < _values.size(); ++i) {
        if (i >= other._values.size()) {
            // This instance has more fields, it is greater than other.
            return false;
        }
        if (_values[i] != other._values[i]) {
            // Both versions differ at this point.
            return _values[i] < other._values[i];
        }
    }

    // Other has at least as much components as this and are all identical.
    return _values.size() < other._values.size();
}
