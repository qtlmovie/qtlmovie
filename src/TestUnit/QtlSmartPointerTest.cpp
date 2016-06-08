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
// Unit test for class QtlSmartPointer
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlSmartPointer.h"

class QtlSmartPointerTest : public QObject
{
    Q_OBJECT
private slots:
    void testSmartPointer();
    void testDowncast();
    void testUpcast();
    void testChangeMutex();
};

#include "QtlSmartPointerTest.moc"
QTL_TEST_CLASS(QtlSmartPointerTest);

//----------------------------------------------------------------------------

// A class which identifies each instance by an explicit value.
// Also count the number of instances in the class.
namespace {
    class TestData
    {
    private:
        TestData(); // inaccessible
        int _value;
        static int _instanceCount;
    public:
        // Constructor
        TestData(int value) : _value(value) {_instanceCount++;}
        TestData(const TestData& other) : _value(other._value) {_instanceCount++;}

        // Destructor
        virtual ~TestData() {_instanceCount--;}

        // Get the object's value
        int value() const {return _value;}

        // Get the number of instances
        static int instanceCount() {return _instanceCount;}
    };

    int TestData::_instanceCount = 0;

    typedef QtlSmartPointer<TestData,QtlMutexLocker> TestDataPtr;
}

// Test case: check various object and pointer movements
void QtlSmartPointerTest::testSmartPointer()
{
    TestDataPtr p1;

    QVERIFY(p1.isNull() == true);
    QVERIFY(p1.count() == 1);
    QVERIFY(TestData::instanceCount() == 0);

    p1.reset(new TestData(12));

    QVERIFY(p1.isNull() == false);
    QVERIFY(p1.count() == 1);
    QVERIFY((*p1).value() == 12);
    QVERIFY(p1->value() == 12);
    QVERIFY(p1.pointer()->value() == 12);
    QVERIFY(TestData::instanceCount() == 1);

    TestDataPtr p2(p1);

    QVERIFY(p1.count() == 2);
    QVERIFY(p2.count() == 2);
    QVERIFY(p1.isNull() == false);
    QVERIFY(p2.isNull() == false);
    QVERIFY(p1->value() == 12);
    QVERIFY(p2->value() == 12);
    QVERIFY(TestData::instanceCount() == 1);

    {
        TestDataPtr p3(p2);

        QVERIFY(p1.count() == 3);
        QVERIFY(p2.count() == 3);
        QVERIFY(p3.count() == 3);
        QVERIFY(p1.isNull() == false);
        QVERIFY(p2.isNull() == false);
        QVERIFY(p3.isNull() == false);
        QVERIFY(p1->value() == 12);
        QVERIFY(p2->value() == 12);
        QVERIFY(p3->value() == 12);
        QVERIFY(TestData::instanceCount() == 1);
    }

    QVERIFY(p1.count() == 2);
    QVERIFY(p2.count() == 2);
    QVERIFY(p1.isNull() == false);
    QVERIFY(p2.isNull() == false);
    QVERIFY(p1->value() == 12);
    QVERIFY(p2->value() == 12);
    QVERIFY(TestData::instanceCount() == 1);

    TestDataPtr p3;

    QVERIFY(p1.count() == 2);
    QVERIFY(p2.count() == 2);
    QVERIFY(p3.count() == 1);
    QVERIFY((p1 == p3) == false);
    QVERIFY((p1 != p3) == true);
    QVERIFY(p1.isNull() == false);
    QVERIFY(p2.isNull() == false);
    QVERIFY(p3.isNull() == true);
    QVERIFY(p1->value() == 12);
    QVERIFY(p2->value() == 12);
    QVERIFY(TestData::instanceCount() == 1);

    p3 = p1;

    QVERIFY(p1.count() == 3);
    QVERIFY(p2.count() == 3);
    QVERIFY(p3.count() == 3);
    QVERIFY((p1 == p3) == true);
    QVERIFY((p1 != p3) == false);
    QVERIFY(p1.isNull() == false);
    QVERIFY(p2.isNull() == false);
    QVERIFY(p3.isNull() == false);
    QVERIFY(p1->value() == 12);
    QVERIFY(p2->value() == 12);
    QVERIFY(p3->value() == 12);
    QVERIFY(TestData::instanceCount() == 1);

    {
        TestData* tmp = new TestData(27);
        QVERIFY(TestData::instanceCount() == 2);
        p2.reset(tmp);
    }

    QVERIFY(p1.count() == 3);
    QVERIFY(p2.count() == 3);
    QVERIFY(p3.count() == 3);
    QVERIFY((p1 == p2) == true);
    QVERIFY((p1 == p3) == true);
    QVERIFY(p1.isNull() == false);
    QVERIFY(p2.isNull() == false);
    QVERIFY(p3.isNull() == false);
    QVERIFY(p1->value() == 27);
    QVERIFY(p2->value() == 27);
    QVERIFY(p3->value() == 27);
    QVERIFY(TestData::instanceCount() == 1);

    p2 = new TestData(41);

    QVERIFY(p1.count() == 2);
    QVERIFY(p2.count() == 1);
    QVERIFY(p3.count() == 2);
    QVERIFY(p1.isNull() == false);
    QVERIFY(p2.isNull() == false);
    QVERIFY(p3.isNull() == false);
    QVERIFY(p1->value() == 27);
    QVERIFY(p2->value() == 41);
    QVERIFY(p3->value() == 27);
    QVERIFY(TestData::instanceCount() == 2);

    {
        // Object pointed by p2 no longer managed but still valid
        TestData* px = p2.release();

        QVERIFY(p1.count() == 2);
        QVERIFY(p2.count() == 1);
        QVERIFY(p3.count() == 2);
        QVERIFY(p1.isNull() == false);
        QVERIFY(p2.isNull() == true);
        QVERIFY(p3.isNull() == false);
        QVERIFY(p1->value() == 27);
        QVERIFY(px->value() == 41);
        QVERIFY(p3->value() == 27);
        QVERIFY(TestData::instanceCount() == 2);

        // Now explicitely deallocate object(was no longer managed)
        delete px;
        QVERIFY(TestData::instanceCount() == 1);
    }

    p3 = new TestData(76);

    QVERIFY(p1.count() == 1);
    QVERIFY(p2.count() == 1);
    QVERIFY(p3.count() == 1);
    QVERIFY(p1.isNull() == false);
    QVERIFY(p2.isNull() == true);
    QVERIFY(p3.isNull() == false);
    QVERIFY(p1->value() == 27);
    QVERIFY(p3->value() == 76);
    QVERIFY(TestData::instanceCount() == 2);

    {
        TestDataPtr p4(p1);

        QVERIFY(p1.count() == 2);
        QVERIFY(p2.count() == 1);
        QVERIFY(p3.count() == 1);
        QVERIFY(p4.count() == 2);
        QVERIFY(p1.isNull() == false);
        QVERIFY(p2.isNull() == true);
        QVERIFY(p3.isNull() == false);
        QVERIFY(p4.isNull() == false);
        QVERIFY(p1->value() == 27);
        QVERIFY(p3->value() == 76);
        QVERIFY(p4->value() == 27);
        QVERIFY(TestData::instanceCount() == 2);

        p1 = 0;

        QVERIFY(p1.count() == 1);
        QVERIFY(p2.count() == 1);
        QVERIFY(p3.count() == 1);
        QVERIFY(p4.count() == 1);
        QVERIFY(p1.isNull() == true);
        QVERIFY(p2.isNull() == true);
        QVERIFY(p3.isNull() == false);
        QVERIFY(p4.isNull() == false);
        QVERIFY(p3->value() == 76);
        QVERIFY(p4->value() == 27);
        QVERIFY(TestData::instanceCount() == 2);

        p3 = 0;

        QVERIFY(p1.count() == 1);
        QVERIFY(p2.count() == 1);
        QVERIFY(p3.count() == 1);
        QVERIFY(p4.count() == 1);
        QVERIFY(p1.isNull() == true);
        QVERIFY(p2.isNull() == true);
        QVERIFY(p3.isNull() == true);
        QVERIFY(p4.isNull() == false);
        QVERIFY(p4->value() == 27);
        QVERIFY(TestData::instanceCount() == 1);
    }

    QVERIFY(TestData::instanceCount() == 0);
}

