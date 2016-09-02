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
// Command line tool to test extraction of a DVD title set.
//
//----------------------------------------------------------------------------

#include "QtlTestCommand.h"
#include "QtlDataPullSynchronousWrapper.h"
#include "QtlDvdTitleSet.h"

//----------------------------------------------------------------------------

class QtlTestDvdVts : public QtlTestCommand
{
    Q_OBJECT

public:
    QtlTestDvdVts() : QtlTestCommand("dvdvts", "ifo-or-vob-file out-file [pgc-number]") {}
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;
};

//----------------------------------------------------------------------------

int QtlTestDvdVts::run(const QStringList& args)
{
    if (args.size() < 2) {
        return syntaxError();
    }

    const QString input(args[0]);
    const QString output(args[1]);
    int pgcNumber = args.size() < 3 ? 0 : args[2].toInt();

    // Load VTS description.
    QtlDvdTitleSet vts(input, &log);
    if (!vts.isLoaded()) {
        err << "Error loading " << input << endl;
        return EXIT_FAILURE;
    }

    // Get list of sectors.
    QtlRangeList sectors(vts.titleSectors(pgcNumber, 0, &log));
    out << "VOB sectors to extract for PGC #" << pgcNumber << ": " << sectors.toString() << endl;

    const quint64 sectorsCount = sectors.totalValueCount();
    out << "Total size: " << sectorsCount << " sectors, " << (sectorsCount * Qtl::DVD_SECTOR_SIZE) << " bytes" << endl;

    // Create binary output file.
    QFile file(output);
    if (!file.open(QFile::WriteOnly)) {
        err << "Error creating " << output << endl;
        return EXIT_FAILURE;
    }

    // Transfer the file using a wrapper test class.
    QtlDataPull* dataPull = vts.dataPull(&log, 0, true, sectors);
    QtlDataPullSynchronousWrapper(dataPull, &file);
    out << "Completed, pulled " << (dataPull->pulledSize() / Qtl::DVD_SECTOR_SIZE) << " sectors, " << dataPull->pulledSize() << " bytes" << endl;
    delete dataPull;

    file.close();

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

#include "QtlTestDvdVts.moc"
namespace {QtlTestDvdVts thisTest;}
