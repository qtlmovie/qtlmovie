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
//! @file QtlDvdDirectory.h
//!
//! Declare the class QtlDvdDirectory.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLDVDDIRECTORY_H
#define QTLDVDDIRECTORY_H

#include "QtlDvdFile.h"

//!
//! A class which describes a directory on DVD.
//!
class QtlDvdDirectory : public QtlDvdFile
{
public:
    //!
    //! Constructor.
    //! @param [in] name File name, without directory.
    //! @param [in] startSector First sector on DVD media.
    //! @param [in] sizeInBytes Size in bytes.
    //!
    QtlDvdDirectory(const QString& name = QString(), int startSector = -1, int sizeInBytes = 0) :
        QtlDvdFile(name, startSector, sizeInBytes),
        _subDirectories(),
        _files()
    {
    }
    //!
    //! Virtual destructor.
    //!
    virtual ~QtlDvdDirectory()
    {
    }
    //!
    //! Get the list of all subdirectories in this directory.
    //! @return The list of all subdirectories in this directory.
    //!
    QList<QtlDvdDirectory> subDirectories() const
    {
        return _subDirectories;
    }
    //!
    //! Get the list of all files in this directory.
    //! @return The list of all files in this directory.
    //!
    QList<QtlDvdFile> files() const
    {
        return _files;
    }
private:
    friend class QtlDvdMedia;
    QList<QtlDvdDirectory> _subDirectories;  //!< All subdirectories in this directory.
    QList<QtlDvdFile>      _files;           //!< All files in this directory.
    //!
    //! Clear the content of this object.
    //!
    virtual void clear()
    {
        QtlDvdFile::clear();
        _subDirectories.clear();
        _files.clear();
    }
};

#endif // QTLDVDDIRECTORY_H
