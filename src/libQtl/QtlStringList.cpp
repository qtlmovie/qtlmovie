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
// Define the class QtlStringList.
//
//----------------------------------------------------------------------------

#include "QtlStringList.h"


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtlStringList::QtlStringList() : QStringList()
{
}

QtlStringList::QtlStringList(const QString& s1) : QStringList(s1)
{
}

QtlStringList::QtlStringList(const QString& s1, const QString& s2) : QStringList(s1)
{
    *this << s2;
}

QtlStringList::QtlStringList(const QString& s1, const QString& s2, const QString& s3) : QStringList(s1)
{
    *this << s2 << s3;
}

QtlStringList::QtlStringList(const QString& s1, const QString& s2, const QString& s3, const QString& s4) : QStringList(s1)
{
    *this << s2 << s3 << s4;
}

QtlStringList::QtlStringList(const QList<QString>& other) : QStringList(other)
{
}


//----------------------------------------------------------------------------
// Get the maximum length of all strings.
//----------------------------------------------------------------------------

int QtlStringList::maxLength() const
{
    int max = 0;
    for (ConstIterator it = begin(); it != end(); ++it) {
        if (it->length() > max) {
            max = it->length();
        }
    }
    return max;
}
