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
// Constructor
//----------------------------------------------------------------------------

QtlOpticalDrive::QtlOpticalDrive() :
    _driveName(),
    _vendorId(),
    _productId(),
    _productRevision(),
    _canReadCd(false),
    _canWriteCd(false),
    _canReadDvd(false),
    _canWriteDvd(false),
    _canReadBluRay(false),
    _canWriteBluRay(false)
{
}


//----------------------------------------------------------------------------
// Locate the first drive in the system matching a check function member.
//----------------------------------------------------------------------------

QtlOpticalDrive QtlOpticalDrive::firstDriveWithCheck(bool QtlOpticalDrive::*check)
{
    // Get all optical drives in the system.
    const QList<QtlOpticalDrive> drives(getAllDrives());

    // List their characteristics and return first drive matching check function.
    foreach (const QtlOpticalDrive& drive, drives) {
        if (drive.*check) {
            return drive;
        }
    }

    // None found, return an empty string.
    return QtlOpticalDrive();
}


//----------------------------------------------------------------------------
// List of optical disc drive features for Windows.
// Copied from IMAPI_FEATURE_PAGE_TYPE in imapi2.h (Microsoft SDK).
// See https://msdn.microsoft.com/en-us/library/windows/desktop/aa366210%28v=vs.85%29.aspx
//----------------------------------------------------------------------------

