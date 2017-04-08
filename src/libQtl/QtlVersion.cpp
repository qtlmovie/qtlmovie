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
// Define the class QtlVersion.
//
//----------------------------------------------------------------------------

#include "QtlVersion.h"
#include "QtlStringUtils.h"


//----------------------------------------------------------------------------
// Constructors and destructors.
//----------------------------------------------------------------------------

QtlVersion::QtlVersion() :
    _values(),
    _beta(-1),
    _text()
{
}

QtlVersion::QtlVersion(const QString& text)
{
    setText(text);
}

QtlVersion::QtlVersion(const QList<int>& values, int beta)
{
    setValues(values, beta);
}

void QtlVersion::clear()
{
    _values.clear();
    _beta = -1;
    _text.clear();
}


//----------------------------------------------------------------------------
// Set the version from a list of integer values.
//----------------------------------------------------------------------------

void QtlVersion::setValues(const QList<int>& values, int beta)
{
    // There is not original text representation.
    clear();

    // Check that no value is negative.
    foreach (int i, values) {
        if (i < 0) {
            return;
        }
    }

    // All values are correct.
    _values = values;

    // Make sure that beta has no other negative value than -1.
    _beta = beta < 0 ? -1 : beta;
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
        if (_beta >= 0) {
            // Default "beta" prefix is "rc" (release candidate).
            text.append("-rc");
            text.append(QString::number(_beta));
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
    clear();

    // Keep the original representation.
    _text = text;

    // Cut the version string.
    QStringList fields(text.split(QRegExp("[-\\.]"), QString::SkipEmptyParts));

    // If the last field starts with a letter, this is beta version.
    if (!fields.isEmpty() && !fields.last().isEmpty() && fields.last().at(0).isLetter()) {
        // Remove last field.
        QString s(fields.takeLast());
        // Remove all leading non-digit characters.
        s.remove(QRegExp("^\\D*"));
        // Interpret the rest as a positive number.
        _beta = qtlToInt(s, -1, 0);
        if (_beta < 0) {
            clear();
            return;
        }
    }

    // Build the list of integers.
    foreach (const QString& s, fields) {
        const int i = qtlToInt(s, -1, 0);
        if (i < 0) {
            clear();
            return;
        }
        _values << i;
    }
}


//----------------------------------------------------------------------------
// Comparison operators.
//----------------------------------------------------------------------------

bool QtlVersion::operator==(const QtlVersion& other) const
{
    return _values == other._values && _beta == other._beta;
}

bool QtlVersion::operator <(const QtlVersion& other) const
{
    // Compare field by field.
    for (int i = 0; i < _values.size(); ++i) {
        if (i >= other._values.size()) {
            // This instance has more fields than other and all previous fields were identical.
            // This instance is greater than other.
            // Example: "1.2.3.1" > "1.2.3"
            return false;
        }
        if (_values[i] != other._values[i]) {
            // Both versions differ at this point.
            return _values[i] < other._values[i];
        }
    }

    // Other has at least as many components as this and are all identical.
    if (_values.size() < other._values.size()) {
        // This instance has less fields than other and all previous fields were identical.
        // This instance is lower than other.
        // Example: "1.2.3" < "1.2.3.1"
        return true;
    }
    else if (isBeta() == other.isBeta()) {
        // All values are identical. This instance and other are either both beta
        // releases or both official releases. If they are both official releases,
        // they are equal and we must return false.
        return _beta < other._beta;
    }
    else {
        // All values are identical. One of this instance and other is beta and
        // the other one is an official releases. The beta version preceeds the
        // official version.
        return _beta >= 0;
    }
}
