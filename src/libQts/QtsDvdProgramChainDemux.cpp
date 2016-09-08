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
// Qts, the Qt MPEG Transport Stream library.
// Define the class QtsDvdProgramChainDemux.
//
//----------------------------------------------------------------------------

#include "QtsDvdProgramChainDemux.h"


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtsDvdProgramChainDemux::QtsDvdProgramChainDemux(const QtsDvdTitleSet& vts,
                                                 int pgcNumber,
                                                 int angleNumber,
                                                 int fallbackPgcNumber,
                                                 int transferSize,
                                                 int minBufferSize,
                                                 Qts::DvdDemuxPolicy demuxPolicy,
                                                 QtlLogger* logger,
                                                 QObject* parent,
                                                 bool useMaxReadSpeed) :
    QtlDataPull(minBufferSize, logger, parent),
    _vts(vts),
    _pgc(_vts.title(pgcNumber)),
    _angleNumber(angleNumber),
    _demuxPolicy(demuxPolicy),
    _sectorChunk(qMax(1, transferSize / QTS_DVD_SECTOR_SIZE)),
    _maxReadSpeed(useMaxReadSpeed),
    _vobStartSector(-1),
    _buffer(_sectorChunk * QTS_DVD_SECTOR_SIZE),
    _inputSectors(log()),
    _dvd(QString(), log()),
    _vobs(_vts.vobFileNames(), log()),
    _report(30000, log()),  // report transfer bandwidth every 30 seconds.
    _inPgcContent(false),
    _writtenSectors(0)
{
    // If the specified PGC does not exist, use the fallback one.
    if (_pgc.isNull()) {
        _pgc = _vts.title(fallbackPgcNumber);
    }

    // Enable progress report.
    if (!_pgc.isNull()) {
        // Set total transfer size in bytes. The actual total size may be slightly
        // smaller, but this is just a hint.
        setProgressMaxHint(qint64(_pgc->totalSectorCount()) * QTS_DVD_SECTOR_SIZE);

        // Set progress interval: every 1 MB.
        setProgressIntervalInBytes(1024 * 1024);
    }
}


//----------------------------------------------------------------------------
// Initialize the transfer.
//----------------------------------------------------------------------------

bool QtsDvdProgramChainDemux::initializeTransfer()
{
    // Filter invalid initial parameters.
    if (_pgc.isNull()) {
        log()->line(tr("Program Chain (PGC) not found in DVD title set"));
        return false;
    }
    if (_angleNumber < 1) {
        log()->line(tr("Invalid angle #%1 in DVD content").arg(_angleNumber));
        return false;
    }

    // Initialize the list of cells to demux.
    _inputSectors.initialize(_pgc);

    // By default, we assume that we read sectors from the PGC content.
    // Later, if we encounter a navigation pack with the wrong original
    // VOB/cell ids, it will be set to false.
    _inPgcContent = true;
    _writtenSectors = 0;

    // Open the input media.
    if (_vts.isEncrypted()) {
        // The content is on an encrypted DVD, use a QtsDvdMedia object.
        // Locate the first VOB. Its first sector is "sector zero" in cells' lists of sectors.
        _vobStartSector = _vts.vobStartSector();
        if (_vobStartSector < 0) {
            log()->line(tr("Cannot locate VOB files on DVD media"));
            return false;
        }
        if (!_dvd.openFromDevice(_vts.deviceName(), _maxReadSpeed)) {
            return false;
        }

        // Start bandwidth reporting (do that only on DVD media, not on files).
        _report.start();
    }

    return true;
}


//----------------------------------------------------------------------------
// Invoked when more data is needed.
//----------------------------------------------------------------------------