#if defined(Q_OS_WIN)
#define IMAPI_FEATURE_PAGE_TYPE_PROFILE_LIST 0
#define IMAPI_FEATURE_PAGE_TYPE_CORE 0x1
#define IMAPI_FEATURE_PAGE_TYPE_MORPHING 0x2
#define IMAPI_FEATURE_PAGE_TYPE_REMOVABLE_MEDIUM 0x3
#define IMAPI_FEATURE_PAGE_TYPE_WRITE_PROTECT 0x4
#define IMAPI_FEATURE_PAGE_TYPE_RANDOMLY_READABLE 0x10
#define IMAPI_FEATURE_PAGE_TYPE_CD_MULTIREAD 0x1d
#define IMAPI_FEATURE_PAGE_TYPE_CD_READ 0x1e
#define IMAPI_FEATURE_PAGE_TYPE_DVD_READ 0x1f
#define IMAPI_FEATURE_PAGE_TYPE_RANDOMLY_WRITABLE 0x20
#define IMAPI_FEATURE_PAGE_TYPE_INCREMENTAL_STREAMING_WRITABLE 0x21
#define IMAPI_FEATURE_PAGE_TYPE_SECTOR_ERASABLE 0x22
#define IMAPI_FEATURE_PAGE_TYPE_FORMATTABLE 0x23
#define IMAPI_FEATURE_PAGE_TYPE_HARDWARE_DEFECT_MANAGEMENT 0x24
#define IMAPI_FEATURE_PAGE_TYPE_WRITE_ONCE 0x25
#define IMAPI_FEATURE_PAGE_TYPE_RESTRICTED_OVERWRITE 0x26
#define IMAPI_FEATURE_PAGE_TYPE_CDRW_CAV_WRITE 0x27
#define IMAPI_FEATURE_PAGE_TYPE_MRW 0x28
#define IMAPI_FEATURE_PAGE_TYPE_ENHANCED_DEFECT_REPORTING 0x29
#define IMAPI_FEATURE_PAGE_TYPE_DVD_PLUS_RW 0x2a
#define IMAPI_FEATURE_PAGE_TYPE_DVD_PLUS_R 0x2b
#define IMAPI_FEATURE_PAGE_TYPE_RIGID_RESTRICTED_OVERWRITE 0x2c
#define IMAPI_FEATURE_PAGE_TYPE_CD_TRACK_AT_ONCE 0x2d
#define IMAPI_FEATURE_PAGE_TYPE_CD_MASTERING 0x2e
#define IMAPI_FEATURE_PAGE_TYPE_DVD_DASH_WRITE 0x2f
#define IMAPI_FEATURE_PAGE_TYPE_DOUBLE_DENSITY_CD_READ 0x30
#define IMAPI_FEATURE_PAGE_TYPE_DOUBLE_DENSITY_CD_R_WRITE 0x31
#define IMAPI_FEATURE_PAGE_TYPE_DOUBLE_DENSITY_CD_RW_WRITE 0x32
#define IMAPI_FEATURE_PAGE_TYPE_LAYER_JUMP_RECORDING 0x33
#define IMAPI_FEATURE_PAGE_TYPE_CD_RW_MEDIA_WRITE_SUPPORT 0x37
#define IMAPI_FEATURE_PAGE_TYPE_BD_PSEUDO_OVERWRITE 0x38
#define IMAPI_FEATURE_PAGE_TYPE_DVD_PLUS_R_DUAL_LAYER 0x3b
#define IMAPI_FEATURE_PAGE_TYPE_BD_READ 0x40
#define IMAPI_FEATURE_PAGE_TYPE_BD_WRITE 0x41
#define IMAPI_FEATURE_PAGE_TYPE_HD_DVD_READ 0x50
#define IMAPI_FEATURE_PAGE_TYPE_HD_DVD_WRITE 0x51
#define IMAPI_FEATURE_PAGE_TYPE_POWER_MANAGEMENT 0x100
#define IMAPI_FEATURE_PAGE_TYPE_SMART 0x101
#define IMAPI_FEATURE_PAGE_TYPE_EMBEDDED_CHANGER 0x102
#define IMAPI_FEATURE_PAGE_TYPE_CD_ANALOG_PLAY 0x103
#define IMAPI_FEATURE_PAGE_TYPE_MICROCODE_UPDATE 0x104
#define IMAPI_FEATURE_PAGE_TYPE_TIMEOUT 0x105
#define IMAPI_FEATURE_PAGE_TYPE_DVD_CSS 0x106
#define IMAPI_FEATURE_PAGE_TYPE_REAL_TIME_STREAMING 0x107
#define IMAPI_FEATURE_PAGE_TYPE_LOGICAL_UNIT_SERIAL_NUMBER 0x108
#define IMAPI_FEATURE_PAGE_TYPE_MEDIA_SERIAL_NUMBER 0x109
#define IMAPI_FEATURE_PAGE_TYPE_DISC_CONTROL_BLOCKS 0x10a
#define IMAPI_FEATURE_PAGE_TYPE_DVD_CPRM 0x10b
#define IMAPI_FEATURE_PAGE_TYPE_FIRMWARE_INFORMATION 0x10c
#define IMAPI_FEATURE_PAGE_TYPE_AACS 0x10d
#define IMAPI_FEATURE_PAGE_TYPE_VCPS 0x110
#endif // Q_OS_WIN


//----------------------------------------------------------------------------
// Get the list of all optical drives in the system.
//----------------------------------------------------------------------------

