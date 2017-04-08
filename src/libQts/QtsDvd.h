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
//! @file QtsDvd.h
//!
//! Common definitions for DVD handling.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDVD_H
#define QTSDVD_H

#include "QtsCore.h"
#include "QtlRangeList.h"

//!
//! Size in bytes of a DVD sector.
//!
static const int QTS_DVD_SECTOR_SIZE = 2048;

//!
//! Reference DVD transfer rate in bytes / second (aka "1x").
//!
static const int QTS_DVD_TRANSFER_RATE = 1385000;

//!
//! Default DVD transfer size in bytes (512 kB, 256 DVD sectors).
//!
static const int QTS_DEFAULT_DVD_TRANSFER_SIZE = 512 * 1024;

//!
//! Qts namespace.
//!
namespace Qts {
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
    //! DVD navigation packs management policy during program chain demux.
    //!
    //! To get a valid DVD content after demux, the navigation packs should be fixed.
    //! However, if the result is used by tools like FFmpeg or VLC, the navigation
    //! packs can be ignored.
    //!
    enum DvdDemuxPolicy {
        NavPacksFixed,      //!< Fix sector addresses in navigation packs.
        NavPacksUnchanged,  //!< Keep navigation packs unmodified.
        NavPacksRemoved     //!< Completely remove navigation packs.
    };

    //!
    //! Flags for QtsDvdMedia::transferRateToString().
    //!
    enum TransferRateFlag {
        TransferDvdBase   = 0x0001,  //!< Include "Nx" using DVD base transfer rate.
        TransferBytes     = 0x0002,  //!< Include bytes per second.
        TransferKiloBytes = 0x0004,  //!< Include kilo-bytes per second.
        TransferKibiBytes = 0x0008,  //!< Include kibi-bytes (base 1024) per second.
        TransferBits      = 0x0010,  //!< Include bits per second.
    };
    Q_DECLARE_FLAGS(TransferRateFlags, TransferRateFlag)
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Qts::TransferRateFlags)

#endif // QTSDVD_H
