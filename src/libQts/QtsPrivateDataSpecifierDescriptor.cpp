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
// Define the class QtsPrivateDataSpecifierDescriptor.
// Qts, the Qt MPEG Transport Stream library.
//
//----------------------------------------------------------------------------

#include "QtsPrivateDataSpecifierDescriptor.h"
#include "QtsPsiUtils.h"


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtsPrivateDataSpecifierDescriptor::QtsPrivateDataSpecifierDescriptor(QtsPrivDataSpec p) :
    QtsAbstractDescriptor(QTS_DID_PRIV_DATA_SPECIF),
    pds(p)
{
    // Always valid.
    _isValid = true;
}

QtsPrivateDataSpecifierDescriptor::QtsPrivateDataSpecifierDescriptor(const QtsDescriptor& desc) :
    QtsAbstractDescriptor(QTS_DID_PRIV_DATA_SPECIF)
{
    // Deserialize the descriptor. Let this method set the validity.
    deserialize(desc);
}


//----------------------------------------------------------------------------
// Binary serialization.
//----------------------------------------------------------------------------

bool QtsPrivateDataSpecifierDescriptor::serialize(QtsDescriptor& descriptor) const
{
    // Serialization initialization.
    QtlByteBlock data;
    if (!serializeInit(descriptor, data)) {
        return false;
    }

    // Serialize the payload.
    data.appendUInt32(pds);

    // End of serialization.
    return serializeEnd(descriptor, data);
}


//----------------------------------------------------------------------------
// Binary deserialization.
//----------------------------------------------------------------------------

bool QtsPrivateDataSpecifierDescriptor::deserialize(const QtsDescriptor& descriptor)
{
    // Clear previous content.
    pds = 0;

    // Start deserialization.
    const quint8* payload;
    int payloadSize;
    if (!deserializeInit(descriptor, payload, payloadSize)) {
        return false;
    }

    // Deserialize the payload.
    _isValid = payloadSize == 4;
    if (_isValid) {
        pds = qFromBigEndian<quint32>(payload);
    }
    return _isValid;
}
