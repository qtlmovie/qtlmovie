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
// Unit test for class QtlVariable
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlVariable.h"

class QtlVariableTest : public QObject
{
    Q_OBJECT
private slots:
    void testElementaryType();
    void testClass();
};

#include "QtlVariableTest.moc"
QTL_TEST_CLASS(QtlVariableTest);

//----------------------------------------------------------------------------

// Test case: usage on elementary types.
void QtlVariableTest::testElementaryType()
{
    typedef QtlVariable<int> IntVariable;

    IntVariable v1;
    QVERIFY(!v1.set());
    QTLVERIFY_THROW(v1.value(), QtlUninitializedException);

    IntVariable v2(v1);
    QVERIFY(!v2.set());
    QTLVERIFY_THROW(v2.value(), QtlUninitializedException);

    v2 = 1;
    QVERIFY(v2.set());
    QCOMPARE(v2.value(), 1);

    IntVariable v3(v2);
    QVERIFY(v3.set());
    QCOMPARE(v3.value(), 1);

    IntVariable v4(2);
    QVERIFY(v4.set());
    QCOMPARE(v4.value(), 2);

    v4 = v1;
    QVERIFY(!v4.set());

    v4 = v2;
    QVERIFY(v4.set());
    QCOMPARE(v4.value(), 1);

    v4.reset();
    QVERIFY(!v4.set());

    v4.reset();
    QVERIFY(!v4.set());

    v1 = 1;
    v2.reset();
    QVERIFY(v1.set());
    QVERIFY(!v2.set());
    QCOMPARE(v1.value(), 1);
    QCOMPARE(v1.value(2), 1);
    QCOMPARE(v2.value(2), 2);

    v1 = 1;
    v2 = 1;
    v3 = 3;
    v4.reset();
    IntVariable v5;
    QVERIFY(v1.set());
    QVERIFY(v2.set());
    QVERIFY(v3.set());
    QVERIFY(!v4.set());
    QVERIFY(!v5.set());
    QVERIFY(v1 == v2);
    QVERIFY(v1 != v3);
    QVERIFY(v1 != v4);
    QVERIFY(v4 != v5);
    QVERIFY(v1 == 1);
    QVERIFY(v1 != 2);
    QVERIFY(v4 != 1);
}

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
        // Constructors
        TestData(int value) : _value(value) {_instanceCount++;}
        TestData(const TestData& other) : _value(other._value) {_instanceCount++;}
        TestData& operator=(const TestData& other) {_value = other._value; return *this;}
        bool operator==(const TestData& other) {return _value == other._value;}

        // Destructor
        ~TestData() {_instanceCount--;}

        // Get the object's value
        int v() const {return _value;}

        // Get the number of instances
        static int instanceCount() {return _instanceCount;}
    };

    int TestData::_instanceCount = 0;
}

// Test case: usage on class types.
void QtlVariableTest::testClass()
{
    typedef QtlVariable<TestData> TestVariable;

    QVERIFY(TestData::instanceCount() == 0);
    {
        TestVariable v1;
        QVERIFY(!v1.set());
        QTLVERIFY_THROW(v1.value().v(), QtlUninitializedException);
        QCOMPARE(TestData::instanceCount(), 0);

        TestVariable v2(v1);
        QVERIFY(!v2.set());
        QCOMPARE(TestData::instanceCount(), 0);

        v2 = TestData(1);
        QVERIFY(v2.set());
        QCOMPARE(v2.value().v(), 1);
        QCOMPARE(TestData::instanceCount(), 1);

        TestVariable v3(v2);
        QVERIFY(v3.set());
        QCOMPARE(TestData::instanceCount(), 2);

        TestVariable v4(TestData(2));
        QVERIFY(v4.set());
        QCOMPARE(TestData::instanceCount(), 3);

        v4 = v1;
        QVERIFY(!v4.set());
        QCOMPARE(TestData::instanceCount(), 2);

        v4 = v2;
        QVERIFY(v4.set());
        QCOMPARE(TestData::instanceCount(), 3);

        v4.reset();
        QVERIFY(!v4.set());
        QCOMPARE(TestData::instanceCount(), 2);

        v4.reset();
        QVERIFY(!v4.set());
        QCOMPARE(TestData::instanceCount(), 2);

        v1 = TestData(1);
        QCOMPARE(TestData::instanceCount(), 3);
        v2.reset();
        QCOMPARE(TestData::instanceCount(), 2);
        QVERIFY(v1.set());
        QVERIFY(!v2.set());
        QCOMPARE(v1.value().v(), 1);
        QCOMPARE(v1.value(TestData(2)).v(), 1);
        QCOMPARE(v2.value(TestData(2)).v(), 2);
        QCOMPARE(TestData::instanceCount(), 2);

        v1 = TestData(1);
        QCOMPARE(TestData::instanceCount(), 2);
        v2 = TestData(1);
        QCOMPARE(TestData::instanceCount(), 3);
        v3 = TestData(3);
        QCOMPARE(TestData::instanceCount(), 3);
        v4.reset();
        QCOMPARE(TestData::instanceCount(), 3);
        TestVariable v5;
        QCOMPARE(TestData::instanceCount(), 3);
        QVERIFY(v1.set());
        QVERIFY(v2.set());
        QVERIFY(v3.set());
        QVERIFY(!v4.set());
        QVERIFY(!v5.set());
        QVERIFY(v1 == v2);
        QVERIFY(v1 != v3);
        QVERIFY(v1 != v4);
        QVERIFY(v4 != v5);
        QCOMPARE(v1.value().v(), 1);
        QVERIFY(v1 == TestData(1));
        QVERIFY(v1 != TestData(2));
        QVERIFY(v4 != TestData(1));
        QCOMPARE(TestData::instanceCount(), 3);
    }
    // Check that the destructor of variable properly destroys the contained object
    QCOMPARE(TestData::instanceCount(), 0);
}
