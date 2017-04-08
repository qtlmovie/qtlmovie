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
//!
//! @file QtsTsPacket.h
//!
//! Declare the class QtsTsPacket.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSTSPACKET_H
#define QTSTSPACKET_H

#include "QtsCore.h"

//!
//! Basic definition of an MPEG-2 transport packet.
//! The binary representation of this object is exactly 188 bytes.
//! Arrays of QtsTsPacket can be mapped on transport stream data buffers.
//!
struct QtsTsPacket
{
    //! Public content is a byte array.
    //! This byte array exactly fills the memory area of a transport packet.
    //! This is why arrays of QtsTsPacket can be mapped on transport stream data buffers.
    quint8 b[QTS_PKT_SIZE];

    //!
    //! Assigment operator.
    //! @param p Other instance to assign.
    //! @return A reference to this object.
    //!
    inline const QtsTsPacket& operator=(const QtsTsPacket& p)
    {
        ::memcpy(b, p.b, QTS_PKT_SIZE);
        return *this;
    }

    //!
    //! Comparison operator.
    //! @param p Other instance to compare with.
    //! @return True if both packets have same content, false otherwise.
    //!
    inline bool operator==(const QtsTsPacket& p) const
    {
        return ::memcmp(b, p.b, QTS_PKT_SIZE) == 0;
    }

    //!
    //! Difference operator.
    //! @param p Other instance to compare with.
    //! @return True if packets have distinct contents, false otherwise.
    //!
    inline bool operator!=(const QtsTsPacket& p) const
    {
        return ::memcmp(b, p.b, QTS_PKT_SIZE) != 0;
    }

    //!
    //! Check if the sync byte is valid in the packet.
    //! @return True if the sync byte is valid.
    //!
    inline bool hasValidSync() const
    {
        return b[0] == QTS_SYNC_BYTE;
    }

    //!
    //! Extract the PID - 13 bits.
    //! @return The PID of the packet.
    //!
    inline QtsPid getPid() const
    {
        return qFromBigEndian<quint16>((const uchar*)(b) + 1) & 0x1FFF;
    }

    //!
    //! Set the PID - 13 bits.
    //! @param [in] pid The PID of the packet.
    //!
    inline void setPid(QtsPid pid)
    {
        b[1] = (b[1] & 0xE0) | ((pid >> 8) & 0x1F);
        b[2] = pid & 0x00FF;
    }

    //!
    //! Extract the payload_unit_start_indicator (PUSI) - 1 bit.
    //! @return True if the PUSI is set, false otherwise.
    //!
    inline bool getPusi() const
    {
        return (b[1] & 0x40) != 0;
    }

    //!
    //! Clear the payload_unit_start_indicator (PUSI) - 1 bit.
    //!
    inline void clearPusi()
    {
        b[1] &= ~0x40;
    }

    //!
    //! Set the payload_unit_start_indicator (PUSI) - 1 bit.
    //!
    inline void setPusi()
    {
        b[1] |= 0x40;
    }

    //!
    //! Extract the transport_error_indicator (TEI) - 1 bit.
    //! @return True if the TEI is set, false otherwise.
    //!
    inline bool getTei() const
    {
        return (b[1] & 0x80) != 0;
    }

    //!
    //! Clear the transport_error_indicator (TEI) - 1 bit.
    //!
    inline void clearTei()
    {
        b[1] &= ~0x80;
    }

    //!
    //! Set the transport_error_indicator (TEI) - 1 bit.
    //!
    inline void setTei()
    {
        b[1] |= 0x80;
    }

    //!
    //! Extract the transport_priority - 1 bit.
    //! @return True if the transport_priority is set, false otherwise.
    //!
    inline bool getPriority() const
    {
        return (b[1] & 0x20) != 0;
    }

    //!
    //! Clear the transport_priority - 1 bit.
    //!
    inline void clearPriority()
    {
        b[1] &= ~0x20;
    }

    //!
    //! Set the transport_priority - 1 bit.
    //!
    inline void setPriority()
    {
        b[1] |= 0x20;
    }

    //!
    //! Extract the transport_scrambling_control - 2 bits.
    //! @return The transport_scrambling_control value.
    //!
    inline quint8 getScrambling() const
    {
        return b[3] >> 6;
    }

