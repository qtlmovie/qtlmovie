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
//! @file QtlDvdFile.h
//!
//! Declare the class QtlDvdFile.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLDVDFILE_H
#define QTLDVDFILE_H

#include "QtlCore.h"

//!
//! A class which describes a file on DVD.
//!
class QtlDvdFile
{
public:
    //!
    //! Constructor.
    //! @param [in] name File name, without directory.
    //! @param [in] startSector First sector on DVD media.
    //! @param [in] sizeInBytes Size in bytes.
    //!
    QtlDvdFile(const QString& name = QString(), int startSector = -1, int sizeInBytes = 0) :
        _name(name),
        _sector(startSector),
        _size(sizeInBytes)
    {
    }
    //!
    //! Virtual destructor.
    //!
    virtual ~QtlDvdFile()
    {
    }
    //!
    //! Get the file name.
    //! @return The file name, without directory.
    //!
    QString name() const
    {
        return _name;
    }
    //!
    //! Get the first sector on DVD media.
    //! @return The first sector on DVD media.
    //!
    int startSector() const
    {
        return _sector;
    }
    //!
    //! Get the file size in bytes.
    //! @return The file size in bytes.
    //!
    int sizeInBytes() const
    {
        return _size;
    }
    //!
    //! Get the file size in sectors.
    //! @return The file size in sectors.
    //!
    int sectorCount() const
    {
        // Cannot use QtlDvdMedia::DVD_SECTOR_SIZE here because of header inclusion order.
        return _size / 2048 + int(_size % 2048 > 0);
    }
protected:
    QString _name;    //!< File name.
    int     _sector;  //!< First sector on DVD media.
    int     _size;    //!< Size in bytes.
    //!
    //! Clear the content of this object.
    //!
    virtual void clear()
    {
        _name.clear();
        _sector = -1;
        _size = 0;
    }
};

#endif // QTLDVDFILE_H
