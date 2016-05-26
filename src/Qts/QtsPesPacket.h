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
//!
//! @file QtsPesPacket.h
//!
//! Declare the class QtsPesPacket.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSPESPACKET_H
#define QTSPESPACKET_H

#include "QtsCore.h"
#include "QtlByteBlock.h"

//!
//! Basic definition of an MPEG-2 PES packet(Packetized Elementary Stream).
//!
class QtsPesPacket
{
public:
    //!
    //! Default constructor.
    //! The PES packet is initially marked invalid.
    //!
    QtsPesPacket()
    {
        initialize(QTS_PID_NULL);
    }

    //!
    //! Constructor from full binary content.
    //! The content is copied into the packet only if valid.
    //! @param [in] addr Address of binary content.
    //! @param [in] size Size of binary content.
    //! @param [in] pid PID from which the PES packet was extracted (informational only).
    //!
    QtsPesPacket(const void* addr, int size, QtsPid pid = QTS_PID_NULL)
    {
        initialize(QtlByteBlock(addr, size), pid);
    }

    //!
    //! Constructor from full binary content.
    //! The content is copied into the packet only if valid.
    //! @param [in] content Binary content.
    //! @param [in] pid PID from which the PES packet was extracted (informational only).
    //!
    QtsPesPacket(const QtlByteBlock& content, QtsPid pid = QTS_PID_NULL)
    {
        initialize(content, pid);
    }

    //!
    //! Reload full binary content.
    //! The content is copied into the packet only if valid.
    //! @param [in] addr Address of binary content.
    //! @param [in] size Size of binary content.
    //! @param [in] pid PID from which the PES packet was extracted (informational only).
    //!
    void reload(const void* addr, int size, QtsPid pid = QTS_PID_NULL)
    {
        initialize(QtlByteBlock(addr, size), pid);
    }

    //!
    //! Reload full binary content.
    //! The content is copied into the packet only if valid.
    //! @param [in] content Binary content.
    //! @param [in] pid PID from which the PES packet was extracted (informational only).
    //!
    void reload(const QtlByteBlock& content, QtsPid pid = QTS_PID_NULL)
    {
        initialize(content, pid);
    }

    //!
    //! Clear packet content, becomes an invalid packet.
    //!
    void clear();

    //!
    //! Check if a packet has valid content.
    //! @return True if the packet content is valid, false otherwise.
    //!
    bool isValid() const
    {
        return _isValid;
    }

    //!
    //! Comparison operator.
    //! The source PID are ignored, only the packet contents are compared.
    //! Note: Invalid packets are never identical
    //! @param [in] other Other instance to compare.
    //! @return True if packets are identical, false otherwise.
    //!
    bool operator==(const QtsPesPacket& other) const;

    //!
    //! Difference operator.
    //! The source PID are ignored, only the packet contents are compared.
    //! Note: Invalid packets are never identical
    //! @param [in] other Other instance to compare.
    //! @return True if packets are identical, false otherwise.
    //!
    bool operator!=(const QtsPesPacket& other) const
    {
        return !(*this == other);
    }

    //!
    //! Get the PID from which the packet was collected (informational only).
    //! @return The PID from which the packet was collected.
    //!
    QtsPid getSourcePid() const
    {
        return _sourcePid;
    }

    //!
    //! Set the PID from which the packet was collected (informational only).
    //! @param [in] pid The PID from which the packet was collected.
    //!
    void setSourcePid(QtsPid pid)
    {
        _sourcePid = pid;
    }

    //!
    //! Get the index of first TS packet of the PES packet in the demultiplexed stream.
    //! @return The index of first TS packet of the PES packet.
    //!
    QtsPacketCounter getFirstTsPacketIndex() const
    {
        return _firstPkt;
    }

    //!
    //! Get the index of last TS packet of the PES packet in the demultiplexed stream.
    //! @return The index of last TS packet of the PES packet.
    //!
    QtsPacketCounter getLastTsPacketIndex() const
    {
        return _lastPkt;
    }

    //!
    //! Set the index of first TS packet of the PES packet in the demultiplexed stream.
    //! @param [in] i The index of first TS packet of the PES packet.
    //!
    void setFirstTsPacketIndex(QtsPacketCounter i)
    {
        _firstPkt = i;
    }

    //!
    //! Set the index of last TS packet of the PES packet in the demultiplexed stream.
    //! @param [in] i The index of last TS packet of the PES packet.
    //!
    void setLastTsPacketIndex(QtsPacketCounter i)
    {
        _lastPkt = i;
    }

    //!
    //! Get the stream identifier of the PES packet.
    //! @return The stream identifier of the PES packet.
    //!
    QtsPesStreamId getStreamId() const
    {
        return QtsPesStreamId(_isValid ? _data[3] : 0);
    }

