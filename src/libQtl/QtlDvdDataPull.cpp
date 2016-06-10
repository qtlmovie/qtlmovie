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
#include "dvdcss.h"


//----------------------------------------------------------------------------
// Constructor and destructor.
//----------------------------------------------------------------------------

QtlDvdDataPull::QtlDvdDataPull(const QString& deviceName,
                               int startSector,
                               int sectorCount,
                               int transferSize,
                               int minBufferSize,
                               QtlLogger* log,
                               QObject* parent) :
    QtlDataPull(minBufferSize, log, parent),
    _deviceName(deviceName),
    _startSector(startSector),
    _sectorCount(sectorCount),
    _sectorChunk(qMax(1, transferSize / DVDCSS_BLOCK_SIZE)),
    _sectorRemain(0),
    _buffer(_sectorChunk * DVDCSS_BLOCK_SIZE),
    _dvdcss(0)
{
}

QtlDvdDataPull::~QtlDvdDataPull()
{
    // Close libdvdcss.
    if (_dvdcss != 0) {
        dvdcss_close(_dvdcss);
        _dvdcss = 0;
    }
}


//----------------------------------------------------------------------------
// Initialize the transfer.
//----------------------------------------------------------------------------

bool QtlDvdDataPull::initializeTransfer()
{
    // Filter invalid initial parameters.
    if (_deviceName.isEmpty() || _startSector < 0 || _sectorCount < 0) {
        return false;
    }

    // Initialize libdvdcss.
    const QByteArray name(_deviceName.toUtf8());
    _dvdcss = dvdcss_open(name.data());
    if (_dvdcss == 0) {
        log()->debug(tr("Cannot initialize libdvdcss on %1, probably not a DVD media").arg(_deviceName));
        return false;
    }

    // Seek to start sector.
    if (dvdcss_seek(_dvdcss, _startSector, DVDCSS_SEEK_MPEG) < 0) {
        log()->line(tr("Error seeking DVD to sector %1").arg(_startSector));
        return false;
    }

    _sectorRemain = _sectorCount;
    return true;
}


//----------------------------------------------------------------------------
// Invoked when more data is needed.
//----------------------------------------------------------------------------

bool QtlDvdDataPull::needTransfer()
{
    int count;

    if (_sectorRemain <= 0) {
        // Transfer completed.
        close();
        return true;
    }
    else if ((count = dvdcss_read(_dvdcss, _buffer.data(), qMin(_sectorChunk, _sectorRemain), DVDCSS_READ_DECRYPT)) <= 0) {
        log()->line(tr("Error reading DVD media"));
        return false;
    }
    else {
        _sectorRemain -= count;
        return write(_buffer.data(), count * DVDCSS_BLOCK_SIZE);
    }
}


//----------------------------------------------------------------------------
// Cleanup the transfer.
//----------------------------------------------------------------------------

void QtlDvdDataPull::cleanupTransfer(bool clean)
{
    // Close libdvdcss.
    if (_dvdcss != 0) {
        dvdcss_close(_dvdcss);
        _dvdcss = 0;
    }
}
