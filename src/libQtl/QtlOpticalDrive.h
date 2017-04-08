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
//!
//! Limitation: Implemented on Windows, Mac and Linux only.
//!
class QtlOpticalDrive
{
public:
    //!
    //! Constructor
    //!
    QtlOpticalDrive();

    //!
    //! Get the list of all optical drives in the system.
    //! @return The list of all optical drives in the system.
    //!
    static QList<QtlOpticalDrive> getAllDrives();

    //!
    //! Get the description of a drive.
    //! @param [in] name A drive name as returned by name().
    //! @return The description of the corresponding drive.
    //! If the drive is not found, isValid() will return false.
    //!
    static QtlOpticalDrive getDrive(const QString& name);

    //!
    //! Check if a drive is valid, ie. contains the description of a real existing drive.
    //!
    bool isValid() const
    {
        return !_name.isEmpty();
    }

    //!
    //! Get the drive name.
    //! All existing drives have a name. But, on Mac, a drive has no device name if the drive
    //! is empty. On Mac, a drive has a device name only when a media is inserted and when
    //! the media is ejected and another one inserted later, the device name may have changed.
    //! @return The drive name.
    //!
    QString name() const
    {
        return _name;
    }

    //!
    //! Get the drive device name.
    //! @return The drive device name.
    //! @see burnerDeviceName()
    //!
    QString deviceName() const
    {
        return _deviceName;
    }

    //!
    //! Get the drive device name for burning media.
    //! On some systems like Mac OS X, the general device name of the drive to mount
    //! a device deviceName() and the device name to burn a media in the drive is not
    //! the same.
    //! @return The drive device name.
    //! @see deviceName()
    //!
    QString burnerDeviceName() const
    {
        return _burnerDeviceName;
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
    //! Check if the drive can read CD media.
    //! @return True if the drive can read CD media, false otherwise.
    //!
    bool canReadCd() const
    {
        return _canReadCd;
    }

    //!
    //! Check if the drive can write CD media.
    //! @return True if the drive can write CD media, false otherwise.
    //!
    bool canWriteCd() const
    {
        return _canWriteCd;
    }

    //!
    //! Check if the drive can read DVD media.
    //! @return True if the drive can read DVD media, false otherwise.
    //!
    bool canReadDvd() const
    {
        return _canReadDvd;
    }

    //!
    //! Check if the drive can write DVD media.
    //! @return True if the drive can write DVD media, false otherwise.
    //!
    bool canWriteDvd() const
    {
        return _canWriteDvd;
    }

    //!
    //! Check if the drive can read Blu-Ray media.
    //! @return True if the drive can read Blu-Ray media, false otherwise.
    //!
    bool canReadBluRay() const
    {
        return _canReadBluRay;
    }

    //!
    //! Check if the drive can write Blu-Ray media.
    //! @return True if the drive can write Blu-Ray media, false otherwise.
    //!
    bool canWriteBluRay() const
    {
        return _canWriteBluRay;
    }

    //!
    //! Locate the first CD reader in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstCdReader()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::_canReadCd);
    }

    //!
    //! Locate the first CD writer in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstCdWriter()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::_canWriteCd);
    }

    //!
    //! Locate the first DVD reader in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstDvdReader()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::_canReadDvd);
    }

    //!
    //! Locate the first DVD writer in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstDvdWriter()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::_canWriteDvd);
    }

    //!
    //! Locate the first Blu-Ray reader in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstBluRayReader()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::_canReadBluRay);
    }

    //!
    //! Locate the first Blu-Ray writer in the system.
    //! @return The located drive or an invalid object if none was found.
    //!
    static QtlOpticalDrive firstBluRayWriter()
    {
        return firstDriveWithCheck(&QtlOpticalDrive::_canWriteBluRay);
    }

private:
    QString _name;              //!< Drive name.
    QString _deviceName;        //!< Drive device name.
    QString _burnerDeviceName;  //!< Drive device name for burning operations.
    QString _vendorId;          //!< Device vendor id.
    QString _productId;         //!< Device product id.
    QString _productRevision;   //!< Device product revision.
    bool    _canReadCd;         //!< The drive can read CD media.
    bool    _canWriteCd;        //!< The drive can write CD media.
    bool    _canReadDvd;        //!< The drive can read DVD media.
    bool    _canWriteDvd;       //!< The drive can write DVD media.
    bool    _canReadBluRay;     //!< The drive can read Blu-Ray media.
    bool    _canWriteBluRay;    //!< The drive can write Blu-Ray media.
#if defined(Q_OS_OSX)
    int     _driveNumber;       //!< Drive number (Mac OS X)
#endif

    //!
    //! Locate the first drive in the system matching a boolean member.
    //! @param [in] check Pointer to bool member to check. Typically one of _canReadBluRay(), _canWriteDvd, etc.
    //! @return The specified value in the located drive or an empty string if none is found.
    //!
    static QtlOpticalDrive firstDriveWithCheck(bool QtlOpticalDrive::*check);

    //!
    //! A function helper to sort optical drives by name.
    //! @param [in] o1 First object to compare.
    //! @param [in] o2 Second object to compare.
    //! @return True is @a o1 is logically less the @a o2.
    //!
    static bool lessThanByName(const QtlOpticalDrive& o1, const QtlOpticalDrive& o2)
    {
        return QString::compare(o1._name, o2._name, Qt::CaseInsensitive) < 0;
    }

#if defined(Q_OS_LINUX) || defined(DOXYGEN)
    //!
    //! Update a boolean field in all elements in an array of QtlOpticalDrive.
    //! @param [in,out] drives Array of QtlOpticalDrive to update.
    //! @param [in] fields Array of strings, one for each element in @arg drives.
    //! When a string is not empty and not "0", the capability is set.
    //! Otherwise, it is left unchanged.
    //! @param [in] capability Pointer to bool member to update. Typically one of _canReadBluRay(), _canWriteDvd, etc.
    //!
    static void updateCapability(QList<QtlOpticalDrive>& drives, const QStringList& fields, bool QtlOpticalDrive::*capability);
#endif
};

#endif // QTLOPTICALDRIVE_H
