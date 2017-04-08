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
// Command line tool to test extraction of a DVD title set.
//
//----------------------------------------------------------------------------

#include "TestToolCommand.h"
#include "QtsDvdTitleSet.h"
#include "QtsDvdProgramChainDemux.h"
#include "QtlDataPullSynchronousWrapper.h"

//----------------------------------------------------------------------------

class TestPgcDemux : public TestToolCommand
{
    Q_OBJECT

public:
    TestPgcDemux() : TestToolCommand
                     ("pgcdemux",
                      "ifo-or-vob-file out-file [fix-navpacks|remove-navpacks|copy-navpacks [pgc-number]]",
                      "Demux a PGC from a DVD video title set (VTS).\n"
                      "Test tool for class QtsDvdProgramChainDemux.") {}
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;
};

//----------------------------------------------------------------------------

int TestPgcDemux::run(const QStringList& args)
{
    if (args.size() < 2) {
        return syntaxError();
    }

    const QString input(args[0]);
    const QString output(args[1]);
    const QString navpackOption(args.size() < 3 ?"" : args[2]);
    int pgcNumber = args.size() < 4 ? 0 : args[3].toInt();

    Qts::DvdDemuxPolicy demuxPolicy = Qts::NavPacksFixed;
    if (navpackOption.startsWith('f', Qt::CaseInsensitive)) {
        demuxPolicy = Qts::NavPacksFixed;
    }
    else if (navpackOption.startsWith('r', Qt::CaseInsensitive)) {
        demuxPolicy = Qts::NavPacksRemoved;
    }
    else if (navpackOption.startsWith('c', Qt::CaseInsensitive)) {
        demuxPolicy = Qts::NavPacksUnchanged;
    }

    // Load VTS description.
    QtsDvdTitleSet vts(input, &log);
    if (!vts.isLoaded()) {
        err << "Error loading " << input << endl;
        return EXIT_FAILURE;
    }

    QtsDvdProgramChainDemux demux(vts,
                                  pgcNumber,
                                  1,       // angleNumber
                                  1,       // fallbackPgcNumber
                                  QTS_DEFAULT_DVD_TRANSFER_SIZE,       // transferSize
                                  QtlDataPull::DEFAULT_MIN_BUFFER_SIZE, // minBufferSize
                                  demuxPolicy,
                                  &log,
                                  0,       // parent
                                  true);   // useMaxReadSpeed

    // Create binary output file.
    QFile file(output);
    if (!file.open(QFile::WriteOnly)) {
        err << "Error creating " << output << endl;
        return EXIT_FAILURE;
    }

    // Transfer the file using a wrapper test class.
    QtlDataPullSynchronousWrapper(&demux, &file);
    out << "Completed, pulled " << (demux.pulledSize() / QTS_DVD_SECTOR_SIZE) << " sectors, " << demux.pulledSize() << " bytes" << endl;

    file.close();

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

#include "TestPgcDemux.moc"
namespace {TestPgcDemux thisTest;}

