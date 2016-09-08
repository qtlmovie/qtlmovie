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

QtlDvdProgramChain::QtlDvdProgramChain(const QtlByteBlock& ifo, int startIndex, int titleNumber, const QtlDvdOriginalCellList& originalCells, QtlLogger* log) :
    _nullLog(),
    _log(log != 0 ? log : &_nullLog),
    _valid(false),
    _titleNumber(titleNumber),
    _duration(0),
    _nextPgc(0),
    _previousPgc(0),
    _parentPgc(0),
    _palette(),
    _cells(),
    _chapters()
{
    // Check that we have at least the minimum size of a PGC entry.
    if (startIndex < 0 || startIndex + 0x00EC > ifo.size()) {
        return;
    }

    // Get basic PGC info.
    const int programCount = ifo[startIndex + 0x0002];
    const int cellCount    = ifo[startIndex + 0x0003];
    _duration    = toPlaybackDuration(ifo.fromBigEndian<quint32>(startIndex + 0x0004));
    _nextPgc     = ifo.fromBigEndian<quint16>(startIndex + 0x009C);
    _previousPgc = ifo.fromBigEndian<quint16>(startIndex + 0x009E);
    _parentPgc   = ifo.fromBigEndian<quint16>(startIndex + 0x00A0);

    // Get the palette in YUV format.
    _palette = ifo.mid(startIndex + 0x00A4, 16 * 4);

    // Start index of the program map.
    // Each program entry is only one byte: the cell number.
    // Programs and cells are numbered starting from 1 (not 0).
    const int pmapStart = startIndex + ifo.fromBigEndian<quint16>(startIndex + 0x00E6);
    if (pmapStart + programCount > ifo.size()) {
        // Program map does not fit into the IFO.
        return;
    }

    // Start index of the "cell playback information table".
    // Each table entry is 0x18 bytes long.
    // See http://dvd.sourceforge.net/dvdinfo/pgc.html#play
    const int cellPlayStart = startIndex + ifo.fromBigEndian<quint16>(startIndex + 0x00E8);
    const int cellPlayEntrySize = 0x0018;
    if (cellPlayStart + (cellPlayEntrySize * cellCount) > ifo.size()) {
        // Cell table does not fit into the IFO.
        return;
    }

    // Start index of the "Cell Position Information Table"
    // Each table entry is 0x04 bytes long.
    // See http://dvd.sourceforge.net/dvdinfo/pgc.html#pos
    const int cellPosStart = startIndex + ifo.fromBigEndian<quint16>(startIndex + 0x00EA);
    const int cellPosEntrySize = 0x0004;
    if (cellPosStart + cellCount * cellPosEntrySize > ifo.size()) {
        // Cell table does not fit into the IFO.
        return;
    }

    // Angle in current cell (0 means normal, for all angles).
    int currentAngle = 0;

    // Loop on all cells.
    for (int cellIndex = 0; cellIndex < cellCount; ++cellIndex) {

        const QtlDvdProgramCellPtr cell(new QtlDvdProgramCell(cellIndex + 1));
        _cells.append(cell);

        // Locate entry in "Cell Playback Information Table"
        const int cellPlayEntry = cellPlayStart + cellIndex * cellPlayEntrySize;

        // Compute angle id from cell category:
        // Bits 7-6: cell type 00=normal, 01=first of angle block, 10=middle of angle block, 11=last of angle block
        // Bits 5-4: block type 00 = normal, 01 = angle block
        const int category = ifo[cellPlayEntry] & 0xF0;
        if (category == 0x50) {
            // 0101 : first angle.
            currentAngle = 1;
        }
        else if ((category == 0x90 || category == 0xD0) && currentAngle != 0) {
            // 1001 : middle angle or 1101 : last angle => next angle in both cases.
            currentAngle++;
        }
        // If category == 0 ("normal"), this is common content.
        cell->_angleId = category == 0 ? 0 : currentAngle;
        if (category == 0xD0) {
            // 1101 : last angle => reset angle count.
            currentAngle = 0;
        }

        // Cell playback duration.
        cell->_duration = toPlaybackDuration(ifo.fromBigEndian<quint32>(cellPlayEntry + 4));

        // Locate entry in "Cell Positon Information Table"
        const int cellPosEntry = cellPosStart + cellIndex * cellPosEntrySize;

        // Get original VOB Id and original Cell Id.
        cell->_originalVobId  = ifo.fromBigEndian<quint16>(cellPosEntry);
        cell->_originalCellId = ifo[cellPosEntry + 3];

        // Loop on all cells in "Cell Address Table" (VTS_C_ADT).
        // We collect all ranges of sectors with the right original VID Id / Cell Id.
        foreach (const QtlDvdOriginalCellPtr& originalCell, originalCells) {
            if (originalCell->originalVobId() == cell->originalVobId() && originalCell->originalCellId() == cell->originalCellId()) {
                cell->_sectors.append(originalCell->sectors());
            }
        }

        // Reduce the list of sectors in the cell.
        cell->_sectors.merge(Qtl::AdjacentOnly);
    }

    // Loop on "Program Map Table" to get the list of chapters.
    QtlDvdProgramCellList::ConstIterator nextCell(_cells.begin());
    int nextCellId = ifo[pmapStart];  // entry cell id of next chapter.
    for (int pgmIndex = 0; pgmIndex < programCount; ++pgmIndex) {

        const QtlDvdProgramChapterPtr chapter(new QtlDvdProgramChapter(pgmIndex + 1));
        _chapters.append(chapter);

        nextCellId = pgmIndex < programCount - 1 ? ifo[pmapStart + pgmIndex + 1] : cellCount + 1;
        while (nextCell != _cells.end() && (*nextCell)->cellId() < nextCellId) {
            chapter->_cells.append(*nextCell);
            ++nextCell;
        }
    }

    // Now we can say the PGC is valid.
    _valid = true;
}


//----------------------------------------------------------------------------
// Get the total number of sectors in all cells.
//----------------------------------------------------------------------------

int QtlDvdProgramChain::totalSectorCount() const
{
    int count = 0;
    foreach (const QtlDvdProgramCellPtr& cell, _cells) {
        if (!cell.isNull()) {
            count += cell->sectors().totalValueCount();
        }
    }
    return count;
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
