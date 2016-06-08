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
// Unit test for class QtlByteBlock
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlByteBlock.h"

class QtlByteBlockTest : public QObject
{
    Q_OBJECT
private slots:
    void testBasic();
    void testAppend();
    void testGet();
};

#include "QtlByteBlockTest.moc"
QTL_TEST_CLASS(QtlByteBlockTest);

//----------------------------------------------------------------------------

// Test case: basic operations.
void QtlByteBlockTest::testBasic()
{
    QtlByteBlock b1;
    QVERIFY(b1.isEmpty());
    QVERIFY(b1.size() == 0);

    QtlByteBlock b2(50, 27);
    QVERIFY(!b2.isEmpty());
    QVERIFY(b2.size() == 50);
    for (int i = 0; i < 50; ++i) {
        QVERIFY(b2[i] == 27);
    }

    QtlByteBlock b3(b1);
    QVERIFY(b3.isEmpty());
    QVERIFY(b3.size() == 0);

    QtlByteBlock b4(b2);
    QVERIFY(!b4.isEmpty());
    QVERIFY(b4.size() == 50);
    for (int i = 0; i < 50; ++i) {
        QVERIFY(b4[i] == 27);
    }

    static const quint8 data[] = {0x11, 0x42, 0x63};
    QtlByteBlock b5(data, sizeof(data));
    QVERIFY(b5.size() == 3);
    QVERIFY(b5[0] == 0x11);
    QVERIFY(b5[1] == 0x42);
    QVERIFY(b5[2] == 0x63);

    b1 = b5;
    QVERIFY(b1.size() == 3);
    QVERIFY(b1[0] == 0x11);
    QVERIFY(b1[1] == 0x42);
    QVERIFY(b1[2] == 0x63);

    b2 = b5;
    QVERIFY(b2.size() == 3);
    QVERIFY(b2[0] == 0x11);
    QVERIFY(b2[1] == 0x42);
    QVERIFY(b2[2] == 0x63);

    QtlByteBlock b6("abcd");
    QVERIFY(b6.size() == 4);
    QVERIFY(b6[0] == 'a');
    QVERIFY(b6[1] == 'b');
    QVERIFY(b6[2] == 'c');
    QVERIFY(b6[3] == 'd');

    b6.copy(data, sizeof(data));
    QVERIFY(b6.size() == 3);
    QVERIFY(b6[0] == 0x11);
    QVERIFY(b6[1] == 0x42);
    QVERIFY(b6[2] == 0x63);

    b6.resize(2);
    QVERIFY(b6.size() == 2);
    QVERIFY(b6[0] == 0x11);
    QVERIFY(b6[1] == 0x42);

    const quint8* p = reinterpret_cast<const quint8*>(b6.enlarge(2));
    QVERIFY(b6.size() == 4);
    QVERIFY(b6[0] == 0x11);
    QVERIFY(b6[1] == 0x42);
    QVERIFY(b6[2] == 0);
    QVERIFY(b6[3] == 0);
    QVERIFY(p == &b6[2]);
}

// Test case: append operations.
void QtlByteBlockTest::testAppend()
{
    static const quint8 data1[] = {0x01, 0x02, 0x03};
    static const quint8 data2[] = {0x04, 0x05, 0x06};

    QtlByteBlock b1(data1, sizeof(data1));
    b1.append(data2, sizeof(data2));
    QVERIFY(b1.size() == 6);
    QVERIFY(b1[0] == 0x01);
    QVERIFY(b1[1] == 0x02);
    QVERIFY(b1[2] == 0x03);
    QVERIFY(b1[3] == 0x04);
    QVERIFY(b1[4] == 0x05);
    QVERIFY(b1[5] == 0x06);

    b1.append(2, 0x10);
    QVERIFY(b1.size() == 8);
    QVERIFY(b1[0] == 0x01);
    QVERIFY(b1[1] == 0x02);
    QVERIFY(b1[2] == 0x03);
    QVERIFY(b1[3] == 0x04);
    QVERIFY(b1[4] == 0x05);
    QVERIFY(b1[5] == 0x06);
    QVERIFY(b1[6] == 0x10);
    QVERIFY(b1[7] == 0x10);

    b1.appendUInt8(0x12);
    QVERIFY(b1.size() == 9);
    QVERIFY(b1[8] == 0x12);

    b1.appendUInt16(0xABCD);
    QVERIFY(b1.size() == 11);
    QVERIFY(b1[9] == 0xAB);
    QVERIFY(b1[10] == 0xCD);

    b1.appendUInt32(0x98765432);
    QVERIFY(b1.size() == 15);
    QVERIFY(b1[11] == 0x98);
    QVERIFY(b1[12] == 0x76);
    QVERIFY(b1[13] == 0x54);
    QVERIFY(b1[14] == 0x32);
}

// Test case: get/extract operations.
void QtlByteBlockTest::testGet()
{
    static const quint8 data[] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };

    QtlByteBlock b(data, sizeof(data));
    QVERIFY(b.size() == 16);
    QVERIFY(b.fromBigEndian<quint8>(3) == 0x04);
    QVERIFY(b.fromBigEndian<quint16>(2) == 0x0304);
    QVERIFY(b.fromBigEndian<quint32>(8) == 0x090A0B0C);
    QVERIFY(b.fromBigEndian<quint64>(5) == Q_UINT64_C(0x060708090A0B0C0D));
}
