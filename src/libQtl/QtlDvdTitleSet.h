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
//! @file QtlDvdTitleSet.h
//!
//! Declare the class QtlDvdTitleSet.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLDVDTITLESET_H
#define QTLDVDTITLESET_H

#include "QtlNullLogger.h"
#include "QtlByteBlock.h"
#include "QtlDataPull.h"
#include "QtlMediaStreamInfo.h"

//!
//! Description of a "Title Set" in a DVD.
//!
//! A "standard" DVD structure contains one or more "video title set" (VTS).
//! Each VTS is a movie, documentary, bonus, etc. The VTS are numbered on
//! two digits "nn" (01, 02, etc.) Each VTS contains the following files:
//!
//! - VTS_nn_0.IFO  <- information file.
//! - VTS_nn_0.VOB  <- optional title set menu, not part of the movie.
//! - VTS_nn_1.VOB  <- first MPEG-PS file in the movie
//! - VTS_nn_2.VOB
//! - etc ...
//! - VTS_nn_0.BUP  <- optional backup units
//!
//! The complete MPEG-PS content of the movie is simply the concatenation
//! of all .VOB file. There are at most 9 VOB files per VTS (1GB per VOB).
//!
//! The content of a VTS is physically contiguous on the DVD, starting from
//! VTS_nn_0.VOB, VTS_nn_1.VOB, etc., ending with the VTS_nn_0.BUP.
//!
class QtlDvdTitleSet : public QObject
{
    Q_OBJECT

public:
    //!
    //! Size in bytes of a DVD sector.
    //!
    static const int DVD_SECTOR_SIZE = 2048;

    //!
    //! Constructor
    //! @param [in] fileName Name of the IFO file or name of one of the VOB files in the title set.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //!
    QtlDvdTitleSet(const QString& fileName = QString(), QtlLogger* log = 0, QObject* parent = 0);

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy (except parent).
    //! If @a other is open on an encrypted DVD, the new instance has a new independent
    //! session on the DVD and the read pointer is set to the beginning of the title set.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlDvdTitleSet(const QtlDvdTitleSet& other, QObject* parent = 0);

    //!
    //! Destructor.
    //!
    ~QtlDvdTitleSet();

    //!
    //! Open and load the description of a title set.
    //! @param [in] fileName Name of the IFO file or name of one of the VOB files in the title set.
    //! @return True on success, false on error.
    //!
    bool open(const QString& fileName = QString());

    //!
    //! Close a title set.
    //!
    void close();

    //!
    //! Check if a title set was successfully open.
    //! @return True if this object contains a valid VTS description, false otherwise.
    //!
    bool isOpen() const
    {
        return !_ifoFileName.isEmpty();
    }

    //!
    //! Get the "title set number", ie the "nn" in "VTS_nn_0.IFO".
    //! @return The title set number.
    //!
    int vtsNumber() const
    {
        return _vtsNumber;
    }

    //!
    //! Get the full absolute file name of the IFO file for the title set.
    //! @return The full absolute file name of the IFO file for the title set.
    //!
    QString ifoFileName() const
    {
        return _ifoFileName;
    }

    //!
    //! Get the number of VOB files for the title set.
    //! @return The number of VOB files for the title set.
    //!
    int vobCount() const
    {
        return _vobFileNames.size();
    }

    //!
    //! Get the list of VOB files for the title set.
    //! @return The list of VOB files for the title set. The returned list
    //! is sorted in replay order. All file names are full absolute paths.
    //!
    QStringList vobFileNames() const
    {
        return _vobFileNames;
    }

    //!
    //! Get the total size in bytes of all VOB's.
    //! @return The total size in bytes of all VOB's.
    //!
    qint64 vobSize() const
    {
        return _vobSize;
    }

    //!
    //! Check if the title set is on an encrypted DVD media.
    //! @return True if the title set is on an encrypted DVD media.
    //!
    bool isEncrypted() const
    {
        return _isEncrypted;
    }

    //!
    //! Get the color palette of the title set in YUV format.
    //! Typically used to render subtitles.
    //! @return The palette in YUV format. Each entry contains 4 bytes: (0, Y, Cr, Cb).
    //!
    QtlByteBlock yuvPalette() const
    {
        return _palette;
    }

    //!
    //! Get the color palette of the title set in RGB format.
    //! Typically used to render subtitles.
    //! @return The palette in RGB format. Each entry contains 4 bytes: (0, R, G, B).
    //!
    QtlByteBlock rgbPalette() const;

    //!
    //! Get the number of video, audio and subtitle streams in the title set.
    //! @return The number of video, audio and subtitle streams in the title set.
    //!
    int streamCount() const
    {
        return _streams.size();
    }

    //!
    //! Get the description of all video, audio and subtitle streams in the title set.
    //! @return A list of smart pointers to the description of all streams.
    //!
    QtlMediaStreamInfoPtrVector streams() const
    {
        return _streams;
    }

    //!
    //! Get the device name of the DVD reader containing the title set.
    //! The returned device name can be used by libdvdcss.
    //! @return The device name or en empty string if no DVD reader was found.
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
    int volumeSize() const
    {
        return _volumeSize;
    }

    //!
    //! Set the next sector to read at the beginning of the MPEG content of the VTS.
    //!
    void rewind()
    {
        _nextSector = 0;
    }

