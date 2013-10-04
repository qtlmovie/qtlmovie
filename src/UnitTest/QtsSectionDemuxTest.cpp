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
// Unit test for class QtsSectionDemux
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlHexa.h"
#include "QtsStandaloneTableDemux.h"
#include "QtsData.h"

class QtsSectionDemuxTest : public QObject
{
    Q_OBJECT
private slots:
    void testTables();
    void testTables_data();
};

#include "QtsSectionDemuxTest.moc"
QTL_TEST_CLASS(QtsSectionDemuxTest);

//----------------------------------------------------------------------------

// Test case: Demux various tables.
void QtsSectionDemuxTest::testTables()
{
    QFETCH(ConstBytePtr, packetsAddress);
    QFETCH(int, packetsSize);
    QFETCH(ConstBytePtr, sectionsAddress);
    QFETCH(int, sectionsSize);
    QFETCH(QtsTableId, tid);

    const QtsTsPacket* const refPackets = reinterpret_cast <const QtsTsPacket*>(packetsAddress);
    const int refPacketsCount = packetsSize / QTS_PKT_SIZE;

    // Demux all packets.
    QtsStandaloneTableDemux demux(QtsAllPids);
    for (int pi = 0; pi < refPacketsCount; ++pi) {
        demux.feedPacket(refPackets[pi]);
    }

    // Check that there is one demuxed table.
    QVERIFY(demux.tableCount() == 1);
    const QtsTablePtr table(demux.tableAt(0));
    QVERIFY(table->tableId() == tid);

    // Check the content of all sections.
    ConstBytePtr sectionData = sectionsAddress;
    for (int si = 0; si < table->sectionCount(); ++si) {
        const QtsSectionPtr section(table->sectionAt(si));
        //  qDebug() << "section size = " << section->size() << "\n"
        //           << "reference section:" << "\n"
        //           << qtlHexa(sectionData, section->size(), Qtl::HexBytesPerLine, 0, 32)
        //           << "demuxed section:" << "\n"
        //           << qtlHexa(section->content(), section->size(), Qtl::HexBytesPerLine, 0, 32);
        QVERIFY(sectionData + section->size() <= sectionsAddress + sectionsSize);
        QVERIFY(::memcmp(section->content(), sectionData, section->size()) == 0);
        sectionData += section->size();
    }
    QVERIFY(sectionData == sectionsAddress + sectionsSize);
}

// Data-sets for testTables()
void QtsSectionDemuxTest::testTables_data()
{
    QTest::addColumn<ConstBytePtr>("packetsAddress");
    QTest::addColumn<int>("packetsSize");
    QTest::addColumn<ConstBytePtr>("sectionsAddress");
    QTest::addColumn<int>("sectionsSize");
    QTest::addColumn<QtsTableId>("tid");

    QTest::newRow("tot_tnt")
            << ConstBytePtr(psi_tot_tnt_packets)
            << psi_tot_tnt_packets_size
            << ConstBytePtr(psi_tot_tnt_sections)
            << psi_tot_tnt_sections_size
            << QTS_TID_TOT;

    QTest::newRow("tdt_tnt")
            << ConstBytePtr(psi_tdt_tnt_packets)
            << psi_tdt_tnt_packets_size
            << ConstBytePtr(psi_tdt_tnt_sections)
            << psi_tdt_tnt_sections_size
            << QTS_TID_TDT;

    QTest::newRow("bat_cplus")
            << ConstBytePtr(psi_bat_cplus_packets)
            << psi_bat_cplus_packets_size
            << ConstBytePtr(psi_bat_cplus_sections)
            << psi_bat_cplus_sections_size
            << QTS_TID_BAT;

    QTest::newRow("bat_tvnum")
            << ConstBytePtr(psi_bat_tvnum_packets)
            << psi_bat_tvnum_packets_size
            << ConstBytePtr(psi_bat_tvnum_sections)
            << psi_bat_tvnum_sections_size
            << QTS_TID_BAT;

    QTest::newRow("cat_r3")
            << ConstBytePtr(psi_cat_r3_packets)
            << psi_cat_r3_packets_size
            << ConstBytePtr(psi_cat_r3_sections)
            << psi_cat_r3_sections_size
            << QTS_TID_CAT;

    QTest::newRow("cat_r6")
            << ConstBytePtr(psi_cat_r6_packets)
            << psi_cat_r6_packets_size
            << ConstBytePtr(psi_cat_r6_sections)
            << psi_cat_r6_sections_size
            << QTS_TID_CAT;

    QTest::newRow("nit_tntv23")
            << ConstBytePtr(psi_nit_tntv23_packets)
            << psi_nit_tntv23_packets_size
            << ConstBytePtr(psi_nit_tntv23_sections)
            << psi_nit_tntv23_sections_size
            << QTS_TID_NIT_ACT;

    QTest::newRow("pat_r4")
            << ConstBytePtr(psi_pat_r4_packets)
            << psi_pat_r4_packets_size
            << ConstBytePtr(psi_pat_r4_sections)
            << psi_pat_r4_sections_size
            << QTS_TID_PAT;

    QTest::newRow("pmt_planete")
            << ConstBytePtr(psi_pmt_planete_packets)
            << psi_pmt_planete_packets_size
            << ConstBytePtr(psi_pmt_planete_sections)
            << psi_pmt_planete_sections_size
            << QTS_TID_PMT;

    QTest::newRow("sdt_r6")
            << ConstBytePtr(psi_sdt_r6_packets)
            << psi_sdt_r6_packets_size
            << ConstBytePtr(psi_sdt_r6_sections)
            << psi_sdt_r6_sections_size
            << QTS_TID_SDT_ACT;
}
