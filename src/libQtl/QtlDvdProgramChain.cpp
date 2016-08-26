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
// Define the class QtlDvdProgramChain.
//
//----------------------------------------------------------------------------

#include "QtlDvdProgramChain.h"
#include "QtlDvdMedia.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlDvdProgramChain::QtlDvdProgramChain(const QtlByteBlock& ifo, int startIndex, int titleNumber, QtlLogger* log) :
    _nullLog(),
    _log(log != 0 ? log : &_nullLog),
    _valid(false),
    _titleNumber(titleNumber),
    _duration(0),
    _programCount(0),
    _cellCount(0),
    _nextPgc(0),
    _previousPgc(0),
    _parentPgc(0),
    _palette(),
    _sectors()
{
    // Check that we have at least the minimum size of a PGC entry.
    if (startIndex < 0 || startIndex + 0x00EC > ifo.size()) {
        return;
    }

    // Get basic PGC info.
    _programCount = ifo[startIndex + 0x0002];
    _cellCount    = ifo[startIndex + 0x0003];
    _duration     = toPlaybackDuration(ifo.fromBigEndian<quint32>(startIndex + 0x0004));
    _nextPgc      = ifo.fromBigEndian<quint16>(startIndex + 0x009C);
    _previousPgc  = ifo.fromBigEndian<quint16>(startIndex + 0x009E);
    _parentPgc    = ifo.fromBigEndian<quint16>(startIndex + 0x00A0);

    // Get the palette in YUV format.
    _palette = ifo.mid(startIndex + 0x00A4, 16 * 4);

    // Start index of the program map.
    // Each program entry is only one byte: the cell number.
    // Programs and cells are numbered starting from 1 (not 0).
    const int programMap = startIndex + ifo.fromBigEndian<quint16>(startIndex + 0x00E6);
    if (programMap + _programCount > ifo.size()) {
        // Program map does not fit into the IFO.
        return;
    }

    // Start index of the cell playback information table.
    // Each cell playback information table entry is 0x18 bytes long.
    // See http://dvd.sourceforge.net/dvdinfo/pgc.html#play
    const int cellPlay = startIndex + ifo.fromBigEndian<quint16>(startIndex + 0x00E8);
    const int cellPlayEntrySize = 0x18;
    if (cellPlay + (cellPlayEntrySize * _cellCount) > ifo.size()) {
        // Cell table does not fit into the IFO.
        return;
    }

    // Get the list of sectors containing the media data for this PGC.
    // First, loop on programs.
    for (int prog = 0; prog < _programCount; ++prog) {
        // Each program entry is only one byte: the cell number.
        const int cellId = ifo[programMap + prog];
        if (cellId < 1 || cellId > _cellCount) {
            // Invalid cell id.
            return;
        }
        // Locate the cell playback information table entry.
        // Cell ids are numbered starting from 1.
        const int cellIndex = cellPlay + (cellPlayEntrySize * (cellId - 1));
        // Get first and last sector of the cell.
        const int firstSector = ifo.fromBigEndian<quint32>(cellIndex + 0x08);
        const int lastSector = ifo.fromBigEndian<quint32>(cellIndex + 0x14);
        if (firstSector < 0 || lastSector < firstSector) {
            // Invalid sector range.
            return;
        }
        // Add the sector range of the cell to the PGC.
        _sectors << QtlRange(firstSector, lastSector);
    }

    // Reduce the list of sectors so that adjacent sectors are merged.
    // But do not reorder the list (don't sort it for instance).
    _sectors.merge(Qtl::AdjacentOnly);

    // Now we can say the PGC is valid.
    _valid = true;
}


//----------------------------------------------------------------------------
// Get the color palette of the title set in RGB format.
//----------------------------------------------------------------------------

QtlByteBlock QtlDvdProgramChain::rgbPalette() const
{
    QtlByteBlock palette(_palette);
    convertPaletteYuvToRgb(palette, _log);
    return palette;
}


//----------------------------------------------------------------------------
// Convert a YUV palette into RGB.
//----------------------------------------------------------------------------