// Two subclasses to test downcasts.
namespace {
    class SubTestData1: public TestData
    {
    public:
        // Constructor
        SubTestData1(int value) : TestData(value) {}
    };

    class SubTestData2: public TestData
    {
    public:
        // Constructor
        SubTestData2(int value) : TestData(value) {}
    };

    typedef QtlSmartPointer<SubTestData1,QtlMutexLocker> SubTestData1Ptr;
    typedef QtlSmartPointer<SubTestData2,QtlMutexLocker> SubTestData2Ptr;
}

// Test case: check downcasts
void QtlSmartPointerTest::testDowncast()
{
    QVERIFY(TestData::instanceCount() == 0);
    TestDataPtr p(new SubTestData2(666));
    QVERIFY(TestData::instanceCount() == 1);
    QVERIFY(!p.isNull());

#if !defined(QTL_NO_RTTI)
    SubTestData1Ptr p1(p.downcast<SubTestData1>());
    QVERIFY(p1.isNull() == true);
    QVERIFY(p.isNull() == false);
    QVERIFY(TestData::instanceCount() == 1);
#endif

    SubTestData2Ptr p2(p.downcast<SubTestData2>());
    QVERIFY(p2.isNull() == false);
    QVERIFY(p.isNull() == true);
    QVERIFY(TestData::instanceCount() == 1);
    QVERIFY(p2->value() == 666);

    p2.clear();
    QVERIFY(TestData::instanceCount() == 0);
}

// Test case: check upcasts
void QtlSmartPointerTest::testUpcast()
{
    QVERIFY(TestData::instanceCount() == 0);
    SubTestData1Ptr p1(new SubTestData1(777));
    QVERIFY(TestData::instanceCount() == 1);
    QVERIFY(p1.isNull() == false);

    TestDataPtr p(p1.upcast<TestData>());
    QVERIFY(p.isNull() == false);
    QVERIFY(p1.isNull() == true);
    QVERIFY(TestData::instanceCount() == 1);
    QVERIFY(p->value() == 777);

    p.clear();
    QVERIFY(TestData::instanceCount() == 0);
}

// Test case: check mutex type change
void QtlSmartPointerTest::testChangeMutex()
{
    QVERIFY(TestData::instanceCount() == 0);
    QtlSmartPointer<TestData,QtlNullMutexLocker> pn(new TestData(888));
    QVERIFY(TestData::instanceCount() == 1);
    QVERIFY(pn.isNull() == false);

    QtlSmartPointer<TestData,QtlMutexLocker> pt(pn.changeMutex<QtlMutexLocker>());
    QVERIFY(pt.isNull() == false);
    QVERIFY(pn.isNull() == true);
    QVERIFY(TestData::instanceCount() == 1);
    QVERIFY(pt->value() == 888);

    pt.clear();
    QVERIFY(TestData::instanceCount() == 0);
}
