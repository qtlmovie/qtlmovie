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
// Qts, the Qt MPEG Transport Stream library.
// Define the class QtsTsPacket.
//
//----------------------------------------------------------------------------

#define QT_FORCE_ASSERTS 1 // enforce assertions, even without debug.
#include "QtsTsPacket.h"


//-----------------------------------------------------------------------------
// This constant is a null (or stuffing) packet.
//-----------------------------------------------------------------------------

const QtsTsPacket QtsNullPacket = {{
    // Header: PID 0x1FFF
    0x47, 0x1F, 0xFF, 0x10,
    // Payload: 184 bytes 0xFF
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
}};


//-----------------------------------------------------------------------------
// This constant is an empty packet (no payload)
//-----------------------------------------------------------------------------

const QtsTsPacket QtsEmptyPacket = {{
    // Header: PID 0x1FFF, has adaptation field, no payload, CC = 0
    0x47, 0x1F, 0xFF, 0x20,
    // Adaptation field length
    183,
    // Flags: none
    0x00,
    // Adaptation field stuffing: 182 bytes 0xFF
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF
}};


//-----------------------------------------------------------------------------
// Sanity check routine. Ensure that the TSPacket structure can
// be used in contiguous memory array and array of packets.
// Can be used once at startup time in paranoid applications.
//-----------------------------------------------------------------------------

void QtsTsPacket::sanityCheck ()
{
    Q_ASSERT(sizeof (QtsTsPacket) == QTS_PKT_SIZE);
    QtsTsPacket p;
    Q_ASSERT(reinterpret_cast<quint8*>(&p) == p.b);
    QtsTsPacket pa[2];
    Q_ASSERT(reinterpret_cast<quint8*>(&(pa[1])) == reinterpret_cast<quint8*>(&(pa[0])) + QTS_PKT_SIZE);
}


//-----------------------------------------------------------------------------
// Check if the packet contains the start of a clear PES header.
//-----------------------------------------------------------------------------

bool QtsTsPacket::startPes() const
{
    const quint8* const pl = getPayload();
    return hasValidSync() &&
            getPusi() &&
            isClear() &&
            hasPayload() &&
            getPayloadSize() >= 3 &&
            pl[0] == 0x00 &&
            pl[1] == 0x00 &&
            pl[2] == 0x01;
}


//-----------------------------------------------------------------------------
// These private methods compute the offset of PCR, OPCR.
// Return 0 if there is none.
//-----------------------------------------------------------------------------

int QtsTsPacket::pcrOffset() const
{
    return hasPcr() && b[4] >= 7 ? 6 : 0;
}

int QtsTsPacket::opcrOffset() const
{
    if (!hasOpcr()) {
        return 0;
    }
    else if (hasPcr()) {
        return b[4] >= 13 ? 12 : 0;
    }
    else {
        return b[4] >= 7 ? 6 : 0;
    }
}


//-----------------------------------------------------------------------------
// Get PCR or OPCR - 42 bits. Return 0 if not found.
//-----------------------------------------------------------------------------

quint64 QtsTsPacket::getPcr() const
{
    const int offset = pcrOffset();
    return offset == 0 ? 0 : qtsGetPcr(b + offset);
}

quint64 QtsTsPacket::getOpcr() const
{
    const int offset = opcrOffset();
    return offset == 0 ? 0 : qtsGetPcr(b + offset);
}


//-----------------------------------------------------------------------------
// Replace PCR or OPCR - 42 bits
//-----------------------------------------------------------------------------

void QtsTsPacket::setPcr(const quint64& pcr)
{
    const int offset = pcrOffset();
    if (offset > 0) {
        qtsPutPcr(b + offset, pcr);
    }
}

void QtsTsPacket::setOpcr(const quint64& opcr)
{
    const int offset = opcrOffset();
    if (offset > 0) {
        qtsPutPcr(b + offset, opcr);
    }
}


//-----------------------------------------------------------------------------
// These private methods compute the offset of PTS, DTS.
// Return 0 if there is none.
//-----------------------------------------------------------------------------

int QtsTsPacket::ptsOffset() const
{
    if (!startPes()) {
        return 0;
    }
    const int plSize = getPayloadSize();
    const quint8* const pl = getPayload();
    if (plSize < 14 || !qtsIsLongHeaderPesStreamId(pl[3])) {
        return 0;
    }
    const quint8 flags = pl[7] >> 6;
    if ((flags & 0x02) == 0 ||
        (flags == 0x02 && (pl[9] & 0xF1) != 0x21) ||
        (flags == 0x03 && (pl[9] & 0xF1) != 0x31) ||
        (pl[11] & 0x01) != 0x01 ||
        (pl[13] & 0x01) != 0x01) {
        return 0;
    }
    return pl + 9 - b;
}

int QtsTsPacket::dtsOffset() const
{
    if (!startPes()) {
        return 0;
    }
    const int plSize = getPayloadSize();
    const quint8* const pl = getPayload();
    if (plSize < 19 ||
        (pl[7] & 0xC0) != 0xC0 ||
        (pl[9] & 0xF1) != 0x31 ||
        (pl[11] & 0x01) != 0x01 ||
        (pl[13] & 0x01) != 0x01 ||
        (pl[14] & 0xF1) != 0x11 ||
        (pl[16] & 0x01) != 0x01 ||
        (pl[18] & 0x01) != 0x01) {
        return 0;
    }
    return pl + 14 - b;
}


//-----------------------------------------------------------------------------
// Get PTS or DTS at specified offset. Return 0 if offset is zero.
//-----------------------------------------------------------------------------

quint64 QtsTsPacket::getPdts(int offset) const
{
    if (offset == 0) {
        return 0;
    }
    else {
        return (quint64(b[offset] & 0x0E) << 29) |
                (quint64(qFromBigEndian<quint16>(b + offset + 1) & 0xFFFE) << 14) |
                (quint64 (qFromBigEndian<quint16>(b + offset + 3)) >> 1);
    }
}
