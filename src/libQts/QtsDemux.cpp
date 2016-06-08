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
// Define the class QtsDemux.
//
//----------------------------------------------------------------------------

#include "QtsDemux.h"


//----------------------------------------------------------------------------
// Constructor / destructor.
//----------------------------------------------------------------------------

QtsDemux::QtsDemux(const QtsPidSet& pidFilter) :
    _packetCount(0),
    _lastPcr(-1),
    _pidFilter(pidFilter)
{
}

QtsDemux::~QtsDemux()
{
}


//-----------------------------------------------------------------------------
// Feed the demux with a TS packet.
//-----------------------------------------------------------------------------

void QtsDemux::feedPacket(const QtsTsPacket& packet)
{
    // Get most recent PCR. Since a PCR is always >= 0, _lastPcr becomes valid.
    if (packet.hasPcr()) {
        _lastPcr = packet.getPcr();
    }

    // Process the packet if from a filtered PID.
    if (_pidFilter[packet.getPid()]) {
        processTsPacket(packet);
    }

    // Count packets after packet processing.
    _packetCount++;
}


//-----------------------------------------------------------------------------
// Set a completely new PID filter
//-----------------------------------------------------------------------------

void QtsDemux::setPidFilter(const QtsPidSet& newPidFilter)
{
    // Get list of removed PID's
    const QtsPidSet removedPids(_pidFilter & ~newPidFilter);

    // Set the new filter
    _pidFilter = newPidFilter;

    // Reset context of all removed PID's
    if (removedPids.any()) {
        for (QtsPid pid = 0; pid < QTS_PID_MAX; ++pid) {
            if (removedPids[pid]) {
                resetPid(pid);
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Add a PID to filter.
//-----------------------------------------------------------------------------

void QtsDemux::addPid(QtsPid pid)
{
    _pidFilter.set(pid);
}


//-----------------------------------------------------------------------------
// Remove one PID from the filtering.
//-----------------------------------------------------------------------------

void QtsDemux::removePid(QtsPid pid)
{
    // If the PID was actually filtered, we need to reset the context
    if (_pidFilter[pid]) {
        _pidFilter.reset(pid);
        resetPid(pid);
    }
}


//-----------------------------------------------------------------------------
// Reset the analysis context (partially built sections and tables).
//-----------------------------------------------------------------------------

void QtsDemux::reset()
{
    _lastPcr = 0;
}


//-----------------------------------------------------------------------------
// Reset the analysis context for one single PID.
// The default implementation does nothing.
//-----------------------------------------------------------------------------

void QtsDemux::resetPid(QtsPid pid)
{
}
