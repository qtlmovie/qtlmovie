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
// Constructors.
//----------------------------------------------------------------------------

QtlDvdDataPull::QtlDvdDataPull(const QString& deviceName,
                               int startSector,
                               int sectorCount,
                               Qtl::BadSectorPolicy badSectorPolicy,
                               int transferSize,
                               int minBufferSize,
                               QtlLogger* log,
                               QObject* parent,
                               bool useMaxReadSpeed) :
    QtlDvdDataPull(deviceName,
                   QtlRangeList(QtlRange(startSector, startSector + sectorCount - 1)),
                   badSectorPolicy,
                   transferSize,
                   minBufferSize,
                   log,
                   parent,
                   useMaxReadSpeed)
{
}


QtlDvdDataPull::QtlDvdDataPull(const QString& deviceName,
                               const QtlRangeList sectorList,
                               Qtl::BadSectorPolicy badSectorPolicy,
                               int transferSize,
                               int minBufferSize,
                               QtlLogger* log,
                               QObject* parent,
                               bool useMaxReadSpeed) :
    QtlDataPull(minBufferSize, log, parent),
    _deviceName(deviceName),
    _sectorList(sectorList),
    _badSectorPolicy(badSectorPolicy),
    _sectorChunk(qMax(1, transferSize / Qtl::DVD_SECTOR_SIZE)),
    _maxReadSpeed(useMaxReadSpeed),
    _currentRange(_sectorList.begin()),
    _nextSector(-1),
    _buffer(_sectorChunk * Qtl::DVD_SECTOR_SIZE),
    _dvd(QString(), log),
    _timeAverage(),
    _timeInstant(),
    _countAverage(0),
    _countInstant(0),
    _reportInterval(30000) // 30 seconds
{
    // Set total transfer size in bytes. In case of ignored bad sectors, the
    // total size will be slightly smaller, but this is just a hint.
    setProgressMaxHint(_sectorList.totalValueCount() * Qtl::DVD_SECTOR_SIZE);

    // Set progress interval: every 1 MB.
    setProgressIntervalInBytes(1024 * 1024);
}


//----------------------------------------------------------------------------
// Initialize the transfer.
//----------------------------------------------------------------------------

bool QtlDvdDataPull::initializeTransfer()
{
    // Filter invalid initial parameters and initialize DVD access.
    if (_dvd.isOpen() || _deviceName.isEmpty() ||!_dvd.openFromDevice(_deviceName, _maxReadSpeed)) {
        return false;
    }

    // Start timers.
    _countAverage = 0;
    _countInstant = 0;
    _timeAverage.start();
    _timeInstant.start();

    // Set position to first sector to read.
    _currentRange = _sectorList.begin();
    _nextSector = -1;

    return true;
}


//----------------------------------------------------------------------------
// Invoked when more data is needed.
//----------------------------------------------------------------------------

bool QtlDvdDataPull::needTransfer(qint64 maxSize)
{
    // Move forward in sector list until we find something to read.
    while (_currentRange != _sectorList.end()) {
        if (_currentRange->isEmpty() || _nextSector > _currentRange->last()) {
            // Empty range or already completely read, look at next one.
            ++_currentRange;
            _nextSector = -1;
        }
        else if (_nextSector >= 0) {
            // Currently in the middle of this range of sectors, continue reading.
            break;
        }
        else {
            // Current range of sectors not yet started, need to seek here.
            log()->debug(tr("Starting transfer of DVD sectors %1").arg(_currentRange->toString()));
            _nextSector = _currentRange->first();
            if (!_dvd.seekSector(_nextSector)) {
                return false;
            }
            break;
        }
    }

    // Is the transfer completed?
    if (_currentRange == _sectorList.end()) {
        close();
        return true;
    }

    Q_ASSERT(_currentRange->first() <= _currentRange->last());
    Q_ASSERT(_nextSector >= _currentRange->first());
    Q_ASSERT(_nextSector <= _currentRange->last());

    // Compute maximum number of sectors to read.
    int count = qMin<int>(_sectorChunk, _currentRange->last() - _nextSector + 1);
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
    _nextSector += count;
    _countAverage += count;
    _countInstant += count;

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
    const int msAverage = _timeAverage.elapsed();
    const int msInstant = _timeInstant.restart();
    const qint64 totalBytes = qint64(_countAverage) * Qtl::DVD_SECTOR_SIZE;
    const qint64 instantBytes = qint64(_countInstant) * Qtl::DVD_SECTOR_SIZE;

    const Qtl::TransferRateFlags flags(Qtl::TransferDvdBase | Qtl::TransferKiloBytes);
    const QString averageRate(QtlDvdMedia::transferRateToString(totalBytes, msAverage, flags));
    const QString instantRate(QtlDvdMedia::transferRateToString(instantBytes, msInstant, flags));

    if (msAverage > 0) {
        QString line(tr("Transfer bandwidth after %1 sectors: ").arg(_countAverage));
        if (msInstant > 0) {
            line.append(instantRate);
            line.append(", ");
        }
        line.append(tr("average: "));
        line.append(averageRate);
        log()->line(line);
    }

    _countInstant = 0;
}
