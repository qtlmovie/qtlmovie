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
// Command line tool to test IFO files on DVD.
//
//----------------------------------------------------------------------------

#include "TestToolCommand.h"
#include "QtlRangeList.h"
#include "QtlFile.h"
#include "QtlSmartPointer.h"

#define DVD_SECTOR_SIZE 2048

//----------------------------------------------------------------------------

class VobFile
{
public:
    QFile     file;
    QFileInfo info;
    QtlRange  sectors;

    VobFile(const QString& fileName = QString(), int first = 0) :
        file(fileName),
        info(fileName),
        sectors(first, first + (info.size() / DVD_SECTOR_SIZE) - 1)
    {
    }
};

typedef QtlSmartPointer<VobFile,QtlNullMutexLocker> VobFilePtr;
typedef QList<VobFilePtr> VobFileList;

//----------------------------------------------------------------------------

class OriginalCell // a cell inside the original input VOB's, before DVD production
{
public:
    int      originalVobId;
    int      originalCellId;
    QtlRange sectors;

    OriginalCell() :
        originalVobId(0),
        originalCellId(0),
        sectors()
    {
    }
};

typedef QtlSmartPointer<OriginalCell,QtlNullMutexLocker> OriginalCellPtr;
typedef QList<OriginalCellPtr> OriginalCellList;

//----------------------------------------------------------------------------

class PgcCell // a cell inside a PGC in the final DVD
{
public:
    const int    cellId;   // within PGC
    int          angleId;  // 0 means common content, for all angles.
    int          originalVobId;
    int          originalCellId;
    int          durationSeconds;
    QString      durationString;
    QtlRangeList sectors;  // not all are used, check VOB & cell ids in navigation packs

    PgcCell(int id = 0) :
        cellId(id),
        angleId(0),
        originalVobId(0),
        originalCellId(0),
        durationSeconds(0),
        durationString(),
        sectors()
    {
    }
};

typedef QtlSmartPointer<PgcCell,QtlNullMutexLocker> PgcCellPtr;
typedef QList<PgcCellPtr> PgcCellList;

//----------------------------------------------------------------------------

class PgcChapter // a chapter inside a PGC (aka "program", "part of a title", "PTT")
{
public:
    const int   chapterId;
    PgcCellList cells;

    PgcChapter(int id = 0) :
        chapterId(id),
        cells()
    {
    }
};

typedef QtlSmartPointer<PgcChapter,QtlNullMutexLocker> PgcChapterPtr;
typedef QList<PgcChapterPtr> PgcChapterList;

//----------------------------------------------------------------------------

class Pgc
{
public:
    const int      pgcId;
    int            durationSeconds;
    QString        durationString;
    int            nextPgc;
    int            previousPgc;
    int            groupPgc;
    QtlByteBlock   yuvPalette;
    PgcCellList    cells;
    PgcChapterList chapters;
    QtlRangeList   sectors;  // not all are used, check VOB & cell ids in navigation packs

    Pgc(int id = 0) :
        pgcId(id),
        durationSeconds(0),
        durationString(),
        nextPgc(0),
        previousPgc(0),
        groupPgc(0),
        yuvPalette(),
        cells(),
        chapters(),
        sectors()
    {
    }
};

typedef QtlSmartPointer<Pgc,QtlNullMutexLocker> PgcPtr;
typedef QList<PgcPtr> PgcList;

//----------------------------------------------------------------------------

class TestIfoDump : public TestToolCommand
{
    Q_OBJECT

public:
    TestIfoDump() : TestToolCommand
                    ("ifodump",
                     "ifo-file-name [demuxed-file [pgc# [angle#]]]",
                     "Dump a DVD IFO file. This is formatted dump from raw IFO content, it does not\n"
                     "use the QtsDvd classes. This is prototype code to test the interpretation of\n"
                     "DVD content. If demuxed-file is present, also demux one PGC/angle.") {}
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;

private:
    bool loadIfo();
    void displayIfo();
    void displayAudioVideoAttributes();
    bool demuxPgc(int pgcNumber, int angleNumber);
    void resetRead();
    bool readSector(QtlByteBlock& buffer, int sector);

