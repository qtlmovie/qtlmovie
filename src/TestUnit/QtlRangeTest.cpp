//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
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
// Unit test for class QtlRange
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlRange.h"
#include "QtlRangeList.h"

class QtlRangeTest : public QObject
{
    Q_OBJECT
private slots:
    void testRange();
    void testAdd();
    void testScale();
    void testOverlap();
    void testAdjacent();
    void testMerge();
    void testClip();
    void testList();
    void testListTotalValueCount();
    void testListAdd();
    void testListScale();
    void testListSort();
    void testListMerge();
    void testListClip();
};

#include "QtlRangeTest.moc"
QTL_TEST_CLASS(QtlRangeTest);

void QtlRangeTest::testRange()
{
    QtlRange r;

    QVERIFY(r.isEmpty());
    QCOMPARE(r.first(), Q_INT64_C(0));
    QCOMPARE(r.last(),  Q_INT64_C(-1));
    QCOMPARE(r.count(), Q_UINT64_C(0));

    QVERIFY(QtlRange(1, 4) == QtlRange(1,  4));
    QVERIFY(QtlRange(1, 4) <= QtlRange(1,  4));
    QVERIFY(QtlRange(1, 4) >= QtlRange(1,  4));
    QVERIFY(QtlRange(1, 4) != QtlRange(1,  3));
    QVERIFY(QtlRange(1, 4) != QtlRange(0,  4));
    QVERIFY(QtlRange(1, 4) <  QtlRange(1,  5));
    QVERIFY(QtlRange(1, 4) <= QtlRange(1,  5));
    QVERIFY(QtlRange(1, 4) <  QtlRange(2,  5));
    QVERIFY(QtlRange(1, 4) <= QtlRange(2,  5));
    QVERIFY(QtlRange(1, 4) >  QtlRange(-1, 4));
    QVERIFY(QtlRange(1, 4) >= QtlRange(-1, 4));
    QVERIFY(QtlRange(1, 4) >  QtlRange(1,  3));
    QVERIFY(QtlRange(1, 4) >= QtlRange(1,  3));

    r.setFirst(27);
    QCOMPARE(r.first(), Q_INT64_C(27));
    QCOMPARE(r.last(),  Q_INT64_C(26));
    QCOMPARE(r.count(), Q_UINT64_C(0));

    r.setLast(12);
    QCOMPARE(r.first(), Q_INT64_C(27));
    QCOMPARE(r.last(),  Q_INT64_C(26));
    QCOMPARE(r.count(), Q_UINT64_C(0));

    r.setLast(37);
    QCOMPARE(r.first(), Q_INT64_C(27));
    QCOMPARE(r.last(),  Q_INT64_C(37));
    QCOMPARE(r.count(), Q_UINT64_C(11));

    r.setFirst(20);
    QCOMPARE(r.first(), Q_INT64_C(20));
    QCOMPARE(r.last(),  Q_INT64_C(37));
    QCOMPARE(r.count(), Q_UINT64_C(18));

    r.setRange(-7, -3);
    QCOMPARE(r.first(), Q_INT64_C(-7));
    QCOMPARE(r.last(),  Q_INT64_C(-3));
    QCOMPARE(r.count(), Q_UINT64_C(5));

    r.clear();
    QVERIFY(r.isEmpty());
    QCOMPARE(r.first(), Q_INT64_C(-7));
    QCOMPARE(r.last(),  Q_INT64_C(-8));
    QCOMPARE(r.count(), Q_UINT64_C(0));
}

