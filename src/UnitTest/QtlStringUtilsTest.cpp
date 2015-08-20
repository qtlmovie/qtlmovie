//----------------------------------------------------------------------------
//
// Copyright (c) 2015, Thierry Lelegard
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
// Unit test for module QtlStringUtils
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlStringUtils.h"
#include <QtCore>
#include <QtDebug>

class QtlStringUtilsTest : public QObject
{
    Q_OBJECT
private slots:
    void testSizeToString();
};

#include "QtlStringUtilsTest.moc"
QTL_TEST_CLASS(QtlStringUtilsTest);

//----------------------------------------------------------------------------

void QtlStringUtilsTest::testSizeToString()
{
    const qlonglong ki = 1024;

    QCOMPARE(qtlSizeToString(0), QStringLiteral("0 B"));
    QCOMPARE(qtlSizeToString(1000), QStringLiteral("1 kB"));
    QCOMPARE(qtlSizeToString(1000, 3, true), QStringLiteral("1000 B"));
    QCOMPARE(qtlSizeToString(1000, 3, false, "FOO"), QStringLiteral("1 kFOO"));

    QCOMPARE(qtlSizeToString(1024), QStringLiteral("1.024 kB"));
    QCOMPARE(qtlSizeToString(1024, 4), QStringLiteral("1.024 kB"));
    QCOMPARE(qtlSizeToString(1024, 3), QStringLiteral("1.024 kB"));
    QCOMPARE(qtlSizeToString(1024, 2), QStringLiteral("1.02 kB"));
    QCOMPARE(qtlSizeToString(1024, 1), QStringLiteral("1 kB"));
    QCOMPARE(qtlSizeToString(1024, 0), QStringLiteral("1 kB"));

    QCOMPARE(qtlSizeToString(-1024), QStringLiteral("-1.024 kB"));
    QCOMPARE(qtlSizeToString(-1024, 4), QStringLiteral("-1.024 kB"));
    QCOMPARE(qtlSizeToString(-1024, 3), QStringLiteral("-1.024 kB"));
    QCOMPARE(qtlSizeToString(-1024, 2), QStringLiteral("-1.02 kB"));
    QCOMPARE(qtlSizeToString(-1024, 1), QStringLiteral("-1 kB"));
    QCOMPARE(qtlSizeToString(-1024, 0), QStringLiteral("-1 kB"));

    QCOMPARE(qtlSizeToString(1), QStringLiteral("1 B"));
    QCOMPARE(qtlSizeToString(1000), QStringLiteral("1 kB"));
    QCOMPARE(qtlSizeToString(1000000), QStringLiteral("1 MB"));
    QCOMPARE(qtlSizeToString(1000000000), QStringLiteral("1 GB"));
    QCOMPARE(qtlSizeToString(Q_INT64_C(1000000000000)), QStringLiteral("1 TB"));
    QCOMPARE(qtlSizeToString(Q_INT64_C(1000000000000000)), QStringLiteral("1 PB"));
    QCOMPARE(qtlSizeToString(Q_INT64_C(1000000000000000000)), QStringLiteral("1 EB"));

    QCOMPARE(qtlSizeToString(1, 3, true), QStringLiteral("1 B"));
    QCOMPARE(qtlSizeToString(ki, 3, true), QStringLiteral("1 kiB"));
    QCOMPARE(qtlSizeToString(ki * ki, 3, true), QStringLiteral("1 MiB"));
    QCOMPARE(qtlSizeToString(ki * ki * ki, 3, true), QStringLiteral("1 GiB"));
    QCOMPARE(qtlSizeToString(ki * ki * ki * ki, 3, true), QStringLiteral("1 TiB"));
    QCOMPARE(qtlSizeToString(ki * ki * ki * ki * ki, 3, true), QStringLiteral("1 PiB"));
    QCOMPARE(qtlSizeToString(ki * ki * ki * ki * ki * ki, 3, true), QStringLiteral("1 EiB"));

    QCOMPARE(qtlSizeToString(10000000), QStringLiteral("10 MB"));
    QCOMPARE(qtlSizeToString(10000000, 3, true), QStringLiteral("9.537 MiB"));
    QCOMPARE(qtlSizeToString(10000000, 4, true), QStringLiteral("9.5367 MiB"));
    QCOMPARE(qtlSizeToString(1000000000, 9, true), QStringLiteral("953.674316406 MiB"));
    QCOMPARE(qtlSizeToString(1000000000, 3, true), QStringLiteral("953.674 MiB"));
}
