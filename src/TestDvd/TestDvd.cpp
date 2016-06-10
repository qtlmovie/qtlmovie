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
// Main program for the TestDvd tool.
//
//----------------------------------------------------------------------------

#include "QtlFile.h"
#include "QtlStdoutLogger.h"
#include "QtlDvdTitleSet.h"

#define READ_CHUNK 1000  // Number of sectors per read operation.


//----------------------------------------------------------------------------
// Program entry point.
//----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    QTextStream out(stdout, QIODevice::WriteOnly);
    QTextStream err(stderr, QIODevice::WriteOnly);
    QtlStdoutLogger log(0, true);

    // Expect at least one argument, the name of a VTS file on DVD.
    if (argc < 2) {
        err << "syntax: " << argv[0] << " ifo-or-vob-file [out-file [sector-count]]" << endl;
        return EXIT_FAILURE;
    }
    const QString input(argv[1]);
    const QString output(argc <= 2 ? "" : (argv[2] != QStringLiteral("-") ? argv[2] : QProcess::nullDevice()));
    int sectorCount = argc <= 3 ? -1 : QString(argv[3]).toInt();

    // Set DVDCSS_VERBOSE=2 for verbose logs from libdvdcss.
    qputenv("DVDCSS_VERBOSE", "2");

    // Load VTS description.
    QtlDvdTitleSet vts(input, &log);
    if (!vts.isOpen()) {
        err << "Error opening " << input << endl;
        return EXIT_FAILURE;
    }

    // Display VTS information.
    out << "IFO file: " << vts.ifoFileName() << endl
        << "VOB size: " << vts.vobSize() << " bytes" << endl
        << "Is encrypted: " << vts.isEncrypted() << endl
        << "VTS number: " << vts.vtsNumber() << endl
        << "VOB count: " << vts.vobCount() << endl
        << "VOB files: " << endl << "    " << vts.vobFileNames().join("\n    ") << endl
        << "DVD device name: " << vts.deviceName() << endl
        << "DVD volume id: \"" << vts.volumeId() << "\"" << endl
        << "Stream count: " << vts.streamCount() << endl;

    // Display stream information.
    foreach (const QtlMediaStreamInfoPtr& stream, vts.streams()) {
        out << "Stream: " << stream->description(false) << endl
            << "    type: " << stream->streamTypeName() << endl
            << "    stream id: " << stream->streamId() << endl;
        if (stream->streamType() == QtlMediaStreamInfo::Video) {
            out << "    width x height: " << stream->width() << " x " << stream->height() << endl
                << "    aspect ratio: " << stream->displayAspectRatioString() << endl;
        }
        else {
            out << "    language:" << stream->language() << endl
                << "    commentary: " << stream->isCommentary() << endl
                << "    impaired: " << stream->impaired() << endl;
            if (stream->streamType() == QtlMediaStreamInfo::Audio) {
                out << "    audio channels: " << stream->audioChannels() << endl;
            }
            else if (stream->streamType() == QtlMediaStreamInfo::Subtitle) {
                out << "    forced: " << stream->forced() << endl;
            }
        }
    }

    // Extract and decrypt VTS content if required.
    if (output.isEmpty()) {
        // Nothing to do.
    }
    else if (!vts.isEncrypted()) {
        err << "DVD is not encrypted, " << output << " not created" << endl;
    }
    else {
        // Create binary output file.
        QFile file(output);
        if (!file.open(QFile::WriteOnly)) {
            err << "Error creating " << output << endl;
            return EXIT_FAILURE;
        }

        if (sectorCount > 0) {
            out << "Reading at most " << sectorCount << " VTS sectors in " << output << endl;
        }
        else {
            sectorCount = vts.sectorCount();
            out << "Reading VTS as " << output << endl;
        }

        // Measure transfer bandwidth.
        QTime time;
        time.start();
        int transferCount = 0;

        char buffer[READ_CHUNK * QtlDvdTitleSet::DVD_SECTOR_SIZE];
        while (sectorCount > 0) {

            // Read some sectors from the VOB's.
            const int count = vts.readSectors(buffer, qMin(READ_CHUNK, sectorCount));
            if (count <= 0) {
                err << "Error reading VTS, readSectors() returns " << count << endl;
                break;
            }
            sectorCount -= count;
            transferCount += count;

            // Write to output file.
            if (!QtlFile::writeBinary(file, buffer, count * QtlDvdTitleSet::DVD_SECTOR_SIZE)) {
                err << "Error writing " << output << endl;
                break;
            }
        }

        // Report transfer bandwidth.
        const int milliSec = time.elapsed();
        if (milliSec > 0) {
            const qint64 bps = (qint64(transferCount) * QtlDvdTitleSet::DVD_SECTOR_SIZE * 8 * 1000) / milliSec;
            out << "Transfer bandwidth: " << bps << " b/s, " << (bps / 8) << " B/s" << endl;
        }

        file.close();
    }

    return EXIT_SUCCESS;
}