void QtlRangeTest::testAdd()
{
    QtlRange r(10, 19);

    r.add(200);
    QCOMPARE(r.first(), Q_INT64_C(210));
    QCOMPARE(r.last(),  Q_INT64_C(219));

    r.setRange(QtlRange::RANGE_MAX - 20, QtlRange::RANGE_MAX - 10);
    r.add(50);
    QVERIFY(r.isEmpty());
    QCOMPARE(r.first(), QtlRange::RANGE_MAX);
    QCOMPARE(r.last(),  QtlRange::RANGE_MAX - 1);

    r.setRange(QtlRange::RANGE_MAX - 20, QtlRange::RANGE_MAX - 10);
    r.add(15);
    QCOMPARE(r.first(), QtlRange::RANGE_MAX - 5);
    QCOMPARE(r.last(),  QtlRange::RANGE_MAX);

    r.setRange(QtlRange::RANGE_MAX - 20, QtlRange::RANGE_MAX);
    r.add(15);
    QCOMPARE(r.first(), QtlRange::RANGE_MAX - 5);
    QCOMPARE(r.last(),  QtlRange::RANGE_MAX);

    r.setRange(0, QtlRange::RANGE_MAX);
    r.add(50);
    QCOMPARE(r.first(), Q_INT64_C(50));
    QCOMPARE(r.last(),  QtlRange::RANGE_MAX);

    r.setFirst(0);
    r.clear();
    r.add(10);
    QCOMPARE(r.first(), Q_INT64_C(10));
    QCOMPARE(r.last(),  Q_INT64_C(9));

    r.setRange(QtlRange::RANGE_MIN + 10, QtlRange::RANGE_MIN + 20);
    r.add(-100);
    QVERIFY(r.isEmpty());
    QCOMPARE(r.first(), QtlRange::RANGE_MIN);
    QCOMPARE(r.last(),  QtlRange::RANGE_MIN);

    r.setRange(QtlRange::RANGE_MIN + 10, QtlRange::RANGE_MIN + 20);
    r.add(-15);
    QCOMPARE(r.first(), QtlRange::RANGE_MIN);
    QCOMPARE(r.last(),  QtlRange::RANGE_MIN + 5);

    r.setRange(QtlRange::RANGE_MIN, QtlRange::RANGE_MIN + 20);
    r.add(-15);
    QCOMPARE(r.first(), QtlRange::RANGE_MIN);
    QCOMPARE(r.last(),  QtlRange::RANGE_MIN + 5);

    r.setRange(QtlRange::RANGE_MIN + 10, 255);
    r.add(-50);
    QCOMPARE(r.first(), QtlRange::RANGE_MIN);
    QCOMPARE(r.last(),  Q_INT64_C(205));
}

void QtlRangeTest::testScale()
{
    QtlRange r;

    r.setRange(10, 12);
    r.scale(100);
    QCOMPARE(r.first(), Q_INT64_C(1000));
    QCOMPARE(r.last(),  Q_INT64_C(1299));

    r.setRange(10, 12);
    r.scale(0);
    QCOMPARE(r.first(), Q_INT64_C(0));
    QCOMPARE(r.last(),  Q_INT64_C(-1));

    r.setRange(-10, 12);
    r.scale(100);
    QCOMPARE(r.first(), Q_INT64_C(0));
    QCOMPARE(r.last(),  Q_INT64_C(-1));

    r.setRange(QtlRange::RANGE_MAX - 20, QtlRange::RANGE_MAX - 10);
    r.scale(1);
    QCOMPARE(r.first(), QtlRange::RANGE_MAX - 20);
    QCOMPARE(r.last(),  QtlRange::RANGE_MAX - 10);

    r.setRange(QtlRange::RANGE_MAX - 20, QtlRange::RANGE_MAX - 10);
    r.scale(2);
    QCOMPARE(r.first(), QtlRange::RANGE_MAX);
    QCOMPARE(r.last(),  QtlRange::RANGE_MAX - 1);
}

