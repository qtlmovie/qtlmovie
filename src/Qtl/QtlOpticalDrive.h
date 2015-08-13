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
//!
//! @file QtlOpticalDrive.h
//!
//! Declare the class QtlOpticalDrive.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLOPTICALDRIVE_H
#define QTLOPTICALDRIVE_H

#include <QtCore>

//!
//! A class which describes an optical disc drive in the system.
//! Restriction: Currently works on Windows only. Return no information on other systems.
//!
class QtlOpticalDrive
{
public:
    //!
    //! Constructor
    //! @param [in] uid Unique disc drive id, as returned by getAllDriveUids().
    //!
    QtlOpticalDrive(const QString& uid = QString());

    //!
    //! Get the list of all optical drives in the system.
    //! @return The list of all unique disc drive ids.
    //!
    static QStringList getAllDriveUids();

    //!
    //! Features which may be implemented by optical drives.
    //!
    //! Copied from IMAPI_FEATURE_PAGE_TYPE in imapi2.h (Microsoft SDK).
    //! From Microsoft documentation: "Note that the range of feature type values
    //! is 0x0000 to 0xFFFF. This enumeration contains those features defined in
    //! the Multimedia Commands - 5 (MMC) specification. For a complete definition
    //! of each feature, see Feature Definitions in the latest release of the MMC5
    //! specification. Other values not defined here may exist. Consumers of this
    //! enumeration should not presume this list to be the only set of valid values."
    //!
    //! It may be questionable to use a Microsoft-defined list of values for a
    //! (possibly) portable class. However, these values are imported from the
    //! standard MMC specification and should be somehow available on other systems.
    //!
    //! @see https://msdn.microsoft.com/en-us/library/windows/desktop/aa366210%28v=vs.85%29.aspx
    //! @see ftp://ftp.t10.org/t10/drafts/mmc5.
    //!
    enum Feature {
        //!
        //! Identifies profiles supported by the logical unit.
        //!
        FEATURE_PROFILE_LIST = 0x00,
        //!
        //! Identifies a logical unit that supports functionality common to all devices.
        //!
        FEATURE_CORE = 0x01,
        //!
        //! Identifies the ability of the logical unit to notify an initiator about operational
        //! changes and accept initiator requests to prevent operational changes.
        //!
        FEATURE_MORPHING = 0x02,
        //!
        //! Identifies a logical unit that has a medium that is removable.
        //!
        FEATURE_REMOVABLE_MEDIUM = 0x03,
        //!
        //! Identifies reporting capability and changing capability for write protection
        //! status of the logical unit.
        //!
        FEATURE_WRITE_PROTECT = 0x04,
        //!
        //! Identifies a logical unit that is able to read data from logical blocks
        //! specified by Logical Block Addresses.
        //!
        FEATURE_RANDOMLY_READABLE = 0x10,
        //!
        //! Identifies a logical unit that conforms to the OSTA Multi-Read specification 1.00,
        //! with the exception of CD Play capability (the CD Audio Feature is not required).
        //!
        FEATURE_CD_MULTIREAD = 0x1d,
        //!
        //! Identifies a logical unit that is able to read CD specific information from the
        //! media and is able to read user data from all types of CD blocks.
        //!
        FEATURE_CD_READ = 0x1e,
        //!
        //! Identifies a logical unit that is able to read DVD specific information from the media.
        //!
        FEATURE_DVD_READ = 0x1f,
        //!
        //! Identifies a logical unit that is able to write data to logical blocks specified by
        //! Logical Block Addresses.
        //!
        FEATURE_RANDOMLY_WRITABLE = 0x20,
        //!
        //! Identifies a logical unit that is able to write data to a contiguous region, and
        //! is able to append data to a limited number of locations on the media.
        //!
        FEATURE_INCREMENTAL_STREAMING_WRITABLE = 0x21,
        //!
        //! Identifies a logical unit that supports erasable media and media that requires
        //! an erase pass before overwrite, such as some magneto-optical technologies.
        //!
        FEATURE_SECTOR_ERASABLE = 0x22,
        //!
        //! Identifies a logical unit that can format media into logical blocks.
        //!
        FEATURE_FORMATTABLE = 0x23,
        //!
        //! Identifies a logical unit that has defect management available to provide a
        //! defect-free contiguous address space.
        //!
        FEATURE_HARDWARE_DEFECT_MANAGEMENT = 0x24,
        //!
        //! Identifies a logical unit that has the ability to record to any previously
        //! unrecorded logical block.
        //!
        FEATURE_WRITE_ONCE = 0x25,
        //!
        //! Identifies a logical unit that has the ability to overwrite logical blocks
        //! only in fixed sets at a time.
        //!
        FEATURE_RESTRICTED_OVERWRITE = 0x26,
        //!
        //! Identifies a logical unit that has the ability to write CD-RW media that is
        //! designed for CAV recording.
        //!
        FEATURE_CDRW_CAV_WRITE = 0x27,
        //!
        //! Indicates that the logical unit is capable of reading a disc with the MRW format.
        //!
        FEATURE_MRW = 0x28,
        //!
        //! Identifies a logical unit that has the ability to perform media certification
        //! and recovered error reporting for logical unit assisted software defect management.
        //!
        FEATURE_ENHANCED_DEFECT_REPORTING = 0x29,
        //!
        //! Indicates that the logical unit is capable of reading a recorded DVD+RW disc.
        //!
        FEATURE_DVD_PLUS_RW = 0x2a,
        //!
        //! Indicates that the logical unit is capable of reading a recorded DVD+R disc.
        //!
        FEATURE_DVD_PLUS_R = 0x2b,
        //!
        //! Identifies a logical unit that has the ability to perform writing only on Blocking boundaries.
        //!
        FEATURE_RIGID_RESTRICTED_OVERWRITE = 0x2c,
        //!
        //! Identifies a logical unit that is able to write data to a CD track.
        //!
        FEATURE_CD_TRACK_AT_ONCE = 0x2d,
        //!
        //! Identifies a logical unit that is able to write a CD in Session at Once mode or Raw mode.
        //!
        FEATURE_CD_MASTERING = 0x2e,
        //!
        //! Identifies a logical unit that has the ability to write data to DVD-R/-RW in Disc at Once mode.
        //!
        FEATURE_DVD_DASH_WRITE = 0x2f,
        //!
        //! Identifies a logical unit that has the ability to read double density CD specific
        //! information from the media (deprecated).
        //!
        FEATURE_DOUBLE_DENSITY_CD_READ = 0x30,
        //!
        //! Identifies a logical unit that has the ability to write to double density CD media (deprecated).
        //!
        FEATURE_DOUBLE_DENSITY_CD_R_WRITE = 0x31,
        //!
        //! Identifies a logical unit that has the ability to write to double density CD-RW media (deprecated).
        //!
        FEATURE_DOUBLE_DENSITY_CD_RW_WRITE = 0x32,
        //!
        //! Identifies a drive that is able to write data to contiguous regions that are allocated
        //! on multiple layers, and is able to append data to a limited number of locations on the media.
        //!
        FEATURE_LAYER_JUMP_RECORDING = 0x33,
        //!
        //! Identifies a logical unit that has the ability to perform writing CD-RW media.
        //!
        FEATURE_CD_RW_MEDIA_WRITE_SUPPORT = 0x37,
        //!
        //! Identifies a drive that provides Logical Block overwrite service on BD-R discs that
        //! are formatted as SRM+POW.
        //!
        FEATURE_BD_PSEUDO_OVERWRITE = 0x38,
        //!
        //! Indicates that the drive is capable of reading a recorded DVD+R Double Layer disc.
        //!
        FEATURE_DVD_PLUS_R_DUAL_LAYER = 0x3b,
        //!
        //! Identifies a logical unit that is able to read control structures and user data from
        //! the Blu-ray disc.
        //!
        FEATURE_BD_READ = 0x40,
        //!
        //! Identifies a drive that is able to write control structures and user data to writeable
        //! Blu-ray discs.
        //!
        FEATURE_BD_WRITE = 0x41,
        //!
        //! Identifies a drive that is able to read HD DVD specific information from the media.
        //!
        FEATURE_HD_DVD_READ = 0x50,
        //!
        //! Indicates the ability to write to HD DVD-R/-RW media.
        //!
        FEATURE_HD_DVD_WRITE = 0x51,
        //!
        //! Identifies a logical unit that is able to perform initiator and logical unit directed
        //! power management.
        //!
        FEATURE_POWER_MANAGEMENT = 0x100,
        //!
        //! Identifies a logical unit that is able to perform Self-Monitoring Analysis and
        //! Reporting Technology (S.M.A.R.T.).
        //!
        FEATURE_SMART = 0x101,
        //!
        //! Identifies a logical unit that is able to move media from a storage area to a mechanism and back.
        //!
        FEATURE_EMBEDDED_CHANGER = 0x102,
        //!
        //! Identifies a logical unit that is able to play CD Audio data directly to an external output.
        //!
        FEATURE_CD_ANALOG_PLAY = 0x103,
        //!
        //! Identifies a logical unit that is able to upgrade its internal microcode via the interface.
        //!
        FEATURE_MICROCODE_UPDATE = 0x104,
        //!
        //! Identifies a logical unit that is able to always respond to commands within a set time period.
        //!
        FEATURE_TIMEOUT = 0x105,
        //!
        //! Identifies a logical unit that is able to perform DVD CSS/CPPM authentication and key management.
        //! This feature also indicates that the logical unit supports CSS for DVD-Video and CPPM for DVD-Audio.
        //!
        FEATURE_DVD_CSS = 0x106,
        //!
        //! Identifies a logical unit that is able to perform reading and writing within initiator specified
        //! (and logical unit verified) performance ranges. This Feature also indicates whether the logical
        //! unit supports the stream playback operation.
        //!
        FEATURE_REAL_TIME_STREAMING = 0x107,
        //!
        //! Identifies a logical unit that has a unique serial number.
        //!
        FEATURE_LOGICAL_UNIT_SERIAL_NUMBER = 0x108,
        //!
        //! Identifies a logical unit that is capable of reading a media serial number of the
        //! currently installed media.
        //!
        FEATURE_MEDIA_SERIAL_NUMBER = 0x109,
        //!
        //! Identifies a logical unit that is able to read and/or write Disc Control Blocks
        //! from or to the media.
        //!
        FEATURE_DISC_CONTROL_BLOCKS = 0x10a,
        //!
        //! Identifies a logical unit that is able to perform DVD CPRM and is able to perform
        //! CPRM authentication and key management.
        //!
        FEATURE_DVD_CPRM = 0x10b,
        //!
        //! Indicates that the logical unit provides the date and time of the creation of the
        //! current firmware revision loaded on the device.
        //!
        FEATURE_FIRMWARE_INFORMATION = 0x10c,
        //!
        //! Identifies a drive that supports AACS and is able to perform AACS authentication process.
        //!
        FEATURE_AACS = 0x10d,
        //!
        //! Identifies a Drive that is able to process disc data structures that are specified in the VCPS.
        //!
        FEATURE_VCPS = 0x110
    };

