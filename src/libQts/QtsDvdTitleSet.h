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
//! @file QtsDvdTitleSet.h
//!
//! Declare the class QtsDvdTitleSet.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDVDTITLESET_H
#define QTSDVDTITLESET_H

#include "QtlNullLogger.h"
#include "QtlByteBlock.h"
#include "QtsDvdMedia.h"
#include "QtlDataPull.h"
#include "QtlMediaStreamInfo.h"
#include "QtsDvdProgramChain.h"
#include "QtsDvdOriginalCell.h"
#include "QtsDvd.h"

class QtsDvdTitleSet;

//!
//! Smart pointer to QtsDvdTitleSet, non thread-safe.
//!
typedef QtlSmartPointer<QtsDvdTitleSet,QtlNullMutexLocker> QtsDvdTitleSetPtr;
Q_DECLARE_METATYPE(QtsDvdTitleSetPtr)

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
class QtsDvdTitleSet
{
public:
    //!
    //! Constructor
    //! @param [in] fileName Name of the IFO file or name of one of the VOB files in the title set.
    //! @param [in] log Where to log errors.
    //!
    QtsDvdTitleSet(const QString& fileName = QString(), QtlLogger* log = 0);

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy (except parent).
    //!
    explicit QtsDvdTitleSet(const QtsDvdTitleSet& other);

    //!
    //! Load the description of a title set.
    //! @param [in] fileName Name of the IFO file or name of one of the VOB files in the title set.
    //! @param [in] dvd If the caller already knows that the file is on a DVD media and this DVD media
    //! is already open, pass it as an optimization. This is optional, using 0 always works.
    //! @return True on success, false on error.
    //!
    bool load(const QString& fileName = QString(), const QtsDvdMedia* dvd = 0);

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
    //! Get the number of titles (or program chains, or PGC) in the title set.
    //! @return The number of titles in the title set.
    //!
    int titleCount() const
    {
        return _pgcs.size();
    }

    //!
    //! Get the description of a given title (or program chain, or PGC) in the title set.
    //! @param [in] titleNumber The title number to get. The first title in the VTS is #1.
    //! @return A smart pointer to the description of the title. Return a null pointer if
    //! @a titleNumber is invalid or if the description of the title was invalid.
    //!
    QtsDvdProgramChainPtr title(int titleNumber) const
    {
        return titleNumber < 1 || titleNumber > _pgcs.size() ? QtsDvdProgramChainPtr() : _pgcs.at(titleNumber - 1);
    }

    //!
    //! Get the description of all sequential titles (or program chain, or PGC) in the title set.
    //! Sometimes, a PGC is not viewed alone, it can have a previous or next PCG to play in sequence.
    //! While title() returns only the description of a given title, allTitles() returns the list
    //! of all PGC's that should be played in sequence.
    //!
    //! @param [in] titleNumber A title number to get. The first title in the VTS is #1.
    //! @return A list of smart pointers to the description of all titles in the chain containing
    //! the specified @a titleNumber. Return an empty list if @a titleNumber is invalid or if the
    //! description of the title was invalid. When the list is not empty, all pointers are not null.
    //!
    QtsDvdProgramChainList allTitles(int titleNumber) const;

    //!
    //! Get the duration of all sequential titles (or program chain, or PGC) in the title set.
    //! @param [in] titleNumber A title number to get. The first title in the VTS is #1.
    //! @return The total duration in seconds of all titles in the chain containing
    //! the specified @a titleNumber.
    //!
    int allTitlesDurationInSeconds(int titleNumber) const;

    //!
    //! Get the longest duration of all sets of sequential titles in the title set.
    //! For each title, we consider the full sequence, using its previous and next title.
    //! @return The longest duration in seconds.
    //!
    int longestDurationInSeconds() const;

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
        return int(_vobSizeInBytes / QTS_DVD_SECTOR_SIZE);
    }

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
    QtlMediaStreamInfoList streams() const
    {
        return _streams;
    }

    //!
    //! Get the number of original input VOB's, before DVD production.
    //! @return The number of original input VOB's, before DVD production.
    //!
    int originalVobCount() const
    {
        return _originalVobCount;
    }

    //!
    //! Get the list of cells in original VOB files, before DVD production.
    //! This is different from the cells inside a Program Chain which describe a cell inside the VOB files of the DVD.
    //! @return List of cells in original VOB files.
    //!
    QtsDvdOriginalCellList originalCells() const
    {
        return _originalCells;
    }

    //!
    //! Create a QtlDataPull to transfer some video content of the title set to a device.
    //!
    //! @param [in] log Where to log errors.
    //! @param parent Optional parent object of the QtlDataPull instance.
    //! @param [in] useMaxReadSpeed If true and the title set is on a DVD media,
    //! try to set the DVD reader to maximum speed.
    //! @param [in] sectorList The list of sectors to extract with 0 as base for first sector of first VOB.
    //! @return A pointer to a new QtlDataPull.
    //!
    QtlDataPull* dataPull(QtlLogger* log = 0, QObject* parent = 0, bool useMaxReadSpeed = false, const QtlRangeList& sectorList = QtlRangeList()) const;

    //!
    //! Check if a file is a .IFO or .VOB.
    //! @param [in] fileName Name of a file.
    //! @return True if the file name is valid for a .IFO or .VOB.
    //!
    static bool isDvdTitleSetFileName(const QString& fileName);

    //!
    //! Compare two QtlMediaStreamInfoPtr for DVD stream ordering.
    //! We reorder stream in "DVD order" for user convenience.
    //! @param [in] p1 First pointer.
    //! @param [in] p2 Second pointer.
    //! @return True is @a p1 is logically less than @a p2.
    //!
    static bool lessThan(const QtlMediaStreamInfoPtr& p1, const QtlMediaStreamInfoPtr& p2);

private:
    QtlNullLogger _nullLog;           //!< Dummy null logger if none specified by caller.
    QtlLogger*    _log;               //!< Where to log errors.
    QString       _deviceName;        //!< DVD device name.
    QString       _volumeId;          //!< Volume identifier.
    int           _volumeSectors;     //!< Volume size in sectors.
    bool          _isEncrypted;       //!< DVD is encrypted, need libdvdcss.
    int           _vtsNumber;         //!< Title set number.
    QString       _ifoFileName;       //!< IFO file name.
    QStringList   _vobFileNames;      //!< List of VOB files.
    qint64        _vobSizeInBytes;    //!< Total size in bytes of all VOB's.
    int           _vobStartSector;    //!< First sector of VOB files on DVD media.
    int           _originalVobCount;  //!< Number of original input VOB's, before DVD production.
    QtlMediaStreamInfoList _streams;       //!< List of streams in the VTS.
    QtsDvdProgramChainList _pgcs;          //!< List program chains in the VTS.
    QtsDvdOriginalCellList _originalCells; //!< List of cells in original input files.

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

#endif // QTSDVDTITLESET_H
