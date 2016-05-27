//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
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
// Parts of this module are copied or inspired from Telxcc, a free open-source
// Teletext extractor from Petr Kutalek.
//
// Home page: https://github.com/petrkutalek/telxcc/
//
// Copyright: (c) 2011-2014 Forers, s. r. o.: telxcc
//
// telxcc conforms to ETSI 300 706 Presentation Level 1.5: Presentation
// Level 1 defines the basic Teletext page, characterised by the use of
// spacing attributes only and a limited alphanumeric and mosaics repertoire.
// Presentation Level 1.5 decoder responds as Level 1 but the character
// repertoire is extended via packets X/26. Selection of national option
// sub-sets related features from Presentation Level 2.5 feature set have
// been implemented, too. (X/28/0 Format 1, X/28/4, M/29/0 and M/29/4 packets)
//
//----------------------------------------------------------------------------
//
// Relevant standards:
//
// - ETSI EN 300 472 V1.3.1 (2003-05)
//   Digital Video Broadcasting (DVB);
//   Specification for conveying ITU-R System B Teletext in DVB bitstreams
// - ETSI EN 300 706 V1.2.1 (2003-04)
//   Enhanced Teletext specification
// - ETSI EN 300 708 V1.2.1 (2003-04)
//   Television systems; Data transmission within Teletext
//
//----------------------------------------------------------------------------
//
// Qts, the Qt MPEG Transport Stream library.
// Define the class QtsTeletextDemux.
//
//----------------------------------------------------------------------------

#include "QtsTeletextDemux.h"
#include "QtsTeletextFrame.h"
#include "QtsTeletextCharset.h"


//-----------------------------------------------------------------------------
// From telxcc code (and maybe other original sources).
//-----------------------------------------------------------------------------

namespace {

    // Static table to remove 8/4 Hamming code.
    const quint8 UNHAM_8_4[256] = {
        0x01, 0xff, 0x01, 0x01, 0xff, 0x00, 0x01, 0xff, 0xff, 0x02, 0x01, 0xff, 0x0a, 0xff, 0xff, 0x07,
        0xff, 0x00, 0x01, 0xff, 0x00, 0x00, 0xff, 0x00, 0x06, 0xff, 0xff, 0x0b, 0xff, 0x00, 0x03, 0xff,
        0xff, 0x0c, 0x01, 0xff, 0x04, 0xff, 0xff, 0x07, 0x06, 0xff, 0xff, 0x07, 0xff, 0x07, 0x07, 0x07,
        0x06, 0xff, 0xff, 0x05, 0xff, 0x00, 0x0d, 0xff, 0x06, 0x06, 0x06, 0xff, 0x06, 0xff, 0xff, 0x07,
        0xff, 0x02, 0x01, 0xff, 0x04, 0xff, 0xff, 0x09, 0x02, 0x02, 0xff, 0x02, 0xff, 0x02, 0x03, 0xff,
        0x08, 0xff, 0xff, 0x05, 0xff, 0x00, 0x03, 0xff, 0xff, 0x02, 0x03, 0xff, 0x03, 0xff, 0x03, 0x03,
        0x04, 0xff, 0xff, 0x05, 0x04, 0x04, 0x04, 0xff, 0xff, 0x02, 0x0f, 0xff, 0x04, 0xff, 0xff, 0x07,
        0xff, 0x05, 0x05, 0x05, 0x04, 0xff, 0xff, 0x05, 0x06, 0xff, 0xff, 0x05, 0xff, 0x0e, 0x03, 0xff,
        0xff, 0x0c, 0x01, 0xff, 0x0a, 0xff, 0xff, 0x09, 0x0a, 0xff, 0xff, 0x0b, 0x0a, 0x0a, 0x0a, 0xff,
        0x08, 0xff, 0xff, 0x0b, 0xff, 0x00, 0x0d, 0xff, 0xff, 0x0b, 0x0b, 0x0b, 0x0a, 0xff, 0xff, 0x0b,
        0x0c, 0x0c, 0xff, 0x0c, 0xff, 0x0c, 0x0d, 0xff, 0xff, 0x0c, 0x0f, 0xff, 0x0a, 0xff, 0xff, 0x07,
        0xff, 0x0c, 0x0d, 0xff, 0x0d, 0xff, 0x0d, 0x0d, 0x06, 0xff, 0xff, 0x0b, 0xff, 0x0e, 0x0d, 0xff,
        0x08, 0xff, 0xff, 0x09, 0xff, 0x09, 0x09, 0x09, 0xff, 0x02, 0x0f, 0xff, 0x0a, 0xff, 0xff, 0x09,
        0x08, 0x08, 0x08, 0xff, 0x08, 0xff, 0xff, 0x09, 0x08, 0xff, 0xff, 0x0b, 0xff, 0x0e, 0x03, 0xff,
        0xff, 0x0c, 0x0f, 0xff, 0x04, 0xff, 0xff, 0x09, 0x0f, 0xff, 0x0f, 0x0f, 0xff, 0x0e, 0x0f, 0xff,
        0x08, 0xff, 0xff, 0x05, 0xff, 0x0e, 0x0d, 0xff, 0xff, 0x0e, 0x0f, 0xff, 0x0e, 0x0e, 0xff, 0x0e
    };

