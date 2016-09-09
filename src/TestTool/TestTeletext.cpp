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
// Command line tool to test DVD.
//
//----------------------------------------------------------------------------

#include "TestToolCommand.h"
#include "QtsSectionDemux.h"
#include "QtsTeletextDemux.h"
#include "QtsTeletextDescriptor.h"
#include "QtsTeletextFrame.h"
#include "QtsProgramAssociationTable.h"
#include "QtsProgramMapTable.h"
#include "QtsTsFile.h"
#include "QtlSmartPointer.h"
#include "QtlSubRipGenerator.h"

typedef QtlSmartPointer<QtlSubRipGenerator,QtlNullMutexLocker> QtlSubRipGeneratorPtr;
typedef QMap<int,QtlSubRipGeneratorPtr> SubRipMap;


//----------------------------------------------------------------------------
// Test class.
//----------------------------------------------------------------------------

class TestTeletext : public TestToolCommand, private QtsTeletextHandlerInterface, private QtsTableHandlerInterface
{
    Q_OBJECT

public:
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;

    TestTeletext() :
        TestToolCommand("teletext",
                        "input-file [output-file-prefix]",
                        "Read an MPEG2-TS file and locate all Teletext subtitles streams.\n"
                        "If output-file-prefix is specified, extract each Teletext stream\n"
                        "in a file named <prefix>_<pid>_<page>.srt"),
        _outputPrefix(),
        _teletextDemux(this),
        _input(),
        _outputs(),
        _frameCount(0)
    {
    }

private:
    QString          _outputPrefix;
    QtsTeletextDemux _teletextDemux;
    QtsTsFile        _input;
    SubRipMap        _outputs;
    int              _frameCount;

    bool locateStreams();
    bool extractStreams();
    virtual void handleTable(QtsSectionDemux& demux, const QtsTable& table) Q_DECL_OVERRIDE;
    virtual void handleTeletextMessage(QtsTeletextDemux& demux, const QtsTeletextFrame& frame) Q_DECL_OVERRIDE;
};


//----------------------------------------------------------------------------
// Locate all Teletext streams in the file.
//----------------------------------------------------------------------------

bool TestTeletext::locateStreams()
{
    if (!_input.isOpen() || !_input.seek(0)) {
        return false;
    }

    QtsSectionDemux sectionDemux(this);
    sectionDemux.addPid(QTS_PID_PAT);
    _teletextDemux.reset();

    QtsTsPacket packet;
    while (sectionDemux.filteredPidCount() > 0 && _input.read(&packet) > 0) {
        sectionDemux.feedPacket(packet);
    }

    if (_teletextDemux.filteredPidCount() == 0) {
        err << "**** No Teletext stream found in " << _input.fileName() << endl;
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
// PSI Table Handler
//----------------------------------------------------------------------------

void TestTeletext::handleTable(QtsSectionDemux& demux, const QtsTable& table)
{
    switch (table.tableId()) {
        case QTS_TID_PAT: {
            demux.removePid(QTS_PID_PAT);
            const QtsProgramAssociationTable pat(table);
            if (pat.isValid()) {
                // Add a filter on the PMT PID of all services.
                foreach (const QtsProgramAssociationTable::ServiceEntry& service, pat.serviceList) {
                    demux.addPid(service.pmtPid);
                }
            }
            break;
        }
        case QTS_TID_PMT: {
            demux.removePid(table.sourcePid());
            const QtsProgramMapTable pmt(table);
            if (pmt.isValid()) {
                // Loop through all elementary streams in the service.
                foreach (const QtsProgramMapTable::StreamEntry& stream, pmt.streams) {
                    // Loop through all teletext descriptors in the stream.
                    for (int index = 0; (index = stream.descs.search(QTS_DID_TELETEXT, index)) < stream.descs.size(); ++index) {
                        const QtsTeletextDescriptor td(*(stream.descs[index]));
                        if (td.isValid()) {
                            // Loop through all language entries.
                            foreach (const QtsTeletextDescriptor::Entry& entry, td.entries) {
                                if (entry.type == QTS_TELETEXT_SUBTITLES || entry.type == QTS_TELETEXT_SUBTITLES_HI) {
                                    _teletextDemux.addPid(stream.pid);
                                    err << "Found Teletext page " << entry.page << " in PID " << stream.pid << " for language " << entry.language << endl;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
    }
}


//----------------------------------------------------------------------------
// Extract all Teletext streams.
//----------------------------------------------------------------------------

bool TestTeletext::extractStreams()
{
    if (!_input.isOpen() || !_input.seek(0)) {
        return false;
    }

    _outputs.clear();
    _frameCount = 0;
    QtsTsPacket packet;
    while (_input.read(&packet) > 0) {
        _teletextDemux.feedPacket(packet);
    }
    _teletextDemux.flushTeletext();
    _outputs.clear();

    if (_frameCount == 0) {
        err << "**** No Teletext frame found in " << _input.fileName() << endl;
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
// Teletext frame handler.
//----------------------------------------------------------------------------

void TestTeletext::handleTeletextMessage(QtsTeletextDemux& demux, const QtsTeletextFrame& frame)
{
    // Search an existing output file.
    // The index in the map is made of pid and page.
    const int index = (frame.pid() << 16) | (frame.page() & 0xFFFF);
    const SubRipMap::ConstIterator it = _outputs.find(index);
    QtlSubRipGeneratorPtr subrip;

    // Create a file if not found.
    if (it != _outputs.end()) {
        subrip = it.value();
    }
    else {
        const QString fileName(QStringLiteral("%1_%2_%3.srt").arg(_outputPrefix).arg(frame.pid()).arg(frame.page()));
        subrip = new QtlSubRipGenerator(fileName);
        if (subrip->isOpen()) {
            out << "Created " << fileName << endl;
            _outputs.insert(index, subrip);
        }
        else {
            err << "*** Error creating " << fileName << endl;
        }
    }

    // Write SRT frame in output file.
    if (!subrip.isNull() && subrip->isOpen()) {
        subrip->addFrame(frame.showTimestamp(), frame.hideTimestamp(), frame.lines());
    }
    _frameCount++;
}


//----------------------------------------------------------------------------
// Program entry point.
//----------------------------------------------------------------------------

int TestTeletext::run(const QStringList& args)
{
    if (args.size() < 1) {
        return syntaxError();
    }

    const QString inputFile(args[0]);
    _input.setFileName(inputFile);
    if (!_input.open()) {
        err << "**** Error opening " << inputFile << endl;
        return EXIT_FAILURE;
    }

    if (args.size() >= 2) {
        _outputPrefix = args[1];
    }
    if (_outputPrefix.isEmpty()) {
        _outputPrefix = QFileInfo(inputFile).completeBaseName();
    }

    return locateStreams() && extractStreams() ? EXIT_SUCCESS : EXIT_FAILURE;
}

//----------------------------------------------------------------------------

#include "TestTeletext.moc"
namespace {TestTeletext thisTest;}