    static int playbackSeconds(quint32 value);
    static QString playbackString(quint32 value);
    static QString paletteString(const QtlByteBlock& data);

    template<typename INT> void displayInt(int index, const QString& name)
    {
        const INT x = _ifo.fromBigEndian<INT>(index);
        out << name << ": 0x" << hex << x << dec << " (" << x << ")" << endl;
    }

    QString          _ifoFileName;
    QString          _demuxFileName;
    QtlByteBlock     _ifo;
    qint64           _totalVobSectors;
    int              _originalVobCount;
    VobFileList      _vobList;
    PgcList          _pgcList;
    OriginalCellList _originalCells;
    VobFilePtr       _currentVob;
    int              _nextSector;
};

//----------------------------------------------------------------------------

int TestIfoDump::run(const QStringList& args)
{
    // Decode command line arguments.
    if (args.size() < 1 || args.size() > 4) {
        return syntaxError();
    }
    _ifoFileName = args[0];
    _demuxFileName = args.value(1, "");
    const int pgcNumber = args.value(2, "1").toInt();
    const int angleNumber = args.value(3, "1").toInt();

    if (!loadIfo()) {
        return EXIT_FAILURE;
    }

    if (_demuxFileName.isEmpty()) {
        displayIfo();
    }
    else {
        demuxPgc(pgcNumber, angleNumber);
    }

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

bool TestIfoDump::loadIfo()
{
    // Read IFO file content.
    _ifo = QtlFile::readBinaryFile(_ifoFileName);
    if (_ifo.isEmpty()) {
        err << "Error reading " << _ifoFileName << endl;
        return false;
    }
    if (_ifo.getLatin1(0, 12) != "DVDVIDEO-VTS") {
        err << "Not a valid IFO file for VTS, incorrect header" << endl;
        return false;
    }
    if (_ifo.size() < 0x3D8) {
        err << "Not a valid IFO file, too small" << endl;
        return false;
    }

    // Locate VOB files.
    const QString ifoSuffix(QStringLiteral("0.IFO"));
    _totalVobSectors = 0;
    if (_ifoFileName.endsWith(ifoSuffix, Qt::CaseInsensitive)) {
        QString prefix(_ifoFileName);
        prefix.chop(ifoSuffix.size());
        for (int i = 1; i < 10; ++i) {
            const VobFilePtr vob(new VobFile(prefix + QString::number(i) + ".VOB", _totalVobSectors));
            if (!vob->info.exists()) {
                break;
            }
            _totalVobSectors += vob->info.size() / DVD_SECTOR_SIZE;
            _vobList.append(vob);
        }
    }

    // Load Program Chains (PGC).
    // IFO offset of Program Chain Table (VTS_PGCI):
    const int pgciStart = _ifo.fromBigEndian<quint32>(0x00CC) * DVD_SECTOR_SIZE;
    const int pgciHeaderSize = 8;
    const int pgciEntrySize = 8;
    if (pgciStart + pgciHeaderSize > _ifo.size()) {
        err << "Invalid Program Chain Table (VTS_PGCI)" << endl;
        return false;
    }
    const int pgcCount = _ifo.fromBigEndian<quint16>(pgciStart);
    const int pgciEnd = pgciStart + pgciHeaderSize + pgcCount * pgciEntrySize;
    const int pgcEnd = pgciStart + _ifo.fromBigEndian<quint32>(pgciStart + 4) + 1;
    if (pgciEnd > pgcEnd || pgcEnd > _ifo.size()) {
        err << "Invalid Program Chain Table (VTS_PGCI)" << endl;
        return false;
    }

    // IFO offset of Cell Address Table (VTS_C_ADT):
    const int cadtStart = _ifo.fromBigEndian<quint32>(0x00E0) * DVD_SECTOR_SIZE;
    const int cadtHeaderSize = 8;
    const int cadtEntrySize = 12;
    if (cadtStart + cadtHeaderSize > _ifo.size()) {
        err << "Invalid Cell Address Table (VTS_C_ADT)" << endl;
        return false;
    }
    _originalVobCount = _ifo.fromBigEndian<quint16>(cadtStart);
    const int cadtEnd = cadtStart + _ifo.fromBigEndian<quint32>(cadtStart + 4) + 1;
    if (cadtEnd > _ifo.size()) {
        err << "Invalid Cell Address Table (VTS_C_ADT)" << endl;
        return false;
    }

    // Loop on all cells in "Cell Address Table" (VTS_C_ADT).
    // This is a global VTS table, not PGC-specific.
    // The "cells" in VTS_C_ADT are different from the "cells" in the PGC.
    for (int cadtIndex = cadtStart + cadtHeaderSize; cadtIndex < cadtEnd; cadtIndex += cadtEntrySize) {
        const OriginalCellPtr cell(new OriginalCell);
        _originalCells.append(cell);
        cell->originalVobId = _ifo.fromBigEndian<quint16>(cadtIndex);
        cell->originalCellId = _ifo[cadtIndex + 2];
        cell->sectors.setFirst(_ifo.fromBigEndian<quint32>(cadtIndex + 4));
        cell->sectors.setLast(_ifo.fromBigEndian<quint32>(cadtIndex + 8));
    }

    // Loop on all PGC descriptions.
    for (int pgcIndex = 0; pgcIndex < pgcCount; ++pgcIndex) {

        const int index = pgciStart + pgciHeaderSize + pgcIndex * pgciEntrySize;
        if ((_ifo[index] & 0x80) == 0) {
            // Unused entry in PGCI.
            continue;
        }

        // Valid PGCI entry.
        const int titleNumber = _ifo[index] & 0x7F;
        const int pgcStart = pgciStart + _ifo.fromBigEndian<quint32>(index + 4);
        if (pgcStart + 0x00EC > _ifo.size()) {
            err << "Invalid PGC entry" << endl;
            return false;
        }
        const PgcPtr pgc(new Pgc(titleNumber));
        _pgcList.append(pgc);

        // Number of chapters and cells in the PGC.
        const int chapterCount = _ifo[pgcStart + 0x0002]; // "number of programs"
        const int cellCount = _ifo[pgcStart + 0x0003];

        // PGC playback duration.
        const int duration = _ifo.fromBigEndian<quint32>(pgcStart + 0x0004);
        pgc->durationSeconds = playbackSeconds(duration);
        pgc->durationString = playbackString(duration);

        // PGC sequence.
        pgc->nextPgc = _ifo.fromBigEndian<quint16>(pgcStart + 0x009C);
        pgc->previousPgc = _ifo.fromBigEndian<quint16>(pgcStart + 0x009E);
        pgc->groupPgc = _ifo.fromBigEndian<quint16>(pgcStart + 0x00A0);

        // Color lookup table.
        pgc->yuvPalette = _ifo.mid(pgcStart + 0x00A4, 16 * 4);

        // Locate the "Program Map Table" (only one byte per entry).
        const int pmaptStart = pgcStart + _ifo.fromBigEndian<quint16>(pgcStart + 0x00E6);
        if (pmaptStart + chapterCount > _ifo.size()) {
            err << "Invalid Program Map Table" << endl;
            return false;
        }

        // Locate the "Cell Playback Information Table"
        const int cpbktStart = pgcStart + _ifo.fromBigEndian<quint16>(pgcStart + 0x00E8);
        const int cpbktEntrySize = 0x0018;
        if (cpbktStart + cellCount * cpbktEntrySize > _ifo.size()) {
            err << "Invalid Cell Playback Information Table" << endl;
            return false;
        }

        // Locate the "Cell Position Information Table"
        const int cpostStart = pgcStart + _ifo.fromBigEndian<quint16>(pgcStart + 0x00EA);
        const int cpostEntrySize = 0x0004;
        if (cpostStart + cellCount * cpostEntrySize > _ifo.size()) {
            err << "Invalid Cell Position Information Table" << endl;
            return false;
        }

        // Angle in current cell (0 means normal, for all angles).
        int currentAngle = 0;

        // Loop on all cells.
        for (int cellIndex = 0; cellIndex < cellCount; ++cellIndex) {

            const PgcCellPtr cell(new PgcCell(cellIndex + 1));
            pgc->cells.append(cell);

            // Locate entry in "Cell Playback Information Table"
            const int cpbktEntry = cpbktStart + cellIndex * cpbktEntrySize;

            // Compute angle id from cell category:
            // Bits 7-6: cell type 00=normal, 01=first of angle block, 10=middle of angle block, 11=last of angle block
            // Bits 5-4: block type 00 = normal, 01 = angle block
            const int category = _ifo[cpbktEntry] & 0xF0;
            if (category == 0x50) {
                // 0101 : first angle.
                currentAngle = 1;
            }
            else if ((category == 0x90 || category == 0xD0) && currentAngle != 0) {
                // 1001 : middle angle or 1101 : last angle => next angle in both cases.
                currentAngle++;
            }
            // If category == 0 ("normal"), this is common content.
            cell->angleId = category == 0 ? 0 : currentAngle;
            if (category == 0xD0) {
                // 1101 : last angle => reset angle count.
                currentAngle = 0;
            }

            // Cell playback duration.
            const int duration = _ifo.fromBigEndian<quint32>(cpbktEntry + 4);
            cell->durationSeconds = playbackSeconds(duration);
            cell->durationString = playbackString(duration);

            // Locate entry in "Cell Positon Information Table"
            const int cpostEntry = cpostStart + cellIndex * cpostEntrySize;

            // Get original VOB Id and original Cell Id.
            cell->originalVobId = _ifo.fromBigEndian<quint16>(cpostEntry);
            cell->originalCellId = _ifo[cpostEntry + 3];

            // Loop on all cells in "Cell Address Table" (VTS_C_ADT).
            // We collect all ranges of sectors with the right original VID Id / Cell Id.
            foreach (const OriginalCellPtr& originalCell, _originalCells) {
                if (originalCell->originalVobId == cell->originalVobId && originalCell->originalCellId == cell->originalCellId) {
                    cell->sectors.append(originalCell->sectors);
                    pgc->sectors.append(originalCell->sectors);
                }
            }

            // Reduce the list of sectors in the cell.
            cell->sectors.merge(Qtl::AdjacentOnly);
        }

        // Reduce the list of sectors in the PGC.
        pgc->sectors.merge(Qtl::AdjacentOnly);

        // Loop on "Program Map Table" to get the list of chapters.
        PgcCellList::ConstIterator nextCell(pgc->cells.begin());
        int nextCellId = _ifo[pmaptStart];  // entry cell id of next chapter.
        for (int pgmIndex = 0; pgmIndex < chapterCount; ++pgmIndex) {

            const PgcChapterPtr chapter(new PgcChapter(pgmIndex + 1));
            pgc->chapters.append(chapter);

            nextCellId = pgmIndex < chapterCount - 1 ? _ifo[pmaptStart + pgmIndex + 1] : cellCount + 1;
            while (nextCell != pgc->cells.end() && (*nextCell)->cellId < nextCellId) {
                chapter->cells.append(*nextCell);
                ++nextCell;
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------

void TestIfoDump::displayIfo()
{
    // List files.
    out << QFileInfo(_ifoFileName).fileName() << " : " << (_ifo.size() / DVD_SECTOR_SIZE) << " sectors, " << _ifo.size() << " bytes" << endl;
    foreach (const VobFilePtr& vob, _vobList) {
        out << vob->info.fileName() << " : " << (vob->info.size() / DVD_SECTOR_SIZE) << " sectors, " << vob->info.size() << " bytes" << endl;
    }
    out << "Total VOB size : " << _totalVobSectors << " sectors, " << (_totalVobSectors * DVD_SECTOR_SIZE) << " bytes" << endl;

    // Pointers into IFO.
    displayInt<quint32>(0x000C, "Last sector of VTS");
    displayInt<quint32>(0x001C, "Last sector of IFO");
    out << "Version: " << QString::number(_ifo[0x21] >> 4) << "." << QString::number(_ifo[0x21] & 0x0F) << endl;
    displayInt<quint32>(0x0022, "VTS category");
    displayInt<quint32>(0x0080, "End byte address of VTS_MAT");
    displayInt<quint32>(0x00C0, "Start sector of menu VOB");
    displayInt<quint32>(0x00C4, "Start sector of title VOB");
    displayInt<quint32>(0x00C8, "Sector pointer to VTS_PTT_SRPT");
    displayInt<quint32>(0x00CC, "Sector pointer to VTS_PGCI");
    displayInt<quint32>(0x00D0, "Sector pointer to VTSM_PGCI_UT");
    displayInt<quint32>(0x00D4, "Sector pointer to VTS_TMAPTI");
    displayInt<quint32>(0x00D8, "Sector pointer to VTSM_C_ADT");
    displayInt<quint32>(0x00DC, "Sector pointer to VTSM_VOBU_ADMAP");
    displayInt<quint32>(0x00E0, "Sector pointer to VTS_C_ADT");
    displayInt<quint32>(0x00E4, "Sector pointer to VTS_VOBU_ADMAP");

    displayAudioVideoAttributes();

    // Display PGC's.
    out << endl
        << "Number of PGC's: " << _pgcList.size() << endl;
    foreach (const PgcPtr& pgc, _pgcList) {
        out << "  PGC #" << pgc->pgcId << endl
            << "    Next PGC: " << pgc->nextPgc
            << ", previous PGC: " << pgc->previousPgc
            << ", group PGC: " << pgc->groupPgc << endl
            << "    Duration: " << pgc->durationSeconds << " seconds (" << pgc->durationString << ")" << endl
            << "    Palette (YUV): " << paletteString(pgc->yuvPalette) << endl
            << "    Number of sectors: " << pgc->sectors.totalValueCount() << endl
            << "    Number of chapters: " << pgc->chapters.size() << endl;

        foreach (const PgcChapterPtr& chapter, pgc->chapters) {
            out << "      Chapter #" << chapter->chapterId << ": cells";
            foreach (const PgcCellPtr& cell, chapter->cells) {
                out << " " << cell->cellId;
            }
            out << endl;
        }

        out << "    Number of cells: " << pgc->cells.size() << endl;
        foreach (const PgcCellPtr& cell, pgc->cells) {
            out << "      Cell #"  << cell->cellId
                << ": VOB=" << cell->originalVobId
                << ", cell=" << cell->originalCellId
                << ", angle=" << cell->angleId
                << ", duration: " << cell->durationSeconds << " seconds (" << cell->durationString << ")"
                << ", sectors: " << cell->sectors.totalValueCount()
                << " in " << cell->sectors.enclosing() << endl;
        }
    }

    // Display ADT (original cells addresses).
    out << endl
        << "Number of original VOB's: " << _originalVobCount << endl
        << "Number of original cells: " << _originalCells.size() << endl;
    int cellIndex = 0;
    foreach (const OriginalCellPtr& cell, _originalCells) {
        out << "  " << ++cellIndex
            << ": VOB=" << cell->originalVobId
            << ", cell=" << cell->originalCellId
            << ", sectors=" << cell->sectors << endl;
    }
}

//----------------------------------------------------------------------------

void TestIfoDump::displayAudioVideoAttributes()
{
    const quint8 vid0 = _ifo[0x0200];
    const quint8 vid1 = _ifo[0x0201];
    const int audioCount = _ifo.fromBigEndian<quint16>(0x202);
    const int audioIndex = 0x204;
    const int subCount = _ifo.fromBigEndian<quint16>(0x254);
    const int subIndex = 0x256;

    out << "Video attributes: ";
    switch ((vid0 >> 4) & 0x03) {
        case 0:  out << "NTSC, "; break;
        case 1:  out << "PAL, "; break;
        default: out << "Standard=" << ((vid0 >> 4) & 0x03) << ", "; break;
    }
    const bool ntsc = ((vid0 >> 4) & 0x03) == 0;
    switch ((vid0 >> 2) & 0x03) {
        case 0:  out << "4:3, "; break;
        case 3:  out << "16:9, "; break;
        default: out << "Aspect=" << ((vid0 >> 2) & 0x03) << ", "; break;
    }
    switch ((vid1 >> 3) & 0x07) {
        case 0:  out << (ntsc ? "720x480" : "720x576"); break;
        case 1:  out << (ntsc ? "704x480" : "704x576"); break;
        case 2:  out << (ntsc ? "352x480" : "352x576"); break;
        case 3:  out << (ntsc ? "352x240" : "352x288"); break;
        default: out << "Resolution=" << ((vid1 >> 3) & 0x07) << ", "; break;
    }
    out << endl;

    out << "Number of audio streams: " << audioCount << endl;
    for (int i = 0; i < audioCount; ++i) {
        const int index = audioIndex + 8 * i;
        const quint8 codingMode = _ifo[index] >> 5;
        const int channelCount = int(_ifo[index + 1] & 0x07) + 1;
        const bool languagePresent = (_ifo[index] & 0x0C) == 0x04;
        const quint8 type = _ifo[index + 5];
        // For audio, the stream ids depend on the codec.
        // Using n as index of audio stream, the stream id is;
        // AC-3 = 0x80 + n, DTS  = 0x88 + n, SDDS = 0x90 + n, LPCM = 0xA0 + n, MP2  = 0xC0 + n, MPEG2-ext = 0xC8 + n
        out << "  Audio #" << i << ": ";
        switch (codingMode) {
            case 0: out << "id 0x" << hex << (0x80 + i) << ", AC-3"; break;
            case 2: out << "id 0x" << hex << (0xC0 + i) << ", MPEG-1"; break;
            case 3: out << "id 0x" << hex << (0xC8 + i) << ", MPEG-2"; break;
            case 4: out << "id 0x" << hex << (0xA0 + i) << ", LPCM"; break;
            case 6: out << "id 0x" << hex << (0x88 + i) << ", DTS"; break;
        }
        out << ", " << dec << channelCount << " channels";
        switch (type) {
            case 1: out << ", normal"; break;
            case 2: out << ", visually impaired"; break;
            case 3: out << ", director comments"; break;
            case 4: out << ", alternate director comments"; break;
        }
        if (languagePresent) {
            out << ", lang=" << _ifo.getLatin1(index + 2, 2);
        }
        out << endl;
    }

    out << "Number of subpicture streams: " << subCount << endl;
    for (int i = 0; i < subCount; ++i) {
        const int index = subIndex + 6 * i;
        const bool languagePresent = (_ifo[index] & 0x03) == 0x01;
        const quint8 type = _ifo[index + 5];
        // For DVD subpictures (=subtitle), the stream id is always 0x20 + index of stream
        out << "  Subpicture #" << i << ": id 0x" << hex << (0x20 + i) << dec << ", ";
        switch (type) {
            case 0: out << "not specified"; break;
            case 1: out << "normal"; break;
            case 2: out << "large"; break;
            case 3: out << "children"; break;
            case 5: out << "normal captions"; break;
            case 6: out << "large captions"; break;
            case 7: out << "children captions"; break;
            case 9: out << "forced"; break;
            case 13: out << "director comments"; break;
            case 14: out << "large director comments"; break;
            case 15: out << "director comments for children"; break;
            default: out << "type=" << type; break;
        }
        if (languagePresent) {
            out << ", lang=" << _ifo.getLatin1(index + 2, 2);
        }
        out << endl;
    }
}

//----------------------------------------------------------------------------

bool TestIfoDump::demuxPgc(int pgcNumber, int angleNumber)
{
    // PGC and Angles are numbered from 1 onwards.
    if (pgcNumber < 1 || pgcNumber > _pgcList.size()) {
        err << "Invalid PGC# " << pgcNumber << endl;
        return false;
    }
    if (angleNumber < 1) {
        err << "Invalid angle# " << angleNumber << endl;
        return false;
    }

    const PgcPtr pgc(_pgcList[pgcNumber - 1]);
    out << "Demuxing PGC #" << pgc->pgcId << endl;

    resetRead();
    QtlByteBlock buffer;

    QFile outFile(_demuxFileName);
    if (!outFile.open(QFile::WriteOnly)) {
        err << "Error creating " << _demuxFileName << endl;
        return false;
    }

    foreach (const PgcCellPtr& cell, pgc->cells) {
        if (cell->angleId != 0 && cell->angleId != angleNumber) {
            out << "Skipping cell #" << cell->cellId << endl;
        }
        else {
            out << "Demuxing cell #" << cell->cellId << endl;
            foreach (const QtlRange& sectors, cell->sectors) {

                out << "  Demuxing sectors " << sectors << endl;
                bool inPgcContent = true;
                int firstSkipped = -1;

                for (int sector = sectors.first(); sector <= sectors.last(); ++sector) {
                    if (!readSector(buffer, sector) || buffer.size() != DVD_SECTOR_SIZE) {
                        return false;
                    }

                    // A VOB sector is an MPEG-2 program stream pack (ISO 13818-1, §2.5.3.3).
                    // The pack header is 14 bytes long, followed by a PES packet.
                    // Check that the start code is correct.
                    if (buffer.fromBigEndian<quint32>(0) != 0x000001BA) {
                        err << "Invalid pack start code at sector " << sector << endl;
                        return false;
                    }

                    // A navigation pack has the following format:
                    // 0000: Pack Header:
                    //       14 bytes, start code 0x000001BA
                    //       See ISO 13818-1, §2.5.3.3
                    // 000E: System Header:
                    //       24 bytes, start code 0x000001BB
                    //       See ISO 13818-1, §2.5.3.5
                    // 0026: PCI (Presentation Control Information) packet:
                    //       986 bytes, start code 0x000001BF (private stream 2), substream id 0x00 (PCI)
                    //       See http://stnsoft.com/DVD/pci_pkt.html
                    // 0400: DSI (Data Search Information) packet:
                    //       1024 bytes, start code 0x000001BF (private stream 2), substream id 0x01 (DSI)
                    //       See http://stnsoft.com/DVD/dsi_pkt.html

                    if (buffer.fromBigEndian<quint32>(0x000E) == 0x000001BB &&  // system header
                        buffer.fromBigEndian<quint32>(0x0400) == 0x000001BF &&  // private stream 2
                        buffer[0x406] == 0x01) // substream id for DSI
                    {
                        // This is a navigation pack. The VOBU is made of this navpack and all subsequents
                        // packs (ie "sectors") up to, but not including, the next navpack.
                        // Check if this VOBU belongs to our target VOB and cell ids.
                        inPgcContent =
                                buffer.fromBigEndian<quint16>(0x041F) == cell->originalVobId &&
                                buffer[0x422] == cell->originalCellId;
                    }

                    // Keep track of segments we skip.
                    if (!inPgcContent && firstSkipped < 0) {
                        firstSkipped = sector;
                    }
                    else if (inPgcContent && firstSkipped >= 0) {
                        out << "  **** Ignored sectors " << QtlRange(firstSkipped, sector - 1) << endl;
                        firstSkipped = -1;
                    }

                    // Write sectors which are part of the PGC content.
                    if (inPgcContent && outFile.write((char*)(buffer.data()), buffer.size()) != buffer.size()) {
                        err << "Error writing " << _demuxFileName << endl;
                        return false;
                    }
                }

                if (firstSkipped >= 0) {
                    out << "  **** Ignored sectors " << QtlRange(firstSkipped, sectors.last()) << endl;
                }
            }
        }
    }

    outFile.close();
    return true;
}

//----------------------------------------------------------------------------

void TestIfoDump::resetRead()
{
    _currentVob.clear();
    _nextSector = -1;
}

//----------------------------------------------------------------------------

bool TestIfoDump::readSector(QtlByteBlock& buffer, int sector)
{
    if (sector < 0) {
        err << "Invalid sector " << sector << endl;
        return false;
    }

    if (sector != _nextSector || _currentVob.isNull() || sector > _currentVob->sectors.last()) {
        // Need to seek somewhere else.
        _currentVob.clear();
        foreach (const VobFilePtr& vob, _vobList) {
            if (vob->sectors.contains(sector)) {
                _currentVob = vob;
                break;
            }
        }
        if (_currentVob.isNull()) {
            err << "Sector " << sector << " not found in VOB files" << endl;
            return false;
        }
        if (!_currentVob->file.isOpen() && !_currentVob->file.open(QFile::ReadOnly)) {
            err << "Error opening " << _currentVob->file.fileName() << endl;
            return false;
        }
        if (!_currentVob->file.seek((sector - _currentVob->sectors.first()) * DVD_SECTOR_SIZE)) {
            err << "Error seeking " << _currentVob->file.fileName() << endl;
            return false;
        }
    }

    buffer.resize(DVD_SECTOR_SIZE);
    if (_currentVob->file.read((char*)(buffer.data()), DVD_SECTOR_SIZE) != DVD_SECTOR_SIZE) {
        err << "Error reading " << _currentVob->file.fileName() << endl;
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------

int TestIfoDump::playbackSeconds(quint32 value)
{
    // The playback time is encoded in BCD as: hh:mm:ss:ff (ff = frame count within second).
    // With 2 MSBits of ff indicating frame rate: 11 = 30 fps, 10 = illegal, 01 = 25 fps, 00 = illegal.

    const int hours   = (((value >> 28) & 0x0F) * 10) + ((value >> 24) & 0x0F);
    const int minutes = (((value >> 20) & 0x0F) * 10) + ((value >> 16) & 0x0F);
    const int seconds = (((value >> 12) & 0x0F) * 10) + ((value >>  8) & 0x0F);

    return (hours * 3600) + (minutes * 60) + seconds;
}

//----------------------------------------------------------------------------

QString TestIfoDump::playbackString(quint32 value)
{
    // The playback time is encoded in BCD as: hh:mm:ss:ff (ff = frame count within second)
    // With 2 MSBits of ff indicating frame rate: 11 = 30 fps, 10 = illegal, 01 = 25 fps, 00 = illegal

    const int hours   = (((value >> 28) & 0x0F) * 10) + ((value >> 24) & 0x0F);
    const int minutes = (((value >> 20) & 0x0F) * 10) + ((value >> 16) & 0x0F);
    const int seconds = (((value >> 12) & 0x0F) * 10) + ((value >>  8) & 0x0F);
    const int frame   = (((value >>  4) & 0x03) * 10) + (value & 0x0F);
    const int rate    = (value >> 6) & 0x03;

    QString result(QStringLiteral("%1:%2:%3.f%4").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')).arg(frame, 2, 10, QChar('0')));
    switch (rate) {
        case 1:
            result.append(" @25 fps");
            break;
        case 3:
            result.append(" @30 fps");
            break;
        default:
            result.append(QStringLiteral(" (invalid frame rate %1)").arg(rate));
            break;
    }
    return result;
}

//----------------------------------------------------------------------------

QString TestIfoDump::paletteString(const QtlByteBlock& data)
{
    QString result;
    for (int i = 0; i < data.size(); ++i) {
        if (i % 4 == 0 && i > 0) {
            result.append(",");
        }
        result.append(QStringLiteral("%1").arg(int(data[i]), 2, 16, QChar('0')));
    }
    return result;
}

//----------------------------------------------------------------------------

#include "TestIfoDump.moc"
namespace {TestIfoDump thisTest;}
