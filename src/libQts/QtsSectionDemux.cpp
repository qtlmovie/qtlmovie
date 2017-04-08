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
// Define the class QtsSectionDemux.
//
//----------------------------------------------------------------------------

#include "QtsSectionDemux.h"


//-----------------------------------------------------------------------------
// ETID analysis context - Default constructor
//-----------------------------------------------------------------------------

QtsSectionDemux::EtidContext::EtidContext() :
    version(0),
    sectExpected(0),
    sectReceived(0),
    sects()
{
}


//-----------------------------------------------------------------------------
// PID analysis context - Default constructor
//-----------------------------------------------------------------------------

QtsSectionDemux::PidContext::PidContext() :
    continuity(0),
    sync(false),
    ts(),
    tids(),
    resetPending(false),
    pusiPktIndex(0)
{
}


//-----------------------------------------------------------------------------
// Demux status information - Default constructor
//-----------------------------------------------------------------------------

QtsSectionDemux::Status::Status()
{
    reset();
}


//-----------------------------------------------------------------------------
// Demux status information
// Constructor from the current status of QtsSectionDemux
//-----------------------------------------------------------------------------

QtsSectionDemux::Status::Status (const QtsSectionDemux& demux)
{
    demux.getStatus(*this);
}


//-----------------------------------------------------------------------------
// Demux status information - Reset content
//-----------------------------------------------------------------------------

void QtsSectionDemux::Status::reset ()
{
    invalidTs = 0;
    discontinuities = 0;
    scrambled = 0;
    invSectLength = 0;
    invSectIndex = 0;
    wrongCrc = 0;
}


//-----------------------------------------------------------------------------
// Demux status information - Check if any counter is non zero
//-----------------------------------------------------------------------------

bool QtsSectionDemux::Status::hasErrors () const
{
    return
        invalidTs != 0 ||
        discontinuities != 0 ||
        scrambled != 0 ||
        invSectLength != 0 ||
        invSectIndex != 0 ||
        wrongCrc != 0;
}


//-----------------------------------------------------------------------------
// QtsSectionDemux constructor.
//-----------------------------------------------------------------------------

QtsSectionDemux::QtsSectionDemux(QtsTableHandlerInterface* tableHandler,
                                 QtsSectionHandlerInterface* sectionHandler,
                                 const QtsPidSet& pidFilter) :
    QtsDemux(pidFilter),
    _tableHandler(tableHandler),
    _sectionHandler(sectionHandler),
    _pids(),
    _status(),
    _inHandler(false),
    _pidInHandler(QTS_PID_NULL),
    _resetPending(false)
{
}


//-----------------------------------------------------------------------------
// Reset the analysis context (partially built sections and tables).
// Useful when the transport stream changes.
// The PID filter and the handlers are not modified.
//-----------------------------------------------------------------------------

void QtsSectionDemux::reset()
{
    if (_inHandler) {
        // In the context of a handler, delay the reset
        _resetPending = true;
    }
    else {
        // Perform the actual reset.
        _pids.clear();
        _status.reset();
        QtsDemux::reset();
    }
}


//-----------------------------------------------------------------------------
// Reset the analysis context for one single PID.
// Forget all previously analyzed sections on this PID.
//-----------------------------------------------------------------------------

void QtsSectionDemux::resetPid(QtsPid pid)
{
    if (_inHandler && _pidInHandler == pid) {
        // In the context of a handler for this PID, delay the reset
        _pids[pid].resetPending = true;
    }
    else {
        // Perform the actual reset.
        _pids.remove(pid);
        QtsDemux::resetPid(pid);
    }
}


//-----------------------------------------------------------------------------
// Private method: Feed the depacketizer with a TS packet.
// The PID has already been filtered in an inlined public method.
//-----------------------------------------------------------------------------

