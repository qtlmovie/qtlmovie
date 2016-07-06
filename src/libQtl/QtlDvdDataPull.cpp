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
// Qtl, Qt utility library.
// Define the class QtlDvdDataPull.
//
//----------------------------------------------------------------------------

#include "QtlDvdDataPull.h"


//----------------------------------------------------------------------------
// Constructor and destructor.
//----------------------------------------------------------------------------

QtlDvdDataPull::QtlDvdDataPull(const QString& deviceName,
                               int startSector,
                               int sectorCount,
                               QtlDvdMedia::BadSectorPolicy badSectorPolicy,
                               int transferSize,
                               int minBufferSize,
                               QtlLogger* log,
                               QObject* parent,
                               bool useMaxReadSpeed) :
    QtlDataPull(minBufferSize, log, parent),
    _deviceName(deviceName),
    _startSector(startSector),
    _endSector(startSector + sectorCount),
    _sectorChunk(qMax(1, transferSize / QtlDvdMedia::DVD_SECTOR_SIZE)),
    _maxReadSpeed(useMaxReadSpeed),
    _buffer(_sectorChunk * QtlDvdMedia::DVD_SECTOR_SIZE),
    _dvd(QString(), log),
    _badSectorPolicy(badSectorPolicy)
{
    // Set total transfer size in bytes. In case of ignored bad sectors, the
    // total size will be slightly smaller, but this is just a hint.
    const qint64 total = qint64(sectorCount) * QtlDvdMedia::DVD_SECTOR_SIZE;
    setProgressMaxHint(total);

    // Set progress interval: every 1 MB.
    setProgressIntervalInBytes(1024 * 1024);
}


//----------------------------------------------------------------------------
// Initialize the transfer.
//----------------------------------------------------------------------------

bool QtlDvdDataPull::initializeTransfer()
{
    // Filter invalid initial parameters.
    if (_dvd.isOpen() || _deviceName.isEmpty() || _startSector < 0 || _endSector < _startSector) {
        return false;
    }

    // Initialize DVD access.
    if (!_dvd.openFromDevice(_deviceName, _maxReadSpeed)) {
        return false;
    }

    // Seek to start sector.
    if (!_dvd.seekSector(_startSector)) {
        _dvd.close();
        return false;
    }
    return true;
}


//----------------------------------------------------------------------------
// Invoked when more data is needed.
//----------------------------------------------------------------------------

bool QtlDvdDataPull::needTransfer(qint64 maxSize)
{
    // Transfer completed.
    if (_dvd.nextSector() >= _endSector) {
        close();
        return true;
    }

    // Compute maximum number of sectors to read.
    int count = qMin(_sectorChunk, _endSector - _dvd.nextSector());
    if (maxSize >= 0) {
        count = qMin(count, int(maxSize / QtlDvdMedia::DVD_SECTOR_SIZE));
    }
    if (count <= 0) {
        return true;
    }

    // Read and write sectors.
    if ((count = _dvd.readSectors(_buffer.data(), count, -1, _badSectorPolicy)) <= 0) {
        return false;
    }
    else {
        return write(_buffer.data(), count * QtlDvdMedia::DVD_SECTOR_SIZE);
    }
}


//----------------------------------------------------------------------------
// Cleanup the transfer.
//----------------------------------------------------------------------------

void QtlDvdDataPull::cleanupTransfer(bool clean)
{
    _dvd.close();
}