    //!
    //! Check if the packet is clear.
    //! @return True if the packet is clear, false if it is scrambled.
    //!
    inline bool isClear() const
    {
        return (b[3] >> 6) == 0;
    }

    //!
    //! Check if the packet is scrambled.
    //! @return True if the packet is scrambled, false if it is clear.
    //!
    inline bool isScrambled() const
    {
        return (b[3] >> 6) != 0;
    }

    //!
    //! Set the transport_scrambling_control - 2 bits.
    //! @param [in] sc The transport_scrambling_control value.
    //!
    inline void setScrambling(quint8 sc)
    {
        b[3] = (b[3] & 0x3F) | (sc << 6);
    }

    //!
    //! Extract the continuity_counter (CC) - 4 bits.
    //! @return The continuity_counter value.
    //!
    inline quint8 getCc() const
    {
        return b[3] & 0x0F;
    }

    //!
    //! Set the continuity_counter (CC) - 4 bits.
    //! @param [in] cc The continuity_counter value.
    //!
    inline void setCc(quint8 cc)
    {
        b[3] = (b[3] & 0xF0) | (cc & 0x0F);
    }

    //!
    //! Check if the packet has an adaptation_field (AF).
    //! @return True if the packet has an adaptation_field, false otherwise.
    //!
    inline bool hasAdaptationField() const
    {
        return (b[3] & 0x20) != 0;
    }

    //!
    //! Compute the adaptation_field (AF) size in bytes.
    //! @return Adaptation field size in bytes (zero if there is no adaptation field).
    //!
    inline int getAdaptationFieldSize() const
    {
        return hasAdaptationField() ? int(b[4]) : 0;
    }

    //!
    //! Compute the size of the TS header (ie. index to TS payload).
    //! @return The size of the TS header (ie. index to TS payload).
    //!
    inline int getHeaderSize() const
    {
        return qMin(4 + (hasAdaptationField() ? (int(b[4]) + 1) : 0), QTS_PKT_SIZE);
    }

    //!
    //! Check if the packet has a payload.
    //! @return True if the packet has a payload, false otherwise.
    //!
    inline bool hasPayload() const
    {
        return (b[3] & 0x10) != 0;
    }

    //!
    //! Get payload start address inside this object.
    //! @return The payload start address inside this object.
    //!
    inline const quint8* getPayload() const
    {
        return b + getHeaderSize();
    }

    //!
    //! Get payload start address inside this object.
    //! @return The payload start address inside this object.
    //!
    inline quint8* getPayload()
    {
        return b + getHeaderSize();
    }

    //!
    //! Compute the payload size.
    //! @return The payload size in bytes.
    //!
    inline int getPayloadSize() const
    {
        return hasPayload() ? QTS_PKT_SIZE - getHeaderSize() : 0;
    }

    //!
    //! Check if packet has a discontinuity_indicator set - 1 bit.
    //! @return True if the packet has a discontinuity_indicator set, false otherwise.
    //!
    inline bool getDiscontinuityIndicator() const
    {
        return getAdaptationFieldSize() > 0 ? ((b[5] & 0x80) != 0) : false;
    }

    //!
    //! Check if packet has a random_access_indicator set - 1 bit.
    //! @return True if the packet has a random_access_indicator set, false otherwise.
    //!
    inline bool getRandomAccessIndicator() const
    {
        return getAdaptationFieldSize() > 0 ? ((b[5] & 0x40) != 0) : false;
    }

    //!
    //! Check if packet has a elementary_stream_priority_indicator (ESPI) set - 1 bit.
    //! @return True if the packet has a elementary_stream_priority_indicator (ESPI) set, false otherwise.
    //!
    inline bool getEspi() const
    {
        return getAdaptationFieldSize() > 0 ? ((b[5] & 0x20) != 0) : false;
    }

    //!
    //! Check if packet has a Program Clock Reference (PCR).
    //! @return True if the packet has a Program Clock Reference (PCR), false otherwise.
    //!
    inline bool hasPcr() const
    {
        return getAdaptationFieldSize() > 0 && (b[5] & 0x10) != 0;
    }