    //!
    //! Check if a drive is valid, ie. contains the description of a real existing drive.
    //!
    bool isValid() const
    {
        return !_discUid.isEmpty() && !_driveName.isEmpty();
    }

    //!
    //! Get the drive device name.
    //! @return The drive device name.
    //!
    QString driveName() const
    {
        return _driveName;
    }

    //!
    //! Get the volume name.
    //! @return The volume name.
    //!
    QString volumeName() const
    {
        return _volumeName;
    }

    //!
    //! Get the disc drive unique id.
    //! @return The disc drive unique id.
    //!
    QString uid() const
    {
        return _discUid;
    }

    //!
    //! Get the device vendor id.
    //! @return The device vendor id.
    //!
    QString vendorId() const
    {
        return _vendorId;
    }

    //!
    //! Get the device product id.
    //! @return The device product id.
    //!
    QString productId() const
    {
        return _productId;
    }

    //!
    //! Get the device product revision.
    //! @return The device product revision.
    //!
    QString productRevision() const
    {
        return _productRevision;
    }

    //!
    //! Get the list of supported features.
    //! @return The list of supported features.
    //!
    QList<Feature> features() const
    {
        return _features;
    }

    //!
    //! Check if the drive can read CD media.
    //! @return True if the drive can read CD media, false otherwise.
    //!
    bool canReadCd() const
    {
        return  _features.contains(FEATURE_CD_READ) ||
                _features.contains(FEATURE_CD_MULTIREAD) ||
                _features.contains(FEATURE_DOUBLE_DENSITY_CD_READ) ||
                _features.contains(FEATURE_CD_ANALOG_PLAY);
    }

