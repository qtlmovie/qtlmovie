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
// Define the class QtlByteBlock.
//
//----------------------------------------------------------------------------

#include "QtlByteBlock.h"


//-----------------------------------------------------------------------------
// Replace the content of a byte block.
//-----------------------------------------------------------------------------

void QtlByteBlock::copy(const void* addr, int size)
{
    resize(size);
    if (size > 0) {
        ::memcpy(data(), addr, size);
    }
}


//-----------------------------------------------------------------------------
// Increase size by n and return pointer to new n-byte area
//-----------------------------------------------------------------------------

void* QtlByteBlock::enlarge(int n)
{
    const int oldsize = size();
    resize(oldsize + n);
    return data() + oldsize;
}


//-----------------------------------------------------------------------------
// Make sure there are at @n bytes available at the specified @a index.
//-----------------------------------------------------------------------------

void*QtlByteBlock::enlargeFrom(int index, int n)
{
    if (index < 0) {
        index = size();
    }
    if (index + n > size()) {
        resize(index + n);
    }
    return data() + index;
}


//-----------------------------------------------------------------------------
// Append raw data to a byte block.
//-----------------------------------------------------------------------------

void QtlByteBlock::append(const void* addr, int size)
{
    if (size > 0 && addr != 0) {
        ::memcpy(enlarge(size), addr, size);
    }
}


//-----------------------------------------------------------------------------
// Various methods to get strings
//-----------------------------------------------------------------------------

bool QtlByteBlock::getUtf8(int& index, int stringSize, QString& s) const
{
    if (index < 0 || index + stringSize > size()) {
        return false;
    }
    else {
        s = QString::fromUtf8(reinterpret_cast<const char*>(data()) + index, stringSize);
        index += stringSize;
        return true;
    }
}

QString QtlByteBlock::getUtf8(int index, int stringSize) const
{
    return index < 0 || index + stringSize > size() ?
        QString() :
        QString::fromUtf8(reinterpret_cast<const char*>(data()) + index, qMin(stringSize, size() - index));
}

bool QtlByteBlock::getLatin1(int& index, int stringSize, QString& s) const
{
    if (index < 0 || index + stringSize > size()) {
        return false;
    }
    else {
        s = QString::fromLatin1(reinterpret_cast<const char*>(data()) + index, stringSize);
        index += stringSize;
        return true;
    }
}

QString QtlByteBlock::getLatin1(int index, int stringSize) const
{
    return index < 0 || index + stringSize > size() ?
        QString() :
        QString::fromLatin1(reinterpret_cast<const char*>(data()) + index, qMin(stringSize, size() - index));
}
