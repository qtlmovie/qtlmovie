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
    _sectorChunk(qMax(1, transferSize / Qtl::DVD_SECTOR_SIZE)),
    _maxReadSpeed(useMaxReadSpeed),
    _buffer(_sectorChunk * Qtl::DVD_SECTOR_SIZE),
    _dvd(QString(), log),
    _timeAverage(),
    _timeInstant(),
    _reportInterval(30000), // 30 seconds
    _reportSector(0),
    _badSectorPolicy(badSectorPolicy)
{
    // Set total transfer size in bytes. In case of ignored bad sectors, the
    // total size will be slightly smaller, but this is just a hint.
    const qint64 total = qint64(sectorCount) * Qtl::DVD_SECTOR_SIZE;
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

    // Start timers.
    _timeAverage.start();
    _timeInstant.start();
    _reportSector = _startSector;

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
        count = qMin(count, int(maxSize / Qtl::DVD_SECTOR_SIZE));
    }
    if (count <= 0) {
        return true;
    }

    // Read sectors.
    if ((count = _dvd.readSectors(_buffer.data(), count, -1, _badSectorPolicy)) <= 0) {
        return false;
    }

    // Report bandwidth.
    if (_timeInstant.elapsed() >= _reportInterval) {
        reportBandwidth();
    }

    // Write sectors.
    return write(_buffer.data(), count * Qtl::DVD_SECTOR_SIZE);
}


//----------------------------------------------------------------------------
// Cleanup the transfer.
//----------------------------------------------------------------------------

void QtlDvdDataPull::cleanupTransfer(bool clean)
{
    reportBandwidth();
    _dvd.close();
}


//----------------------------------------------------------------------------
// Report bandwidth.
//----------------------------------------------------------------------------

void QtlDvdDataPull::reportBandwidth()
{
    const int nextSector = _dvd.nextSector();
    const int msAverage = _timeAverage.elapsed();
    const int msInstant = _timeInstant.restart();
    const qint64 totalBytes = qint64(nextSector - _startSector) * Qtl::DVD_SECTOR_SIZE;
    const qint64 instantBytes = qint64(nextSector - _reportSector) * Qtl::DVD_SECTOR_SIZE;

    const Qtl::TransferRateFlags flags(Qtl::TransferDvdBase | Qtl::TransferKiloBytes);
    const QString averageRate(QtlDvdMedia::transferRateToString(totalBytes, msAverage, flags));
    const QString instantRate(QtlDvdMedia::transferRateToString(instantBytes, msInstant, flags));

    if (msAverage > 0) {
        QString line(tr("Transfer bandwidth after %1 sectors: ").arg(nextSector - _startSector));
        if (msInstant > 0) {
            line.append(instantRate);
            line.append(", ");
        }
        line.append(tr("average: "));
        line.append(averageRate);
        log()->line(line);
    }

    _reportSector = nextSector;
}
