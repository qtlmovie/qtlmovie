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
#include "QtlStringUtils.h"
#include "QtlStringList.h"
#include <QtDebug>

#if defined(Q_OS_WIN)
#include <QAxObject>
#elif defined(Q_OS_LINUX)
#include "QtlFile.h"
#elif defined(Q_OS_OSX)
#include "QtlSynchronousProcess.h"
#endif


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlOpticalDrive::QtlOpticalDrive() :
    _name(),
    _deviceName(),
    _burnerDeviceName(),
    _vendorId(),
    _productId(),
    _productRevision(),
    _canReadCd(false),
    _canWriteCd(false),
    _canReadDvd(false),
    _canWriteDvd(false),
    _canReadBluRay(false),
    _canWriteBluRay(false)
#if defined(Q_OS_OSX)
    , _driveNumber(0)
#endif
{
}


//----------------------------------------------------------------------------
// Locate the first drive in the system matching a boolean member.
//----------------------------------------------------------------------------

QtlOpticalDrive QtlOpticalDrive::firstDriveWithCheck(bool QtlOpticalDrive::*check)
{
    // Get all optical drives in the system and return first drive matching bool member.
    foreach (const QtlOpticalDrive& drive, getAllDrives()) {
        if (drive.*check) {
            return drive;
        }
    }

    // None found, return an empty string.
    return QtlOpticalDrive();
}


//----------------------------------------------------------------------------
// Get the description of a drive.
//----------------------------------------------------------------------------

QtlOpticalDrive QtlOpticalDrive::getDrive(const QString &name)
{
    const QList<QtlOpticalDrive> drives(QtlOpticalDrive::getAllDrives());
    foreach (const QtlOpticalDrive& drive, drives) {
        if (drive.name() == name) {
            return drive;
        }
    }
    return QtlOpticalDrive();
}

//----------------------------------------------------------------------------
// Update a boolean field in all elements in an array of QtlOpticalDrive.
//----------------------------------------------------------------------------

#if defined(Q_OS_LINUX)

void QtlOpticalDrive::updateCapability(QList<QtlOpticalDrive>& drives, const QStringList& fields, bool QtlOpticalDrive::*capability)
{
    QList<QtlOpticalDrive>::Iterator itDrives(drives.begin());
    QStringList::ConstIterator itFields(fields.begin());

    while (itDrives != drives.end() && itFields != fields.end()) {
        if (!itFields->isEmpty() && *itFields != "0") {
            (*itDrives).*capability = true;
        }
        ++itDrives;
        ++itFields;
    }
}

#endif


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
                    discDrive._name = discDrive._deviceName = discDrive._burnerDeviceName = name;
                    break;
                }
            }

            // If we could not get the drive name, the object is useless.
            if (discDrive._name.isEmpty()) {
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

        // On Windows, the order of drives is strange. Sort them by name.
        qSort(drives.begin(), drives.end(), lessThanByName);
    }