    //!
    //! Read a given number of sectors from the MPEG content of the VTS.
    //! This operation is reserved to encrypted DVD's.
    //! Unencrypted DVD's must be read using standard file operations.
    //! @param [out] buffer Where to read sectors into. Must be at least as large as 2048 x @a count bytes.
    //! @param [in] count Number of sectors to read.
    //! @param [in] position Index of the sector where to seek first. Read at current sector if negative.
    //! @return Number of sectors read. Always read as many sectors as possible. If the returned value
    //! is less than @a count, then either an error or the end of VTS occured. Return zero at end of VTS
    //! and -1 on error (such as trying to read an unencrypted DVD).
    //! @see isEncrypted()
    //!
    int readSectors(void *buffer, int count, int position = -1);

    //!
    //! Get the next sector position to read from the start of the VTS.
    //! The first sector of the VTS is at index zero.
    //! @return The index of the next sector to read.
    //!
    int nextSector() const
    {
        return _nextSector;
    }

    //!
    //! Get the total size in sectors of the MPEG content of the VTS.
    //! @return The total size in sectors.
    //!
    int sectorCount() const
    {
        return int(_vobSize / DVD_SECTOR_SIZE);
    }

    //!
    //! Create a QtlDataPull to transfer of the video content of the title set to a device.
    //! @param parent Optional parent object of the QtlDataPull instance.
    //! @return An instance of QtlDataPull which can transfer the content of
    //! the title set. The QtlDataPull is created but not started. This object will
    //! delete itself at the end of the transfer.
    //!
    QtlDataPull* dataPull(QObject* parent = 0) const;

    //!
    //! Check if a file is a .IFO or .VOB.
    //! @param [in] fileName Name of a file.
    //! @return True if the file name is valid for a .IFO or .VOB.
    //!
    static bool isDvdTitleSetFileName(const QString& fileName);

    //!
    //! Convert a YUV palette into RGB.
    //! @param [in,out] palette Palette to modify.
    //! On input, each entry contains 4 bytes: (0, Y, Cr, Cb).
    //! On output, each entry contains 4 bytes: (0, R, G, B).
    //! @param [in] log Where to log errors.
    //!
    static void convertPaletteYuvToRgb(QtlByteBlock& palette, QtlLogger* log);

    //!
    //! Compare two QtlMediaStreamInfoPtr for DVD stream ordering.
    //! We reorder stream in "DVD order" for user convenience.
    //! @param [in] p1 First pointer.
    //! @param [in] p2 Second pointer.
    //! @return True is @a p1 is logically less than @a p2.
    //!
    static bool lessThan(const QtlMediaStreamInfoPtr& p1, const QtlMediaStreamInfoPtr& p2);

private:
    QtlNullLogger    _nullLog;            //!< Dummy null logger if none specified by caller.
    QtlLogger*       _log;                //!< Where to log errors.
    QString          _deviceName;         //!< DVD device name.
    QString          _volumeId;           //!< Volume identifier.
    int              _volumeSize;         //!< Volume size in sectors.
    bool             _isEncrypted;        //!< DVD is encrypted, need libdvdcss.
    struct dvdcss_s* _dvdcss;             //!< Handle to libdvdcss (don't include dvdcss.h in this .h).
    int              _vtsNumber;          //!< Title set number.
    QString          _ifoFileName;        //!< IFO file name.
    QStringList      _vobFileNames;       //!< List of VOB files.
    qint64           _vobSize;            //!< Total size in bytes of all VOB's.
    int              _vobStartSector;     //!< First sector of VOB files on DVD media.
    int              _nextSector;         //!< Next sector to read, relative to the beginning of the MPEG content of the VTS.
    QtlByteBlock     _palette;            //!< VTS color palette in YUV format.
    QtlMediaStreamInfoPtrVector _streams; //!< List of streams in the VTS.

    //!
    //! Build the IFO and VOB file names for the VTS.
    //! Also compute the total VOB size.
    //! @param [in] fileName Name of the IFO file or name of one of the VOB files in the title set.
    //! @return True on success, false on error.
    //!
    bool buildFileNames(const QString& fileName);

    //!
    //! Build device name for libdvdcss from IFO file name.
    //! @return True on success, false on error.
    //!
    bool buildDeviceName();

    //!
    //! Read the content of the VTS IFO.
    //! @return True on success, false on error.
    //!
    bool readVtsIfo();

    //!
    //! Open DVD structure and check if the media is encrypted.
    //! @return True on success, false on error.
    //!
    bool readDvdStructure();

    //!
    //! Read exactly @a sectorCount sectors from the DVD.
    //! Optionally seek to @a seekOffset first.
    //! If @a sectorCount is negative or zero and @a seekSector is positive, only seek is performed.
    //! @param [out] buffer Where to read sectors into. Must be at least as large as 2048 x @a sectorCount bytes.
    //! @param [in] sectorCount Number of sectors to read.
    //! @param [in] seekSector Where to seek first, ignored if negative.
    //! @param [in] vobContent If true, read possibly encrypted VOB data, otherwise read metadata.
    //! @return True on success, false on error.
    //!
    bool readDvdSectors(void* buffer, int sectorCount, int seekSector, bool vobContent);

    //!
    //! Locate an entry inside a directory.
    //! The directory to search is defined by its start sector on media and size in bytes.
    //! @param [in] dirSector First sector of the directory content.
    //! @param [in] dirSize Size in bytes of the directory content.
    //! @param [in] entryName Name of the directory entry to search. Not case-sensitive. No UDF version number.
    //! @param [out] entrySector Returned first sector of the entry (file or directory).
    //! @param [out] entrySize Returned size in bytes of the entry (file or directory).
    //! @param [out] isDirectory Returned true if the entry is a directory.
    //! @return True on success, false on error.
    //!
    bool locateDirectoryEntry(int dirSector, int dirSize, const QString& entryName, int& entrySector, int& entrySize, bool& isDirectory);
};

#endif // QTLDVDTITLESET_H
