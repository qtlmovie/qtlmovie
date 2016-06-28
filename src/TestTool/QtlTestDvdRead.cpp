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
#include "dvdcss.h"

class QtlTestDvdRead : public QtlTestCommand
{
    Q_OBJECT
public:
    QtlTestDvdRead() : QtlTestCommand("dvdread", "device-name") {}
    virtual int run(const QStringList& args);
private:
    void displayBandwidth(const QTime& time, int sectorCount);
};

//----------------------------------------------------------------------------

int QtlTestDvdRead::run(const QStringList& args)
{
    if (args.size() != 1) {
        return syntaxError();
    }

    // Open libdvdcss
    const QByteArray deviceName(args[0].toUtf8());
    dvdcss_t dvd = dvdcss_open_log(deviceName.data(), 0, 0, 2);
    if (dvd == 0) {
        out << "Cannot initialize libdvdcss on " << deviceName << endl;
        return EXIT_FAILURE;
    }

    // Seek to start of DVD.
    int count = dvdcss_seek(dvd, 0, DVDCSS_SEEK_MPEG);
    if (count < 0) {
        out << "error: dvdcss_seek returns " << count << endl;
        return EXIT_FAILURE;
    }

    // Measure transfer bandwidth.
    QTime time;
    time.start();

    const int sectorsPerRead = 256;
    const int reportInterval = 100000;

    char buffer[sectorsPerRead * DVDCSS_BLOCK_SIZE];
    int nextReport = reportInterval;
    int currentSector = 0;

    do {
        count = dvdcss_read(dvd, buffer, sectorsPerRead, DVDCSS_READ_DECRYPT);
        if (count > 0) {
            currentSector += count;
        }
        if (count != sectorsPerRead) {
            out << "dvdcss_read: requested " << sectorsPerRead << " sectors, got " << count << ", current sector: " << currentSector << endl;
        }
        if (currentSector >= nextReport) {
            displayBandwidth(time, currentSector);
            nextReport += reportInterval;
        }
    } while (count > 0);

    displayBandwidth(time, currentSector);
    out << "Completed, " << currentSector << " sectors read" << endl;

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

void QtlTestDvdRead::displayBandwidth(const QTime& time, int sectorCount)
{
    const int milliSec = time.elapsed();
    if (milliSec > 0) {
        const qint64 bps = (qint64(sectorCount) * DVDCSS_BLOCK_SIZE * 8 * 1000) / milliSec;
        out << "Transfer bandwidth: " << bps << " b/s, " << (bps / 8) << " B/s after " << sectorCount << " sectors" << endl;
    }
}

//----------------------------------------------------------------------------

#include "QtlTestDvdRead.moc"
namespace {QtlTestDvdRead thisTest;}
