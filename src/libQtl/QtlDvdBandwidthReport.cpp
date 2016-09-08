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
// Define the class QtlDvdBandwidthReport.
//
//----------------------------------------------------------------------------

#include "QtlDvdBandwidthReport.h"
#include "QtlStringUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlDvdBandwidthReport::QtlDvdBandwidthReport(int reportInterval, QtlLogger* log) :
    _log(log),
    _started(false),
    _timeAverage(),
    _timeInstant(),
    _countAverage(0),
    _countInstant(0),
    _reportInterval(reportInterval)
{
}


//----------------------------------------------------------------------------
// Start the transfer.
//----------------------------------------------------------------------------

void QtlDvdBandwidthReport::start()
{
    if (!_started) {
        _countAverage = 0;
        _countInstant = 0;
        _timeAverage.start();
        _timeInstant.start();
        _started = true;
    }
}


//----------------------------------------------------------------------------
// Declare the transfer of sectors from DVD media.
//----------------------------------------------------------------------------

void QtlDvdBandwidthReport::transfered(int sectorCount)
{
    if (_started) {
        if (sectorCount > 0) {
            _countAverage += sectorCount;
            _countInstant += sectorCount;
        }
        if (_timeInstant.elapsed() >= _reportInterval) {
            reportBandwidth();
        }
    }
}


//----------------------------------------------------------------------------
// Report bandwidth immediately.
//----------------------------------------------------------------------------

void QtlDvdBandwidthReport::reportBandwidth()
{
    if (_started) {
        const int msAverage = _timeAverage.elapsed();
        const int msInstant = _timeInstant.restart();
        const qint64 totalBytes = qint64(_countAverage) * Qtl::DVD_SECTOR_SIZE;
        const qint64 instantBytes = qint64(_countInstant) * Qtl::DVD_SECTOR_SIZE;

        const Qtl::TransferRateFlags flags(Qtl::TransferDvdBase | Qtl::TransferKiloBytes);
        const QString averageRate(transferRateToString(totalBytes, msAverage, flags));
        const QString instantRate(transferRateToString(instantBytes, msInstant, flags));

        if (msAverage > 0 && _log != 0) {
            QString line(QObject::tr("Transfer bandwidth after %1 sectors: ").arg(_countAverage));
            if (msInstant > 0) {
                line.append(instantRate);
                line.append(", ");
            }
            line.append(QObject::tr("average: "));
            line.append(averageRate);
            _log->line(line);
        }

        _countInstant = 0;
    }
}


//----------------------------------------------------------------------------
// Build a human-readable string for DVD transfer rate.
//----------------------------------------------------------------------------

QString QtlDvdBandwidthReport::transferRateToString(qint64 bytes, qint64 milliSeconds, Qtl::TransferRateFlags flags)
{
    QString result;
    if (milliSeconds > 0) {
        const QString separator(QStringLiteral(", "));
        if (flags & Qtl::TransferDvdBase) {
            result = qtlFractionToString(bytes * 1000, milliSeconds * Qtl::DVD_TRANSFER_RATE, 1) + "x";
        }
        if (flags & Qtl::TransferBytes) {
            if (!result.isEmpty()) {
                result.append(separator);
            }
            result.append(QStringLiteral("%1 B/s").arg(bytes * 1000 / milliSeconds));
        }
        if (flags & Qtl::TransferKiloBytes) {
            if (!result.isEmpty()) {
                result.append(separator);
            }
            result.append(QStringLiteral("%1 kB/s").arg(bytes / milliSeconds));
        }
        if (flags & Qtl::TransferKibiBytes) {
            if (!result.isEmpty()) {
                result.append(separator);
            }
            result.append(QStringLiteral("%1 kiB/s").arg(bytes * 1000 / milliSeconds * 1024));
        }
        if (flags & Qtl::TransferBits) {
            if (!result.isEmpty()) {
                result.append(separator);
            }
            result.append(QStringLiteral("%1 b/s").arg(bytes * 8000 / milliSeconds));
        }
    }
    return result;
}
