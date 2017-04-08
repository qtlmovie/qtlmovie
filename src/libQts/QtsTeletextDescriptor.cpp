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
// Define the class QtsTeletextDescriptor.
// Qts, the Qt MPEG Transport Stream library.
//
// References:
// [1] ETSI EN 300 706 V1.2.1, "Enhanced Teletext specification"
// [2] ETSI EN 300 468 V1.11.1, "Digital Video Broadcasting (DVB);
//     Specification for Service Information (SI) in DVB systems"
//
// Teletext page encoding:
// - There is a 3-bit magazine number "M". M=0 means 8 (see [1], section 3.1).
// - There is an 8-bit page number 0xNN. The hexa digits NN are used as
//   decimal digits as in BCD.
// - The actual page number in decimal is MNN.
// Example: magazine = 0x00 (means 8), page number = 0x89, then the actual
// page number in decimal representation is 889.
//
//----------------------------------------------------------------------------

#include "QtsTeletextDescriptor.h"
#include "QtsPsiUtils.h"


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtsTeletextDescriptor::QtsTeletextDescriptor() :
    QtsAbstractDescriptor(QTS_DID_TELETEXT)
{
    // An empty descriptor is valid.
    _isValid = true;
}

QtsTeletextDescriptor::QtsTeletextDescriptor(const QtsDescriptor& desc) :
    QtsAbstractDescriptor(QTS_DID_TELETEXT)
{
    // Deserialize the descriptor. Let this method set the validity.
    deserialize(desc);
}


//----------------------------------------------------------------------------
// Binary serialization.
//----------------------------------------------------------------------------

bool QtsTeletextDescriptor::serialize(QtsDescriptor& descriptor) const
{
    // Serialization initialization.
    QtlByteBlock data;
    if (!serializeInit(descriptor, data)) {
        return false;
    }

    // Serialize all entries.
    foreach (const Entry& e, entries) {
        if (!qtsAppendIso639Language(e.language, data)) {
            return false; // error, language too long.
        }
        // See encoding of page number in file header comments.
        const quint8 mag = quint8((e.page / 100) % 8);
        const quint8 page = (((e.page / 10) % 10) << 4) | (e.page % 10);
        data.appendUInt8((quint8(e.type) << 3) | mag);
        data.appendUInt8(page);
    }

    // End of serialization.
    return serializeEnd(descriptor, data);
}


//----------------------------------------------------------------------------
// Binary deserialization.
//----------------------------------------------------------------------------

bool QtsTeletextDescriptor::deserialize(const QtsDescriptor& descriptor)
{
    // Clear previous content.
    entries.clear();

    // Start deserialization.
    const quint8* payload;
    int payloadSize;
    if (!deserializeInit(descriptor, payload, payloadSize)) {
        return false;
    }

    // Deserialize the payload.
    // The payload is a list of 5-byte entries.
    while (payloadSize >= 5) {

        // Deserialize one entry.
        Entry e;
        e.language = qtsGetIso639Language(payload);
        e.type = payload[3] >> 3;
        // See encoding of page number in file header comments.
        const quint8 mag = payload[3] & 0x07;
        const quint8 page = payload[4];
        e.page = 100 * int(mag == 0 ? 8 : mag) + 10 * int(page >> 4) + int(page & 0x0F);
        entries << e;

        // Point to next entry.
        payload += 5;
        payloadSize -= 5;
    }

    // Check that there is no partial entry left.
    return _isValid = payloadSize == 0;
}
