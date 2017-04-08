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
// Unit test for class QtsSection
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtsSection.h"
#include "QtsData.h"

class QtsSectionTest : public QObject
{
    Q_OBJECT
private slots:
    void testSections();
    void testSections_data();
};

#include "QtsSectionTest.moc"
QTL_TEST_CLASS(QtsSectionTest);

//----------------------------------------------------------------------------

// Load various sections.
void QtsSectionTest::testSections()
{
    QFETCH(ConstBytePtr, sectionsAddress);
    QFETCH(int, sectionsSize);
    QFETCH(QtsTableId, tid);
    QFETCH(QtsPid, pid);
    QFETCH(QtsCrc32::Validation, validation);
    QFETCH(bool, isLongHeader);

    QtsSection section(sectionsAddress, sectionsSize, pid, validation);
    QVERIFY(section.isValid());
    QVERIFY(section.tableId() == tid);
    QVERIFY(section.isLongSection() == isLongHeader);
    QVERIFY(section.isShortSection() == !isLongHeader);
    QVERIFY(section.sourcePid() == pid);

    section.clear();
    QVERIFY(!section.isValid());
}

// Data-sets for testSections()
void QtsSectionTest::testSections_data()
{
    QTest::addColumn<ConstBytePtr>("sectionsAddress");
    QTest::addColumn<int>("sectionsSize");
    QTest::addColumn<QtsTableId>("tid");
    QTest::addColumn<QtsPid>("pid");
    QTest::addColumn<QtsCrc32::Validation>("validation");
    QTest::addColumn<bool>("isLongHeader");

    QTest::newRow("tot_tnt")
            << ConstBytePtr(psi_tot_tnt_sections)
            << psi_tot_tnt_sections_size
            << QTS_TID_TOT
            << QTS_PID_TOT
            << QtsCrc32::Check
            << false;

    QTest::newRow("bat_tvnum")
            << ConstBytePtr(psi_bat_tvnum_sections)
            << psi_bat_tvnum_sections_size
            << QTS_TID_BAT
            << QTS_PID_BAT
            << QtsCrc32::Check
            << true;

    QTest::newRow("nit_tntv23")
            << ConstBytePtr(psi_nit_tntv23_sections)
            << psi_nit_tntv23_sections_size
            << QTS_TID_NIT_ACT
            << QTS_PID_NIT
            << QtsCrc32::Check
            << true;
}
