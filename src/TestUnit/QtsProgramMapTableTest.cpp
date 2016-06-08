//----------------------------------------------------------------------------
//
// Copyright (c) 2013, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
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
// Unit test for class QtsProgramMapTable
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtsProgramMapTable.h"
#include "QtsData.h"

class QtsProgramMapTableTest : public QObject
{
    Q_OBJECT
private slots:
    void testPlanete();
};

#include "QtsProgramMapTableTest.moc"
QTL_TEST_CLASS(QtsProgramMapTableTest);

//----------------------------------------------------------------------------

// Test case: demux one table from TNT network.
void QtsProgramMapTableTest::testPlanete()
{
    // Description of this table:
    //
    // * PMT, TID 2 (0x02)
    //   Section: 0 (last: 0), version: 21, size: 119 bytes
    //   Program: 772 (0x0304), PCR PID: 163 (0x00A3)
    //   Program information:
    //   - Descriptor 0: CA, Tag 9 (0x09), 5 bytes
    //     CA System Id: 0x4ADC (SafeAccess), ECM PID: 1642 (0x066A), no CECM
    //   Elementary stream: type 0x1B (AVC video), PID: 163 (0x00A3)
    //   - Descriptor 0: Stream Identifier, Tag 82 (0x52), 1 bytes
    //     Stream Id: 41 (0x29)
    //   - Descriptor 1: AVC Video, Tag 40 (0x28), 4 bytes
    //     0000:  03 00 03 00                                      ....
    //   - Descriptor 2: CA, Tag 9 (0x09), 32 bytes
    //     CA System Id: 0x0100 (MediaGuard)
    //     ECM PID: 1641 (0x0669), OPI: 367 (0x016F)
    //     Subscription, offer: 0x0000000000000010, expiration: 23/11/2007
    //     ECM PID: 1640 (0x0668), OPI: 343 (0x0157)
    //     Subscription, offer: 0x0000040000000004, expiration: 23/11/2007
    //   Elementary stream: type 0x04 (MPEG-2 Audio), PID: 92 (0x005C)
    //   - Descriptor 0: Stream Identifier, Tag 82 (0x52), 1 bytes
    //     Stream Id: 42 (0x2A)
    //   - Descriptor 1: ISO-639 Language, Tag 10 (0x0A), 4 bytes
    //     Language: fra, Type: 0 (undefined)
    //   - Descriptor 2: CA, Tag 9 (0x09), 32 bytes
    //     CA System Id: 0x0100 (MediaGuard)
    //     ECM PID: 1641 (0x0669), OPI: 367 (0x016F)
    //     Subscription, offer: 0x0000000000000010, expiration: 23/11/2007
    //     ECM PID: 1640 (0x0668), OPI: 343 (0x0157)
    //     Subscription, offer: 0x0000040000000004, expiration: 23/11/2007

    // Build a binary table.
    const QtsSection section(psi_pmt_planete_sections, psi_pmt_planete_sections_size, QTS_PID_NULL, QtsCrc32::Check);
    QVERIFY(section.isValid());
    QtsTable table;
    table.addSection(section);
    QVERIFY(table.isValid());

    // Deserialize the table.
    const QtsProgramMapTable pmt(table);
    QVERIFY(pmt.isValid());

    // Check program-level info.
    QVERIFY(pmt.isCurrent());
    QVERIFY(pmt.version() == 21);
    QVERIFY(pmt.serviceId == 0x0304);
    QVERIFY(pmt.pcrPid == 0x00A3);
    QVERIFY(pmt.descs.size() == 1);
    QVERIFY(pmt.descs[0]->tag() == QTS_DID_CA);

    // Check stream-level info.
    QVERIFY(pmt.streams.size() == 2);

    QVERIFY(pmt.streams[0].pid == 0x00A3);
    QVERIFY(pmt.streams[0].type == QTS_ST_AVC_VIDEO);
    QVERIFY(pmt.streams[0].isVideo());
    QVERIFY(!pmt.streams[0].isAudio());
    QVERIFY(!pmt.streams[0].isSubtitles());
    QVERIFY(pmt.streams[0].descs.size() == 3);
    QVERIFY(pmt.streams[0].descs[0]->tag() == QTS_DID_STREAM_ID);
    QVERIFY(pmt.streams[0].descs[1]->tag() == QTS_DID_AVC_VIDEO);
    QVERIFY(pmt.streams[0].descs[2]->tag() == QTS_DID_CA);

    QVERIFY(pmt.streams[1].pid == 0x005C);
    QVERIFY(pmt.streams[1].type == QTS_ST_MPEG2_AUDIO);
    QVERIFY(!pmt.streams[1].isVideo());
    QVERIFY(pmt.streams[1].isAudio());
    QVERIFY(!pmt.streams[1].isSubtitles());
    QVERIFY(pmt.streams[1].descs.size() == 3);
    QVERIFY(pmt.streams[1].descs[0]->tag() == QTS_DID_STREAM_ID);
    QVERIFY(pmt.streams[1].descs[1]->tag() == QTS_DID_LANGUAGE);
    QVERIFY(pmt.streams[1].descs[2]->tag() == QTS_DID_CA);
}