    //!
    //! Check if the drive can write CD media.
    //! @return True if the drive can write CD media, false otherwise.
    //!
    bool canWriteCd() const
    {
        return  _features.contains(FEATURE_CDRW_CAV_WRITE) ||
                _features.contains(FEATURE_CD_TRACK_AT_ONCE) ||
                _features.contains(FEATURE_CD_MASTERING) ||
                _features.contains(FEATURE_DOUBLE_DENSITY_CD_R_WRITE) ||
                _features.contains(FEATURE_DOUBLE_DENSITY_CD_RW_WRITE) ||
                _features.contains(FEATURE_CD_RW_MEDIA_WRITE_SUPPORT);
    }

    //!
    //! Check if the drive can read DVD media.
    //! @return True if the drive can read DVD media, false otherwise.
    //!
    bool canReadDvd() const
    {
        return  _features.contains(FEATURE_DVD_READ) ||
                _features.contains(FEATURE_DVD_PLUS_R) ||
                _features.contains(FEATURE_DVD_PLUS_RW) ||
                _features.contains(FEATURE_DVD_PLUS_R_DUAL_LAYER);
    }

    //!
    //! Check if the drive can write DVD media.
    //! @return True if the drive can write DVD media, false otherwise.
    //!
    bool canWriteDvd() const
    {
        return  _features.contains(FEATURE_DVD_DASH_WRITE);
    }

