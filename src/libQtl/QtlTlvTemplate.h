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
// Define the template class QtlTlv.
//
//----------------------------------------------------------------------------

#ifndef QTLTLVTEMPLATE_H
#define QTLTLVTEMPLATE_H

#include "QtlTlv.h"


//----------------------------------------------------------------------------
// Set the binary value.
//----------------------------------------------------------------------------

template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
void QtlTlv<TAG,LENGTH,ORDER>::setValue(const QtlByteBlock& value)
{
    const int max = intLengthMax();
    if (value.size() > max) {
        _value = value.mid(0, max);
    }
    else {
        _value = value;
    }
}


//----------------------------------------------------------------------------
// Serialize the TLV at end of a given byte block.
//----------------------------------------------------------------------------

template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
void QtlTlv<TAG,LENGTH,ORDER>::appendTo(QtlByteBlock& data) const
{
    data.appendToByteOrder<TAG>(_tag, ORDER);
    data.appendToByteOrder<LENGTH>(static_cast<LENGTH>(_value.size()), ORDER);
    data.append(_value);
}


//----------------------------------------------------------------------------
// Deserialize the TLV from a given byte block.
//----------------------------------------------------------------------------

template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
bool QtlTlv<TAG,LENGTH,ORDER>::readAt(const QtlByteBlock& data, int& index, int end)
{
    if (end < 0) {
        end = data.size();
    }
    TAG tag = 0;
    LENGTH length = 0;
    int tempIndex = index;
    // Check the validity of the TLV data.
    // Note 1: Must check "length >= 0" when LENGTH is a signed type.
    // Note 2: But "length >= 0" generates a warning when LENGTH is an unsigned
    // type on some compilers => use "length + 1 >= 1".
    // Note 3: "tempIndex + int(length)" must be computed after cheching length validity.
    const bool ok =
            tempIndex + int(sizeof(TAG)) + int(sizeof(LENGTH)) <= end &&
            data.fromByteOrder<TAG>(tempIndex, tag, ORDER) &&
            data.fromByteOrder<LENGTH>(tempIndex, length, ORDER) &&
            length + 1 >= 1 &&
            length <= lengthMax() &&
            tempIndex + int(length) <= end;

    if (ok) {
        _tag = tag;
        _value = data.mid(tempIndex, static_cast<int>(length));
        index = tempIndex + static_cast<int>(length);
    }
    return ok;
}

#endif // QTLTLVTEMPLATE_H
