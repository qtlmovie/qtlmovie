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
// Define the template class QtlTlvList.
//
//----------------------------------------------------------------------------

#ifndef QTLTLVLISTTEMPLATE_H
#define QTLTLVLISTTEMPLATE_H

#include "QtlTlvList.h"


//----------------------------------------------------------------------------
// Serialize the TLV list at end of a given byte block.
//----------------------------------------------------------------------------

template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
void QtlTlvList<TAG,LENGTH,ORDER>::appendTo(QtlByteBlock& data) const
{
    foreach (const Tlv& tlv, *this) {
        tlv.appendTo(data);
    }
}


//----------------------------------------------------------------------------
// Deserialize the TLV list from a given byte block.
//----------------------------------------------------------------------------

template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
bool QtlTlvList<TAG,LENGTH,ORDER>::readAt(const QtlByteBlock& data, int& index, int end)
{
    if (end < 0) {
        end = data.size();
    }
    bool ok = true;
    int tempIndex = index;
    Tlv tlv;
    while (tempIndex < end && ok) {
        ok = tlv.readAt(data, tempIndex, end);
        if (ok) {
            this->append(tlv);
        }
    }
    return ok;
}


//----------------------------------------------------------------------------
// Validate the TLV list according to its tags definitions.
//----------------------------------------------------------------------------

template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
bool QtlTlvList<TAG,LENGTH,ORDER>::validate() const
{
    // Number of occurences of tags.
    QMap<TAG,int> tagCount;

    // Check all tags in the list.
    foreach (const Tlv& tlv, *this) {

        // Find tag in the definitions.
        typename TagDefinitionMap::ConstIterator it = _tagMap.find(tlv.tag());
        if (it == _tagMap.end()) {
            // This tag is not allowed.
            return false;
        }

        // Check value size.
        if (tlv.valueSize() < it.value().minSize || tlv.valueSize() > it.value().maxSize) {
            // Invalid TLV value size
            return false;
        }

        // Count tags.
        tagCount[tlv.tag()]++;
    }

    // It this point, all tags in the list are valid.
    // Now, check that the cardinality of each tag.
    for (typename TagDefinitionMap::ConstIterator it = _tagMap.begin(); it != _tagMap.end(); ++it) {
        const int count = tagCount[it.key()];
        if (count < it.value().minCount || count > it.value().maxCount) {
            // Invalid number of occurences of this tag.
            return false;
        }
    }

    // Now, the list is valid.
    return true;
}

#endif // QTLTLVLISTTEMPLATE_H
