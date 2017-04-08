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
// Define the class QtsAbstractDescriptor.
// Qts, the Qt MPEG Transport Stream library.
//
//----------------------------------------------------------------------------

#include "QtsAbstractDescriptor.h"


//----------------------------------------------------------------------------
// Serialization helpers
//----------------------------------------------------------------------------

bool QtsAbstractDescriptor::serializeInit(QtsDescriptor& descriptor, QtlByteBlock& data) const
{
    // Invalidate the binary descriptor (serialization just starts).
    descriptor.invalidate();

    // Check that this object is valid before serializing..
    if (!_isValid) {
        return false;
    }
    else {
        // Build binary header.
        data.resize(2);
        data.reserve(QTS_MAX_DESCRIPTOR_SIZE);
        data[0] = _descTag; // descriptor_tag
        data[1] = 0;        // descriptor_length
        return true;
    }
}

bool QtsAbstractDescriptor::serializeEnd(QtsDescriptor& descriptor, QtlByteBlock& data) const
{
    // Check validity of the binary data.
    if (data.size() < 2 || data.size() > QTS_MAX_DESCRIPTOR_SIZE || data[0] != _descTag) {
        descriptor.invalidate();
    }
    else {
        // Update descriptor_length field.
        data[1] = quint8(data.size() - 2);
        // Assign the binary descriptor object.
        descriptor = QtsDescriptor(data);
    }
    return descriptor.isValid();
}


//----------------------------------------------------------------------------
// Deserialization helpers
//----------------------------------------------------------------------------

bool QtsAbstractDescriptor::deserializeInit(const QtsDescriptor& descriptor, const quint8*& payload, int& payloadSize)
{
    // Invalidate this object (deserialization just starts).
    _isValid = false;

    // Check binary descriptor.
    if (descriptor.isValid() && descriptor.tag() == _descTag) {
        payload = descriptor.payload();
        payloadSize = descriptor.payloadSize();
        return true;
    }
    else {
        payload = 0;
        payloadSize = 0;
        return false;
    }
}
