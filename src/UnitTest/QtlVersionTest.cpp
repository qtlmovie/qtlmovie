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
// Unit test for class QtlVersion
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlVersion.h"

class QtlVersionTest : public QObject
{
    Q_OBJECT
private slots:
    void testConstructors();
    void testOperations();
};

#include "QtlVersionTest.moc"
QTL_TEST_CLASS(QtlVersionTest);

//----------------------------------------------------------------------------

// Test case: constructors.
void QtlVersionTest::testConstructors()
{
    QtlVersion v1;
    QVERIFY(!v1.isValid());
    QVERIFY(v1.elementCount() == 0);
    QVERIFY(v1.values().isEmpty());
    QVERIFY(v1.text().isEmpty());

    QList<int> values;
    values << 3 << 1 << 27;

    QtlVersion v2(values);
    QVERIFY(v2.isValid());
    QVERIFY(v2.elementCount() == 3);
    QVERIFY(v2.values().size() == 3);
    QVERIFY(v2.values() == values);
    QVERIFY(v2.text() == "3.1.27");

    QtlVersion v3("1.34-56.1");
    QVERIFY(v3.isValid());
    QVERIFY(v3.elementCount() == 4);
    QVERIFY(v3.values().size() == 4);
    QVERIFY(v3.values()[0] == 1);
    QVERIFY(v3.values()[1] == 34);
    QVERIFY(v3.values()[2] == 56);
    QVERIFY(v3.values()[3] == 1);
    QVERIFY(v3.text() == "1.34-56.1");

    QtlVersion v4(v3);
    QVERIFY(v4.isValid());
    QVERIFY(v4.elementCount() == 4);
    QVERIFY(v4.values().size() == 4);
    QVERIFY(v4.values()[0] == 1);
    QVERIFY(v4.values()[1] == 34);
    QVERIFY(v4.values()[2] == 56);
    QVERIFY(v4.values()[3] == 1);
    QVERIFY(v4.text() == "1.34-56.1");
    QVERIFY(v4 == v3);
}

// Test case: basic operations.
void QtlVersionTest::testOperations()
{
    QtlVersion v1, v2;

    QList<int> values;
    values << 3 << 1 << 27;

    v1.setValues(values);
    QVERIFY(v1.isValid());
    QVERIFY(v1.elementCount() == 3);
    QVERIFY(v1.values().size() == 3);
    QVERIFY(v1.values() == values);
    QVERIFY(v1.text() == "3.1.27");

    v2.setText("3.1-27");
    QVERIFY(v2.isValid());
    QVERIFY(v2.elementCount() == 3);
    QVERIFY(v2.values().size() == 3);
    QVERIFY(v2.values() == values);
    QVERIFY(v2.text() == "3.1-27");

    QVERIFY(v1 == v2);
    QVERIFY(!(v1 != v2));
    QVERIFY(!(v1 < v2));
    QVERIFY(v1 <= v2);
    QVERIFY(!(v1 > v2));
    QVERIFY(v1 >= v2);

    v1.clear();
    QVERIFY(!v1.isValid());
    QVERIFY(v1.elementCount() == 0);
    QVERIFY(v1.values().isEmpty());
    QVERIFY(v1.text().isEmpty());
    QVERIFY(v1 != v2);

    v1 = v2;
    QVERIFY(v1.isValid());
    QVERIFY(v1.elementCount() == 3);
    QVERIFY(v1.values().size() == 3);
    QVERIFY(v1.values() == values);
    QVERIFY(v1.text() == "3.1-27");

    QVERIFY(QtlVersion("1.3.4") == QtlVersion("1-3.4"));
    QVERIFY(QtlVersion("1.3.4") < QtlVersion("1.3.5"));
    QVERIFY(QtlVersion("1.3.4") > QtlVersion("1.3"));
    QVERIFY(QtlVersion("1.2") > QtlVersion("1.1.34"));
    QVERIFY(QtlVersion("1.3.1") < QtlVersion("1.3.2"));
}
