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
    void testRanges();
    void testOverlap();
    void testAdjacent();
    void testMerge();
    void testLists();
    void testListSort();
    void testListMerge();
};

#include "QtlRangeTest.moc"
QTL_TEST_CLASS(QtlRangeTest);

void QtlRangeTest::testRanges()
{
    QtlRange<int> r;
    QVERIFY(r.isEmpty());
    QCOMPARE(r.first(), 1);
    QCOMPARE(r.last(), 0);
    QCOMPARE(r.count(), Q_UINT64_C(0));

    QVERIFY(QtlRange<int>(1, 4) == QtlRange<int>(1, 4));
    QVERIFY(QtlRange<int>(1, 4) <= QtlRange<int>(1, 4));
    QVERIFY(QtlRange<int>(1, 4) >= QtlRange<int>(1, 4));
    QVERIFY(QtlRange<int>(1, 4) != QtlRange<int>(1, 3));
    QVERIFY(QtlRange<int>(1, 4) != QtlRange<int>(0, 4));
    QVERIFY(QtlRange<int>(1, 4) <  QtlRange<int>(1, 5));
    QVERIFY(QtlRange<int>(1, 4) <= QtlRange<int>(1, 5));
    QVERIFY(QtlRange<int>(1, 4) <  QtlRange<int>(2, 5));
    QVERIFY(QtlRange<int>(1, 4) <= QtlRange<int>(2, 5));
    QVERIFY(QtlRange<int>(1, 4) >  QtlRange<int>(-1, 4));
    QVERIFY(QtlRange<int>(1, 4) >= QtlRange<int>(-1, 4));
    QVERIFY(QtlRange<int>(1, 4) >  QtlRange<int>(1, 3));
    QVERIFY(QtlRange<int>(1, 4) >= QtlRange<int>(1, 3));

    r.setFirst(27);
    QCOMPARE(r.first(), 27);
    QCOMPARE(r.last(), 26);
    QCOMPARE(r.count(), Q_UINT64_C(0));

    r.setLast(12);
    QCOMPARE(r.first(), 27);
    QCOMPARE(r.last(), 26);
    QCOMPARE(r.count(), Q_UINT64_C(0));

    r.setLast(37);
    QCOMPARE(r.first(), 27);
    QCOMPARE(r.last(), 37);
    QCOMPARE(r.count(), Q_UINT64_C(11));

    r.setFirst(20);
    QCOMPARE(r.first(), 20);
    QCOMPARE(r.last(), 37);
    QCOMPARE(r.count(), Q_UINT64_C(18));

    r.setRange(-7, -3);
    QCOMPARE(r.first(), -7);
    QCOMPARE(r.last(), -3);
    QCOMPARE(r.count(), Q_UINT64_C(5));

    r.clear();
    QVERIFY(r.isEmpty());
    QCOMPARE(r.first(), -7);
    QCOMPARE(r.last(), -8);
    QCOMPARE(r.count(), Q_UINT64_C(0));
}

void QtlRangeTest::testOverlap()
{
    QVERIFY(QtlRange<int>(1, 4).overlap(QtlRange<int>(1, 4)));
    QVERIFY(QtlRange<int>(1, 4).overlap(QtlRange<int>(1, 1)));
    QVERIFY(QtlRange<int>(1, 4).overlap(QtlRange<int>(4, 4)));
    QVERIFY(QtlRange<int>(1, 4).overlap(QtlRange<int>(4, 7)));
    QVERIFY(QtlRange<int>(1, 4).overlap(QtlRange<int>(1, 4)));
    QVERIFY(QtlRange<int>(1, 1).overlap(QtlRange<int>(1, 4)));
    QVERIFY(QtlRange<int>(4, 4).overlap(QtlRange<int>(1, 4)));
    QVERIFY(QtlRange<int>(4, 7).overlap(QtlRange<int>(1, 4)));
    QVERIFY(QtlRange<int>(1, 4).overlap(QtlRange<int>(-10, 40)));

    QVERIFY(!QtlRange<int>(1, 4).overlap(QtlRange<int>(5, 7)));
    QVERIFY(!QtlRange<int>(1, 4).overlap(QtlRange<int>(-10, 0)));
    QVERIFY(!QtlRange<int>(1, 4).overlap(QtlRange<int>(1000, 7)));
    QVERIFY(!QtlRange<int>(1, 4).overlap(QtlRange<int>(-500, -10)));
    QVERIFY(!QtlRange<int>(1, 4).overlap(QtlRange<int>(500, 700)));
}