void QtlRangeTest::testOverlap()
{
    QVERIFY(QtlRange(1, 4).overlap(QtlRange(1,   4)));
    QVERIFY(QtlRange(1, 4).overlap(QtlRange(1,   1)));
    QVERIFY(QtlRange(1, 4).overlap(QtlRange(4,   4)));
    QVERIFY(QtlRange(1, 4).overlap(QtlRange(4,   7)));
    QVERIFY(QtlRange(1, 4).overlap(QtlRange(1,   4)));
    QVERIFY(QtlRange(1, 1).overlap(QtlRange(1,   4)));
    QVERIFY(QtlRange(4, 4).overlap(QtlRange(1,   4)));
    QVERIFY(QtlRange(4, 7).overlap(QtlRange(1,   4)));
    QVERIFY(QtlRange(1, 4).overlap(QtlRange(-10, 40)));

    QVERIFY(!QtlRange(1, 4).overlap(QtlRange(5,    7)));
    QVERIFY(!QtlRange(1, 4).overlap(QtlRange(-10,  0)));
    QVERIFY(!QtlRange(1, 4).overlap(QtlRange(1000, 7)));
    QVERIFY(!QtlRange(1, 4).overlap(QtlRange(-500, -10)));
    QVERIFY(!QtlRange(1, 4).overlap(QtlRange(500,  700)));
}

void QtlRangeTest::testAdjacent()
{
    QVERIFY( QtlRange(1, 4).adjacent(QtlRange(5, 10), Qtl::AdjacentBefore));
    QVERIFY(!QtlRange(1, 4).adjacent(QtlRange(5, 10), Qtl::AdjacentAfter));
    QVERIFY( QtlRange(1, 4).adjacent(QtlRange(5, 10), Qtl::AdjacentAny));

    QVERIFY(!QtlRange(1, 4).adjacent(QtlRange(6, 10), Qtl::AdjacentBefore));
    QVERIFY(!QtlRange(1, 4).adjacent(QtlRange(6, 10), Qtl::AdjacentAfter));
    QVERIFY(!QtlRange(1, 4).adjacent(QtlRange(6, 10), Qtl::AdjacentAny));

    QVERIFY(!QtlRange(1, 4).adjacent(QtlRange(-10, 0), Qtl::AdjacentBefore));
    QVERIFY( QtlRange(1, 4).adjacent(QtlRange(-10, 0), Qtl::AdjacentAfter));
    QVERIFY( QtlRange(1, 4).adjacent(QtlRange(-10, 0), Qtl::AdjacentAny));

    QVERIFY(!QtlRange(2, 7).adjacent(QtlRange(2, 1), Qtl::AdjacentBefore));
    QVERIFY( QtlRange(2, 7).adjacent(QtlRange(2, 1), Qtl::AdjacentAfter));
    QVERIFY( QtlRange(2, 7).adjacent(QtlRange(2, 1), Qtl::AdjacentAny));

    QVERIFY(QtlRange(2, 0).adjacent(QtlRange(2, 1), Qtl::AdjacentBefore));
    QVERIFY(QtlRange(2, 0).adjacent(QtlRange(2, 1), Qtl::AdjacentAfter));
    QVERIFY(QtlRange(2, 0).adjacent(QtlRange(2, 1), Qtl::AdjacentAny));
}

void QtlRangeTest::testMerge()
{
    QtlRange r;

    r.setRange(2, 7);
    r.merge(QtlRange(100, 150));
    QCOMPARE(r.first(), Q_INT64_C(2));
    QCOMPARE(r.last(),  Q_INT64_C(150));

    r.setRange(2, 7);
    r.merge(QtlRange(-100, 0));
    QCOMPARE(r.first(), Q_INT64_C(-100));
    QCOMPARE(r.last(),  Q_INT64_C(7));

    r.setRange(2, 7);
    r.merge(QtlRange(3, 6));
    QCOMPARE(r.first(), Q_INT64_C(2));
    QCOMPARE(r.last(),  Q_INT64_C(7));

    r.setRange(2, 7);
    r.merge(QtlRange(2, 5));
    QCOMPARE(r.first(), Q_INT64_C(2));
    QCOMPARE(r.last(),  Q_INT64_C(7));

    r.setRange(2, 7);
    r.merge(QtlRange(2, 20));
    QCOMPARE(r.first(), Q_INT64_C(2));
    QCOMPARE(r.last(),  Q_INT64_C(20));

    r.setRange(2, 7);
    r.merge(QtlRange(0, 7));
    QCOMPARE(r.first(), Q_INT64_C(0));
    QCOMPARE(r.last(),  Q_INT64_C(7));

    r.setRange(2, 7);
    r.merge(QtlRange(5, 7));
    QCOMPARE(r.first(), Q_INT64_C(2));
    QCOMPARE(r.last(),  Q_INT64_C(7));

    r.setRange(2, 7);
    r.merge(QtlRange(-100, 5));
    QCOMPARE(r.first(), Q_INT64_C(-100));
    QCOMPARE(r.last(),  Q_INT64_C(7));
}

