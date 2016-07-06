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
    //! Reference DVD transfer rate in bytes / second (aka "1x").
    //!
    static const int DVD_TRANSFER_RATE = 1385000;

    //!
    //! Constructor
    //! @param [in] fileName Name of a file or directory anywhere on the DVD media.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //! @param [in] useMaxReadSpeed If true, try to set the DVD reader to maximum speed.
    //!
    QtlDvdMedia(const QString& fileName = QString(), QtlLogger* log = 0, QObject* parent = 0, bool useMaxReadSpeed = false);

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
    //! @param [in] useMaxReadSpeed If true, try to set the DVD reader to maximum speed.
    //! @return True on success, false on error.
    //!
    bool openFromFile(const QString& fileName, bool useMaxReadSpeed = false);

    //!
    //! Open and load the description of a DVD media starting from its device name.
    //! @param [in] deviceName Name of the device containing the DVD media.
    //! @param [in] useMaxReadSpeed If true, try to set the DVD reader to maximum speed.
    //! @return True on success, false on error.
    //!
    bool openFromDevice(const QString& deviceName, bool useMaxReadSpeed = false);

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
    //! @return True on success, false on error.
    //!
    bool seekSector(int position);

    //!
    //! Get the position of the next sector to read.
    //! @return Sector number.
    //!
    int nextSector() const
    {
        return _nextSector;
    }

    //!
    //! Management policy of bad or corrupted sectors.
    //!
    //! Some DVD may intentionally include bad sectors as "protection" in the hope
    //! that copy programs will fail when encountering read errors.
    //!
    //! Note that if bad sectors are skipped, the current position on DVD moves past
    //! the number of requested sectors. Do not assume that the next position after
    //! readSectors() is nextSector() + @a count, check nextSector().
    //!
    //! Consequently, when reading the complete content of a DVD to create an ISO image,
    //! use ReadBadSectorsAsZero, do not use SkipBadSectors since this would change the
    //! layout of the media and corrupt the file structure.
    //!
    enum BadSectorPolicy {
        ErrorOnBadSectors,     //!< Stop and return an error when reading a bad sector.
        SkipBadSectors,        //!< Skip bad sectors, ignore them, do not include them in returned data.
        ReadBadSectorsAsZero   //!< Return bad sectors as if they contained all zeroes.
    };

    //!
    //! Read a given number of sectors from the DVD media.
    //! @param [out] buffer Where to read sectors into. Must be at least as large as 2048 x @a count bytes.
    //! @param [in] count Number of sectors to read.
    //! @param [in] position Index of the sector where to seek first. Read at current sector if negative.
    //! Valid values are in the range 0 to volumeSizeInSectors()-1.
    //! @param [in] badSectorPolicy How to handle bad sectors.
    //! @return Number of sectors read. Always read as many sectors as possible. If the returned value
    //! is less than @a count, then either an error or the end of media occured. Specifically, if
    //! the returned value is 0 then the end of media was already reached and if the returned value
    //! is negative then there was an error before anything could be read.
    //! @see BadSectorPolicy
    //!
    int readSectors(void *buffer, int count, int position = -1, BadSectorPolicy badSectorPolicy = SkipBadSectors);

    //!
    //! Get the number of Video Title Sets (VTS) on the DVD.
    //! @return The number of Video Title Sets (VTS) on the DVD.
    //!
    int vtsCount() const
    {
        return _vtsCount;
    }

    //!
    //! Get a description of the root directory.
    //! Can be used to describe the complete file system on DVD.
    //! @return A description of the root directory.
    //!
    QtlDvdDirectory rootDirectory() const
    {
        return _rootDirectory;
    }

    //!
    //! Locate a file in the DVD.
    //! @param [in] fileName Name of the file to locate. It the absolute path name of the file
    //! starts with the mount point of the DVD, then only the relative part is used. Otherwise,
    //! the @a fileName must be a path relative to the DVD root directory.
    //! @param [in] cs Case sensitivity when looking for file names.
    //! @return Description of the file. Return an invalid file if not found.
    //!
    QtlDvdFile searchFile(const QString& fileName, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

    //!
    //! Build the full path name of a Video Title Set information file on the DVD.
    //! @param [in] vtsNumber VTS number. It is not necessary that the VTS exists.
    //! @return The path name of the corresponding .IFO file. It the DVD is open,
    //! return a full file path. If the DVD is not open, return the file path
    //! relative to the DVD root.
    //!
    QString vtsInformationFileName(int vtsNumber) const;

    //!
    //! Build the full path name of a Video Title Set video file on the DVD.
    //! @param [in] vtsNumber VTS number. It is not necessary that the VTS exists.
    //! @param [in] vobIndex Index of the VOB file, starting at 1.
    //! @return The path name of the corresponding .VOB file. It the DVD is open,
    //! return a full file path. If the DVD is not open, return the file path
    //! relative to the DVD root.
    //!
    QString vtsVideoFileName(int vtsNumber, int vobIndex) const;

    //!
    //! Get the description of a Video Title Set information file on the DVD.
    //! @param [in] vtsNumber VTS number.
    //! @return Description of the file. Return an invalid file if not found.
    //!
    QtlDvdFile vtsInformationFile(int vtsNumber) const;

    //!
    //! Get the description of a Video Title Set video file on the DVD.
    //! @param [in] vtsNumber VTS number.
    //! @param [in] vobIndex Index of the VOB file, starting at 1.
    //! @return Description of the file. Return an invalid file if not found.
    //!
    QtlDvdFile vtsVideoFile(int vtsNumber, int vobIndex) const;

    //!
    //! Check if a file name matches a Video Title Set information file (VTS_nn_0.IFO).
    //! @param [in] fileName File name.
    //! @return The VTS number ("nn") or -1 if the file is not a valid VTS IFO.
    //!
    static int vtsInformationFileNumber(const QString& fileName);

    //!
    //! Get the list of all files on the DVD media.
    //! The list is sorted by physical placement of files on the media.
    //! Non-file space (meta-data, directories, unused space) is represented
    //! by "place holders", ie. QtlDvdFile with an empty file name.
    //! @return The list of all files on the DVD media.
    //!
    QList<QtlDvdFilePtr> allFiles() const
    {
        return _allFiles;
    }

    //!
    //! Load all encryption keys into the cache of libdvdcss.
    //! @return True on success, false on error.
    //!
    bool loadAllEncryptionKeys();

    //!
    //! Flags for transferRateToString().
    //!
    enum TransferRateFlag {
        TransferDvdBase   = 0x0001,  //!< Include "Nx" using DVD base transfer rate.
        TransferBytes     = 0x0002,  //!< Include bytes per second.
        TransferKiloBytes = 0x0004,  //!< Include kilo-bytes per second.
        TransferKibiBytes = 0x0008,  //!< Include kibi-bytes (base 1024) per second.
        TransferBits      = 0x0010,  //!< Include bits per second.
    };
    Q_DECLARE_FLAGS(TransferRateFlags, TransferRateFlag)

    //!
    //! Build a human-readable string for DVD transfer rate.
    //! @param [in] bytes Number of bytes transfered.
    //! @param [in] milliSeconds Number of milliseconds to transfer @a bytes.
    //! @param [in] flags A set of flags to specify the format.
    //! @return A human readable string.
    //!
    static QString transferRateToString(qint64 bytes, qint64 milliSeconds, TransferRateFlags flags = TransferDvdBase);

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
    int              _vtsCount;      //!< Number of video title sets.
    struct dvdcss_s* _dvdcss;        //!< Handle to libdvdcss (don't include dvdcss.h in this .h).
    int              _nextSector;    //!< Next sector to read.
    QtlDvdDirectory  _rootDirectory; //!< Description of root directory.
    QList<QtlDvdFilePtr> _allFiles;  //!< List of all files on DVD.
    QList<QtlDvdFilePtr>::ConstIterator _currentFile; //!< File area where _nextSector is.

    //!
    //! Read the file structure under the specified directory.
    //! @param [in,out] dir A directory to update.
    //! @param [in] depth Depth in the file system.
    //! @param [in] inRoot True when @a dir is the root directory of the DVD.
    //! @param [in] inVideoTs True when @a dir is VIDEO_TS.
    //! @return True on success, false on error.
    //!
    bool readDirectoryStructure(QtlDvdDirectory& dir, int depth, bool inRoot, bool inVideoTs);

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlDvdMedia)
};

//!
//! Smart pointer to QtlDvdMedia, non thread-safe.
//!
typedef QtlSmartPointer<QtlDvdMedia,QtlNullMutexLocker> QtlDvdMediaPtr;

//
// Qt type system insertions.
//
Q_DECLARE_OPERATORS_FOR_FLAGS(QtlDvdMedia::TransferRateFlags)
Q_DECLARE_METATYPE(QtlDvdMediaPtr)

#endif // QTLDVDMEDIA_H