    //!
    //! Check if the drive can read Blu-Ray media.
    //! @return True if the drive can read Blu-Ray media, false otherwise.
    //!
    bool canReadBluRay() const
    {
        return  _features.contains(FEATURE_BD_READ);
    }

    //!
    //! Check if the drive can write Blu-Ray media.
    //! @return True if the drive can write Blu-Ray media, false otherwise.
    //!
    bool canWriteBluRay() const
    {
        return  _features.contains(FEATURE_BD_WRITE) ||
                _features.contains(FEATURE_BD_PSEUDO_OVERWRITE);
    }

    //!
    //! Locate the first CD reader in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstCdReader()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::canReadCd);
    }

    //!
    //! Locate the first CD writer in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstCdWriter()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::canWriteCd);
    }

    //!
    //! Locate the first DVD reader in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstDvdReader()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::canReadDvd);
    }

    //!
    //! Locate the first DVD writer in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstDvdWriter()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::canWriteDvd);
    }

    //!
    //! Locate the first Blu-Ray reader in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstBluRayReader()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::canReadBluRay);
    }

    //!
    //! Locate the first Blu-Ray writer in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstBluRayWriter()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::canWriteBluRay);
    }

private:
    QString _driveName;       //!< Drive device name.
    QString _volumeName;      //!< Volume name.
    QString _discUid;         //!< Disc drive unique id.
    QString _vendorId;        //!< Device vendor id.
    QString _productId;       //!< Device product id.
    QString _productRevision; //!< Device product revision.
    QList<Feature> _features; //!< List of supported features.

    //!
    //! Locate the first drive in the system matching a check function member.
    //! @param [in] check Pointer to member function returning bool.
    //!                   Typically one of canReadBluRay(), canWriteDvd(), etc.
    //! @return The specified value in the located drive or an empty string if none is found.
    //!
    static QtlOpticalDrive firstDriveWithCheck(bool (QtlOpticalDrive::*check)() const);
};

#endif // QTLOPTICALDRIVE_H
