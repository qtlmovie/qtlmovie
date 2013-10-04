//----------------------------------------------------------------------------
//
// Copyright(c) 2013, Thierry Lelegard
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
// CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//
// Qts, the Qt MPEG Transport Stream library.
// Define the class QtsPesPacket.
//
//----------------------------------------------------------------------------

#include "QtsPesPacket.h"


//-----------------------------------------------------------------------------
// Helper for constructors
//-----------------------------------------------------------------------------

void QtsPesPacket::initialize(QtsPid pid)
{
    // Initially invalid
    _isValid = false;
    _headerSize = 0;
    _sourcePid = pid;
    _firstPkt = 0;
    _lastPkt = 0;
    _data.clear();
}


//-----------------------------------------------------------------------------
// Helper for constructors
//-----------------------------------------------------------------------------

void QtsPesPacket::initialize(const QtlByteBlock& bb, QtsPid pid)
{
    // Common initialization.
    initialize(pid);

    // Fixed common header size
    const quint8* data = bb.data();
    const int size = bb.size();
    if (size < 6) {
        return;
    }

    // Check start code prefix: 00 00 01
    if (data[0] != 0 || data[1] != 0 || data[2] != 1) {
        return;
    }

    // Packet structure depends on stream_id
    if (qtsIsLongHeaderPesStreamId(data[3])) {
        // Header size
        if(size < 9) {
            return;
        }
        _headerSize = 9 + int(data[8]);
        if (size < _headerSize) {
            return;
        }
    }
    else {
        // No additional header fields
        _headerSize = 6;
    }

    // Passed all checks.
    _isValid = true;
    _data = bb;
}


//-----------------------------------------------------------------------------
// Clear packet content, becomes an invalid packet.
//-----------------------------------------------------------------------------

void QtsPesPacket::clear()
{
    _isValid = false;
    _headerSize = 0;
    _sourcePid = QTS_PID_NULL;
    _data.clear();
}


//-----------------------------------------------------------------------------
// Comparison.
// The source PID are ignored, only the packet contents are compared.
// Note: Invalid packets are never identical
//-----------------------------------------------------------------------------

bool QtsPesPacket::operator==(const QtsPesPacket& other) const
{
    return _isValid && other._isValid && _data == other._data;
}


//-----------------------------------------------------------------------------
// Check if the PES packet contains MPEG-2 video (also applies to MPEG-1 video)
//-----------------------------------------------------------------------------

bool QtsPesPacket::isMpeg2Video() const
{
    // Must have a video stream_id and payload must start with 00 00 01
    if (!qtsIsVideoPesStreamId(getStreamId())) {
        return false;
    }
    else {
        const quint8* pl = payload();
        const int size = payloadSize();
        return size >= 3 && pl[0] == 0x00 && pl[1] == 0x00 && pl[2] == 0x01;
    }
}


//-----------------------------------------------------------------------------
// Check if the PES packet contains AVC.
//-----------------------------------------------------------------------------

bool QtsPesPacket::isAvc() const
{
    // Must have a video stream_id and payload must start with 00 00 00 [00...] 01
    if (!qtsIsVideoPesStreamId(getStreamId())) {
        return false;
    }
    else {
        const quint8* pl = payload();
        int size = payloadSize();
        while(size > 0 && *pl == 0x00) {
            ++pl;
            --size;
        }
        return size > 0 && *pl == 0x01 && pl > payload() + 2;
    }
}


//-----------------------------------------------------------------------------
// Check if the PES packet contains AC-3 or Enhanced-AC-3.
//-----------------------------------------------------------------------------

bool QtsPesPacket::isAc3() const
{
    // Payload must start with 0B 77
    const quint8* pl = payload();
    const int size = payloadSize();
    return size > 2 && pl[0] == 0x0B && pl[1] == 0x77;
}
