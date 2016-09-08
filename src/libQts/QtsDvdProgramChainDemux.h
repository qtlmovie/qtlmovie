//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
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
//! @file QtsDvdProgramChainDemux.h
//!
//! Declare the class QtsDvdProgramChainDemux.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDVDPROGRAMCHAINDEMUX_H
#define QTSDVDPROGRAMCHAINDEMUX_H

#include "QtlDataPull.h"
#include "QtlByteBlock.h"
#include "QtsDvdTitleSet.h"
#include "QtsDvdBandwidthReport.h"
#include "QtsDvd.h"

//!
//! A class to demultiplex a Program Chain (PGC) from a DVD Video Title Set (VTS).
//! This class pulls data from either an encrypted DVD or regular VTS files into asynchronous devices such as QProcess.
//! @see QtlDataPull
//!
class QtsDvdProgramChainDemux : public QtlDataPull
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! Progress reporting is automatically enabled.
    //! @param [in] vts Video title set to demux.
    //! @param [in] pgcNumber Program chain number to demux. Starting at 1.
    //! @param [in] angleNumber Angle number. Starting at 1.
    //! @param [in] fallbackPgcNumber Program chain number if @a pgcNumber does
    //! not exist in the title set. If negative or zero, no fallback is used and
    //! an error is generated if @a pgcNumber does not exist.
    //! @param [in] transferSize Data transfer size in bytes.
    //! @param [in] minBufferSize The minimum buffer size is the lower limit of the
    //! buffered data. When the amount of data not yet written to the device is lower
    //! than this size, new data is pulled from the DVD.
    //! @param [in] demuxPolicy Management policy for navigation packs.
    //! @param [in] log Optional message logger.
    //! @param [in] parent Optional parent object.
    //! @param [in] useMaxReadSpeed If true, try to set the DVD reader to maximum speed.
    //!
    QtsDvdProgramChainDemux(const QtsDvdTitleSet& vts,
                            int pgcNumber,
                            int angleNumber,
                            int fallbackPgcNumber = 0,
                            int transferSize = QTS_DEFAULT_DVD_TRANSFER_SIZE,
                            int minBufferSize = DEFAULT_MIN_BUFFER_SIZE,
                            Qts::DvdDemuxPolicy demuxPolicy = Qts::NavPacksFixed,
                            QtlLogger* log = 0,
                            QObject* parent = 0,
                            bool useMaxReadSpeed = false);

protected:
    //!
    //! Initialize the transfer.
    //! Reimplemented from QtlDataPull.
    //! @return True on success, false on error.
    //!
    virtual bool initializeTransfer() Q_DECL_OVERRIDE;

    //!
    //! Invoked when more data is needed.
    //! Reimplemented from QtlDataPull.
    //! @param [in] maxSize Maximum size in bytes of the requested transfer.
    //! @return True on success, false on error.
    //!
    virtual bool needTransfer(qint64 maxSize) Q_DECL_OVERRIDE;

    //!
    //! Cleanup the transfer.
    //! Reimplemented from QtlDataPull.
    //! @param [in] clean If true, this is a clean termination.
    //!
    virtual void cleanupTransfer(bool clean) Q_DECL_OVERRIDE;

