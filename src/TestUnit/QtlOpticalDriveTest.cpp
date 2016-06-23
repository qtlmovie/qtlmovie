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
// Unit test for class QtlOpticalDrive
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlOpticalDrive.h"

class QtlOpticalDriveTest : public QObject
{
    Q_OBJECT
private slots:
    void testInvalid();
    void testSystem();
};

#include "QtlOpticalDriveTest.moc"
QTL_TEST_CLASS(QtlOpticalDriveTest);

//----------------------------------------------------------------------------

// Test case: uninitialized objects are invalid.

void QtlOpticalDriveTest::testInvalid()
{
    QtlOpticalDrive drive;
    QVERIFY(!drive.isValid());
    QVERIFY(drive.deviceName().isEmpty());
    QVERIFY(drive.productId().isEmpty());
    QVERIFY(drive.productRevision().isEmpty());
    QVERIFY(drive.vendorId().isEmpty());
    QVERIFY(!drive.canReadCd());
    QVERIFY(!drive.canWriteCd());
    QVERIFY(!drive.canReadDvd());
    QVERIFY(!drive.canWriteDvd());
    QVERIFY(!drive.canReadBluRay());
    QVERIFY(!drive.canWriteBluRay());
}

//----------------------------------------------------------------------------

// Test case: check that the returned data are valid.
// This is not a fully reproduceable test since the hardware configuration
// varies from one test to another.

void QtlOpticalDriveTest::testSystem()
{
    // Get the list of all optical drives in the system.
    const QList<QtlOpticalDrive> drives(QtlOpticalDrive::getAllDrives());

    // Check that all drives have names.
    foreach (const QtlOpticalDrive& drive, drives) {
        QVERIFY(drive.isValid());
        QVERIFY(!drive.name().isEmpty());
    }

    // Check that default drives have the right capability.
    QtlOpticalDrive drive;

    drive = QtlOpticalDrive::firstCdReader();
    if (drive.isValid()) {
        QVERIFY(!drive.name().isEmpty());
        QVERIFY(drive.canReadCd());
    }

    drive = QtlOpticalDrive::firstCdWriter();
    if (drive.isValid()) {
        QVERIFY(!drive.name().isEmpty());
        QVERIFY(drive.canWriteCd());
    }

    drive = QtlOpticalDrive::firstDvdReader();
    if (drive.isValid()) {
        QVERIFY(!drive.name().isEmpty());
        QVERIFY(drive.canReadDvd());
    }

    drive = QtlOpticalDrive::firstDvdWriter();
    if (drive.isValid()) {
        QVERIFY(!drive.name().isEmpty());
        QVERIFY(drive.canWriteDvd());
    }

    drive = QtlOpticalDrive::firstBluRayReader();
    if (drive.isValid()) {
        QVERIFY(!drive.name().isEmpty());
        QVERIFY(drive.canReadBluRay());
    }

    drive = QtlOpticalDrive::firstBluRayWriter();
    if (drive.isValid()) {
        QVERIFY(!drive.name().isEmpty());
        QVERIFY(drive.canWriteBluRay());
    }
}
