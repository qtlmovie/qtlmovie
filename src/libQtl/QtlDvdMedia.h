//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2016, Thierry Lelegard
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
//! @file QtlDvdMedia.h
//!
//! Declare the class QtlDvdMedia.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLDVDMEDIA_H
#define QTLDVDMEDIA_H

#include "QtlNullLogger.h"
#include "QtlDvdDirectory.h"

//!
//! Description of a DVD media.
//!
class QtlDvdMedia : public QObject
{
    Q_OBJECT

public:
    //!
    //! Size in bytes of a DVD sector.
    //!
    static const int DVD_SECTOR_SIZE = 2048;

    //!
    //! Constructor
    //! @param [in] fileName Name of a file or directory anywhere on the DVD media.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //!
    QtlDvdMedia(const QString& fileName = QString(), QtlLogger* log = 0, QObject* parent = 0);

    //!
    //! Destructor.
    //!
    ~QtlDvdMedia();

    //!
    //! Get the message logger.
    //! @return The message logger, nevel null.
    //!
    QtlLogger* log() const
    {
        return _log;
    }

    //!
    //! Open and load the description of a DVD media starting from a file on the DVD.
    //! The description of the file structure is also loaded.
    //! @param [in] fileName Name of a file or directory anywhere on the DVD media.
    //! @return True on success, false on error.
    //!
    bool openFromFile(const QString& fileName);

    //!
    //! Open and load the description of a DVD media starting from its device name.
    //! @param [in] deviceName Name of the device containing the DVD media.
    //! @param [in] loadFileStructure If true (the default), load the file structure of the DVD.
    //! @return True on success, false on error.
    //!
    bool openFromDevice(const QString& deviceName, bool loadFileStructure = true);

    //!
    //! Close a DVD media.
    //!
    void close();

    //!
    //! Check if a DVD media was successfully open.
    //! @return True if this object contains a valid DVD description, false otherwise.
    //!
    bool isOpen() const
    {
        return _isOpen;
    }

    //!
    //! Check if the DVD media is encrypted.
    //! @return True if the DVD media is encrypted.
    //!
    bool isEncrypted() const;

    //!
    //! Get the root directory name of the DVD (ie mount point).
    //! This information is only available if the DVD was open using a constructor or openFromFile().
    //! @return The root directory name of the DVD or an empty string if no DVD reader was found or
    //! if the DVD was open using openFromDevice().
    //!
    QString rootName() const
    {
        return _rootName;
    }

    //!
    //! Get the device name of the DVD reader containing the DVD media.
    //! @return The device name or an empty string if no DVD reader was found.
    //!
    QString deviceName() const
    {
        return _deviceName;
    }

    //!
    //! Get the volume identifier of the DVD.
    //! @return DVD volume identifier, empty if not on a DVD media.
    //!
    QString volumeId() const
    {
        return _volumeId;
    }

    //!
    //! Get the volume size (in sectors) of the DVD.
    //! @return DVD volume size (in sectors).
    //!
    int volumeSizeInSectors() const
    {
        return _volumeSize;
    }

    //!
    //! Set the position of the next sector to read.
    //! @param [in] position Index of the next sector to read.
    //! Valid values are in the range 0 to volumeSizeInSectors()-1.
    //! @param [in] vobContent If true, seek into possibly encrypted VOB data.
    //! In case of doubt, specify @c true, the default.
    //! @return True on success, false on error.
    //!
    bool seekSector(int position, bool vobContent = true);

    //!
    //! Get the position of the next sector to read.
    //! @return Sector number.
    //!
    int nextSector() const
    {
        return _nextSector;
    }

    //!
    //! Read a given number of sectors from the DVD media.
    //! @param [out] buffer Where to read sectors into. Must be at least as large as 2048 x @a count bytes.
    //! @param [in] count Number of sectors to read.
    //! @param [in] position Index of the sector where to seek first. Read at current sector if negative.
    //! Valid values are in the range 0 to volumeSizeInSectors()-1.
    //! @param [in] vobContent If true, read possibly encrypted VOB data, otherwise read unencrypted data.
    //! In case of doubt, specify @c true, the default.
    //! @param [in] skipBadSectors If true, ignore and skip bad sectors which may be used as "protection".
    //! Note that if bad sectors are skipped, the current position moves past the number of requested
    //! sector. Do not assume that the next position is nextSector() + @a count, check nextSector().
    //! @return Number of sectors read. Always read as many sectors as possible. If the returned value
    //! is less than @a count, then either an error or the end of media occured.
    //!
    int readSectors(void *buffer, int count, int position = -1, bool vobContent = true, bool skipBadSectors = true);

    //!
    //! Get a description of the root directory.
    //! Can be used to describe the complete file system on DVD.
    //! This information is not available if the DVD was open using openFromDevice()
    //! with @a loadFileStructure set to @c false.
    //! @return A description of the root directory.
    //!
    QtlDvdDirectory rootDirectory() const
    {
        return _rootDirectory;
    }

    //!
    //! Locate a file in the DVD.
    //! This operation will fail if the DVD was open using openFromDevice()
    //! with @a loadFileStructure set to @c false.
    //! @param [in] fileName Name of the file to locate. It the absolute path name of the file
    //! starts with the mount point of the DVD, then only the relative part is used. Otherwise,
    //! the @a fileName must be a path relative to the DVD root directory.
    //! @param [in] cs Case sensitivity when looking for file names.
    //! @return Description of the file. Return an invalid file if not found.
    //!
    QtlDvdFile searchFile(const QString& fileName, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

signals:
    //!
    //! Emitted when a new DVD media is open.
    //! @param [in] volumeId Volume identifier of the new media.
    //!
    void newMedia(const QString& volumeId);

    //!
    //! Emitted when the DVD media is closed.
    //!
    void closed();

private:
    QtlNullLogger    _nullLog;       //!< Dummy null logger if none specified by caller.
    QtlLogger*       _log;           //!< Where to log errors.
    bool             _isOpen;        //!< Is fully open and ready.
    QString          _deviceName;    //!< DVD device name.
    QString          _rootName;      //!< DVD root directory (ie. mount point).
    QString          _volumeId;      //!< Volume identifier.
    int              _volumeSize;    //!< Volume size in sectors.
    struct dvdcss_s* _dvdcss;        //!< Handle to libdvdcss (don't include dvdcss.h in this .h).
    int              _nextSector;    //!< Next sector to read.
    QtlDvdDirectory  _rootDirectory; //!< Description of root directory

    //!
    //! Read the file structure under the specified directory.
    //! @param [in,out] dir A directory to update.
    //! @param [in] depth Depth in the file system.
    //! @return True on success, false on error.
    //!
    bool readDirectoryStructure(QtlDvdDirectory& dir, int depth);

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlDvdMedia)
};

#endif // QTLDVDMEDIA_H