bool QtsDvdProgramChainDemux::needTransfer(qint64 maxSize)
{
    // Get next sector to read.
    const int sectorAddress = _inputSectors.currentSectorAddress();

    // Is the transfer completed?
    if (sectorAddress < 0) {
        close();
        return true;
    }

    // Maximum number of contiguous sectors to read.
    int sectorCount = qMin(_sectorChunk, _inputSectors.currentSectorCount());
    if (maxSize >= 0) {
        sectorCount = qMin(sectorCount, int(maxSize / QTS_DVD_SECTOR_SIZE));
    }
    if (sectorCount <= 0) {
        return sectorCount;
    }

    // Read sectors.
    if (_vobStartSector >= 0) {
        // Reading DVD media. Compute logical sector address on DVD media.
        const int lba = _vobStartSector + sectorAddress;
        // Seek (if required) and read.
        sectorCount = _dvd.readSectors(_buffer.data(), sectorCount, (lba == _dvd.nextSector() ? -1 : lba), Qts::SkipBadSectors);
    }
    else {
        // Reading VOB files.
        sectorCount = _vobs.read(sectorAddress, _buffer.data(), sectorCount);
    }

    // Read status?
    bool success = sectorCount > 0;

    if (success) {
        // Successful read. Report transfered data size.
        _report.transfered(sectorCount);

        // Demux buffer content.
        success = demuxBuffer(sectorCount);

        // Move forward within input list of sectors.
        _inputSectors.advance(sectorCount);
    }

    return success;
}


//----------------------------------------------------------------------------
// Cleanup the transfer.
//----------------------------------------------------------------------------

void QtsDvdProgramChainDemux::cleanupTransfer(bool clean)
{
    // Final bandwidth report.
    _report.reportBandwidth();

    // Close files and devices.
    if (_vobStartSector >= 0) {
        // Reading DVD media.
        _dvd.close();
    }
    else {
        // Reading VOB files.
        _vobs.close();
    }
}


//----------------------------------------------------------------------------
// Demux the sectors in the buffer, write demuxed sectors to superclass.
//----------------------------------------------------------------------------