    // Static table to swap bits in a byte.
    const quint8 REVERSE_8[256] = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
    };

    // Text foreground color codes.
    const char* const TELETEXT_COLORS[8] = {
        //0=black, 1=red,     2=green,   3=yellow,  4=blue,    5=magenta, 6=cyan,    7=white
        "#000000", "#ff0000", "#00ff00", "#ffff00", "#0000ff", "#ff00ff", "#00ffff", "#ffffff"
    };
}


//-----------------------------------------------------------------------------
// Remove 8/4 and 24/18 Hamming code. Code from telxcc.
//-----------------------------------------------------------------------------

quint8 QtsTeletextDemux::unham_8_4(quint8 a)
{
    const quint8 r = UNHAM_8_4[a];
    return r == 0xFF ? 0x00 : r & 0x0F;
}

quint32 QtsTeletextDemux::unham_24_18(quint32 a)
{
    quint8 test = 0;

    // Tests A-F correspond to bits 0-6 respectively in 'test'.
    for (quint8 i = 0; i < 23; i++) {
        test ^= ((a >> i) & 0x01) * (i + 33);
    }

    // Only parity bit is tested for bit 24
    test ^= ((a >> 23) & 0x01) * 32;

    if ((test & 0x1F) != 0x1F) {
        // Not all tests A-E correct
        if ((test & 0x20) == 0x20) {
            // F correct: Double error
            return 0xFFFFFFFF;
        }
        // Test F incorrect: Single error
        a ^= 1 << (30 - test);
    }

    return (a & 0x000004) >> 2 | (a & 0x000070) >> 3 | (a & 0x007F00) >> 4 | (a & 0x7F0000) >> 5;
}


//-----------------------------------------------------------------------------
// Convert a page number between binary and BCD.
//-----------------------------------------------------------------------------

int QtsTeletextDemux::pageBcdToBinary(int bcd)
{
    return 100 * ((bcd >> 8) & 0x0F) + 10 * ((bcd >> 4) & 0x0F) + (bcd & 0x0F);
}

int QtsTeletextDemux::pageBinaryToBcd(int bin)
{
    return (((bin / 100) % 10) << 8) | (((bin / 10) % 10) << 4) | (bin % 10);
}


//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

QtsTeletextDemux::QtsTeletextDemux(QtsTeletextHandlerInterface* handler, const QtsPidSet& pidFilter) :
    QtsPesDemux(0, pidFilter),
    _txtHandler(handler),
    _pids(),
    _addColors(),
    _inHandler(false),
    _pidInHandler(QTS_PID_NULL),
    _resetPending(false)
{
}

QtsTeletextDemux::~QtsTeletextDemux()
{
    flushTeletext();
}

QtsTeletextDemux::TeletextPage::TeletextPage() :
    frameCount(0),
    showTimestamp(0),
    hideTimestamp(0),
    tainted(false),
    charset()
{
    ::memset(text, 0, sizeof(text));
}

void QtsTeletextDemux::TeletextPage::reset(quint64 timestamp)
{
    showTimestamp = timestamp;
    hideTimestamp = 0;
    tainted = false;
    ::memset(text, 0, sizeof(text));
}

QtsTeletextDemux::PidContext::PidContext() :
    timeStamper(),
    resetPending(false),
    receivingData(false),
    transMode(TRANSMODE_SERIAL),
    currentPage(0),
    pages()
{
}


//-----------------------------------------------------------------------------
// Reset the analysis context (partially built TELETEXT packets).
//-----------------------------------------------------------------------------

void QtsTeletextDemux::reset()
{
    if (_inHandler) {
        // In the context of a handler, delay the reset
        _resetPending = true;
    }
    else {
        // Perform the actual reset.
        _pids.clear();
        // Invoke superclass.
        QtsPesDemux::reset();
    }
}