private:
    //!
    //! Demux the sectors in the buffer, write demuxed sectors to superclass.
    //! @param [in] sectorCount Number of sectors to demux in buffer.
    //! @return True on success, false on error.
    //!
    bool demuxBuffer(int sectorCount);

    //!
    //! A class which is used to read from VOB files.
    //! Not used in case of encrypted DVD media.
    //!
    class VobFileSet
    {
    public:
        //!
        //! Constructor.
        //! @param [in] vobFileNames List of VOB file names.
        //! @param [in] log Where to log messages, cannot be null.
        //!
        VobFileSet(const QStringList& vobFileNames, QtlLogger* log);
        //!
        //! Destructor.
        //!
        ~VobFileSet();
        //!
        //! Read some contiguous sectors.
        //! @param [in] sectorAddress Sector address to read, 0 being the first sector of the first file.
        //! @param [out] buffer Where to read sectors.
        //! @param [in] maxSectorCount Max number of sectors to read.
        //! @return Number of sectors actually read. Can be less than @a maxSectorCount
        //! if the end of a VOB file is reached. Return -1 on error.
        //!
        int read(int sectorAddress, void* buffer, int maxSectorCount);
        //!
        //! Close all VOB files.
        //!
        void close();

    private:
        struct File {             //!< Description of one VOB file.
            QFile    file;        //!< File access.
            int      nextSector;  //!< Current sector position in file, -1 if closed.
            QtlRange sectors;     //!< Range of sectors in this file (inside full VTS content).
        };
        QtlLogger*     _log;      //!< Message logger.
        QVector<File*> _files;    //!< Vector of VOB file descriptions.
        int            _current;  //!< Index of VOB file being currently demuxed.
    };

    //!
    //! A class to compute ranges in contiguous input sectors.
    //!
    class InputSectors
    {
    public:
        //!
        //! Constructor.
        //! @param [in] log Where to log messages, cannot be null.
        //!
        InputSectors(QtlLogger* log);
        //!
        //! Initialize the walk through the list of sectors.
        //! @param [in] pgc PGC description.
        //!
        void initialize(const QtsDvdProgramChainPtr& pgc);
        //!
        //! Get current sector address to read.
        //! @return Current sector address to read, -1 at end of sector list.
        //!
        int currentSectorAddress() const;
        //!
        //! Get number of contiguous sectors at current sector address.
        //! @return Number of contiguous sectors at current sector address.
        //!
        int currentSectorCount() const;
        //!
        //! Advance current position by a number of sectors.
        //! @param [in] sectorCount Number of sectors to advance.
        //!
        void advance(int sectorCount);
        //!
        //! Get the original VOB Id and original Cell Id of the current cell.
        //! @param [out] vobId Original VOB Id.
        //! @param [out] cellId Original Cell Id.
        //!
        void getCurrentOriginalIds(int& vobId, int& cellId) const;

    private:
        QtlLogger*            _log;            //!< Message logger.
        QtsDvdProgramCellList _cells;          //!< List of PGC cells.
        int                   _currentCell;    //!< Index of current cell in _cells.
        QtlRangeList          _ranges;         //!< List of sector ranges in current cell.
        int                   _currentRange;   //!< Index of current range in _rangeList.
        int                   _currentSector;  //!< Next sector to read in current range.
    };

    //
    // Private members
    //
    const QtsDvdTitleSet   _vts;             //!< Title set to demux.
    QtsDvdProgramChainPtr  _pgc;             //!< PGC to demux.
    const int              _angleNumber;     //!< Angle to demux.
    Qts::DvdDemuxPolicy    _demuxPolicy;     //!< Management policy for navigation packs.
    const int              _sectorChunk;     //!< Number of sectors per transfer.
    const bool             _maxReadSpeed;    //!< Set the DVD reader to maximum speed.
    int                    _vobStartSector;  //!< First sector of VOB files on DVD media.
    QtlByteBlock           _buffer;          //!< Transfer buffer.
    InputSectors           _inputSectors;    //!< Computation of input sectors to read.
    QtsDvdMedia            _dvd;             //!< Access through DVD media.
    VobFileSet             _vobs;            //!< Access through VOB files.
    QtsDvdBandwidthReport  _report;          //!< To report transfer bandwidth.
    bool                   _inPgcContent;    //!< True if current sectors are part of the PGC content.
    int                    _writtenSectors;  //!< Number of written sectors.

    // Unaccessible operations.
    QtsDvdProgramChainDemux() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtsDvdProgramChainDemux)
};

#endif // QTSDVDPROGRAMCHAINDEMUX_H