#elif defined(Q_OS_LINUX)

    // Linux implementation, based on the content of the /proc filesystem.
    //
    // The content of /proc/sys/dev/cdrom/info is something like:
    // ---------------- cut here ---------------------
    // CD-ROM information, Id: cdrom.c 3.20 2003/12/17
    //
    // drive name:             sr0
    // drive speed:            40
    // drive # of slots:       1
    // Can close tray:         1
    // Can open tray:          1
    // Can lock tray:          1
    // Can change speed:       1
    // Can select disk:        0
    // Can read multisession:  1
    // Can read MCN:           1
    // Reports media changed:  1
    // Can play audio:         1
    // Can write CD-R:         0
    // Can write CD-RW:        0
    // Can read DVD:           1
    // Can write DVD-R:        0
    // Can write DVD-RAM:      0
    // Can read MRW:           1
    // Can write MRW:          1
    // Can write RAM:          1
    // ---------------- cut here ---------------------
    // If there are several drives, there is one column per drive.

    // Loop on all lines from /proc/sys/dev/cdrom/info
    foreach (const QString& line, QtlFile::readTextLinesFile("/proc/sys/dev/cdrom/info", 50000)) {

        // Drop lines not containing ':'
        const int colon = line.indexOf(QChar(':'));
        if (colon < 0) {
            continue;
        }

        // Get the left-hand side of the ':', remove spaces, make it lowercase.
        // We will use it as a tag, more robust to slight modifications.
        QString tag(line.mid(0, colon).toLower());
        tag.remove(QRegExp("\\s+"));

        // Get the right-hand side of the ':', split into individual fields.
        const QStringList fields(line.mid(colon + 1).split(QRegExp("\\s+"), QString::SkipEmptyParts));

        if (tag == "drivename" && drives.isEmpty()) {
            // "drivename" is the initial tag, giving device names.
            foreach (const QString& name, fields) {

                // Initialize a drive object with the complete device name.
                QtlOpticalDrive drive;
                drive._name = drive._deviceName = drive._burnerDeviceName = QStringLiteral("/dev/%1").arg(name);

                // The "commercial" informations are available elsewhere:
                // $ cat /sys/block/sr0/device/vendor
                // hp
                // $ cat /sys/block/sr0/device/model
                // DVDROM DH40N
                // $ cat /sys/block/sr0/device/rev
                // DD01
                drive._vendorId = QtlFile::readTextFile(QStringLiteral("/sys/block/%1/device/vendor").arg(name)).trimmed();
                drive._productId = QtlFile::readTextFile(QStringLiteral("/sys/block/%1/device/model").arg(name)).trimmed();
                drive._productRevision= QtlFile::readTextFile(QStringLiteral("/sys/block/%1/device/rev").arg(name)).trimmed();

                // No way to check that the drive can read CD's. Assume always true.
                drive._canReadCd = true;

                // Add the drive in the list.
                drives << drive;
            }
        }
        else if (tag.startsWith("canwritecd")) {
            updateCapability(drives, fields, &QtlOpticalDrive::_canWriteCd);
        }
        else if (tag.startsWith("canreaddvd")) {
            updateCapability(drives, fields, &QtlOpticalDrive::_canReadDvd);
        }
        else if (tag.startsWith("canwritedvd")) {
            updateCapability(drives, fields, &QtlOpticalDrive::_canWriteDvd);
        }
        else if (tag.startsWith("canreadblu") || tag.startsWith("canreadbd")) {
            // No real known tag, just guessing future extension...
            updateCapability(drives, fields, &QtlOpticalDrive::_canReadBluRay);
        }
        else if (tag.startsWith("canwriteblu") || tag.startsWith("canwritebd")) {
            // No real known tag, just guessing future extension...
            updateCapability(drives, fields, &QtlOpticalDrive::_canWriteBluRay);
        }
    }