//-----------------------------------------------------------------------------
// Reset the analysis context for one single PID.
//-----------------------------------------------------------------------------

void QtsTeletextDemux::resetPid(QtsPid pid)
{
    if (_inHandler && _pidInHandler == pid) {
        // In the context of a handler for this PID, delay the reset
        _pids[pid].resetPending = true;
    }
    else {
        // Perform the actual reset.
        _pids.remove(pid);
        // Invoke superclass.
        QtsPesDemux::resetPid(pid);
    }
}


//-----------------------------------------------------------------------------
// This hook is invoked when a complete PES packet is available.
//-----------------------------------------------------------------------------

void QtsTeletextDemux::handlePesPacket(const QtsPesPacket& packet)
{
    // Invoke superclass.
    QtsPesDemux::handlePesPacket(packet);

    // Create PID context if non existent.
    const QtsPid pid = packet.getSourcePid();
    PidContext& pc(_pids[pid]);

    // Make sure the PID context knows our demux, if just created.
    pc.timeStamper.setDemux(this);
    pc.timeStamper.processPesPacket(packet);

    // Explore PES payload.
    const quint8* pl = packet.payload();
    int plSize = packet.payloadSize();

    // The first byte is a data_identifier.
    if (plSize < 1 || *pl < QTS_TELETEXT_PES_FIRST_EBU_DATA_ID || *pl > QTS_TELETEXT_PES_LAST_EBU_DATA_ID) {
        // Not a valid Teletext PES packet.
        return;
    }
    pl++;
    plSize--;

    // Loop on all data units inside the PES payload.
    while (plSize >= 2) {

        // Data unit header (2 bytes).
        const int unitId = pl[0];
        const int unitSize = pl[1];
        plSize -= 2;
        pl += 2;

        // Filter Teletext packets.
        if (unitSize <= plSize &&
            unitSize == QTS_TELETEXT_PACKET_SIZE &&
            (unitId == QTS_TELETEXT_DATA_UNIT_ID_NON_SUBTITLE || unitId == QTS_TELETEXT_DATA_UNIT_ID_SUBTITLE))
        {
            // Reverse bitwise endianess of each data byte via lookup table, ETS 300 706, chapter 7.1.
            quint8 pkt[QTS_TELETEXT_PACKET_SIZE];
            for (int i = 0; i < unitSize; ++i) {
                pkt[i] = REVERSE_8[pl[i]];
            }
            processTeletextPacket(pid, pc, unitId, pkt);
        }

        // Point to next data unit.
        plSize -= unitSize;
        pl += unitSize;
    }
}


//-----------------------------------------------------------------------------
// Process one Teletext packet.
// The following code is mostly copied from telxcc, thank you Petr !
//-----------------------------------------------------------------------------

