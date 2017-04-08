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
//! @file QtsDemux.h
//!
//! Declare the class QtsDemux.
//!
//----------------------------------------------------------------------------

#ifndef QTSDEMUX_H
#define QTSDEMUX_H

#include "QtsCore.h"
#include "QtsTsPacket.h"

//!
//! Abstract base class for transport stream demultiplexers.
//!
class QtsDemux
{
public:
    //!
    //! Constructor
    //! @param [in] pidFilter Set of PID's to filter.
    //!
    explicit QtsDemux(const QtsPidSet& pidFilter = QtsAllPids);

    //!
    //! Virtual destructor.
    //!
    virtual ~QtsDemux();

    //!
    //! Feed the demux with a TS packet.
    //! @param [in] packet TS packet from the stream to demultiplex.
    //! If the PID of the packet is not in the PID filter, it is ignored.
    //!
    void feedPacket(const QtsTsPacket& packet);

    //!
    //! Set the list of PID's to filter.
    //! @param [in] pidFilter Set of PID's to filter.
    //!
    virtual void setPidFilter(const QtsPidSet& pidFilter);

    //!
    //! Add a PID to filter.
    //! @param [in] pid PID to filter.
    //!
    virtual void addPid(QtsPid pid);

    //!
    //! Remove a PID to filter.
    //! @param [in] pid PID to no longer filter.
    //!
    virtual void removePid(QtsPid pid);

    //!
    //! Get the number of PID's which are filtered by the demux.
    //! @return The number of PID's which are filtered by the demux.
    //!
    int filteredPidCount() const
    {
        return _pidFilter.count();
    }

    //!
    //! Reset the analysis context (partially built PES packets).
    //! Useful when the transport stream changes.
    //! The PID filter and the handlers are not modified.
    //!
    virtual void reset();

    //!
    //! Reset the analysis context for one single PID.
    //! @param [in] pid PID to reset.
    //!
    virtual void resetPid(QtsPid pid);

    //!
    //! Get the last PCR found in the stream.
    //! @return Last PCR value or negative if none was found.
    //!
    qint64 lastPcr() const
    {
        return _lastPcr;
    }

    //!
    //! Get the number of process TS packets.
    //! @return The number of process TS packets.
    //!
    QtsPacketCounter packetCount() const
    {
        return _packetCount;
    }

    //!
    //! Reset the number of process TS packets.
    //!
    void resetPacketCount()
    {
        _packetCount = 0;
    }

private:
    QtsPacketCounter _packetCount; //!< Number of TS packets in demultiplexed stream.
    qint64           _lastPcr;     //!< Last PCR is any TS packet, any PID.
    QtsPidSet        _pidFilter;   //!< PIDs to filter.

    //!
    //! Feed the demux with a TS packet (PID already filtered).
    //! Must be implemented by subclasses.
    //! @param [in] packet The TS packet to process.
    //!
    virtual void processTsPacket(const QtsTsPacket& packet) = 0;

    // Unaccessible operations.
    QtsDemux() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtsDemux)
};

#endif // QTSDEMUX_H