void QtlRangeTest::testAdjacent()
{
    QVERIFY(QtlRange<int>(1, 4).adjacent(QtlRange<int>(5, 10), Qtl::AdjacentBefore));
    QVERIFY(!QtlRange<int>(1, 4).adjacent(QtlRange<int>(5, 10), Qtl::AdjacentAfter));
    QVERIFY(QtlRange<int>(1, 4).adjacent(QtlRange<int>(5, 10), Qtl::AdjacentAny));

    QVERIFY(!QtlRange<int>(1, 4).adjacent(QtlRange<int>(6, 10), Qtl::AdjacentBefore));
    QVERIFY(!QtlRange<int>(1, 4).adjacent(QtlRange<int>(6, 10), Qtl::AdjacentAfter));
    QVERIFY(!QtlRange<int>(1, 4).adjacent(QtlRange<int>(6, 10), Qtl::AdjacentAny));

    QVERIFY(!QtlRange<int>(1, 4).adjacent(QtlRange<int>(-10, 0), Qtl::AdjacentBefore));
    QVERIFY(QtlRange<int>(1, 4).adjacent(QtlRange<int>(-10, 0), Qtl::AdjacentAfter));
    QVERIFY(QtlRange<int>(1, 4).adjacent(QtlRange<int>(-10, 0), Qtl::AdjacentAny));

    QVERIFY(!QtlRange<int>(2, 7).adjacent(QtlRange<int>(2, 1), Qtl::AdjacentBefore));
    QVERIFY(QtlRange<int>(2, 7).adjacent(QtlRange<int>(2, 1), Qtl::AdjacentAfter));
    QVERIFY(QtlRange<int>(2, 7).adjacent(QtlRange<int>(2, 1), Qtl::AdjacentAny));

    QVERIFY(QtlRange<int>(2, 0).adjacent(QtlRange<int>(2, 1), Qtl::AdjacentBefore));
    QVERIFY(QtlRange<int>(2, 0).adjacent(QtlRange<int>(2, 1), Qtl::AdjacentAfter));
    QVERIFY(QtlRange<int>(2, 0).adjacent(QtlRange<int>(2, 1), Qtl::AdjacentAny));
}

void QtlRangeTest::testMerge()
{
    QtlRange<int> r;

    r.setRange(2, 7);
    r.merge(QtlRange<int>(100, 150));
    QCOMPARE(r.first(), 2);
    QCOMPARE(r.last(), 150);

    r.setRange(2, 7);
    r.merge(QtlRange<int>(-100, 0));
    QCOMPARE(r.first(), -100);
    QCOMPARE(r.last(), 7);

    r.setRange(2, 7);
    r.merge(QtlRange<int>(3, 6));
    QCOMPARE(r.first(), 2);
    QCOMPARE(r.last(), 7);

    r.setRange(2, 7);
    r.merge(QtlRange<int>(2, 5));
    QCOMPARE(r.first(), 2);
    QCOMPARE(r.last(), 7);

    r.setRange(2, 7);
    r.merge(QtlRange<int>(2, 20));
    QCOMPARE(r.first(), 2);
    QCOMPARE(r.last(), 20);

    r.setRange(2, 7);
    r.merge(QtlRange<int>(0, 7));
    QCOMPARE(r.first(), 0);
    QCOMPARE(r.last(), 7);

    r.setRange(2, 7);
    r.merge(QtlRange<int>(5, 7));
    QCOMPARE(r.first(), 2);
    QCOMPARE(r.last(), 7);

    r.setRange(2, 7);
    r.merge(QtlRange<int>(-100, 5));
    QCOMPARE(r.first(), -100);
    QCOMPARE(r.last(), 7);
}

void QtlRangeTest::testLists()
{
    QtlRangeList<int> l1(QtlRange<int>(2, 27));
    QCOMPARE(l1.size(), 1);

    QtlRangeList<int> l2({QtlRange<int>(2, 27)});
    QCOMPARE(l2.size(), 1);

    QtlRangeList<int> l3({QtlRange<int>(2, 27), QtlRange<int>(-2, 3)});
    QCOMPARE(l3.size(), 2);
}

