//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2017, Thierry Lelegard
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
// Command line tool to test DVD.
//
//----------------------------------------------------------------------------

#include "TestToolCommand.h"
#include "QtlFile.h"
#include "QtlStringUtils.h"
#include "QtlStdoutLogger.h"
#include "QtsDvdTitleSet.h"
#include "QtsDvdMedia.h"

//----------------------------------------------------------------------------

class TestDvd : public TestToolCommand
{
    Q_OBJECT

public:
    TestDvd() : TestToolCommand
                ("dvd",
                 "ifo-or-vob-file",
                 "Display information about a DVD media and one of its VTS."
                 "Test tool for classes QtsDvdMedia and QtsDvdTitleSet.") {}
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;

private:
    void displayFile(const QString& indent, const QtsDvdFile& file);
    void displayDirectory(const QString& indent, const QtsDvdDirectory& dir);
};

//----------------------------------------------------------------------------

void TestDvd::displayFile(const QString& indent, const QtsDvdFile& file)
{
    out << indent << file.name()
        << ", " << file.sizeInBytes() << " bytes"
        << ", LBA " << file.startSector() << "-" << (file.endSector() - 1)
        << endl;
}

void TestDvd::displayDirectory(const QString& indent, const QtsDvdDirectory& dir)
{
    displayFile(indent + "Directory: ", dir);
    foreach (const QtsDvdFilePtr& file, dir.files()) {
        if (!file.isNull()) {
            displayFile(indent + "    File: ", *file);
        }
    }
    foreach (const QtsDvdDirectoryPtr& subdir, dir.subDirectories()) {
        if (!subdir.isNull()) {
            displayDirectory(indent + "    ", *subdir);
        }
    }
}

//----------------------------------------------------------------------------

int TestDvd::run(const QStringList& args)
{
    // Expect at least one argument, the name of a VTS file on DVD.
    if (args.size() < 1) {
        return syntaxError();
    }

    const QString input(args[0]);

    // Load DVD media description.
    QtsDvdMedia dvd(input, &log);
    dvd.loadAllEncryptionKeys();

    // Display DVD information.
    out << "Input file: " << input << endl
        << "Is open: " << dvd.isOpen() << endl
        << "Is encrypted: " << dvd.isEncrypted() << endl
        << "Root name: " << dvd.rootName() << endl
        << "Device name: " << dvd.deviceName() << endl
        << "Volume id: \"" << dvd.volumeId() << "\"" << endl
        << "Volume size: " << dvd.volumeSizeInSectors() << " sectors" << endl
        << "VTS count: " << dvd.vtsCount() << endl
        << "IFO file names:" << endl;
    for (int vts = 1; vts <= dvd.vtsCount(); ++vts) {
        out << "    " << dvd.vtsInformationFileName(vts) << endl;
    }
    out << "File structure:" << endl;
    displayDirectory("", dvd.rootDirectory());
    out << endl;

    // Display DVD file layout.
    out << "File layout:" << endl;
    foreach (const QtsDvdFilePtr& file, dvd.allFiles()) {
        out << "    " << file->description() << ", LBA " << file->startSector() << "-" << (file->endSector() - 1) << endl;
    }
    out << endl;

    // Load VTS description.
    QtsDvdTitleSet vts(input, &log);
    if (!vts.isLoaded()) {
        err << "Error loading " << input << endl;
        return EXIT_FAILURE;
    }

    // Display VTS information.
    out << "IFO file: " << vts.ifoFileName() << endl
        << "VOB size: " << vts.vobSizeInBytes() << " bytes" << endl
        << "Is encrypted: " << vts.isEncrypted() << endl
        << "VTS number: " << vts.vtsNumber() << endl
        << "Title count: " << vts.titleCount() << " PGC" << endl
        << "Longest duration: " << vts.longestDurationInSeconds() << " seconds"
        << " (" << qtlSecondsToString(vts.longestDurationInSeconds()) << ")" << endl
        << "VOB count: " << vts.vobCount() << endl
        << "VOB files: " << endl << "    " << vts.vobFileNames().join("\n    ") << endl
        << "DVD device name: " << vts.deviceName() << endl
        << "DVD volume id: \"" << vts.volumeId() << "\"" << endl
        << "DVD volume size: " << vts.volumeSizeInSectors() << " sectors" << endl
        << "VOB start sector: " << vts.vobStartSector() << endl
        << "VOB sector count: " << vts.vobSectorCount() << endl
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

    // Display PGC information.
    for (int i = 1; i <= vts.titleCount(); ++i) {
        const QtsDvdProgramChainPtr pgc(vts.title(i));
        if (pgc.isNull()) {
            out << "PGC #" << i << " has a null pointer" << endl;
            continue;
        }
        if (!pgc->isValid()) {
            out << "PGC #" << i << " is invalid" << endl;
            continue;
        }
        out << "PGC #" << i << ": current:" << pgc->titleNumber()
            << ", previous:" << pgc->previousTitleNumber()
            << ", next:" << pgc->nextTitleNumber()
            << ", parent:" << pgc->parentTitleNumber() << endl
            << "    Duration: " << pgc->durationInSeconds() << " seconds (" << qtlSecondsToString(pgc->durationInSeconds()) << ")" << endl
            << "    YUV palette: " << QtsDvdProgramChain::paletteToString(pgc->yuvPalette()) << endl
            << "    RGB palette: " << QtsDvdProgramChain::paletteToString(pgc->rgbPalette()) << endl
            << "    Chapters: " << pgc->chapterCount()
            << ", cells: " << pgc->cellCount()
            << ", total sectors: " << pgc->totalSectorCount() << endl;
        foreach (const QtsDvdProgramChapterPtr& chapter, pgc->chapters()) {
            out << "    Chapter #" << chapter->chapterId() << ", cells:";
            foreach (const QtsDvdProgramCellPtr& cell, chapter->cells()) {
                out << " " << cell->cellId();
            }
            out << endl;
        }
        foreach (const QtsDvdProgramCellPtr& cell, pgc->cells()) {
            out << "    Cell #" << cell->cellId()
                << ", angle: " << cell->angleId()
                << ", " << qtlSecondsToString(cell->durationInSeconds())
                << ", " << cell->sectors().totalValueCount()
                << " sectors in " << cell->sectors().size() << " ranges"
                << endl;
        }
    }

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

#include "TestDvd.moc"
namespace {TestDvd thisTest;}