QList<QtlOpticalDrive> QtlOpticalDrive::getAllDrives()
{
    QList<QtlOpticalDrive> drives;

#if defined(Q_OS_WIN)

    // Windows implementation.
    // Create a COM object of class MsftDiscMaster2 from IMAPI2.
    QAxObject discMaster("IMAPI2.MsftDiscMaster2");
    if (discMaster.isNull()) {
        // Could not load COM object, error, no drive found.
        return drives;
    }

    // Get the number of optical disc drives.
    // Note that if the returned QVariant is invalid, toInt() returns zero.
    const int discCount = discMaster.property("Count").toInt();

    for (int discIndex = 0; discIndex < discCount; ++discIndex) {

        // Get the optical disc unique identifier.
        // Note that if the returned QVariant is invalid, toString() returns an empty string.
        const QString uid(discMaster.dynamicCall("Item(int)", discIndex).toString());
        if (!uid.isEmpty()) {

            // Create a COM object of class MsftDiscRecorder2 from IMAPI2.
            QAxObject discRecorder("IMAPI2.MsftDiscRecorder2");
            if (discRecorder.isNull()) {
                // Could not load COM object, cannot read disc drive properties.
                continue;
            }

            // Open the optical disc. There is no real way to make sure it succeeds (AFAIK).
            discRecorder.dynamicCall("InitializeDiscRecorder(QString)", uid);

            // Get simple properties.
            QtlOpticalDrive discDrive;
            discDrive._vendorId = discRecorder.property("VendorId").toString().trimmed();
            discDrive._productId = discRecorder.property("ProductId").toString().trimmed();
            discDrive._productRevision = discRecorder.property("ProductRevision").toString().trimmed();

            // The drive name is normally returned as a list of volume path names.
            // Microsoft doc says "list of drive letters and NTFS mount points for the device".
            // Just keep the first one.
            const QVariantList pathNames(discRecorder.property("VolumePathNames").toList());
            foreach (const QVariant& var, pathNames) {
                QString name(var.toString().trimmed());
                // Remove '\' after ':' (keep only drive names).
                name.remove(QRegExp("\\\\.*"));
                if (!name.isEmpty()) {
                    discDrive._driveName = name;
                    break;
                }
            }

            // If we could not get the drive name, the object is useless.
            if (discDrive._driveName.isEmpty()) {
                continue;
            }

            // Get the list of drive features.
            const QVariantList featureList(discRecorder.property("SupportedFeaturePages").toList());
            foreach (const QVariant& var, featureList) {
                if (var.canConvert(QMetaType::Int)) {
                    switch (var.toInt()) {
                        case IMAPI_FEATURE_PAGE_TYPE_CD_READ:
                        case IMAPI_FEATURE_PAGE_TYPE_CD_MULTIREAD:
                        case IMAPI_FEATURE_PAGE_TYPE_DOUBLE_DENSITY_CD_READ:
                        case IMAPI_FEATURE_PAGE_TYPE_CD_ANALOG_PLAY:
                            discDrive._canReadCd = true;
                            break;
                        case IMAPI_FEATURE_PAGE_TYPE_CDRW_CAV_WRITE:
                        case IMAPI_FEATURE_PAGE_TYPE_CD_TRACK_AT_ONCE:
                        case IMAPI_FEATURE_PAGE_TYPE_CD_MASTERING:
                        case IMAPI_FEATURE_PAGE_TYPE_DOUBLE_DENSITY_CD_R_WRITE:
                        case IMAPI_FEATURE_PAGE_TYPE_DOUBLE_DENSITY_CD_RW_WRITE:
                        case IMAPI_FEATURE_PAGE_TYPE_CD_RW_MEDIA_WRITE_SUPPORT:
                            discDrive._canWriteCd = true;
                            break;
                        case IMAPI_FEATURE_PAGE_TYPE_DVD_READ:
                        case IMAPI_FEATURE_PAGE_TYPE_DVD_PLUS_R:
                        case IMAPI_FEATURE_PAGE_TYPE_DVD_PLUS_RW:
                        case IMAPI_FEATURE_PAGE_TYPE_DVD_PLUS_R_DUAL_LAYER:
                            discDrive._canReadDvd = true;
                            break;
                        case IMAPI_FEATURE_PAGE_TYPE_DVD_DASH_WRITE:
                            discDrive._canWriteDvd = true;
                            break;
                        case IMAPI_FEATURE_PAGE_TYPE_BD_READ:
                            discDrive._canReadBluRay = true;
                            break;
                        case IMAPI_FEATURE_PAGE_TYPE_BD_WRITE:
                        case IMAPI_FEATURE_PAGE_TYPE_BD_PSEUDO_OVERWRITE:
                            discDrive._canWriteBluRay = true;
                            break;
                    }
                }
            }

            // Drive complete, add it in the list.
            drives << discDrive;
        }
    }
#endif

    return drives;
}