void QtlDvdProgramChain::convertPaletteYuvToRgb(QtlByteBlock& palette, QtlLogger* log)
{
    if (palette.size() % 4 != 0 && log != 0) {
        log->line(QObject::tr("Palette conversion: palette size is %1 bytes, not a multiple of 4").arg(palette.size()));
    }

    // On input, each entry contains 4 bytes: (0, Y, Cr, Cb).
    // On output, each entry contains 4 bytes: (0, R, G, B).
    //
    // The real formula is the following;
    //    r = y + 1.402 * (cr - 128)
    //    g = y - 0.34414 * (cb - 128) - 0.71414 * (cr - 128)
    //    b = y + 1.722 * (cb - 128)
    //
    // The following algorithm is suggested in http://en.wikipedia.org/wiki/YUV.
    //    cr = cr - 128
    //    cb = cb - 128
    //    r = y + cr + (cr >> 2) + (cr >> 3) + (cr >> 5)
    //    g = y - ((cb >> 2) + (cb >> 4) + (cb >> 5)) - ((cr >> 1) + (cr >> 3) + (cr >> 4) + (cr >> 5))
    //    b = y + cb + (cb >> 1) + (cb >> 2) + (cb >> 6)
    // which gives the following values:
    //    r = y + 1.40625 * (cr - 128)
    //    g = y - 0.34375 * (cb - 128) - 0.71875 * (cr - 128)
    //    b = y + 1.765625 * (cb - 128)

    for (int base = 0; base + 4 <= palette.size(); base += 4) {
        if (palette[base] != 0 && log != 0) {
            log->line(QObject::tr("Palette conversion: unexpected value 0x%1, should be 0").arg(int(palette[base]), 2, 16, QChar('0')));
        }
        int y = int(palette[base+1]);
        int cr = int(palette[base+2]) - 128;
        int cb = int(palette[base+3]) - 128;
        int r = y + cr + (cr >> 2) + (cr >> 3) + (cr >> 5);
        int g = y - ((cb >> 2) + (cb >> 4) + (cb >> 5)) - ((cr >> 1) + (cr >> 3) + (cr >> 4) + (cr >> 5));
        int b = y + cb + (cb >> 1) + (cb >> 2) + (cb >> 6);
        palette[base+1] = quint8(qBound(0, r, 255));
        palette[base+2] = quint8(qBound(0, g, 255));
        palette[base+3] = quint8(qBound(0, b, 255));
    }
}


//----------------------------------------------------------------------------
// Convert a YUV or RBG palette into a string.
//----------------------------------------------------------------------------

QString QtlDvdProgramChain::paletteToString(const QtlByteBlock& palette)
{
    QString result;
    for (int base = 0; base + 4 <= palette.size(); base += 4) {
        const quint8 r = palette[base+1];
        const quint8 g = palette[base+2];
        const quint8 b = palette[base+3];
        if (!result.isEmpty()) {
            result.append(",");
        }
        result.append(QStringLiteral("%1%2%3").arg(r,2,16,QChar('0')).arg(g,2,16,QChar('0')).arg(b,2,16,QChar('0')));
    }
    return result;
}


//----------------------------------------------------------------------------
// Decode a 32-bit value as a playback duration.
//----------------------------------------------------------------------------

int QtlDvdProgramChain::toPlaybackDuration(quint32 value)
{
    // The playback time is encoded in BCD as: hh:mm:ss:ff (ff = frame count within second).
    // With 2 MSBits of ff indicating frame rate: 11 = 30 fps, 10 = illegal, 01 = 25 fps, 00 = illegal.

    const int hours    = (((value  >> 28) & 0x0F) * 10) + ((value  >> 24) & 0x0F);
    const int minutes  = (((value  >> 20) & 0x0F) * 10) + ((value  >> 16) & 0x0F);
    const int seconds  = (((value  >> 12) & 0x0F) * 10) + ((value  >>  8) & 0x0F);
    // const int frame = (((value  >>  4) & 0x03) * 10) + (value  & 0x0F);
    // const int rate  = (value  >> 6) & 0x03;

    return (hours * 3600) + (minutes * 60) + seconds;
}


//----------------------------------------------------------------------------
// Get the sum of all title set playback durations in seconds.
//----------------------------------------------------------------------------

int QtlDvdProgramChainPtrList::totalDurationInSeconds() const
{
    int duration = 0;
    foreach (const QtlDvdProgramChainPtr& pcg, *this) {
        if (!pcg.isNull()) {
            duration += pcg->durationInSeconds();
        }
    }
    return duration;
}