bool QtsDvdProgramChainDemux::demuxBuffer(int sectorCount)
{
    // End of read sectors in buffers.
    const int bufferEnd = sectorCount * QTS_DVD_SECTOR_SIZE;
    Q_ASSERT(bufferEnd <= _buffer.size());

    // Original VOB id and cell id in the current cell.
    int vobId = 0;
    int cellId = 0;
    _inputSectors.getCurrentOriginalIds(vobId, cellId);

    // Now process all sectors one by one.
    for (int sectorStart = 0; sectorStart < bufferEnd; sectorStart += QTS_DVD_SECTOR_SIZE) {

        // A VOB sector is an MPEG-2 program stream pack (ISO 13818-1, §2.5.3.3).
        // The pack header is 14 bytes long, followed by a PES packet.

        // Check that the start code is correct.
        if (_buffer.fromBigEndian<quint32>(sectorStart) != 0x000001BA) {
            log()->line(tr("Invalid pack start code encountered"));
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

        // Check if the sector is a navigation pack.
        const bool isNavPack =
                _buffer.fromBigEndian<quint32>(sectorStart + 0x000E) == 0x000001BB &&  // system header
                _buffer.fromBigEndian<quint32>(sectorStart + 0x0400) == 0x000001BF &&  // private stream 2
                _buffer[sectorStart + 0x406] == 0x01;                                  // substream id for DSI

        // A VOBU (VOB Unit) is made of one navigation pack and all subsequents
        // packs (ie "sectors") up to, but not including, the next navigation pack.
        if (isNavPack){
            // Check if this VOBU belongs to our target VOB and cell ids.
            _inPgcContent =
                    _buffer.fromBigEndian<quint16>(sectorStart + 0x041F) == vobId &&
                    _buffer[sectorStart + 0x422] == cellId;

            // Fix navigation pack content if requested.
            if (_demuxPolicy == Qts::NavPacksFixed) {
                // The LBA (Logical Block Address) of the navigation pack is present in the PCI and in the DCI.
                // We must fix both.
                _buffer.storeUInt32(sectorStart + 0x002D, _writtenSectors); // in PCI
                _buffer.storeUInt32(sectorStart + 0x040B, _writtenSectors); // in DSI
            }
        }

        // Write sectors which are part of the PGC content.
        if (_inPgcContent && (!isNavPack || _demuxPolicy != Qts::NavPacksRemoved)) {
            if (!write(&_buffer[sectorStart], QTS_DVD_SECTOR_SIZE)) {
                return false;
            }
            ++_writtenSectors;
        }
    }

    // Success.
    return true;
}


//----------------------------------------------------------------------------
// VobFileSet: Constructor and destructor.
//----------------------------------------------------------------------------

QtsDvdProgramChainDemux::VobFileSet::VobFileSet(const QStringList& vobFileNames, QtlLogger* log) :
    _log(log),
    _files(vobFileNames.size()),
    _current(-1)
{
    int nextSector = 0;
    for (int i = 0; i < vobFileNames.size(); ++i) {
        _files[i] = new File;
        _files[i]->file.setFileName(vobFileNames[i]);
        _files[i]->nextSector = -1;
        _files[i]->sectors.setRange(nextSector, nextSector + (QFileInfo(vobFileNames[i]).size() / QTS_DVD_SECTOR_SIZE) - 1);
        nextSector = _files[i]->sectors.last() + 1;
    }
}

QtsDvdProgramChainDemux::VobFileSet::~VobFileSet()
{
    close();
    for (int i = 0; i < _files.size(); ++i) {
        delete _files[i];
    }
    _files.clear();
    _current = -1;
}


//----------------------------------------------------------------------------
// VobFileSet: Read some contiguous sectors.
//----------------------------------------------------------------------------

int QtsDvdProgramChainDemux::VobFileSet::read(int sectorAddress, void* buffer, int maxSectorCount)
{
    // Does the current VOB contain the sector to read?
    if (_current < 0 || _current >= _files.size() || !_files[_current]->sectors.contains(sectorAddress)) {
        // There is no current VOB or the next sector to read is not in current VOB file.
        // Search the right VOB file:
        for (_current = 0; _current < _files.size() && !_files[_current]->sectors.contains(sectorAddress); ++_current) {
        }
    }

    // If sector not found in any VOB file, abort.
    if (_current < 0 || _current >= _files.size()) {
        _log->line(tr("Sector %1 not found in any VOB file").arg(sectorAddress));
        return -1;
    }

    // Now, we known in which file to read.
    File& vob(*_files[_current]);

    // If not the current sector in the file, need to seek into this file.
    const qint64 position = (sectorAddress - vob.sectors.first()) * QTS_DVD_SECTOR_SIZE;
    if (vob.nextSector != sectorAddress) {
        // Open it first if not yet done.
        if (!vob.file.isOpen() && !vob.file.open(QFile::ReadOnly)) {
            _log->line(tr("Error opening VOB file %1 (%2)")
                       .arg(vob.file.fileName()).arg(vob.file.errorString()));
            return -1;
        }
        if (!vob.file.seek(position)) {
            _log->line(tr("Error seeking VOB file %1 to position %2 (%3)")
                       .arg(vob.file.fileName()).arg(position).arg(vob.file.errorString()));
            return -1;
        }
        vob.nextSector = sectorAddress;
    }

    // Limit the number of sectors to read to the rest of file.
    const int sectorCount = qMin<int>(maxSectorCount, vob.sectors.last() - sectorAddress + 1);
    const qint64 size = qint64(sectorCount) * QTS_DVD_SECTOR_SIZE;

    // Now read the file.
    const qint64 got = vob.file.read((char*)(buffer), size);
    if (got != size) {
        _log->line(tr("Error reading VOB file %1 at position %2, requested %3 bytes, got %4 (%5)")
                   .arg(vob.file.fileName()).arg(position).arg(size).arg(got).arg(vob.file.errorString()));
        return -1;
    }

    // Update current sector in current file.
    vob.nextSector += sectorCount;
    return sectorCount;
}


//----------------------------------------------------------------------------
// VobFileSet: Close all VOB files.
//----------------------------------------------------------------------------

void QtsDvdProgramChainDemux::VobFileSet::close()
{
    for (int i = 0; i < _files.size(); ++i) {
        if (_files[i]->file.isOpen()) {
            _files[i]->file.close();
        }
    }
}


//----------------------------------------------------------------------------
// InputSectors: Initialization.
// Structure of the sectors to read:
//   Level 1: Inside the VOB content
//            List of cells: _cells, index: _currentCell
//   Level 2: Inside each cell)
//            List of ranges of sectors: _ranges, index: _currentRange
//   Level 3: Inside each range of sectors
//            Next sector to read: _currentSector
//----------------------------------------------------------------------------

QtsDvdProgramChainDemux::InputSectors::InputSectors(QtlLogger* log) :
    _log(log),
    _cells(),
    _currentCell(-1),
    _ranges(),
    _currentRange(-1),
    _currentSector(-1)
{
}

void QtsDvdProgramChainDemux::InputSectors::initialize(const QtsDvdProgramChainPtr& pgc)
{
    // Initialize list of cells.
    if (pgc.isNull()) {
        _cells.clear();
    }
    else {
        _cells = pgc->cells();
    }
    _currentCell = -1;

    // Move to the first sector.
    advance(0);
}


//----------------------------------------------------------------------------
// InputSectors: Get current position.
//----------------------------------------------------------------------------

int QtsDvdProgramChainDemux::InputSectors::currentSectorAddress() const
{
    if (_currentCell >= 0 && _currentCell < _cells.size()) {
        Q_ASSERT(_currentRange >= 0);
        Q_ASSERT(_currentRange < _ranges.size());
        Q_ASSERT(_currentSector >= _ranges[_currentRange].first());
        Q_ASSERT(_currentSector <= _ranges[_currentRange].last());
        return _currentSector;
    }
    else {
        return -1;
    }
}

int QtsDvdProgramChainDemux::InputSectors::currentSectorCount() const
{
    if (_currentCell >= 0 && _currentCell < _cells.size()) {
        Q_ASSERT(_currentRange >= 0);
        Q_ASSERT(_currentRange < _ranges.size());
        Q_ASSERT(_currentSector >= _ranges[_currentRange].first());
        Q_ASSERT(_currentSector <= _ranges[_currentRange].last());
        return _ranges[_currentRange].last() - _currentSector + 1;
    }
    else {
        return -1;
    }
}


//----------------------------------------------------------------------------
// InputSectors: Advance current position by a number of sectors.
//----------------------------------------------------------------------------

void QtsDvdProgramChainDemux::InputSectors::advance(int sectorCount)
{
    // No negative "advance", always move forward.
    sectorCount = qMax(0, sectorCount);

    // If initial state.
    if (_currentCell < 0) {
        _currentCell = 0;
        _currentRange = -1;
    }

    // Move forward in cell list the specified number of sectors.
    while (_currentCell < _cells.size()) {

        // Initialize cell.
        if (_currentRange < 0) {
            _log->debug(tr("Demuxing cell #%1").arg(_cells[_currentCell]->cellId()));
            _ranges = _cells[_currentCell]->sectors();
            _currentRange = 0;
            _currentSector = -1;
        }

        // If we reached the last sector range in current cell. Move to next cell.
        if (_currentRange >= _ranges.size()) {
            ++_currentCell;
            _currentRange = -1;
            continue;
        }

        // Initialize range of sectors.
        if (_currentSector < 0) {
            _currentSector = _ranges[_currentRange].first();
        }

        Q_ASSERT(_currentSector >= _ranges[_currentRange].first());
        Q_ASSERT(_currentSector <= _ranges[_currentRange].last());

        // Available sectors in current range.
        const int remain = _ranges[_currentRange].last() - _currentSector + 1;

        // If number of sectors to advance is less than the remaining sectors,
        // we stay in this range and we are done.
        if (sectorCount < remain) {
            _currentSector += sectorCount;
            break;
        }

        // Move to next range.
        sectorCount -= remain;
        ++_currentRange;
        _currentSector = -1;
    }
}


//----------------------------------------------------------------------------
// InputSectors: Get the current original VOB Id and original Cell Id.
//----------------------------------------------------------------------------

void QtsDvdProgramChainDemux::InputSectors::getCurrentOriginalIds(int& vobId, int& cellId) const
{
    if (_currentCell >= 0 && _currentCell < _cells.size() && !_cells[_currentCell].isNull()) {
        vobId = _cells[_currentCell]->originalVobId();
        cellId = _cells[_currentCell]->originalCellId();
    }
    else {
        vobId = cellId = 0;
    }
}
