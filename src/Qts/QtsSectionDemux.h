//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2016, Thierry Lelegard
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
//! @file QtsSectionDemux.h
//!
//! Declare the class QtsSectionDemux.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSSECTIONDEMUX_H
#define QTSSECTIONDEMUX_H

#include "QtsCore.h"
#include "QtsDemux.h"
#include "QtsTable.h"
#include "QtsSection.h"
#include "QtsExtTableId.h"
#include "QtsSectionHandlerInterface.h"
#include "QtsTableHandlerInterface.h"

//!
//! This class extracts PSI/SI sections and tables from TS packets.
//!
//! General notes:
//! - Long sections are validated against their CRC32.
//!   Corrupted sections are not reported.
//! - Sections with the 'next' indicator are ignored.
//!   Only sections with the 'current' indicator are reported.
//! - Tables with long sections are reported only when a new
//!   version is available.
//!
class QtsSectionDemux : public QtsDemux
{
public:
    //!
    //! Constructor.
    //! @param [in] tableHandler Handler to invoke for each new table. Default: none.
    //! @param [in] sectionHandler Handler to invoke for each section. Default: none.
    //! @param [in] pidFilter Set of PID's to filter. Default: none.
    //!
    QtsSectionDemux(QtsTableHandlerInterface* tableHandler = 0,
                    QtsSectionHandlerInterface* sectionHandler = 0,
                    const QtsPidSet& pidFilter = QtsNoPid);

    //!
    //! Set the table handler.
    //! @param [in] handler The table handler.
    //!
    void setTableHandler(QtsTableHandlerInterface* handler)
    {
        _tableHandler = handler;
    }

    //!
    //! Set the section handler.
    //! @param [in] handler The section handler.
    //!
    void setSectionHandler(QtsSectionHandlerInterface* handler)
    {
        _sectionHandler = handler;
    }

    //!
    //! Reset the analysis context (partially built sections and tables).
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
    //! Demux status information
    //!
    struct Status
    {
        quint64 invalidTs;       //!< Number of invalid TS packets.
        quint64 discontinuities; //!< Number of TS packets discontinuities.
        quint64 scrambled;       //!< Number of scrambled TS packets (undecoded).
        quint64 invSectLength;   //!< Number of invalid section length.
        quint64 invSectIndex;    //!< Number of invalid section index.
        quint64 wrongCrc;        //!< Number of sections with wrong CRC32.
        //!
        //! Default constructor
        //!
        Status();
        //!
        //! Constructor from the current status of a QtsSectionDemux.
        //! @param [in] demux The demux to query.
        //!
        Status(const QtsSectionDemux& demux);
        //!
        //! Reset content.
        //!
        void reset();
        //!
        //! Check if any counter is non zero.
        //! @return True if any counter is non zero.
        //!
        bool hasErrors() const;
    };

    //!
    //! Get the current status of the demux.
    //! @param [out] status Receives the current status of the demux.
    //!
    void getStatus(Status& status) const
    {
        status = _status;
    }

    //!
    //! Check if the demux has errors.
    //! @return True if any error was found.
    //!
    bool hasErrors() const
    {
        return _status.hasErrors();
    }

private:
    //!
    //! This internal structure contains the analysis context for one TID/TIDext into one PID.
    //!
    struct EtidContext
    {
        quint8 version;            //!< Version of this table.
        int    sectExpected;       //!< Number of expected sections in table.
        int    sectReceived;       //!< Number of received sections in table.
        QtsSectionPtrVector sects; //!< Array of sections.
        //!
        //! Default constructor.
        //!
        EtidContext();
    };

    //!
    //! Map of EtidContext index by ETID.
    //!
    typedef QMap<QtsExtTableId,EtidContext> EtidContextMap;

    //!
    //! This internal structure contains the analysis context for one PID.
    //!
    struct PidContext
    {
        quint8           continuity;    //!< Last continuity counter.
        bool             sync;          //!< We are synchronous in this PID.
        QtlByteBlock     ts;            //!< TS payload buffer.
        EtidContextMap   tids;          //!< TID analysis contexts.
        bool             resetPending;  //!< Delayed reset on this PID.
        QtsPacketCounter pusiPktIndex;  //!< Index of last PUSI packet in this PID.
        //!
        //! Default constructor.
        //!
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
    //! Map of PidContext index by PID.
    //!
    typedef QMap<QtsPid,PidContext> PidContextMap;

    //!
    //! Feed the demux with a TS packet (PID already filtered).
    //! @param [in] packet The TS packet to process.
    //!
    void processTsPacket(const QtsTsPacket& packet);

    // Private members:
    QtsTableHandlerInterface*   _tableHandler;   //!< Handler to invoke for each new table.
    QtsSectionHandlerInterface* _sectionHandler; //!< Handler to invoke for each section.
    PidContextMap               _pids;           //!< State of the PID's analysis.
    Status                      _status;         //!< State of the demux.
    bool                        _inHandler;      //!< True when in the context of a table/section handler.
    QtsPid                      _pidInHandler;   //!< PID which is currently processed by handler.
    bool                        _resetPending;   //!< Delayed reset().

    // Unaccessible operations.
    Q_DISABLE_COPY(QtsSectionDemux)
};

#endif // QTSSECTIONDEMUX_H
