//----------------------------------------------------------------------------
//
// Copyright (c) 2015, Thierry Lelegard
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
// Qtl, Qt utility library.
// Define the class QtlOpticalDrive.
//
//----------------------------------------------------------------------------

#include "QtlOpticalDrive.h"

#if defined(Q_OS_WIN)
#include <QAxObject>
#endif


//----------------------------------------------------------------------------
// Get the list of all optical drives in the system.
//----------------------------------------------------------------------------

QStringList QtlOpticalDrive::getAllDriveUids()
{
    QStringList discUids;

#if defined(Q_OS_WIN)
    // Create a COM object of class MsftDiscMaster2 from IMAPI2.
    QAxObject discMaster("IMAPI2.MsftDiscMaster2");
    if (discMaster.isNull()) {
        // Could not load COM object, error, no drive found.
        return QStringList();
    }

    // Get the number of optical disc drives.
    // Note that if the returned QVariant is invalid, toInt() returns zero.
    const int discCount = discMaster.property("Count").toInt();

    for (int i = 0; i < discCount; ++i) {
        // Get the optical disc unique identifier.
        // Note that if the returned QVariant is invalid, toString() returns an empty string.
        const QString uid(discMaster.dynamicCall("Item(int)", i).toString());
        if (!uid.isEmpty()) {
            discUids << uid;
        }
    }
#endif

    return discUids;
}


//----------------------------------------------------------------------------
// Locate the first drive in the system matching a check function member.
//----------------------------------------------------------------------------

QtlOpticalDrive QtlOpticalDrive::firstDriveWithCheck(bool (QtlOpticalDrive::*check)() const)
{
    // Get all optical drives in the system.
    const QStringList discUids(QtlOpticalDrive::getAllDriveUids());

    // List their characteristics and return first drive matching check function.
    foreach (const QString& uid, discUids) {
        const QtlOpticalDrive disc(uid);
        if ((disc.*check)()) {
            return disc;
        }
    }

    // None found, return an empty string.
    return QString();
}


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlOpticalDrive::QtlOpticalDrive(const QString& uid) :
    _discUid(uid)
{
#if defined(Q_OS_WIN)
    // Keep default empty values if no disc id is specified.
    if (uid.isEmpty()) {
        return;
    }

    // Create a COM object of class MsftDiscRecorder2 from IMAPI2.
    QAxObject discRecorder("IMAPI2.MsftDiscRecorder2");
    if (discRecorder.isNull()) {
        // Could not load COM object, cannot read disc drive properties.
        return;
    }

    // Open the optical disc. There is no real way to make sure it succeeds (AFAIK).
    discRecorder.dynamicCall("InitializeDiscRecorder(QString)", uid);

    // Get simple properties.
    _volumeName = discRecorder.property("VolumeName").toString().trimmed();
    _vendorId = discRecorder.property("VendorId").toString().trimmed();
    _productId = discRecorder.property("ProductId").toString().trimmed();
    _productRevision = discRecorder.property("ProductRevision").toString().trimmed();

    // The drive name is normally returned as a list of volume path names.
    // Microsoft doc says "list of drive letters and NTFS mount points for the device".
    // Just keep the first one.
    const QVariantList pathNames(discRecorder.property("VolumePathNames").toList());
    foreach (const QVariant& var, pathNames) {
        QString name(var.toString().trimmed());
        // Remove '\' after ':' (keep only drive names).
        name.remove(QRegExp("\\\\.*"));
        if (!name.isEmpty()) {
            _driveName = name;
            break;
        }
    }

    // Get the list of features.
    const QVariantList featureList(discRecorder.property("SupportedFeaturePages").toList());
    foreach (const QVariant& var, featureList) {
        if (var.canConvert(QMetaType::Int)) {
            _features << Feature(var.toInt());
        }
    }
#endif
}
