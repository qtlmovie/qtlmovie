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
//! @file QtlDvdProgramChapter.h
//!
//! Declare the class QtlDvdProgramChapter.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLDVDPROGRAMCHAPTER_H
#define QTLDVDPROGRAMCHAPTER_H

#include "QtlDvdProgramCell.h"

class QtlDvdProgramChapter;

//!
//! Smart pointer to QtlDvdProgramChapter, non thread-safe.
//!
typedef QtlSmartPointer<QtlDvdProgramChapter,QtlNullMutexLocker> QtlDvdProgramChapterPtr;
Q_DECLARE_METATYPE(QtlDvdProgramChapterPtr)

//!
//! List of smart pointers to QtlDvdProgramChapterChapter, non thread-safe.
//!
typedef QList<QtlDvdProgramChapterPtr> QtlDvdProgramChapterList;

//!
//! A class describing a chapter inside a DVD program.
//!
class QtlDvdProgramChapter
{
public:
    //!
    //! Constructor.
    //! @param id Chapter id within program chain (PGC).
    //!
    QtlDvdProgramChapter(int id = 0);

    //!
    //! Get the chapter id in the DVD program.
    //! @return The chapter id in the DVD program.
    //!
    int chapterId() const
    {
        return _chapterId;
    }

    //!
    //! Get the list of program cells in this chapter.
    //! @return The list of program cells in this chapter.
    //!
    QtlDvdProgramCellList cells() const
    {
        return _cells;
    }

private:
    const int             _chapterId;  //!< Chapter id in the DVD program.
    QtlDvdProgramCellList _cells;      //!< List of cells.

    // This class is responsible for building our instances.
    friend class QtlDvdProgramChain;
};

#endif // QTLDVDPROGRAMCHAPTER_H
