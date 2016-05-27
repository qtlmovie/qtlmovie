//----------------------------------------------------------------------------
//
// Copyright (c) 2013, Thierry Lelegard
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
// Main program for the TestTeletext tool.
//
//----------------------------------------------------------------------------

#include "QtsSectionDemux.h"
#include "QtsTeletextDemux.h"
#include "QtsTeletextDescriptor.h"
#include "QtsTeletextFrame.h"
#include "QtsProgramAssociationTable.h"
#include "QtsProgramMapTable.h"
#include "QtsTsFile.h"
#include "QtlSmartPointer.h"
#include <QtDebug>


//----------------------------------------------------------------------------
// Description of an SRT output file.
//----------------------------------------------------------------------------

class OutputFile
{
public:
    OutputFile(QtsPid pid, int page, const QString& outputPrefix);
    ~OutputFile() {close();}
    bool isOpen() const {return _file.isOpen();}
    QtsPid pid() const {return _pid;}
    int page() const {return _page;}
    QTextStream& stream() {return _stream;}
    void close();

private:
    QtsPid      _pid;
    int         _page;
    QString     _fileName;
    QFile       _file;
    QTextStream _stream;
};

typedef QtlSmartPointer<OutputFile,QtlNullMutexLocker> OutputFilePtr;
typedef QList<OutputFilePtr> OutputFileList;

OutputFile::OutputFile(QtsPid pid, int page, const QString& outputPrefix) :
    _pid(pid),
    _page(page),
    _fileName(QStringLiteral("%1_%2_%3.srt").arg(outputPrefix).arg(pid).arg(page)),
    _file(_fileName),
    _stream(&_file)
{
    _stream.setCodec("UTF-8");
    _stream.setGenerateByteOrderMark(true);

    if (_file.open(QFile::WriteOnly)) {
        qWarning() << "Created" << _fileName;
    }
    else {
        qWarning() << "*** Error creating" << _fileName;
    }
}

void OutputFile::close()
{
    if (_file.isOpen()) {
        _stream.flush();
        _file.close();
    }
}


//----------------------------------------------------------------------------
// Class implementing the Teletext extraction.
//----------------------------------------------------------------------------

class TeletextExtractor : private QtsTeletextHandlerInterface, private QtsTableHandlerInterface
{
public:
    TeletextExtractor(const QString& inputFile, const QString& outputPrefix);
    bool locateStreams();
    bool extractStreams();

private:
    const QString    _outputPrefix;
    QtsTeletextDemux _teletextDemux;
    QtsTsFile        _input;
    OutputFileList   _outputs;
    int              _frameCount;

    virtual void handleTable(QtsSectionDemux& demux, const QtsTable& table);
    virtual void handleTeletextMessage(QtsTeletextDemux& demux, const QtsTeletextFrame& frame);
};

TeletextExtractor::TeletextExtractor(const QString& inputFile, const QString& outputPrefix) :
    _outputPrefix(outputPrefix.isEmpty() ? QFileInfo(inputFile).completeBaseName() : outputPrefix),
    _teletextDemux(this),
    _input(inputFile),
    _outputs(),
    _frameCount(0)
{
    if (!_input.open()) {
        qWarning() << "**** Error opening" << inputFile;
    }
}


//----------------------------------------------------------------------------
// Locate all Teletext streams in the file.
//----------------------------------------------------------------------------

bool TeletextExtractor::locateStreams()
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
        qWarning() << "**** No Teletext stream found in" << _input.fileName();
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
// PSI Table Handler
//----------------------------------------------------------------------------

void TeletextExtractor::handleTable(QtsSectionDemux& demux, const QtsTable& table)
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
                                    qWarning() << "Found Teletext page" << entry.page << "in PID" << stream.pid << "for language" << entry.language;
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

bool TeletextExtractor::extractStreams()
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
        qWarning() << "**** No Teletext frame found in" << _input.fileName();
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
// Teletext frame handler.
//----------------------------------------------------------------------------

void TeletextExtractor::handleTeletextMessage(QtsTeletextDemux& demux, const QtsTeletextFrame& frame)
{
    // Search an existing output file.
    OutputFilePtr out;
    for (OutputFileList::ConstIterator it = _outputs.begin(); out.isNull() && it != _outputs.end(); ++it) {
        if (!it->isNull() && (*it)->pid() == frame.pid && (*it)->page() == frame.page) {
            out = *it;
        }
    }

    // Create a file if not found.
    if (out.isNull()) {
        out = new OutputFile(frame.pid, frame.page, _outputPrefix);
        _outputs << out;
    }

    // Write SRT frame in output file.
    if (!out.isNull() && out->isOpen()) {
        out->stream() << frame.srtFrame();
    }
    _frameCount++;
}


//----------------------------------------------------------------------------
// Program entry point.
//----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3) {
        qWarning("syntax: %s input-file [output-file-prefix]", argv[0]);
        return EXIT_FAILURE;
    }
    else {
        TeletextExtractor ext(argv[1], argc < 3 ? "" : argv[2]);
        return ext.locateStreams() && ext.extractStreams() ? EXIT_SUCCESS : EXIT_FAILURE;
    }
}
