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
//! @file QtsDvdProgramCell.h
//!
//! Declare the class QtsDvdProgramCell.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDVDPROGRAMCELL_H
#define QTSDVDPROGRAMCELL_H

#include "QtsDvd.h"
#include "QtlRangeList.h"
#include "QtlSmartPointer.h"

class QtsDvdProgramCell;

//!
//! Smart pointer to QtsDvdProgramCell, non thread-safe.
//!
typedef QtlSmartPointer<QtsDvdProgramCell,QtlNullMutexLocker> QtsDvdProgramCellPtr;
Q_DECLARE_METATYPE(QtsDvdProgramCellPtr)

//!
//! List of smart pointers to QtsDvdProgramCellCell, non thread-safe.
//!
typedef QList<QtsDvdProgramCellPtr> QtsDvdProgramCellList;

//!
//! A class describing a cell inside a DVD program.
//!
class QtsDvdProgramCell
{
public:
    //!
    //! Constructor.
    //! @param id Cell id within program chain (PGC).
    //!
    explicit QtsDvdProgramCell(int id = 0);

    //!
    //! Get the cell id in the DVD program.
    //! @return The cell id in the DVD program.
    //!
    int cellId() const
    {
        return _cellId;
    }

    //!
    //! Get the angle id for the content of this cell.
    //! @return The angle id for the content of this cell or zero
    //! if this cell holds common content, for all angles.
    //!
    int angleId() const
    {
        return _angleId;
    }

    //!
    //! Get the original VOB id inside the original input files, before DVD production.
    //! This is has no relation with the numbers of the VOB files on DVD.
    //! @return The original VOB id inside the original input files.
    //!
    int originalVobId() const
    {
        return _originalVobId;
    }

    //!
    //! Get the original cell id inside the original input files, before DVD production.
    //! This is has no relation with the cell id's in the VOB files on DVD.
    //! @return The original cell id inside the original input files.
    //!
    int originalCellId() const
    {
        return _originalCellId;
    }

    //!
    //! Get the cell playback duration in seconds.
    //! @return The cell playback duration in seconds.
    //!
    int durationInSeconds() const
    {
        return _duration;
    }

    //!
    //! Get the list of sectors for this cell.
    //! This is a list of ranges of sectors on DVD. The sectors are sequentially numbered from
    //! zero, starting at the first sector of the corresponding VTS_nn_1.VOB. Note that not
    //! all of these sectors are used, check the original VOB id and original cell id in the
    //! navigation packs in order to verify if a VOBU actually belongs to the cell (a VOBU
    //! is made of a navigation pack and all subsequent packs up to, but not including, the
    //! next navigation pack).
    //! @return The list of sectors for this cell.
    //!
    QtlRangeList sectors() const
    {
        return _sectors;
    }

private:
    const int    _cellId;         //!< Cell id within program chain (PGC).
    int          _angleId;        //!< Angle id for the cell content, zero for common content.
    int          _originalVobId;  //!< Original VOB id inside the original input files.
    int          _originalCellId; //!< Original cell id inside the original input files.
    int          _duration;       //!< Duration in seconds.
    QtlRangeList _sectors;        //!< List of sectors for this cell.

    // This class is responsible for building our instances.
    friend class QtsDvdProgramChain;
};

#endif // QTSDVDPROGRAMCELL_H