#elif defined(Q_OS_OSX)

    // Mac OS X implementation, based of the output of the "drutil" command.
    // First, the command "drutil list" lists all drives. The output looks like:
    // ---------------- cut here --------------------------------
    //    Vendor   Product           Rev   Bus       SupportLevel
    // 1  NECVMWar VMware SATA CD01  1.00  ATAPI     Unsupported
    // ---------------- cut here --------------------------------

    // Execute "drutil list".
    // As a foolproof precaution, don't let the process run more than 5 seconds and 32 KB of output.
    const QtlSynchronousProcess drutilList("drutil", QStringList("list"), 5000, 32768);

    // Get the output of "drutil list".
    QStringList lines(drutilList.standardOutputLines());
    if (lines.size() < 2) {
        // Not even one drive found.
        return drives;
    }

    // Locate the position of the header fields.
    const QString& head(lines.first());
    const int posVendor  = head.indexOf("Vendor", 0, Qt::CaseInsensitive);
    const int posProduct = head.indexOf("Product", posVendor, Qt::CaseInsensitive);
    const int posRev     = head.indexOf("Rev", posProduct, Qt::CaseInsensitive);
    const int posBus     = head.indexOf("Bus", posRev, Qt::CaseInsensitive);
    if (posVendor < 0 || posProduct < 0 || posRev < 0 || posBus < 0) {
        qWarning() << "QtlOpticalDrive: unexpected header for \"drutil list\": " << head;
        return drives;
    }

    // Iterate over all output lines, starting with the second one.
    foreach (const QString& line, lines) {
        QtlOpticalDrive drive;

        // If the line as at least as long as "Bus" position and first field is an integer.
        if (line.size() >= posBus && (drive._driveNumber = qtlToInt(line.left(posVendor))) >= 0) {

            // Then we have a drive description.
            drive._vendorId = line.mid(posVendor, posProduct - posVendor).simplified();
            drive._productId = line.mid(posProduct, posRev - posProduct).simplified();
            drive._productRevision = line.mid(posRev, posBus - posRev).simplified();
            drive._name = QStringLiteral("%1: %2 %3").arg(drive._driveNumber).arg(drive._vendorId).arg(drive._productId).simplified();

            // Add the drive in the list. For the moment, we do no know the drive's capabilities.
            drives << drive;
        }
    }

    // Then, for each drive, we run the command "drutil info -drive N".
    // The output looks like:
    // ---------------- cut here -------------------------------------------
    //   Vendor   Product           Rev
    //     NECVMWar VMware SATA CD01  1.00
    //
    //       Interconnect: ATAPI
    //       SupportLevel: Unsupported
    //       Profile Path: None
    //              Cache: 2048k
    //           CD-Write: -R, -RW, BUFE, CDText, Test, IndexPts, ISRC
    //          DVD-Write: -R, -R DL, -RAM, -RW, +R, +R DL, +RW, BUFE, Test
    //         Strategies: CD-TAO, CD-SAO, CD-Raw, DVD-DAO
    // ---------------- cut here -------------------------------------------
    //
    // And then we run "drutil status -drive N" to see if a media is inserted
    // and what is the device name. The output looks like:
    // ---------------- cut here -------------------------------------------
    // Vendor   Product           Rev
    // NECVMWar VMware SATA CD01  1.00
    //
    //           Type: DVD+R         Name: /dev/disk2
    //       Sessions: 1             Tracks: 1
    //   Overwritable:   00:00:00    blocks:        0 /   0.00MB /   0.00MiB
    //     Space Free:   00:00:00    blocks:        0 /   0.00MB /   0.00MiB
    //     Space Used:  342:12:68    blocks:  1539968 /   3.15GB /   2.94GiB
    //    Writability:
    //      Book Type: DVD+R (v1)
    //       Media ID: MCC 004
    // ---------------- cut here -------------------------------------------

    // Iterate over previous list of drives and drive numbers
    for (QList<QtlOpticalDrive>::Iterator it = drives.begin(); it != drives.end(); ++it) {

        // There is no real way to get the read capabilities. Assume that all drives can read DVD's.
        it->_canReadCd = it->_canReadDvd = true;

        // Execute "drutil info" and iterate over all output lines.
        const QtlSynchronousProcess drutilInfo("drutil", QtlStringList("info", "-drive", QString::number(it->_driveNumber)), 5000, 32768);
        foreach (const QString& line, drutilInfo.standardOutputLines()) {
            if (line.contains("CD-Write:", Qt::CaseInsensitive)) {
                it->_canWriteCd = true;
            }
            if (line.contains("DVD-Write:", Qt::CaseInsensitive)) {
                it->_canWriteDvd = true;
            }
            // Don't really know for BluRay, just guessing what the output could be.
            // In fact, it has been observed that the capability to read BluRay
            // (without write capability) is not reported by drutil.
            if (line.contains("BD-Write:", Qt::CaseInsensitive) || line.contains("BluRay-Write:", Qt::CaseInsensitive)) {
                it->_canReadBluRay = it->_canWriteBluRay = true;
            }
        }

        // Execute "drutil status" and iterate over all output lines.
        const QtlSynchronousProcess drutilStatus("drutil", QtlStringList("status", "-drive", QString::number(it->_driveNumber)), 5000, 32768);
        foreach (const QString& line, drutilStatus.standardOutputLines()) {
            const int index = line.indexOf(QRegExp("Name:\\s+/dev/"));
            if (index >= 0) {
                it->_deviceName = line.mid(index + 5).trimmed();
                // If the device name is /dev/disk.., then the burning device is the corresponding /dev/rdisk.
                if (it->_deviceName.startsWith("/dev/disk")) {
                    it->_burnerDeviceName = "/dev/r" + it->_deviceName.mid(5);
                }
            }
        }
    }

#endif // System-specific implementations

    return drives;
}
