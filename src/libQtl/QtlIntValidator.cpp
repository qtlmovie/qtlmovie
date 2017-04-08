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
// Define the class QtlIntValidator.
// Qtl, Qt utility library.
//
//-----------------------------------------------------------------------------

#include "QtlIntValidator.h"

const QRegExp QtlIntValidator::_hexaRegExp(QStringLiteral("0[xX][0-9A-Fa-f]+"));
const QRegExp QtlIntValidator::_decRegExp(QStringLiteral("[0-9]+"));


//-----------------------------------------------------------------------------
// Validate an input string. Reimplemented from QValidator::validate().
//-----------------------------------------------------------------------------

QValidator::State QtlIntValidator::validate(QString& input, int& pos) const
{
    // Remove leading and trailing spaces.
    input = input.trimmed();

    // Is this the beginning of a [hexa]decimal string?
    if (input.isEmpty() || input == "0x" || input == "0X") {
        return Intermediate;
    }

    // Is this an [hexa]decimal string?
    bool ok = false;
    quint32 value = 0;
    if (_decRegExp.exactMatch(input)) {
        value = input.toULong(&ok, 10);
    }
    else if (_hexaRegExp.exactMatch(input)) {
        value = input.mid(2).toULong(&ok, 16);
    }

    // Check validity of the string.
    if (!ok || value > _maximum) {
        return Invalid;
    }
    else if (value < _minimum) {
        return Intermediate;
    }
    else {
        return Acceptable;
    }
}