void QtsTeletextDemux::processTeletextPacket(QtsPid pid, QtsTeletextDemux::PidContext& pc, quint8 dataUnitId, const quint8* pkt)
{
    // Structure of a Teletext packet. See ETSI 300 706, section 7.1.
    // - Clock run-in: 1 byte
    // - Framing code: 1 byte
    // - Address: 2 bytes
    // - Data: 40 bytes

    // Variable names conform to ETS 300 706, chapter 7.1.2.
    quint8 address = (unham_8_4(pkt[3]) << 4) | unham_8_4(pkt[2]);
    quint8 m = address & 0x07;
    if (m == 0) {
        m = 8;
    }
    quint8 y = (address >> 3) & 0x1F;
    const quint8* data = pkt + 4;
    quint8 designationCode = (y > 25) ? unham_8_4(data[0]) : 0x00;

    if (y == 0) {
        // Page number and control bits
        quint16 pageNumber = (m << 8) | (unham_8_4(data[1]) << 4) | unham_8_4(data[0]);
        quint8 charset = ((unham_8_4(data[7]) & 0x08) | (unham_8_4(data[7]) & 0x04) | (unham_8_4(data[7]) & 0x02)) >> 1;

        // ETS 300 706, chapter 9.3.1.3:
        //
        // When set to '1' the service is designated to be in Serial mode and the transmission
        // of a page is terminated by the next page header with a different page number.
        // When set to '0' the service is designated to be in Parallel mode and the transmission
        // of a page is terminated by the next page header with a different page number but the
        // same magazine number.
        //
        // The same setting shall be used for all page headers in the service.
        //
        // ETS 300 706, chapter 7.2.1: Page is terminated by and excludes the next page header packet
        // having the same magazine address in parallel transmission mode, or any magazine address in
        // serial transmission mode.
        //
        pc.transMode = TransMode(unham_8_4(data[7]) & 0x01);

        // FIXME: Well, this is not ETS 300 706 kosher, however we are interested in TELETEXT_SUBTITLE only
        if (pc.transMode == TRANSMODE_PARALLEL && dataUnitId != QTS_TELETEXT_DATA_UNIT_ID_SUBTITLE) {
            return;
        }

        if (pc.receivingData &&
            ((pc.transMode == TRANSMODE_SERIAL && pageOf(pageNumber) != pageOf(pc.currentPage)) ||
             (pc.transMode == TRANSMODE_PARALLEL && pageOf(pageNumber) != pageOf(pc.currentPage) && m == magazineOf(pc.currentPage))))
        {
            pc.receivingData = false;
        }

        // A new frame starts on a page. If this page had a non-empty frame in progress, flush it now.
        TeletextPage& page(pc.pages[pageNumber]);
        if (page.tainted) {
            // It would not be nice if subtitle hides previous video frame, so we contract 40 ms (1 frame @25 fps)
            page.hideTimestamp = pc.timeStamper.lastTimeStamp() - 40;
            processTeletextPage(pid, pc, pageNumber);
        }

        // Start new page.
        pc.currentPage = pageNumber;
        page.reset(pc.timeStamper.lastTimeStamp());
        page.charset.resetX28(charset);
        pc.receivingData = true;
    }
    else if (m == magazineOf(pc.currentPage) && y >= 1 && y <= 23 && pc.receivingData) {
        // ETS 300 706, chapter 9.4.1: Packets X/26 at presentation Levels 1.5, 2.5, 3.5 are used for addressing
        // a character location and overwriting the existing character defined on the Level 1 page
        // ETS 300 706, annex B.2.2: Packets with Y = 26 shall be transmitted before any packets with Y = 1 to Y = 25;
        // so page_buffer.text[y][i] may already contain any character received
        // in frame number 26, skip original G0 character
        TeletextPage& page(pc.pages[pc.currentPage]);
        for (quint8 i = 0; i < 40; i++) {
            if (page.text[y][i] == 0x00) {
                page.text[y][i] = page.charset.teletextToUcs2(data[i]);
            }
        }
        page.tainted = true;
    }
    else if (m == magazineOf(pc.currentPage) && y == 26 && pc.receivingData) {
        // ETS 300 706, chapter 12.3.2: X/26 definition
        quint8 x26Row = 0;
        quint8 x26Col = 0;

        quint32 triplets[13] = { 0 };
        for (quint8 i = 1, j = 0; i < 40; i += 3, j++) {
            triplets[j] = unham_24_18((data[i + 2] << 16) | (data[i + 1] << 8) | data[i]);
        }

        for (quint8 j = 0; j < 13; j++) {
            if (triplets[j] == 0xffffffff) {
                // invalid data (HAM24/18 uncorrectable error detected), skip group
                continue;
            }

            const quint8 data = (triplets[j] & 0x3f800) >> 11;
            const quint8 mode = (triplets[j] & 0x7c0) >> 6;
            const quint8 address = triplets[j] & 0x3f;
            const bool rowAddressGroup = (address >= 40) && (address <= 63);

            TeletextPage& page(pc.pages[pc.currentPage]);

            // ETS 300 706, chapter 12.3.1, table 27: set active position
            if (mode == 0x04 && rowAddressGroup) {
                x26Row = address - 40;
                if (x26Row == 0) {
                    x26Row = 24;
                }
                x26Col = 0;
            }

            // ETS 300 706, chapter 12.3.1, table 27: termination marker
            if (mode >= 0x11 && mode <= 0x1f && rowAddressGroup) {
                break;
            }

            // ETS 300 706, chapter 12.3.1, table 27: character from G2 set
            if (mode == 0x0f && rowAddressGroup) {
                x26Col = address;
                if (data > 31) {
                    page.text[x26Row][x26Col] = page.charset.g2ToUcs2(data);
                }
            }

            // ETS 300 706, chapter 12.3.1, table 27: G0 character with diacritical mark
            if (mode >= 0x11 && mode <= 0x1f && !rowAddressGroup) {
                x26Col = address;
                page.text[x26Row][x26Col] = page.charset.g2AccentToUcs2(data, mode - 0x11);
            }
        }
    }
    else if (m == magazineOf(pc.currentPage) && y == 28 && pc.receivingData) {
        // TODO:
        //   ETS 300 706, chapter 9.4.7: Packet X/28/4
        //   Where packets 28/0 and 28/4 are both transmitted as part of a page, packet 28/0 takes precedence over 28/4 for all but the colour map entry coding.
        if (designationCode == 0 || designationCode == 4) {
            // ETS 300 706, chapter 9.4.2: Packet X/28/0 Format 1
            // ETS 300 706, chapter 9.4.7: Packet X/28/4
            const quint32 triplet0 = unham_24_18((data[3] << 16) | (data[2] << 8) | data[1]);
            // ETS 300 706, chapter 9.4.2: Packet X/28/0 Format 1 only
            if ((triplet0 & 0x0f) == 0x00) {
                pc.pages[pc.currentPage].charset.setX28((triplet0 & 0x3f80) >> 7);
            }
        }
    }
    else if (m == magazineOf(pc.currentPage) && y == 29) {
        // TODO:
        //   ETS 300 706, chapter 9.5.1 Packet M/29/0
        //   Where M/29/0 and M/29/4 are transmitted for the same magazine, M/29/0 takes precedence over M/29/4.
        if (designationCode == 0 || designationCode == 4) {
            // ETS 300 706, chapter 9.5.1: Packet M/29/0
            // ETS 300 706, chapter 9.5.3: Packet M/29/4
            const quint32 triplet0 = unham_24_18((data[3] << 16) | (data[2] << 8) | data[1]);
            // ETS 300 706, table 11: Coding of Packet M/29/0
            // ETS 300 706, table 13: Coding of Packet M/29/4
            if ((triplet0 & 0xff) == 0x00) {
                pc.pages[pc.currentPage].charset.setM29((triplet0 & 0x3f80) >> 7);
            }
        }
    }
    else if ((m == 8) && (y == 30)) {
        // ETS 300 706, chapter 9.8: Broadcast Service Data Packets.
        // We can find here "Programme Identification Data" and absolute data / time stamps.
        // It is processed in telxcc but is not interesting for us .
    }
}


