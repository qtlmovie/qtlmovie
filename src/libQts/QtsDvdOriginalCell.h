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
//! @file QtsDvdOriginalCell.h
//!
//! Declare the class QtsDvdOriginalCell.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDVDORIGINALCELL_H
#define QTSDVDORIGINALCELL_H

#include "QtsDvd.h"
#include "QtlRange.h"
#include "QtlSmartPointer.h"

class QtsDvdOriginalCell;

//!
//! Smart pointer to QtsDvdOriginalCell, non thread-safe.
//!
typedef QtlSmartPointer<QtsDvdOriginalCell,QtlNullMutexLocker> QtsDvdOriginalCellPtr;
Q_DECLARE_METATYPE(QtsDvdOriginalCellPtr)

//!
//! List of smart pointers to QtsDvdOriginalCell, non thread-safe.
//!
typedef QList<QtsDvdOriginalCellPtr> QtsDvdOriginalCellList;

//!
//! A class describing a cell inside the original input VOB's of a DVD, before DVD production.
//!
//! This is different from the cells inside a Program Chain which describe a cell inside the VOB files of the DVD.
//! @see QtsDvdProgramCell
//!
class QtsDvdOriginalCell
{
public:
    //!
    //! Constructor.
    //! @param [in] vobId Original VOB id inside the original input files.
    //! Valid VOD id values start at 1.
    //! @param [in] cellId Original cell id inside the original input files.
    //! Valid cell id values start at 1.
    //! @param [in] sectors Range of sectors on DVD for this cell.
    //! Sector zero is the first sector of the corresponding VTS_nn_1.VOB.
    //!
    explicit QtsDvdOriginalCell(int vobId = 0, int cellId = 0, const QtlRange& sectors = QtlRange());

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
    //! Get the range of sectors for this cell.
    //! This is the range of sectors on DVD. The sectors are sequentially numbered from
    //! zero, starting at the first sector of the corresponding VTS_nn_1.VOB.
    //! @return The range of sectors for this cell.
    //!
    QtlRange sectors() const
    {
        return _sectors;
    }

private:
    int      _originalVobId;  //!< Original VOB id inside the original input files.
    int      _originalCellId; //!< Original cell id inside the original input files.
    QtlRange _sectors;        //!< Range of sectors on DVD for this cell.
};

#endif // QTSDVDORIGINALCELL_H