    //!
    //! Check if packet has an Original Program Clock Reference (OPCR).
    //! @return True if the packet has an Original Program Clock Reference (OPCR), false otherwise.
    //!
    inline bool hasOpcr() const
    {
        return getAdaptationFieldSize() > 0 && (b[5] & 0x08) != 0;
    }

    //!
    //! Get the Program Clock Reference (PCR) - 42 bits.
    //! @return PCR value or zero if not found.
    //!
    quint64 getPcr() const;

    //!
    //! Get the Original Program Clock Reference (OPCR) - 42 bits.
    //! @return OPCR value or zero if not found.
    //!
    quint64 getOpcr() const;

    //!
    //! Replace the Program Clock Reference (PCR) value - 42 bits.
    //! Do nothing if the packet has not PCR.
    //! @param pcr PCR value to replace.
    //!
    void setPcr(const quint64& pcr);

    //!
    //! Replace the Original Program Clock Reference (OPCR) value - 42 bits.
    //! Do nothing if the packet has not OPCR.
    //! @param opcr OPCR value to replace.
    //!
    void setOpcr(const quint64& opcr);

    //!
    //! Check if the packet contains the start of a clear PES header.
    //! @return True if the packet contains the start of a clear PES header, false otherwise.
    //!
    bool startPes() const;

    //!
    //! Check if the TS packet contains a Presentation Time Stamp (PTS).
    //! Technically, the PTS and DTS are part of the PES packet, not the TS packet.
    //! If the TS packet is the first TS packet of a PES packet, it is possible
    //! that the PTS and/or DTS are present in the PES packet but outside the
    //! first TS packet. This is possible but rare. So, we provide here a fast
    //! way of getting PTS and/or DTS from the TS packet if available.
    //! @return True if the TS packet contains a Presentation Time Stamp (PTS).
    //!
    inline bool hasPts() const
    {
        return ptsOffset() > 0;
    }

    //!
    //! Check if the TS packet contains a Decoding Time Stamp (DTS).
    //! @return True if the TS packet contains a Decoding Time Stamp (DTS).
    //! @see hasPts()
    //!
    inline bool hasDTS() const
    {
        return dtsOffset() > 0;
    }

    //!
    //! Get the PTS - 33 bits
    //! @return The PTS value or zero if there is none.
    //! @see hasPts()
    //!
    quint64 getPts() const
    {
        return getPdts(ptsOffset());
    }

    //!
    //! Get the DTS - 33 bits
    //! @return The DTS value or zero if there is none.
    //! @see hasPts()
    //! @see hasDts()
    //!
    quint64 getDts() const
    {
        return getPdts(dtsOffset());
    }

    //!
    //! Sanity check routine.
    //! Ensure that the QtsTsPacket structure can be used in contiguous memory
    //! array and array of packets. Crash the application if this is not the case.
    //! Can be used once at startup time in paranoid applications.
    //!
    static void sanityCheck();

private:
    //!
    //! Compute the offset of PCR.
    //! @return PCR offset in packet or zero if there is none.
    //!
    int pcrOffset() const;

    //!
    //! Compute the offset of OPCR.
    //! @return OPCR offset in packet or zero if there is none.
    //!
    int opcrOffset() const;

    //!
    //! Compute the offset of PTS.
    //! @return PTS offset in packet or zero if there is none.
    //!
    int ptsOffset() const;

    //!
    //! Compute the offset of DTS.
    //! @return DTS offset in packet or zero if there is none.
    //!
    int dtsOffset() const;

    //!
    //! Get the PTS or DTS at specified offset.
    //! @param [in] offset Offset in packet.
    //! @return A PTS/DTS value or zero if offset is zero.
    //!
    quint64 getPdts(int offset) const;
};

//!
//! This constant is a null (or stuffing) packet.
//!
extern const QtsTsPacket QtsNullPacket;

//!
//! This constant is an empty packet (no payload).
//! PID and CC shall be updated for use in specific PID's.
//!
extern const QtsTsPacket QtsEmptyPacket;

//!
//! Vector of transport stream packets.
//!
typedef QVector<QtsTsPacket> QtsTsPacketVector;

#endif // QTSTSPACKET_H