//-----------------------------------------------------------------------------
// Process one Teletext page.
// The following code is mostly copied from telxcc, thank you Petr !
//-----------------------------------------------------------------------------

void QtsTeletextDemux::processTeletextPage(QtsPid pid, QtsTeletextDemux::PidContext& pc, int pageNumber)
{
    // Reference to the page content.
    QtsTeletextDemux::TeletextPage& page(pc.pages[pageNumber]);

    // Optimization: slicing column by column -- higher probability we could find boxed area start mark sooner
    bool pageIsEmpty = true;
    for (quint8 col = 0; pageIsEmpty && col < 40; col++) {
        for (quint8 row = 1; pageIsEmpty && row < 25; row++) {
            if (page.text[row][col] == 0x0B) {
                pageIsEmpty = false;
            }
        }
    }
    if (pageIsEmpty) {
        return;
    }

    // Adjust frame count and timestamps.
    page.frameCount++;
    if (page.showTimestamp > page.hideTimestamp) {
        page.hideTimestamp = page.showTimestamp;
    }

    // Prepare the Teletext frame.
    QtsTeletextFrame frame(pid, pageBcdToBinary(pageNumber), page.frameCount, page.showTimestamp, page.hideTimestamp);

    // Process page data.
    for (quint8 row = 1; row < 25; row++) {
        QString line;

        // Anchors for string trimming purpose
        quint8 colStart = 40;
        quint8 colStop = 40;

        for (qint8 col = 39; col >= 0; col--) {
            if (page.text[row][col] == 0x0B) {
                colStart = col;
                break;
            }
        }
        if (colStart > 39) {
            // Line is empty
            continue;
        }

        for (quint8 col = colStart + 1; col <= 39; col++) {
            if (page.text[row][col] > 0x20) {
                if (colStop > 39) {
                    colStart = col;
                }
                colStop = col;
            }
            if (page.text[row][col] == 0x0A) {
                break;
            }
        }
        if (colStop > 39) {
            // Line is empty
            continue;
        }

        // ETS 300 706, chapter 12.2: Alpha White ("Set-After") - Start-of-row default condition.
        // used for colour changes _before_ start box mark
        // white is default as stated in ETS 300 706, chapter 12.2
        // black(0), red(1), green(2), yellow(3), blue(4), magenta(5), cyan(6), white(7)
        quint8 foregroundColor = 0x07;
        bool fontTagOpened = false;

        for (quint8 col = 0; col <= colStop; col++) {
            // v is just a shortcut
            quint16 v = page.text[row][col];

            if (col < colStart) {
                if (v <= 0x7) {
                    foregroundColor = v;
                }
            }

            if (col == colStart) {
                if (foregroundColor != 0x7 && _addColors) {
                    line.append("<font color=\"");
                    line.append(TELETEXT_COLORS[foregroundColor]);
                    line.append("\">");
                    fontTagOpened = true;
                }
            }

            if (col >= colStart) {
                if (v <= 0x7) {
                    // ETS 300 706, chapter 12.2: Unless operating in "Hold Mosaics" mode,
                    // each character space occupied by a spacing attribute is displayed as a SPACE.
                    if (_addColors) {
                        if (fontTagOpened) {
                            line.append("</font> ");
                            fontTagOpened = false;
                        }

                        // black is considered as white for telxcc purpose
                        // telxcc writes <font/> tags only when needed
                        if (v > 0x00 && v < 0x07) {
                            line.append("<font color=\"");
                            line.append(TELETEXT_COLORS[v]);
                            line.append("\">");
                            fontTagOpened = true;
                        }
                    }
                    else {
                        v = 0x20;
                    }
                }

                // Translate some chars into entities, if in colour mode, to replace unsafe HTML tag chars
                if (v >= 0x20 && _addColors) {
                    struct HtmlEntity {
                        quint16 character;
                        const char* entity;
                    };
                    static const HtmlEntity entities[] = {
                        {'<', "&lt;"},
                        {'>', "&gt;"},
                        {'&', "&amp;"},
                        {0, 0}
                    };
                    for (const HtmlEntity* p = entities; p->entity != 0; ++p) {
                        if (v == p->character) {
                            line.append(p->entity);
                            v = 0;  // v < 0x20 won't be printed in next block
                            break;
                        }
                    }
                }

                if (v >= 0x20) {
                    line.append(QChar(ushort(v)));
                }
            }
        }

        // No tag will be left opened!
        if (_addColors && fontTagOpened) {
            line.append("</font>");
            fontTagOpened = false;
        }

        // Line is now complete.
        frame.addLine(line);
    }

    // Now call the user-specified handler.
    // Mark that we are in the context of handlers.
    // This is used to prevent the destruction of PID contexts during the execution of a handler.
    _inHandler = true;
    _pidInHandler = pid;

    // Handle complete packet
    try {
        if (_txtHandler != 0) {
            _txtHandler->handleTeletextMessage(*this, frame);
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
    }
    else if (pc.resetPending) {
        // Reset of this PID was requested by a handler.
        pc.resetPending = false;
        resetPid(pid);
    }
}


//-----------------------------------------------------------------------------
// Flush any pending Teletext message.
//-----------------------------------------------------------------------------

void QtsTeletextDemux::flushTeletext()
{
    for (PidContextMap::Iterator itPid = _pids.begin(); itPid != _pids.end(); ++itPid) {
        for (TeletextPageMap::Iterator itPage = itPid->pages.begin(); itPage != itPid->pages.end(); ++itPage) {
            if (itPage->tainted) {
                // Use the last timestamp (ms) for end of message.
                // This time, we do not subtract any frames, there will be no more frames.
                itPage->hideTimestamp = itPid->timeStamper.lastTimeStamp();
                processTeletextPage(itPid.key(), itPid.value(), itPage.key());
                itPage->reset(itPid->timeStamper.lastTimeStamp());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Get the number of Teletext frames found in a given page.
//-----------------------------------------------------------------------------

int QtsTeletextDemux::frameCount(int page, QtsPid pid) const
{
    // Internally, Teletext page numbers are stored in Binary-Coded Decimal.
    const int bcdPage = pageBinaryToBcd(page);

    if (pid != QTS_PID_NULL) {
        const PidContextMap::ConstIterator itPid = _pids.find(pid);
        if (itPid != _pids.end()) {
            const TeletextPageMap::ConstIterator itPage = itPid->pages.find(bcdPage);
            return itPage == itPid->pages.end() ? 0 : itPage->frameCount;
        }
    }
    else {
        for (PidContextMap::ConstIterator itPid = _pids.begin(); itPid != _pids.end(); ++itPid) {
            const TeletextPageMap::ConstIterator itPage = itPid->pages.find(bcdPage);
            if (itPage != itPid->pages.end() && itPage->frameCount > 0) {
                return itPage->frameCount;
            }
        }
    }
    return 0;
}