void QtlRangeTest::testClip()
{
    QtlRange r;

    r.setRange(10, 20);
    r.clip(QtlRange(10, 20));
    QCOMPARE(r.first(), Q_INT64_C(10));
    QCOMPARE(r.last(),  Q_INT64_C(20));

    r.setRange(10, 20);
    r.clip(QtlRange(20, 50));
    QCOMPARE(r.first(), Q_INT64_C(20));
    QCOMPARE(r.last(),  Q_INT64_C(20));

    r.setRange(10, 20);
    r.clip(QtlRange(0, 12));
    QCOMPARE(r.first(), Q_INT64_C(10));
    QCOMPARE(r.last(),  Q_INT64_C(12));

    r.setRange(10, 20);
    r.clip(QtlRange(100, 200));
    QCOMPARE(r.first(), Q_INT64_C(100));
    QCOMPARE(r.last(),  Q_INT64_C(99));

    r.setRange(10, 20);
    r.clip(QtlRange(0, 2));
    QCOMPARE(r.first(), Q_INT64_C(3));
    QCOMPARE(r.last(),  Q_INT64_C(2));

    r.setRange(10, 20);
    r.clip(QtlRange(0, 15));
    QCOMPARE(r.first(), Q_INT64_C(10));
    QCOMPARE(r.last(),  Q_INT64_C(15));

    r.setRange(10, 20);
    r.clip(QtlRange(15, 200));
    QCOMPARE(r.first(), Q_INT64_C(15));
    QCOMPARE(r.last(),  Q_INT64_C(20));
}

void QtlRangeTest::testList()
{
    QtlRangeList l1(QtlRange(2, 27));
    QCOMPARE(l1.size(), 1);

    QtlRangeList l2({QtlRange(2, 27)});
    QCOMPARE(l2.size(), 1);

    QtlRangeList l3({QtlRange(2, 27), QtlRange(-2, 3)});
    QCOMPARE(l3.size(), 2);
}

void QtlRangeTest::testListTotalValueCount()
{
    QtlRangeList l;

    l << QtlRange(2, 27)
      << QtlRange(50, 220)
      << QtlRange()
      << QtlRange(0, 255);

    QCOMPARE(l.size(), 4);
    QCOMPARE(l.totalValueCount(), quint64(26 + 171 + 0 + 256));
}

void QtlRangeTest::testListAdd()
{
    QtlRangeList l;

    l << QtlRange(2,  27)
      << QtlRange(50, 220)
      << QtlRange(0,  QtlRange::RANGE_MAX);

    l.add(50);
    QCOMPARE(l.size(), 3);

    QCOMPARE(l[0].first(), Q_INT64_C(52));
    QCOMPARE(l[0].last(),  Q_INT64_C(77));
    QCOMPARE(l[1].first(), Q_INT64_C(100));
    QCOMPARE(l[1].last(),  Q_INT64_C(270));
    QCOMPARE(l[2].first(), Q_INT64_C(50));
    QCOMPARE(l[2].last(),  QtlRange::RANGE_MAX);

    l.clear();
    l << QtlRange(2,  27)
      << QtlRange(50, 220)
      << QtlRange(QtlRange::RANGE_MIN + 10, 255);

    l.add(-50);
    QCOMPARE(l.size(), 3);

    QCOMPARE(l[0].first(), Q_INT64_C(-48));
    QCOMPARE(l[0].last(),  Q_INT64_C(-23));
    QCOMPARE(l[1].first(), Q_INT64_C(0));
    QCOMPARE(l[1].last(),  Q_INT64_C(170));
    QCOMPARE(l[2].first(), QtlRange::RANGE_MIN);
    QCOMPARE(l[2].last(),  Q_INT64_C(205));
}

