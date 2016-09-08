//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2016, Thierry Lelegard
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
// Command line tool to test the DVD read performances.
//
//----------------------------------------------------------------------------

#include "QtlTestCommand.h"
#include "QtlDvdMedia.h"
#include "QtlDvdBandwidthReport.h"

class QtlTestDvdRead : public QtlTestCommand
{
    Q_OBJECT
public:
    QtlTestDvdRead() : QtlTestCommand("dvdread", "device-name") {}
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;
private:
    void displayBandwidth(const QTime& timeAverage, const QTime& timeInstant, int sectorCount, int instantSectorCount);
};

//----------------------------------------------------------------------------

int QtlTestDvdRead::run(const QStringList& args)
{
    if (args.size() != 1) {
        return syntaxError();
    }
    const QString deviceName(args[0]);

    // Open libdvdcss
    QtlDvdMedia dvd(QString(), &log);
    if (!dvd.openFromDevice(deviceName, true)) {
        out << "Cannot initialize libdvdcss on " << deviceName << endl;
        return EXIT_FAILURE;
    }

    // Load all CSS keys
    dvd.loadAllEncryptionKeys();

    // Seek to start of DVD.
    if (!dvd.seekSector(0)) {
        out << "error seeking to sector 0" << endl;
        return EXIT_FAILURE;
    }

    // Measure transfer bandwidth, both globally and instantly.
    QTime timeAverage;
    QTime timeInstant;
    timeAverage.start();
    timeInstant.start();

    const int sectorsPerRead = 256;
    const int reportInterval = 50000;

    char buffer[sectorsPerRead * Qtl::DVD_SECTOR_SIZE];
    int nextReport = reportInterval;
    int currentSector = 0;
    int lastInstantSector = 0;
    int count = 0;

    do {
        count = dvd.readSectors(buffer, sectorsPerRead);
        if (count > 0) {
            currentSector += count;
        }
        if (count != sectorsPerRead) {
            out << "Requested " << sectorsPerRead << " sectors, got " << count << ", current sector: " << currentSector << endl;
        }
        if (currentSector >= nextReport) {
            displayBandwidth(timeAverage, timeInstant, currentSector, currentSector - lastInstantSector);
            nextReport += reportInterval;
            lastInstantSector = currentSector;
            timeInstant.start();
        }
    } while (count > 0);

    displayBandwidth(timeAverage, timeInstant, currentSector, currentSector - lastInstantSector);
    out << "Completed, " << currentSector << " sectors read" << endl;

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

void QtlTestDvdRead::displayBandwidth(const QTime& timeAverage, const QTime& timeInstant, int sectorCount, int instantSectorCount)
{
    const int msAverage = timeAverage.elapsed();
    const int msInstant = timeInstant.elapsed();
    const qint64 totalBytes = qint64(sectorCount) * Qtl::DVD_SECTOR_SIZE;
    const qint64 instantBytes = qint64(instantSectorCount) * Qtl::DVD_SECTOR_SIZE;

    if (msAverage > 0) {
        out << "Transfer bandwidth after " << sectorCount << " sectors: ";
        if (msInstant > 0) {
            out << QtlDvdBandwidthReport::transferRateToString(instantBytes, msInstant, Qtl::TransferDvdBase | Qtl::TransferKiloBytes) << ", ";
        }
        out << "average: " << QtlDvdBandwidthReport::transferRateToString(totalBytes, msAverage, Qtl::TransferDvdBase | Qtl::TransferKiloBytes) << endl;
    }
}

//----------------------------------------------------------------------------

#include "QtlTestDvdRead.moc"
namespace {QtlTestDvdRead thisTest;}
