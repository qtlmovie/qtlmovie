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
//!
//! @file QtsPesDemux.h
//!
//! Declare the class QtsPesDemux.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSPESDEMUX_H
#define QTSPESDEMUX_H

#include "QtsDemux.h"
#include "QtsPesPacket.h"
#include "QtsPesHandlerInterface.h"

//!
//! This class extracts PES packets from TS packets.
//!
class QtsPesDemux : public QtsDemux
{
public:
    //!
    //! Constructor
    //! @param [in] handler User handler for PES packet notification.
    //! @param [in] pidFilter Set of PID's to filter.
    //!
    explicit QtsPesDemux(QtsPesHandlerInterface* handler = 0, const QtsPidSet& pidFilter = QtsAllPids);

    //!
    //! Set the PES packet handler.
    //! @param [in] handler The PES packet handler.
    //!
    void setPesHandler(QtsPesHandlerInterface* handler)
    {
        _pesHandler = handler;
    }

    //!
    //! Reset the analysis context (partially built PES packets).
    //! Useful when the transport stream changes.
    //! The PID filter and the handlers are not modified.
    //!
    virtual void reset() Q_DECL_OVERRIDE;

    //!
    //! Reset the analysis context for one single PID.
    //! @param [in] pid PID to reset.
    //!
    virtual void resetPid(QtsPid pid) Q_DECL_OVERRIDE;

protected:
    //!
    //! This hook is invoked when a complete PES packet is available.
    //! Can be overloaded by subclasses to add intermediate processing.
    //! @param [in] packet The PES packet.
    //!
    virtual void handlePesPacket(const QtsPesPacket& packet);

private:
    //!
    //! This internal structure contains the analysis context for one PID.
    //!
    struct PidContext
    {
        QtsPacketCounter pesCount;     //!< Number of detected valid PES packets on this PID.
        quint8           continuity;   //!< Last continuity counter in the PID.
        bool             sync;         //!< We are synchronous in this PID.
        QtsPacketCounter firstPkt;     //!< Index of first TS packet for current PES packet.
        QtsPacketCounter lastPkt;      //!< Index of last TS packet for current PES packet.
        QtlByteBlock     ts;           //!< TS payload buffer
        bool             resetPending; //!< Delayed reset on this PID
        //!
        //! Default constructor:
        PidContext();
        //!
        //! Called when packet synchronization is lost on the PID.
        //!
        void syncLost()
        {
            sync = false;
            ts.clear();
        }
    };

    //!
    //! Map of PID analysis contexts, indexed by PID value.
    //!
    typedef QMap<QtsPid,PidContext> PidContextMap;

    //!
    //! Feed the demux with a TS packet (PID already filtered).
    //! @param [in] packet The TS packet to process.
    //!
    virtual void processTsPacket(const QtsTsPacket& packet) Q_DECL_OVERRIDE;

    //!
    //! Process a complete PES packet
    //! @param [in] pid The source PID.
    //! @param [in,out] pc The PID analysis context.
    //!
    void processPesPacket(QtsPid pid, PidContext& pc);

    // Private members:
    QtsPesHandlerInterface* _pesHandler;    //!< User handler.
    PidContextMap           _pids;          //!< Map of PID analysis contexts.
    bool                    _inHandler;     //!< True when in the context of a handler
    QtsPid                  _pidInHandler;  //!< PID which is currently processed by handler
    bool                    _resetPending;  //!< Delayed reset().

    // Unaccessible operations.
    QtsPesDemux() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtsPesDemux)
};

#endif // QTSPESDEMUX_H