void QtlRangeTest::testListSort()
{
    QtlRangeList<int> l;

    l.sort();
    QVERIFY(l.isEmpty());

    l << QtlRange<int>(2, 27)
      << QtlRange<int>(-2, 47)
      << QtlRange<int>(45, 53)
      << QtlRange<int>(-2, 0)
      << QtlRange<int>(44, 53)
      << QtlRange<int>(-100, 1000)
      << QtlRange<int>(1, -1);

    QCOMPARE(l.size(), 7);
    l.sort();
    QCOMPARE(l.size(), 7);

    QCOMPARE(l[0].first(), -100);
    QCOMPARE(l[0].last(), 1000);
    QCOMPARE(l[1].first(), -2);
    QCOMPARE(l[1].last(), 0);
    QCOMPARE(l[2].first(), -2);
    QCOMPARE(l[2].last(), 47);
    QCOMPARE(l[3].first(), 1);
    QCOMPARE(l[3].last(), 0);
    QCOMPARE(l[4].first(), 2);
    QCOMPARE(l[4].last(), 27);
    QCOMPARE(l[5].first(), 44);
    QCOMPARE(l[5].last(), 53);
    QCOMPARE(l[6].first(), 45);
    QCOMPARE(l[6].last(), 53);

    l.clear();
    l << QtlRange<int>(105, 150)
      << QtlRange<int>(28, 47)
      << QtlRange<int>(2, 27)
      << QtlRange<int>(100, 110)
      << QtlRange<int>(-1, 2)
      << QtlRange<int>(40, 45)
      << QtlRange<int>(99, 99);

    l.sort();
    // Expected: (-1, 2) (2, 27) (28, 47) (40, 45) (99, 99) (100, 110) (105, 150)
    QCOMPARE(l.size(), 7);

    QCOMPARE(l[0].first(), -1);
    QCOMPARE(l[0].last(), 2);
    QCOMPARE(l[1].first(), 2);
    QCOMPARE(l[1].last(), 27);
    QCOMPARE(l[2].first(), 28);
    QCOMPARE(l[2].last(), 47);
    QCOMPARE(l[3].first(), 40);
    QCOMPARE(l[3].last(), 45);
    QCOMPARE(l[4].first(), 99);
    QCOMPARE(l[4].last(), 99);
    QCOMPARE(l[5].first(), 100);
    QCOMPARE(l[5].last(), 110);
    QCOMPARE(l[6].first(), 105);
    QCOMPARE(l[6].last(), 150);
}

void QtlRangeTest::testListMerge()
{
    QtlRangeList<int> l;

    l << QtlRange<int>(105, 150)
      << QtlRange<int>(28, 47)
      << QtlRange<int>(2, 27)
      << QtlRange<int>(100, 110)
      << QtlRange<int>(-1, 2)
      << QtlRange<int>(40, 45)
      << QtlRange<int>(99, 99);

    QCOMPARE(l.size(), 7);
    l.merge(Qtl::Sorted | Qtl::NoDuplicate);
    // Expected: (-1, 47) (99, 150)
    QCOMPARE(l.size(), 2);

    QCOMPARE(l[0].first(), -1);
    QCOMPARE(l[0].last(), 47);
    QCOMPARE(l[1].first(), 99);
    QCOMPARE(l[1].last(), 150);

    l.clear();
    l << QtlRange<int>(105, 150)
      << QtlRange<int>(28, 47)
      << QtlRange<int>(2, 27)
      << QtlRange<int>(100, 110)
      << QtlRange<int>(-1, 2)
      << QtlRange<int>(40, 45)
      << QtlRange<int>(99, 99);

    QCOMPARE(l.size(), 7);
    l.merge(Qtl::Sorted | Qtl::AdjacentOnly);
    // Expected: (-1, 2) (2, 47) (40, 45) (99, 110) (105, 150)
    QCOMPARE(l.size(), 5);

    QCOMPARE(l[0].first(), -1);
    QCOMPARE(l[0].last(), 2);
    QCOMPARE(l[1].first(), 2);
    QCOMPARE(l[1].last(), 47);
    QCOMPARE(l[2].first(), 40);
    QCOMPARE(l[2].last(), 45);
    QCOMPARE(l[3].first(), 99);
    QCOMPARE(l[3].last(), 110);
    QCOMPARE(l[4].first(), 105);
    QCOMPARE(l[4].last(), 150);

    l.clear();
    l << QtlRange<int>(105, 150)
      << QtlRange<int>(2, 100)
      << QtlRange<int>(100, 110)
      << QtlRange<int>(111, 122)
      << QtlRange<int>(99, 99);

    QCOMPARE(l.size(), 5);
    l.merge(Qtl::AdjacentOnly);
    // Expected: (105, 150) (2, 100) (100, 122) (99, 99)
    QCOMPARE(l.size(), 4);

    QCOMPARE(l[0].first(), 105);
    QCOMPARE(l[0].last(), 150);
    QCOMPARE(l[1].first(), 2);
    QCOMPARE(l[1].last(), 100);
    QCOMPARE(l[2].first(), 100);
    QCOMPARE(l[2].last(), 122);
    QCOMPARE(l[3].first(), 99);
    QCOMPARE(l[3].last(), 99);
}