void QtlRangeTest::testListScale()
{
    QtlRangeList l;

    l << QtlRange(10, 12)
      << QtlRange(-10, 12)
      << QtlRange(5, QtlRange::RANGE_MAX)
      << QtlRange(QtlRange::RANGE_MAX - 20, QtlRange::RANGE_MAX - 10);

    l.scale(100);
    QCOMPARE(l.size(), 4);

    QCOMPARE(l[0].first(), Q_INT64_C(1000));
    QCOMPARE(l[0].last(),  Q_INT64_C(1299));
    QCOMPARE(l[1].first(), Q_INT64_C(0));
    QCOMPARE(l[1].last(),  Q_INT64_C(-1));
    QCOMPARE(l[2].first(), Q_INT64_C(500));
    QCOMPARE(l[2].last(),  QtlRange::RANGE_MAX);
    QCOMPARE(l[3].first(), QtlRange::RANGE_MAX);
    QCOMPARE(l[3].last(),  QtlRange::RANGE_MAX - 1);
}

void QtlRangeTest::testListSort()
{
    QtlRangeList l;

    l.sort();
    QVERIFY(l.isEmpty());

    l << QtlRange(2, 27)
      << QtlRange(-2, 47)
      << QtlRange(45, 53)
      << QtlRange(-2, 0)
      << QtlRange(44, 53)
      << QtlRange(-100, 1000)
      << QtlRange(1, -1);

    QCOMPARE(l.size(), 7);
    l.sort();
    QCOMPARE(l.size(), 7);

    QCOMPARE(l[0].first(), Q_INT64_C(-100));
    QCOMPARE(l[0].last(),  Q_INT64_C(1000));
    QCOMPARE(l[1].first(), Q_INT64_C(-2));
    QCOMPARE(l[1].last(),  Q_INT64_C(0));
    QCOMPARE(l[2].first(), Q_INT64_C(-2));
    QCOMPARE(l[2].last(),  Q_INT64_C(47));
    QCOMPARE(l[3].first(), Q_INT64_C(1));
    QCOMPARE(l[3].last(),  Q_INT64_C(0));
    QCOMPARE(l[4].first(), Q_INT64_C(2));
    QCOMPARE(l[4].last(),  Q_INT64_C(27));
    QCOMPARE(l[5].first(), Q_INT64_C(44));
    QCOMPARE(l[5].last(),  Q_INT64_C(53));
    QCOMPARE(l[6].first(), Q_INT64_C(45));
    QCOMPARE(l[6].last(),  Q_INT64_C(53));

    l.clear();
    l << QtlRange(105, 150)
      << QtlRange(28, 47)
      << QtlRange(2, 27)
      << QtlRange(100, 110)
      << QtlRange(-1, 2)
      << QtlRange(40, 45)
      << QtlRange(99, 99);

    l.sort();
    // Expected: (-1, 2) (2, 27) (28, 47) (40, 45) (99, 99) (100, 110) (105, 150)
    QCOMPARE(l.size(), 7);

    QCOMPARE(l[0].first(), Q_INT64_C(-1));
    QCOMPARE(l[0].last(),  Q_INT64_C(2));
    QCOMPARE(l[1].first(), Q_INT64_C(2));
    QCOMPARE(l[1].last(),  Q_INT64_C(27));
    QCOMPARE(l[2].first(), Q_INT64_C(28));
    QCOMPARE(l[2].last(),  Q_INT64_C(47));
    QCOMPARE(l[3].first(), Q_INT64_C(40));
    QCOMPARE(l[3].last(),  Q_INT64_C(45));
    QCOMPARE(l[4].first(), Q_INT64_C(99));
    QCOMPARE(l[4].last(),  Q_INT64_C(99));
    QCOMPARE(l[5].first(), Q_INT64_C(100));
    QCOMPARE(l[5].last(),  Q_INT64_C(110));
    QCOMPARE(l[6].first(), Q_INT64_C(105));
    QCOMPARE(l[6].last(),  Q_INT64_C(150));
}

