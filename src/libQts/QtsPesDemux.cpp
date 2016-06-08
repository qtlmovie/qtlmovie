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
// Define the class QtsPesDemux.
//
//----------------------------------------------------------------------------

#include "QtsPesDemux.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

QtsPesDemux::QtsPesDemux(QtsPesHandlerInterface* handler, const QtsPidSet& pidFilter) :
    QtsDemux(pidFilter),
    _pesHandler(handler),
    _pids(),
    _inHandler(false),
    _pidInHandler(QTS_PID_NULL),
    _resetPending(false)
{
}


//-----------------------------------------------------------------------------
// PidContext constructor
//-----------------------------------------------------------------------------

QtsPesDemux::PidContext::PidContext() :
    pesCount(0),
    continuity(0),
    sync(false),
    firstPkt(0),
    lastPkt(0),
    ts(),
    resetPending(false)
{
}


//-----------------------------------------------------------------------------
// Reset the analysis context (partially built PES packets).
//-----------------------------------------------------------------------------

void QtsPesDemux::reset()
{
    if (_inHandler) {
        // In the context of a handler, delay the reset
        _resetPending = true;
    }
    else {
        // Perform the actual reset.
        _pids.clear();
        QtsDemux::reset();
    }
}


//-----------------------------------------------------------------------------
// Reset the analysis context for one single PID.
//-----------------------------------------------------------------------------

void QtsPesDemux::resetPid(QtsPid pid)
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
// Feed the demux with a TS packet (PID already filtered).
//-----------------------------------------------------------------------------

void QtsPesDemux::processTsPacket(const QtsTsPacket& packet)
{
    // Reject invalid packets
    if (!packet.hasValidSync()) {
        return;
    }

    // Get PID and check if context exists
    const QtsPid pid = packet.getPid();
    PidContextMap::Iterator pci = _pids.find(pid);
    bool pcExists = pci != _pids.end();

    // If no context established and not at a unit start, ignore packet
    if (!pcExists && !packet.getPusi()) {
        return;
    }

    // If at a unit start and the context exists, process previous PES packet in context
    if (pcExists && packet.getPusi() && pci.value().sync) {
        // Process packet, invoke all handlers.
        processPesPacket(pid, pci.value());
        // Recheck PID context in case it was reset by a handler.
        pci = _pids.find(pid);
        pcExists = pci != _pids.end();
    }

    // If the packet is scrambled, we cannot get PES content.
    // Usually, if the PID becomes scrambled, it will remain scrambled for a while => release context.
    if (packet.isScrambled()) {
        if (pcExists) {
            _pids.remove(pid);
        }
        return;
    }

    // TS packet payload
    const int plSize = packet.getPayloadSize();
    const quint8* pl = packet.getPayload();

    // If the packet contains a unit start
    if (packet.getPusi()) {
        // If the beginning of a PUSI payload is 00 00 01, this is a PES packet
        // (it is not possible to have 00 00 01 in a PUSI packet containing sections).
        if (plSize >= 3 && pl[0] == 0 && pl[1] == 0 && pl[2] == 1) {
            // We are at the beginning of a PES packet. Create context if non existent.
            PidContext& pc(_pids[pid]);
            pc.continuity = packet.getCc();
            pc.sync = true;
            pc.ts.copy(pl, plSize);
            pc.resetPending = false;
            pc.firstPkt = packetCount();
            pc.lastPkt = packetCount();
        }
        else if (pcExists) {
            // This PID does not contain PES packet, reset context
            _pids.remove(pid);
        }
        // PUSI packet processing done.
        return;
    }

    // At this point, the TS packet contains part of a PES packet, but not beginning.
    // Check that PID context is valid.
    if (!pcExists || !pci.value().sync) {
        return;
    }
    PidContext& pc(pci.value());

    // Ignore duplicate packets (same CC)
    if (packet.getCc() == pc.continuity) {
        return;
    }

    // Check if we are still synchronized.
    if (packet.getCc() != (pc.continuity + 1) % QTS_CC_MAX) {
        pc.syncLost();
        return;
    }
    pc.continuity = packet.getCc();

    // Append the TS payload in PID context.
    int capacity = pc.ts.capacity();
    if (pc.ts.size() + plSize > capacity) {
        // Internal reallocation needed in ts buffer.
        // Do not allow implicit reallocation, do it manually for better performance.
        // Use two predefined thresholds: 64 kB and 512 kB. Above that, double the size.
        // Note that 64 kB is OK for audio PIDs. Video PIDs are usually unbounded. The
        // maximum observed PES rate is 2 PES/s, meaning 512 kB / PES at 8 Mb/s.
        if (capacity < 64 * 1024) {
            pc.ts.reserve(64 * 1024);
        }
        else if (capacity < 512 * 1024) {
            pc.ts.reserve(512 * 1024);
        }
        else {
            pc.ts.reserve(2 * capacity);
        }
    }
    pc.ts.append(pl, plSize);

    // Last TS packet containing actual data for this PES packet
    pc.lastPkt = packetCount();
}


//-----------------------------------------------------------------------------
// Process a complete PES packet
//-----------------------------------------------------------------------------

void QtsPesDemux::processPesPacket(QtsPid pid, PidContext& pc)
{
    // Build a PES packet object around the TS buffer
    QtsPesPacket pp(pc.ts, pid);
    if (!pp.isValid()) {
        return;
    }

    // Count valid PES packets
    pc.pesCount++;

    // Location of the PES packet inside the demultiplexed stream
    pp.setFirstTsPacketIndex(pc.firstPkt);
    pp.setLastTsPacketIndex(pc.lastPkt);

    // Mark that we are in the context of handlers.
    // This is used to prevent the destruction of PID contexts during the execution of a handler.
    _inHandler = true;
    _pidInHandler = pid;

    // Handle complete packet
    try {
        handlePesPacket(pp);
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
    }
    else if (pc.resetPending) {
        // Reset of this PID was requested by a handler.
        pc.resetPending = false;
        resetPid(pid);
    }
}


//-----------------------------------------------------------------------------
// This hook is invoked when a complete PES packet is available.
//-----------------------------------------------------------------------------

void QtsPesDemux::handlePesPacket(const QtsPesPacket& packet)
{
    if (_pesHandler != 0) {
        _pesHandler->handlePesPacket(*this, packet);
    }
}
