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
// Qts, the Qt MPEG Transport Stream library.
// Define the class QtsDescriptor.
//
//----------------------------------------------------------------------------

#include "QtsDescriptor.h"


//-----------------------------------------------------------------------------
// Constructors.
// Note that the max size of a descriptor is 257 bytes: 2 (header) + 255
//-----------------------------------------------------------------------------

QtsDescriptor::QtsDescriptor(const void* addr, int size) :
    _data()
{
    // Check the validity of the descriptor content.
    if (size >= 2 && size <= QTS_MAX_DESCRIPTOR_SIZE && (reinterpret_cast<const quint8*>(addr))[1] == size - 2) {
        _data.copy(addr, size);
    }
}

QtsDescriptor::QtsDescriptor(const QtlByteBlock& content) :
    _data()
{
    // Check the validity of the descriptor content.
    if (content.size() >= 2 && content.size() <= QTS_MAX_DESCRIPTOR_SIZE && content[1] == content.size() - 2) {
        _data = content;
    }
}


//-----------------------------------------------------------------------------
// Assignment operator.
//-----------------------------------------------------------------------------

const QtsDescriptor& QtsDescriptor::operator=(const QtsDescriptor& other)
{
    if (&other != this) {
        _data = other._data;
    }
    return *this;
}


//-----------------------------------------------------------------------------
// Check if a descriptor has valid content.
//-----------------------------------------------------------------------------

bool QtsDescriptor::isValid() const
{
    return _data.size() >= 2 && int(_data[1]) + 2 == _data.size();
}


//-----------------------------------------------------------------------------
// Replace the payload of the descriptor.
//-----------------------------------------------------------------------------

void QtsDescriptor::replacePayload(const void* addr, int size)
{
    if (size > QTS_MAX_DESCRIPTOR_PAYLOAD_SIZE) {
        // Payload size too long, invalidate descriptor.
        _data.clear();
    }
    else if (!_data.isEmpty()) {
        Q_ASSERT(_data.size() >= 2);
        // Erase previous payload.
        _data.remove(2, _data.size() - 2);
        // Add new payload.
        _data.append(addr, size);
        // Adjust descriptor size
        _data[1] = quint8(_data.size() - 2);
    }
}


//-----------------------------------------------------------------------------
// Resize (truncate or extend) the payload of the descriptor.
//-----------------------------------------------------------------------------

void QtsDescriptor::resizePayload(int size)
{
    if (size > QTS_MAX_DESCRIPTOR_PAYLOAD_SIZE) {
        // Payload size too long, invalidate descriptor.
        _data.clear();
    }
    else if (!_data.isEmpty()) {
        Q_ASSERT(_data.size() >= 2);
        // Resize payload.
        const int oldSize = _data.size() - 2;
        _data.resize(size + 2);
        // If payload extended, zero additional bytes
        if (size > oldSize) {
            ::memset(_data.data() + 2 + oldSize, 0, size - oldSize);
        }
        // Adjust descriptor size
        _data[1] = quint8(_data.size() - 2);
    }
}
