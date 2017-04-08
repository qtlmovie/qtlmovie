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
// Command line tool to test optical drives.
//
//----------------------------------------------------------------------------

#include "TestToolCommand.h"
#include "QtlOpticalDrive.h"

class TestOptical : public TestToolCommand
{
    Q_OBJECT
public:
    TestOptical() : TestToolCommand("optical", "", "Test features in QtlOpticalDrive class.") {}
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;
};

//----------------------------------------------------------------------------

int TestOptical::run(const QStringList& args)
{
    // Get the list of all optical drives in the system.
    const QList<QtlOpticalDrive> drives(QtlOpticalDrive::getAllDrives());

    out << "Found " << drives.size() << " optical drives" << endl;

    foreach (const QtlOpticalDrive& drive, drives) {
        out << endl
            << "====== Drive: " << drive.name() << endl
            << "Device name: " << drive.deviceName() << endl
            << "Burner device name: " << drive.burnerDeviceName() << endl
            << "Vendor id: " << drive.vendorId() << endl
            << "Product id: " << drive.productId() << endl
            << "Product revision: " << drive.productRevision() << endl
            << "Can read CD: " << drive.canReadCd() << endl
            << "Can write CD: " << drive.canWriteCd() << endl
            << "Can read DVD: " << drive.canReadDvd() << endl
            << "Can write DVD: " << drive.canWriteDvd() << endl
            << "Can read Blu-Ray: " << drive.canReadBluRay() << endl
            << "Can write Blu-Ray: " << drive.canWriteBluRay() << endl;
    }

    out << endl
        << "====== Default drives:" << endl
        << "First CD reader: " << QtlOpticalDrive::firstCdReader().name() << endl
        << "First CD writer: " << QtlOpticalDrive::firstCdWriter().name() << endl
        << "First DVD reader: " << QtlOpticalDrive::firstDvdReader().name() << endl
        << "First DVD writer: " << QtlOpticalDrive::firstDvdWriter().name() << endl
        << "First Blu-Ray reader: " << QtlOpticalDrive::firstBluRayReader().name() << endl
        << "First Blu-Ray writer: " << QtlOpticalDrive::firstBluRayWriter().name() << endl;

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

#include "TestOptical.moc"
namespace {TestOptical thisTest;}
