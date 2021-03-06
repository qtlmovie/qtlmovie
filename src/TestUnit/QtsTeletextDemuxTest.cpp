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
// Unit test for class QtsTeletextDemux
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtsSectionDemux.h"
#include "QtsTeletextDemux.h"
#include "QtsTeletextDescriptor.h"
#include "QtsTeletextFrame.h"
#include "QtsProgramAssociationTable.h"
#include "QtsProgramMapTable.h"
#include "QtsTsFile.h"
#include "QtlSubRipGenerator.h"
#include "QtlFile.h"

class QtsTeletextDemuxTest : public QObject, private QtsTableHandlerInterface, private QtsTeletextHandlerInterface
{
    Q_OBJECT
public:
    QtsTeletextDemuxTest();
private slots:
    void testReferenceTeletext();
private:
    QString            _genString;
    QTextStream        _genStream;
    QtlSubRipGenerator _subrip;
    QtsTsFile          _file;

    virtual void handleTable(QtsSectionDemux& demux, const QtsTable& table) Q_DECL_OVERRIDE;
    virtual void handleTeletextMessage(QtsTeletextDemux& demux, const QtsTeletextFrame& frame) Q_DECL_OVERRIDE;
};

#include "QtsTeletextDemuxTest.moc"
QTL_TEST_CLASS(QtsTeletextDemuxTest);


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtsTeletextDemuxTest::QtsTeletextDemuxTest() :
    _genString(),
    _genStream(&_genString, QIODevice::WriteOnly),
    _subrip(&_genStream),
    _file(":/test/test-teletext.stream")
{
}


//----------------------------------------------------------------------------
// Test one reference stream containing Teletext.
//----------------------------------------------------------------------------

void QtsTeletextDemuxTest::testReferenceTeletext()
{
    QVERIFY(_subrip.isOpen());

    QtsSectionDemux sectionDemux(this);
    sectionDemux.addPid(QTS_PID_PAT);
    sectionDemux.addPid(160); // known PMT PID for test stream

    QtsTeletextDemux teletextDemux(this);
    teletextDemux.addPid(1068);        // known Teletext PID for test stream
    // teletextDemux.setAddColors(true);  // add font color HTML tags

    QVERIFY(_file.open(QFile::ReadOnly));
    QtsTsPacket packet;
    while (_file.read(&packet) > 0) {
        sectionDemux.feedPacket(packet);
        teletextDemux.feedPacket(packet);
    }
    teletextDemux.flushTeletext();
    _file.close();
    _subrip.close();

    QVERIFY(sectionDemux.packetCount() == 1987);
    QVERIFY(teletextDemux.packetCount() == 1987);
    QVERIFY(teletextDemux.frameCount(889, 1068) == 9);
    QVERIFY(teletextDemux.frameCount(889, 1000) == 0);
    QVERIFY(teletextDemux.frameCount(889) == 9);
    QVERIFY(teletextDemux.frameCount(888) == 0);
    QVERIFY(teletextDemux.frameCount(777) == 0);

    QStringList refLines(QtlFile::readTextLinesFile(":/test/test-teletext.srt"));
    QVERIFY(refLines.size() == 45);

    // As a side effect of split, the trailing \n generates an additional line.
    QStringList genLines(_genString.split(QRegExp("\\r*\\n")));
    QVERIFY(genLines.size() == 46);
    genLines.removeLast();

    QVERIFY(genLines == refLines);
}


//----------------------------------------------------------------------------
// PSI/SI table handler.
//----------------------------------------------------------------------------

void QtsTeletextDemuxTest::handleTable(QtsSectionDemux& demux, const QtsTable& table)
{
    // Process the table.
    switch (table.tableId()) {
        case QTS_TID_PAT: {
            const QtsProgramAssociationTable pat(table);
            QVERIFY(pat.isValid());
            QVERIFY(pat.serviceList.size() == 1);
            const QtsProgramAssociationTable::ServiceEntry& service(pat.serviceList.first());
            QVERIFY(service.serviceId == 4006);
            QVERIFY(service.pmtPid == 160);
            break;
        }
        case QTS_TID_PMT: {
            const QtsProgramMapTable pmt(table);
            QVERIFY(pmt.isValid());
            QVERIFY(pmt.streams.size() == 6);
            const QtsProgramMapTable::StreamEntry& stream(pmt.streams.last());
            QVERIFY(stream.pid == 1068);
            QVERIFY(stream.descs.size() == 2);
            const QtsTeletextDescriptor td(*(stream.descs[0]));
            QVERIFY(td.isValid());
            QVERIFY(td.entries.size() == 2);
            QVERIFY(td.entries[0].type == QTS_TELETEXT_SUBTITLES_HI);
            QVERIFY(td.entries[0].page == 888);
            QVERIFY(td.entries[0].language == "fra");
            QVERIFY(td.entries[1].type == QTS_TELETEXT_SUBTITLES);
            QVERIFY(td.entries[1].page == 889);
            QVERIFY(td.entries[1].language == "fra");
            break;
        }
    }
}


//----------------------------------------------------------------------------
// Teletext handler.
//----------------------------------------------------------------------------

void QtsTeletextDemuxTest::handleTeletextMessage(QtsTeletextDemux& demux, const QtsTeletextFrame& frame)
{
    QVERIFY(frame.pid() == 1068);
    QVERIFY(frame.page() == 889);
    _subrip.addFrame(frame.showTimestamp(), frame.hideTimestamp(), frame.lines());
}
