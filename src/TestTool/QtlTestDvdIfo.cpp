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
// Command line tool to test IFO files on DVD.
//
//----------------------------------------------------------------------------

#include "QtlTestCommand.h"
#include "QtlFile.h"

class QtlTestDvdIfo : public QtlTestCommand
{
    Q_OBJECT

public:
    QtlTestDvdIfo() : QtlTestCommand("dvdifo", "ifo-file-name") {}
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;

private:
    static const int DVD_SECTOR_SIZE = 2048;
    void displayAudioVideoAttributes();
    void displayPgc(int pgci);

    template<typename INT> void displayInt(int index, const QString& name)
    {
        const INT x = _ifo.fromBigEndian<INT>(index);
        out << name << ": 0x" << hex << x << dec << " (" << x << ")" << endl;
    }

    QString      _fileName;
    QtlByteBlock _ifo;
    int          _totalVobSectors;
};

//----------------------------------------------------------------------------

int QtlTestDvdIfo::run(const QStringList& args)
{
    if (args.size() != 1) {
        return syntaxError();
    }

    // Read IFO file content.
    _fileName = args[0];
    _ifo = QtlFile::readBinaryFile(_fileName);
    if (_ifo.isEmpty()) {
        err << "Error reading " << args[0] << endl;
        return EXIT_FAILURE;
    }
    if (_ifo.getLatin1(0, 12) != "DVDVIDEO-VTS") {
        err << "Not a valid IFO file for VTS" << endl;
        return EXIT_FAILURE;
    }

    out << "IFO file name: " << _fileName << endl
        << "IFO file size: " << (_ifo.size() / DVD_SECTOR_SIZE) << " sectors, " << _ifo.size() << " bytes" << endl;

    // List VOB files, if any.
    _totalVobSectors = 0;
    if (_fileName.endsWith("0.IFO", Qt::CaseInsensitive)) {
        QString prefix(_fileName);
        prefix.chop(5);
        for (int i = 1; i < 10; ++i) {
            const QFileInfo info(prefix + QString::number(i) + ".VOB");
            if (!info.exists()) {
                break;
            }
            out << info.fileName() << " : " << (info.size() / DVD_SECTOR_SIZE) << " sectors, " << info.size() << " bytes" << endl;
            _totalVobSectors += info.size() / DVD_SECTOR_SIZE;
        }
        out << "Total VOB size : " << _totalVobSectors << " sectors, " << (_totalVobSectors * DVD_SECTOR_SIZE) << " bytes" << endl;
    }

    // Pointers into IFO.
    displayInt<quint32>(0x000C, "Last sector of VTS");
    displayInt<quint32>(0x001C, "Last sector of IFO");
    out << "Version: " << QString::number(_ifo[0x21] >> 4) << "." << QString::number(_ifo[0x21] & 0x0F) << endl;
    displayInt<quint32>(0x0022, "VTS category");
    displayInt<quint32>(0x0080, "End byte address of VTS_MAT");
    displayInt<quint32>(0x00C0, "Start sector of menu VOB");
    displayInt<quint32>(0x00C4, "Start sector of title VOB");
    displayInt<quint32>(0x00C8, "Sector pointer to VTS_PTT_SRPT");
    displayInt<quint32>(0x00CC, "Sector pointer to VTS_PGCI");
    displayInt<quint32>(0x00D0, "Sector pointer to VTSM_PGCI_UT");
    displayInt<quint32>(0x00D4, "Sector pointer to VTS_TMAPTI");
    displayInt<quint32>(0x00D8, "Sector pointer to VTSM_C_ADT");
    displayInt<quint32>(0x00DC, "Sector pointer to VTSM_VOBU_ADMAP");
    displayInt<quint32>(0x00E0, "Sector pointer to VTS_C_ADT");
    displayInt<quint32>(0x00E4, "Sector pointer to VTS_VOBU_ADMAP");

    displayAudioVideoAttributes();
    displayPgc(_ifo.fromBigEndian<quint32>(0x00CC) * DVD_SECTOR_SIZE);

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

void QtlTestDvdIfo::displayAudioVideoAttributes()
{
    const quint8 vid0 = _ifo[0x0200];
    const quint8 vid1 = _ifo[0x0201];
    const int audioCount = _ifo.fromBigEndian<quint16>(0x202);
    const int audioIndex = 0x204;
    const int subCount = _ifo.fromBigEndian<quint16>(0x254);
    const int subIndex = 0x256;

    out << "Video attributes: ";
    switch ((vid0 >> 4) & 0x03) {
        case 0:  out << "NTSC, "; break;
        case 1:  out << "PAL, "; break;
        default: out << "Standard=" << ((vid0 >> 4) & 0x03) << ", "; break;
    }
    const bool ntsc = ((vid0 >> 4) & 0x03) == 0;
    switch ((vid0 >> 2) & 0x03) {
        case 0:  out << "4:3, "; break;
        case 3:  out << "16:9, "; break;
        default: out << "Aspect=" << ((vid0 >> 2) & 0x03) << ", "; break;
    }
    switch ((vid1 >> 3) & 0x07) {
        case 0:  out << (ntsc ? "720x480" : "720x576"); break;
        case 1:  out << (ntsc ? "704x480" : "704x576"); break;
        case 2:  out << (ntsc ? "352x480" : "352x576"); break;
        case 3:  out << (ntsc ? "352x240" : "352x288"); break;
        default: out << "Resolution=" << ((vid1 >> 3) & 0x07) << ", "; break;
    }
    out << endl;

    out << "Number of audio streams: " << audioCount << endl;
    for (int i = 0; i < audioCount; ++i) {
        const int index = audioIndex + 8 * i;
        const quint8 codingMode = _ifo[index] >> 5;
        const int channelCount = int(_ifo[index + 1] & 0x07) + 1;
        const bool languagePresent = (_ifo[index] & 0x0C) == 0x04;
        const quint8 type = _ifo[index + 5];
        // For audio, the stream ids depend on the codec.
        // Using n as index of audio stream, the stream id is;
        // AC-3 = 0x80 + n, DTS  = 0x88 + n, SDDS = 0x90 + n, LPCM = 0xA0 + n, MP2  = 0xC0 + n, MPEG2-ext = 0xC8 + n
        out << "  Audio #" << i << ": ";
        switch (codingMode) {
            case 0: out << "id 0x" << hex << (0x80 + i) << ", AC-3"; break;
            case 2: out << "id 0x" << hex << (0xC0 + i) << ", MPEG-1"; break;
            case 3: out << "id 0x" << hex << (0xC8 + i) << ", MPEG-2"; break;
            case 4: out << "id 0x" << hex << (0xA0 + i) << ", LPCM"; break;
            case 6: out << "id 0x" << hex << (0x88 + i) << ", DTS"; break;
        }
        out << ", " << dec << channelCount << " channels";
        switch (type) {
            case 1: out << ", normal"; break;
            case 2: out << ", visually impaired"; break;
            case 3: out << ", director comments"; break;
            case 4: out << ", alternate director comments"; break;
        }
        if (languagePresent) {
            out << ", lang=" << _ifo.getLatin1(index + 2, 2);
        }
        out << endl;
    }

    out << "Number of subpicture streams: " << subCount << endl;
    for (int i = 0; i < subCount; ++i) {
        const int index = subIndex + 6 * i;
        const bool languagePresent = (_ifo[index] & 0x03) == 0x01;
        const quint8 type = _ifo[index + 5];
        // For DVD subpictures (=subtitle), the stream id is always 0x20 + index of stream
        out << "  Subpicture #" << i << ": id 0x" << hex << (0x20 + i) << dec << ", ";
        switch (type) {
            case 0: out << "not specified"; break;
            case 1: out << "normal"; break;
            case 2: out << "large"; break;
            case 3: out << "children"; break;
            case 5: out << "normal captions"; break;
            case 6: out << "large captions"; break;
            case 7: out << "children captions"; break;
            case 9: out << "forced"; break;
            case 13: out << "director comments"; break;
            case 14: out << "large director comments"; break;
            case 15: out << "director comments for children"; break;
            default: out << "type=" << type; break;
        }
        if (languagePresent) {
            out << ", lang=" << _ifo.getLatin1(index + 2, 2);
        }
        out << endl;
    }
}

//----------------------------------------------------------------------------

void QtlTestDvdIfo::displayPgc(int pgci)
{
    const int pgcCount = _ifo.fromBigEndian<quint16>(pgci);
    const int endAddress = _ifo.fromBigEndian<quint32>(pgci + 4);
    out << "Number of PGC: " << pgcCount << ", PGC table size: " << (endAddress + 1) << " bytes" << endl;

    for (int pgcIndex = 0; pgcIndex < pgcCount; ++pgcIndex) {
        const int descIndex = pgci + 8 + 8 * pgcIndex;
        if (descIndex + 8 > _ifo.size()) {
            break;
        }
        if ((_ifo[descIndex] & 0x80) == 0) {
            continue;
        }
        const int titleNumber = _ifo[descIndex] & 0x7F;
        const int pgc = pgci + _ifo.fromBigEndian<quint32>(descIndex + 4);
        out << "  PGC #" << pgcIndex << ", title #" << titleNumber << endl;
        if (pgc < pgci || pgc + 0x00EC > _ifo.size()) {
            continue;
        }
        out << "    Number of programs: " << int(_ifo[pgc + 0x0002]) << endl
            << "    Number of cells: " << int(_ifo[pgc + 0x0003]) << endl;

        // The playback time is encoded in BCD as: hh:mm:ss:ff (ff = frame count within second)
        // With 2 MSBits of ff indicating frame rate: 11 = 30 fps, 10 = illegal, 01 = 25 fps, 00 = illegal
        const quint32 playbackTime = _ifo.fromBigEndian<quint32>(pgc + 0x0004);
        const int hours   = (((playbackTime >> 28) & 0x0F) * 10) + ((playbackTime >> 24) & 0x0F);
        const int minutes = (((playbackTime >> 20) & 0x0F) * 10) + ((playbackTime >> 16) & 0x0F);
        const int seconds = (((playbackTime >> 12) & 0x0F) * 10) + ((playbackTime >>  8) & 0x0F);
        const int frame   = (((playbackTime >>  4) & 0x03) * 10) + (playbackTime & 0x0F);
        const int rate    = (playbackTime >> 6) & 0x03;
        out << QStringLiteral("    Duration %1:%2:%3, frame %4").arg(hours).arg(minutes).arg(seconds).arg(frame);
        switch (rate) {
            case 1: out << " @25 fps"; break;
            case 3: out << " @30 fps"; break;
            default: out << " invalid frame rate " << rate; break;
        }
        out << endl;

        // Display audio streams
        out << "    Audio streams:";
        for (int i = 0; i < 8; ++i) {
            const int index = pgc + 0x000C + 2 * i;
            if ((_ifo[index] & 0x80) != 0) {
                out << " " << int(_ifo[index] & 0x07);
            }
        }
        out << endl;

        // Chaining:
        const quint16 nextPgc     = _ifo.fromBigEndian<quint16>(pgc + 0x009C);
        const quint16 previousPgc = _ifo.fromBigEndian<quint16>(pgc + 0x009E);
        const quint16 parentPgc   = _ifo.fromBigEndian<quint16>(pgc + 0x00A0);
        out << "    Next PGC: " << nextPgc << ", previous PGC: " << previousPgc << ", parent PGC: " << parentPgc << endl;

        // Color palette:
        out << "    Palette (0, Y, Cr, Cb):";
        for (int i = 0; i < 16; ++i) {
            const int index = pgc + 0x00A4 + 4 * i;
            out << (i == 0 ? " " : ",") << QStringLiteral("%1%2%3%4")
                   .arg(int(_ifo[index]), 2, 16, QChar('0'))
                   .arg(int(_ifo[index+1]), 2, 16, QChar('0'))
                   .arg(int(_ifo[index+2]), 2, 16, QChar('0'))
                   .arg(int(_ifo[index+3]), 2, 16, QChar('0'));
        }
        out << endl;
    }
}

//----------------------------------------------------------------------------

#include "QtlTestDvdIfo.moc"
namespace {QtlTestDvdIfo thisTest;}