void QtlRangeTest::testListMerge()
{
    QtlRangeList l;

    l << QtlRange(105, 150)
      << QtlRange(28, 47)
      << QtlRange(2, 27)
      << QtlRange(100, 110)
      << QtlRange(-1, 2)
      << QtlRange(40, 45)
      << QtlRange(99, 99);

    QCOMPARE(l.size(), 7);
    l.merge(Qtl::Sorted | Qtl::NoDuplicate);
    // Expected: (-1, 47) (99, 150)
    QCOMPARE(l.size(), 2);

    QCOMPARE(l[0].first(), Q_INT64_C(-1));
    QCOMPARE(l[0].last(),  Q_INT64_C(47));
    QCOMPARE(l[1].first(), Q_INT64_C(99));
    QCOMPARE(l[1].last(),  Q_INT64_C(150));

    l.clear();
    l << QtlRange(105, 150)
      << QtlRange(28, 47)
      << QtlRange(2, 27)
      << QtlRange(100, 110)
      << QtlRange(-1, 2)
      << QtlRange(40, 45)
      << QtlRange(99, 99);

    QCOMPARE(l.size(), 7);
    l.merge(Qtl::Sorted | Qtl::AdjacentOnly);
    // Expected: (-1, 2) (2, 47) (40, 45) (99, 110) (105, 150)
    QCOMPARE(l.size(), 5);

    QCOMPARE(l[0].first(), Q_INT64_C(-1));
    QCOMPARE(l[0].last(),  Q_INT64_C(2));
    QCOMPARE(l[1].first(), Q_INT64_C(2));
    QCOMPARE(l[1].last(),  Q_INT64_C(47));
    QCOMPARE(l[2].first(), Q_INT64_C(40));
    QCOMPARE(l[2].last(),  Q_INT64_C(45));
    QCOMPARE(l[3].first(), Q_INT64_C(99));
    QCOMPARE(l[3].last(),  Q_INT64_C(110));
    QCOMPARE(l[4].first(), Q_INT64_C(105));
    QCOMPARE(l[4].last(),  Q_INT64_C(150));

    l.clear();
    l << QtlRange(105, 150)
      << QtlRange(2, 100)
      << QtlRange(100, 110)
      << QtlRange(111, 122)
      << QtlRange(99, 99);

    QCOMPARE(l.size(), 5);
    l.merge(Qtl::AdjacentOnly);
    // Expected: (105, 150) (2, 100) (100, 122) (99, 99)
    QCOMPARE(l.size(), 4);

    QCOMPARE(l[0].first(), Q_INT64_C(105));
    QCOMPARE(l[0].last(),  Q_INT64_C(150));
    QCOMPARE(l[1].first(), Q_INT64_C(2));
    QCOMPARE(l[1].last(),  Q_INT64_C(100));
    QCOMPARE(l[2].first(), Q_INT64_C(100));
    QCOMPARE(l[2].last(),  Q_INT64_C(122));
    QCOMPARE(l[3].first(), Q_INT64_C(99));
    QCOMPARE(l[3].last(),  Q_INT64_C(99));
}

void QtlRangeTest::testListClip()
{
    QtlRangeList l;

    l << QtlRange(105, 150)
      << QtlRange(28,  47)
      << QtlRange(2,   27)
      << QtlRange(100, 110)
      << QtlRange(-1,  2)
      << QtlRange(40,  45)
      << QtlRange(99,  99);

    QCOMPARE(l.size(), 7);
    l.clip(QtlRange(10, 99));
    QCOMPARE(l.size(), 4);

    QCOMPARE(l[0].first(), Q_INT64_C(28));
    QCOMPARE(l[0].last(),  Q_INT64_C(47));
    QCOMPARE(l[1].first(), Q_INT64_C(10));
    QCOMPARE(l[1].last(),  Q_INT64_C(27));
    QCOMPARE(l[2].first(), Q_INT64_C(40));
    QCOMPARE(l[2].last(),  Q_INT64_C(45));
    QCOMPARE(l[3].first(), Q_INT64_C(99));
    QCOMPARE(l[3].last(),  Q_INT64_C(99));
}
