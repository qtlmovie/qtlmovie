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
// Define the class QtsDvdDataPull.
//
//----------------------------------------------------------------------------

#include "QtsDvdDataPull.h"


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtsDvdDataPull::QtsDvdDataPull(const QString& deviceName,
                               int startSector,
                               int sectorCount,
                               Qts::BadSectorPolicy badSectorPolicy,
                               int transferSize,
                               int minBufferSize,
                               QtlLogger* log,
                               QObject* parent,
                               bool useMaxReadSpeed) :
    QtsDvdDataPull(deviceName,
                   QtlRangeList(QtlRange(startSector, startSector + sectorCount - 1)),
                   badSectorPolicy,
                   transferSize,
                   minBufferSize,
                   log,
                   parent,
                   useMaxReadSpeed)
{
}


QtsDvdDataPull::QtsDvdDataPull(const QString& deviceName,
                               const QtlRangeList sectorList,
                               Qts::BadSectorPolicy badSectorPolicy,
                               int transferSize,
                               int minBufferSize,
                               QtlLogger* log,
                               QObject* parent,
                               bool useMaxReadSpeed) :
    QtlDataPull(minBufferSize, log, parent),
    _deviceName(deviceName),
    _sectorList(sectorList),
    _badSectorPolicy(badSectorPolicy),
    _sectorChunk(qMax(1, transferSize / QTS_DVD_SECTOR_SIZE)),
    _maxReadSpeed(useMaxReadSpeed),
    _currentRange(_sectorList.begin()),
    _nextSector(-1),
    _buffer(_sectorChunk * QTS_DVD_SECTOR_SIZE),
    _dvd(QString(), log),
    _report(30000, log) // report transfer bandwidth every 30 seconds.
{
    // Set total transfer size in bytes. In case of ignored bad sectors, the
    // total size will be slightly smaller, but this is just a hint.
    setProgressMaxHint(_sectorList.totalValueCount() * QTS_DVD_SECTOR_SIZE);

    // Set progress interval: every 1 MB.
    setProgressIntervalInBytes(1024 * 1024);
}


//----------------------------------------------------------------------------
// Initialize the transfer.
//----------------------------------------------------------------------------

bool QtsDvdDataPull::initializeTransfer()
{
    // Filter invalid initial parameters and initialize DVD access.
    if (_dvd.isOpen() || _deviceName.isEmpty() ||!_dvd.openFromDevice(_deviceName, _maxReadSpeed)) {
        return false;
    }

    // Start timers.
    _report.start();

    // Set position to first sector to read.
    _currentRange = _sectorList.begin();
    _nextSector = -1;

    return true;
}


//----------------------------------------------------------------------------
// Invoked when more data is needed.
//----------------------------------------------------------------------------

bool QtsDvdDataPull::needTransfer(qint64 maxSize)
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
        count = qMin(count, int(maxSize / QTS_DVD_SECTOR_SIZE));
    }
    if (count <= 0) {
        return true;
    }

    // Read sectors.
    if ((count = _dvd.readSectors(_buffer.data(), count, -1, _badSectorPolicy)) <= 0) {
        return false;
    }
    _nextSector += count;
    _report.transfered(count);

    // Write sectors.
    return write(_buffer.data(), count * QTS_DVD_SECTOR_SIZE);
}


//----------------------------------------------------------------------------
// Cleanup the transfer.
//----------------------------------------------------------------------------

void QtsDvdDataPull::cleanupTransfer(bool clean)
{
    _report.reportBandwidth();
    _dvd.close();
}
