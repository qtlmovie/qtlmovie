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
//! @file QtsDvdProgramChapter.h
//!
//! Declare the class QtsDvdProgramChapter.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDVDPROGRAMCHAPTER_H
#define QTSDVDPROGRAMCHAPTER_H

#include "QtsDvd.h"
#include "QtsDvdProgramCell.h"

class QtsDvdProgramChapter;

//!
//! Smart pointer to QtsDvdProgramChapter, non thread-safe.
//!
typedef QtlSmartPointer<QtsDvdProgramChapter,QtlNullMutexLocker> QtsDvdProgramChapterPtr;
Q_DECLARE_METATYPE(QtsDvdProgramChapterPtr)

//!
//! List of smart pointers to QtsDvdProgramChapterChapter, non thread-safe.
//!
typedef QList<QtsDvdProgramChapterPtr> QtsDvdProgramChapterList;

//!
//! A class describing a chapter inside a DVD program.
//!
class QtsDvdProgramChapter
{
public:
    //!
    //! Constructor.
    //! @param id Chapter id within program chain (PGC).
    //!
    QtsDvdProgramChapter(int id = 0);

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
    QtsDvdProgramCellList cells() const
    {
        return _cells;
    }

private:
    const int             _chapterId;  //!< Chapter id in the DVD program.
    QtsDvdProgramCellList _cells;      //!< List of cells.

    // This class is responsible for building our instances.
    friend class QtsDvdProgramChain;
};

#endif // QTSDVDPROGRAMCHAPTER_H