void QtsSectionDemux::processTsPacket(const QtsTsPacket& pkt)
{
    // Reject invalid packets
    if (!pkt.hasValidSync()) {
        _status.invalidTs++;
        return;
    }

    // Get PID and reference to the PID context.
    // The PID context is created if did not exist.
    QtsPid pid(pkt.getPid());
    PidContext& pc(_pids[pid]);

    // If TS packet is scrambled, we cannot decode it and we loose
    // synchronization on this PID (usually, PID's carrying sections
    // are not scrambled).
    if (pkt.isScrambled()) {
        _status.scrambled++;
        pc.syncLost();
        return;
    }

    // Check continuity counter on this PID (only if we have not lost
    // the synchronization on this PID).
    if (pc.sync) {
        // Ignore duplicate packets (same CC)
        if (pkt.getCc() == pc.continuity) {
            return;
        }
        // Check if we are still synchronized
        if (pkt.getCc() != (pc.continuity + 1) % QTS_CC_MAX) {
            _status.discontinuities++;
            pc.syncLost();
        }
    }

    // Keep continuity counter to check next packet.
    pc.continuity = pkt.getCc();

    // Locate TS packet payload
    int headerSize = pkt.getHeaderSize();

    if (!pkt.hasPayload() || headerSize >= QTS_PKT_SIZE) {
        return;
    }

    quint8 pointerField = 0;
    const quint8* payload = 0;
    int payloadSize = 0;
    QtsPacketCounter pusiPktIndex = pc.pusiPktIndex;

    if (pkt.getPusi()) {
        // Keep track of last packet containing a PUSI in this PID
        pc.pusiPktIndex = packetCount();
        // Payload Unit Start Indicator (PUSI) is set.
        // Filter out PES packets. A PES packet starts with the "start code prefix"
        // 00 00 01. This sequence cannot be found in a TS packet with sections
        // (would be 00 = pointer field, 00 = PAT, 01 = not possible for a PAT).
        if (headerSize + 3 <= QTS_PKT_SIZE &&
            pkt.b[headerSize] == 0x00 &&
            pkt.b[headerSize + 1] == 0x00 &&
            pkt.b[headerSize + 2] == 0x01) {
            // Losing sync, will skip all TS packet until next PUSI
            pc.syncLost();
            return;
        }
        // First byte of payload is a pointer field
        pointerField = pkt.b[headerSize];
        payload = pkt.b + headerSize + 1;
        payloadSize = QTS_PKT_SIZE - headerSize - 1;
        // Ignore packet and loose sync if inconsistent pointer field
        if (pointerField >= payloadSize) {
            pc.syncLost();
            return;
        }
        if (pointerField == 0) {
            pusiPktIndex = packetCount();
        }
    }
    else {
        // PUSI not set, first byte of payload is section data
        pointerField = 0xFF;
        payload = pkt.b + headerSize;
        payloadSize = QTS_PKT_SIZE - headerSize;
    }

    // Ignore packet if there is no payload to analyze.
    if (payloadSize <= 0) {
        return;
    }

    // If no previous synchronization, skip incomplete sections
    if (!pc.sync) {
        // If no new section in this packet, ignore it
        if (!pkt.getPusi()) {
            return;
        }
        // Skip end of previous section
        payload += pointerField;
        payloadSize -= pointerField;
        pointerField = 0;
        // We have found the beginning of a section, we are now synchronized.
        pc.sync = true;
    }

    // Copy TS packet payload in PID context
    pc.ts.append(payload, payloadSize);

    // Locate TS buffer by address and size.
    const quint8* tsStart = pc.ts.data();
    int tsSize = pc.ts.size();

    // If current packet has a PUSI, locate start of this new section
    // inside the TS buffer. This is not useful to locate the section but
    // it is used to check that the previous section was not truncated
    // (example: detect incorrect stream as generated with old version
    // of Thomson Grass Valley NetProcessor).
    const quint8* pusiSection = 0;
    if (pkt.getPusi()) {
        pusiSection = tsStart + tsSize - payloadSize + pointerField;
    }

    // Loop on all complete sections in the buffer.
    // If there is less than 3 bytes in the buffer, we cannot even
    // determine the section length.
    while (tsSize >= 3) {

        // Get section header.
        bool sectionOk = true;
        QtsExtTableId etid(tsStart[0]);
        quint16 sectionLength = qFromBigEndian<quint16>(tsStart + 1);
        bool longHeader = (sectionLength & 0x8000) != 0;
        sectionLength = (sectionLength & 0x0FFF) + QTS_SHORT_SECTION_HEADER_SIZE;

        // Lose synchronization when invalid section length.
        if (sectionLength > QTS_MAX_PRIVATE_SECTION_SIZE ||
            sectionLength < QTS_MIN_SHORT_SECTION_SIZE ||
            (longHeader && sectionLength < QTS_MIN_LONG_SECTION_SIZE)) {
            // Invalid section length.
            _status.invSectLength++;
            pc.syncLost();
            return;
        }

        // Exit when end of section is missing. Wait for next TS packets.
        if (tsSize < sectionLength) {
            break;
        }

        // If we detect that the section is incorrectly truncated, skip it.
        if (pusiSection != 0 && tsStart < pusiSection && tsStart + sectionLength > pusiSection) {
            sectionOk = false;
            // Resynchronize to actual section start
            sectionLength = pusiSection - tsStart;
        }

        // We have a complete section in the pc.ts buffer. Analyze it.
        quint8 version = 0;
        bool isNext = false;
        quint8 sectionNumber = 0;
        quint8 lastSectionNumber = 0;

        if (sectionOk && longHeader) {
            etid = QtsExtTableId(etid.tid(), qFromBigEndian<quint16>(tsStart + 3));
            version = (tsStart[5] >> 1) & 0x1F;
            isNext = (tsStart[5] & 0x01) == 0;
            sectionNumber = tsStart[6];
            lastSectionNumber = tsStart[7];
            // Check that the section number fits in the range
            if (sectionNumber > lastSectionNumber) {
                _status.invSectIndex++;
                sectionOk = false;
            }
        }

        // Sections with the 'next' indicator are ignored
        if (isNext) {
            sectionOk = false;
        }

        if (sectionOk) {
            // Get reference to the ETID context for this PID.
            // The ETID context is created if did not exist.
            EtidContext& tc(pc.tids[etid]);

            // If this is a new version of the table, reset the TID context.
            // Note that short sections do not have versions, so the version
            // field is implicitely zero. However, every short section must
            // be considered as a new version since there is no way to track
            // versions.
            if (!longHeader ||             // short section
                tc.sectExpected == 0 ||    // new TID on this PID
                tc.version != version) {   // new version

                tc.version = version;
                tc.sectExpected = int(lastSectionNumber) + 1;
                tc.sectReceived = 0;
                tc.sects.resize(tc.sectExpected);
                // Mark all section entries as unused
                for (int si = 0; si < tc.sectExpected; si++) {
                    tc.sects[si].reset();
                }
            }

            // Check that the total number of sections in the table
            // has not changed since last section.
            if (lastSectionNumber != tc.sectExpected - 1) {
                _status.invSectIndex++;
                sectionOk = false;
            }

            // Create a new Section object if necessary (ie. if a section
            // hendler is registered or if this is a new section).
            QtsSectionPtr sectPtr;

            if (sectionOk && (_sectionHandler != 0 || tc.sects[sectionNumber].isNull())) {
                sectPtr = new QtsSection(tsStart, sectionLength, pid, QtsCrc32::Check);
                sectPtr->setFirstTsPacketIndex(pusiPktIndex);
                sectPtr->setLastTsPacketIndex(packetCount());
                if (!sectPtr->isValid()) {
                    _status.wrongCrc++;  // only possible error (hum?)
                    sectionOk = false;
                }
            }

            // Mark that we are in the context of a table or section handler.
            // This is used to prevent the destruction of PID contexts during
            // the execution of a handler.
            _inHandler = true;
            _pidInHandler = pid;

            try {
                // If a handler is defined for sections, invoke it.
                if (sectionOk && _sectionHandler != 0) {
                    _sectionHandler->handleSection(*this, *sectPtr);
                }

                // Save the section in the TID context if this is a new one.
                if (sectionOk && tc.sects[sectionNumber].isNull()) {

                    // Save the section
                    tc.sects[sectionNumber] = sectPtr;
                    tc.sectReceived++;

                    // If the table is completed and a handler is present, build the table.
                    if (tc.sectReceived == tc.sectExpected && _tableHandler != 0) {

                        // Build the table
                        QtsTable table;
                        for (int i = 0; i < tc.sects.size(); ++i) {
                            table.addSection(tc.sects[i]);
                        }

                        // Invoke the table handler
                        _tableHandler->handleTable(*this, table);
                    }
                }
            }
            catch (...) {
                _inHandler = false;
                throw;
            }

            // End of handler-calling sequence. Now process the delayed destructions.
            _inHandler = false;

            if (_resetPending) {
                // Full reset was requested by a handler.
                _resetPending = false;
                reset();
                return;
            }

            if (pc.resetPending) {
                // Reset of this PID was requested by a handler.
                pc.resetPending = false;
                resetPid(pid);
                return;
            }
        }

        // Move to next section in the buffer
        tsStart += sectionLength;
        tsSize -= sectionLength;

        // The next section necessarily starts in current packet
        pusiPktIndex = packetCount();

        // If start of next area is 0xFF (invalid TID value), the rest of
        // the packet is stuffing. Skip it.
        if (tsSize > 0 && tsStart[0] == 0xFF) {
            tsSize = 0;
        }
    }

    // If an incomplete section remains in the buffer, move it back to
    // the start of the buffer.
    if (tsSize <= 0) {
        // TS buffer becomes empty
        pc.ts.clear();
    }
    else if (tsStart > pc.ts.data()) {
        // Remove start of TS buffer
        pc.ts.remove(0, tsStart - pc.ts.data());
    }
}
