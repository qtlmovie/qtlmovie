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
// Unit test for class QtlFileSystemInfo
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlFileSystemInfo.h"
#include "QtlStringUtils.h"
#include <QtCore>
#include <QtDebug>

class QtlFileSystemInfoTest : public QObject
{
    Q_OBJECT
private slots:
    void testInvalid();
    void testSystem();
};

#include "QtlFileSystemInfoTest.moc"
QTL_TEST_CLASS(QtlFileSystemInfoTest);

//----------------------------------------------------------------------------

// Test case: uninitialized objects are invalid.

void QtlFileSystemInfoTest::testInvalid()
{
    QtlFileSystemInfo info;
    QVERIFY(info.rootName().isEmpty());
    QVERIFY(info.totalBytes() < 0);
    QVERIFY(info.freeBytes() < 0);
}

//----------------------------------------------------------------------------

// Test case: check that the returned data are valid.
// This is not a fully reproduceable test since the hardware configuration
// varies from one test to another.

// To view the result, define the environment variable QTLFILESYSTEMINFO_TEST
// to a non-empty string.
// Windows PowerShell:
//   PS> $env:QTLFILESYSTEMINFO_TEST="true"
//   PS> .\UnitTest.exe QtlFileSystemInfoTest
// Linux bash:
//   $ QTLFILESYSTEMINFO_TEST="true" ./UnitTest QtlFileSystemInfoTest

void QtlFileSystemInfoTest::testSystem()
{
    // Check environment variable QTLFILESYSTEMINFO_TEST
    const bool verbose = !QProcessEnvironment::systemEnvironment().value("QTLFILESYSTEMINFO_TEST").isEmpty();

    // Get the list of all file systems.
    const QList<QtlFileSystemInfo> infos(QtlFileSystemInfo::getAllFileSystems());
    const QStringList roots(QtlFileSystemInfo::getAllRoots());

    // There must be at least one file system.
    QVERIFY(!infos.isEmpty());
    QVERIFY(!roots.isEmpty());

    // Check that all file systems have known root names.
    foreach (const QtlFileSystemInfo& info, infos) {
        QVERIFY(!info.rootName().isEmpty());
        QVERIFY(roots.contains(info.rootName()));
    }

    // Display the configuration in verbose mode.
    if (verbose) {
        qDebug() << "Found" << infos.size() << "file systems";
        foreach (const QtlFileSystemInfo& info, infos) {
            qDebug() << "====== Root:" << info.rootName();
            qDebug() << "Total bytes:" << info.totalBytes() << qtlSizeToString(info.totalBytes(), 2) << qtlSizeToString(info.totalBytes(), 2, true);
            qDebug() << "Free bytes:" << info.freeBytes() << qtlSizeToString(info.freeBytes(), 2) << qtlSizeToString(info.freeBytes(), 2, true);
        }
    }
}