    //!
    //! Set the stream identifier of the PES packet.
    //! @param [in] sid The stream identifier of the PES packet.
    //!
    void setStreamId(QtsPesStreamId sid)
    {
        if (_isValid) {
            _data[3] = quint8(sid);
        }
    }

    //!
    //! Check if the packet has a long header.
    //! @return True if the packet has a long header.
    //!
    bool hasLongHeader() const
    {
        return _isValid && qtsIsLongHeaderPesStreamId(QtsPesStreamId(_data[3]));
    }

    //!
    //! Get a copy of the full binary content of the packet.
    //! @return The full binary content of the packet.
    //!
    QtlByteBlock content() const
    {
        return _data;
    }

    //!
    //! Get the size in bytes of the packet.
    //! @return The size in bytes of the packet.
    //!
    int size() const
    {
        return _data.size();
    }

    //!
    //! Get the address of the PES header in the packet.
    //! Do not modify the content.
    //! @return Address of the PES header in the packet.
    //! May be invalidated when the packet is reloaded.
    //! Zero if the packet is invalid.
    //!
    const quint8* header() const
    {
        return _isValid ? _data.data() : 0;
    }

    //!
    //! Get the size of the PES header in the packet.
    //! @return Size of the PES header in the packet.
    //!
    int headerSize() const
    {
        return _isValid ? _headerSize : 0;
    }

    //!
    //! Get the address of the PES payload in the packet.
    //! Do not modify the content.
    //! @return Address of the PES payload in the packet.
    //! May be invalidated when the packet is reloaded.
    //! Zero if the packet is invalid.
    //!
    const quint8* payload() const
    {
        return _isValid ? _data.data() + _headerSize : 0;
    }

    //!
    //! Get the size of the PES payload in the packet.
    //! @return Size of the PES payload in the packet.
    //!
    int payloadSize() const
    {
        return _isValid ? _data.size() - _headerSize : 0;
    }

    //!
    //! Check if the PES packet contains MPEG-2 video (also applies to MPEG-1 video).
    //! @return True if the PES packet contains MPEG-2 video.
    //!
    bool isMpeg2Video() const;

    //!
    //! Check if the PES packet contains AVC / H.264.
    //! @return True if the PES packet contains AVC video.
    //!
    bool isAvc() const;

    //!
    //! Check if the PES packet contains AC-3 or Enhanced-AC-3 audio.
    //!
    //! Warning: As specified in ETSI TS 102 366, an AC-3 audio frame always
    //! starts with 0x0B77. This is what we check here. However, it is still
    //! possible that other encodings may start from time to time with 0x0B77.
    //! Thus, it is safe to say that a PID in which all PES packets start with
    //! 0x0B77(ie isAc3() returns true) contains AC-3. However, if only
    //! a few PES packets start with 0x0B77, it is safe to say that it should be
    //! something else.
    //!
    //! @return True if the PES packet contains AC-3 or Enhanced-AC-3 audio.
    //!
    bool isAc3() const;

    //!
    //! Check if the PES packet contains a PTS (Presentation Time Stamp).
    //! @return True if the PES packet contains a PTS.
    //!
    bool hasPts() const;

    //!
    //! Get the PTS (Presentation Time Stamp) - 33 bits.
    //! @return PTS value or zero if not found.
    //!
    quint64 getPts() const;

    //!
    //! Replace the PTS (Presentation Time Stamp) value - 33 bits.
    //! Do nothing if the packet has not PTS.
    //! @param pts PTS value to replace.
    //!
    void setPts(const quint64& pts);

    //!
    //! Check if the PES packet contains a DTS (Decoding Time Stamp).
    //! @return True if the PES packet contains a DTS.
    //!
    bool hasDts() const;

    //!
    //! Get the DTS (Decoding Time Stamp) - 33 bits.
    //! @return DTS value or zero if not found.
    //!
    quint64 getDts() const;

    //!
    //! Replace the DTS (Decoding Time Stamp) value - 33 bits.
    //! Do nothing if the packet has not DTS.
    //! @param dts DTS value to replace.
    //!
    void setDts(const quint64& dts);

private:
    // Private fields
    bool             _isValid;     //!< Content of _data is a valid packet.
    int              _headerSize;  //!< PES header size in bytes.
    QtsPid           _sourcePid;   //!< Source PID (informational).
    QtsPacketCounter _firstPkt;    //!< Index of first packet in stream.
    QtsPacketCounter _lastPkt;     //!< Index of last packet in stream.
    QtlByteBlock     _data;        //!< Full binary content of the packet.

    //!
    //! Helper for constructor.
    //! @param [in] pid Source PID.
    //!
    void initialize(QtsPid pid);

    //!
    //! Helper for constructor.
    //! @param [in] bb PES packet binary content.
    //! @param [in] pid Source PID.
    //!
    void initialize(const QtlByteBlock& bb, QtsPid pid);
};

#endif // QTSPESPACKET_H
