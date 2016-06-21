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
#include "QtlDvdMedia.h"
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
    //! Constructor
    //! @param [in] fileName Name of the IFO file or name of one of the VOB files in the title set.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //!
    QtlDvdTitleSet(const QString& fileName = QString(), QtlLogger* log = 0, QObject* parent = 0);

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy (except parent).
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlDvdTitleSet(const QtlDvdTitleSet& other, QObject* parent = 0);

    //!
    //! Load the description of a title set.
    //! @param [in] fileName Name of the IFO file or name of one of the VOB files in the title set.
    //! @return True on success, false on error.
    //!
    bool load(const QString& fileName = QString());

    //!
    //! Clear object content.
    //!
    void clear();

    //!
    //! Check if a title set was successfully loaded.
    //! @return True if this object contains a valid VTS description, false otherwise.
    //!
    bool isLoaded() const
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
    //! Get the title set playback duration in seconds.
    //! @return The title set playback duration in seconds.
    //!
    int durationInSeconds() const
    {
        return _duration;
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
    qint64 vobSizeInBytes() const
    {
        return _vobSizeInBytes;
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
    int volumeSizeInSectors() const
    {
        return _volumeSectors;
    }

    //!
    //! Get the first sector of the MPEG content of the VTS on DVD media.
    //! All VOB's of a given title set are always contiguous on a DVD media.
    //! @return The first sector or -1 if the not on a DVD media.
    //!
    int vobStartSector() const
    {
        return _vobStartSector;
    }

    //!
    //! Get the total size in sectors of the MPEG content of the VTS.
    //! @return The total size in sectors.
    //!
    int vobSectorCount() const
    {
        return int(_vobSizeInBytes / QtlDvdMedia::DVD_SECTOR_SIZE);
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
    //! Create a QtlDataPull to transfer of the video content of the title set to a device.
    //! @param [in] log Where to log errors.
    //! @param parent Optional parent object of the QtlDataPull instance.
    //! @return An instance of QtlDataPull which can transfer the content of
    //! the title set. The QtlDataPull is created but not started. This object will
    //! delete itself at the end of the transfer.
    //!
    QtlDataPull* dataPull(QtlLogger* log = 0, QObject* parent = 0) const;

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
    QtlNullLogger _nullLog;        //!< Dummy null logger if none specified by caller.
    QtlLogger*    _log;            //!< Where to log errors.
    QString       _deviceName;     //!< DVD device name.
    QString       _volumeId;       //!< Volume identifier.
    int           _volumeSectors;  //!< Volume size in sectors.
    bool          _isEncrypted;    //!< DVD is encrypted, need libdvdcss.
    int           _vtsNumber;      //!< Title set number.
    int           _duration;       //!< Title set duration in seconds.
    QString       _ifoFileName;    //!< IFO file name.
    QStringList   _vobFileNames;   //!< List of VOB files.
    qint64        _vobSizeInBytes; //!< Total size in bytes of all VOB's.
    int           _vobStartSector; //!< First sector of VOB files on DVD media.
    QtlByteBlock  _palette;        //!< VTS color palette in YUV format.
    QtlMediaStreamInfoPtrVector _streams; //!< List of streams in the VTS.

    //!
    //! Build the IFO and VOB file names for the VTS.
    //! Also compute the total VOB size.
    //! @param [in] fileName Name of the IFO file or name of one of the VOB files in the title set.
    //! @return True on success, false on error.
    //!
    bool buildFileNames(const QString& fileName);

    //!
    //! Read the content of the VTS IFO.
    //! @return True on success, false on error.
    //!
    bool readVtsIfo();
};

#endif // QTLDVDTITLESET_H
