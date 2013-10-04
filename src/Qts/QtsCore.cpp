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
// Core definitions for Qts.
//
//----------------------------------------------------------------------------

#include "QtsCore.h"


//-----------------------------------------------------------------------------
// These PID sets respectively contains no PID and all PID's.
// The default constructor for PIDSet (std::bitset) sets all bits to 0.
//-----------------------------------------------------------------------------

const QtsPidSet QtsNoPid;
const QtsPidSet QtsAllPids(~QtsNoPid);


//-----------------------------------------------------------------------------
// This routine extracts a PCR from a stream.
// Use 6 bytes at address b. Return a 42-bit value.
//-----------------------------------------------------------------------------

quint64 qtsGetPcr(const void* b)
{
    const quint32 v32 = qFromBigEndian<quint32>((const uchar*)(b));
    const quint16 v16 = qFromBigEndian<quint16>((const uchar*)(b) + 4);
    const quint64 pcrBase = (quint64(v32) << 1) | quint64(v16 >> 15);
    const quint64 pcrExt = quint64(v16 & 0x01FF);
    return pcrBase * 300 + pcrExt;
}


//-----------------------------------------------------------------------------
// This routine inserts a PCR in a stream.
// Writes 6 bytes at address b.
//-----------------------------------------------------------------------------

void qtsPutPcr(void* b, const quint64& pcr)
{
    const quint64 pcrBase = pcr / 300;
    const quint64 pcrExt = pcr % 300;
    qToBigEndian(quint32(pcrBase >> 1), (uchar*)(b));
    qToBigEndian(quint16((pcrBase << 15) | 0x7E00 | pcrExt), (uchar*)(b) + 4);
}


//-----------------------------------------------------------------------------
// Check if a SID value indicates a PES packet with long header
//-----------------------------------------------------------------------------

bool qtsIsLongHeaderPesStreamId(QtsPesStreamId sid)
{
    return
        sid != QTS_SID_PSMAP &&    // Program stream map
        sid != QTS_SID_PAD &&      // Padding stream
        sid != QTS_SID_PRIV2 &&    // Private stream 2
        sid != QTS_SID_ECM &&      // ECM stream
        sid != QTS_SID_EMM &&      // EMM stream
        sid != QTS_SID_PSDIR &&    // Program stream directory
        sid != QTS_SID_DSMCC &&    // DSM-CC data
        sid != QTS_SID_H222_1_E;   // H.222.1 type E
}


//-----------------------------------------------------------------------------
// Check if an ST value indicates a PES stream
//-----------------------------------------------------------------------------

bool qtsIsPesStreamType(QtsStreamType st)
{
    return
        st == QTS_ST_MPEG1_VIDEO ||
        st == QTS_ST_MPEG2_VIDEO ||
        st == QTS_ST_MPEG1_AUDIO ||
        st == QTS_ST_MPEG2_AUDIO ||
        st == QTS_ST_PES_PRIV    ||
        st == QTS_ST_MPEG2_ATM   ||
        st == QTS_ST_MPEG4_VIDEO ||
        st == QTS_ST_MPEG4_AUDIO ||
        st == QTS_ST_MPEG4_PES   ||
        st == QTS_ST_MDATA_PES   ||
        st == QTS_ST_AVC_VIDEO   ||
        st == QTS_ST_AAC_AUDIO   ||
        st == QTS_ST_AC3_AUDIO   ||
        st == QTS_ST_EAC3_AUDIO;
}


//-----------------------------------------------------------------------------
// Check if an ST value indicates a video stream
//-----------------------------------------------------------------------------

bool qtsIsVideoStreamType(QtsStreamType st)
{
    return
        st == QTS_ST_MPEG1_VIDEO ||
        st == QTS_ST_MPEG2_VIDEO ||
        st == QTS_ST_MPEG4_VIDEO ||
        st == QTS_ST_AVC_VIDEO;
}


//-----------------------------------------------------------------------------
// Check if an ST value indicates an audio stream
//-----------------------------------------------------------------------------

bool qtsIsAudioStreamType(QtsStreamType st)
{
    return
        st == QTS_ST_MPEG1_AUDIO ||
        st == QTS_ST_MPEG2_AUDIO ||
        st == QTS_ST_MPEG4_AUDIO ||
        st == QTS_ST_AAC_AUDIO   ||
        st == QTS_ST_AC3_AUDIO   ||
        st == QTS_ST_EAC3_AUDIO;
}


//-----------------------------------------------------------------------------
// Check if an ST value indicates a stream carrying sections
//-----------------------------------------------------------------------------

bool qtsIsSectionStreamType(QtsStreamType st)
{
    return
        st == QTS_ST_PRIV_SECT ||
        st == QTS_ST_DSMCC_UN ||
        st == QTS_ST_DSMCC_SECT ||
        st == QTS_ST_MPEG4_SECT ||
        st == QTS_ST_MDATA_SECT;
}
