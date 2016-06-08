//----------------------------------------------------------------------------
//
// Copyright (c) 2015, Thierry Lelegard
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
//! @file QtlFileSystemInfo.h
//!
//! Declare the class QtlFileSystemInfo.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLFILESYSTEMINFO_H
#define QTLFILESYSTEMINFO_H

#include "QtlCore.h"

//!
//! A class which describes file system.
//!
class QtlFileSystemInfo
{
public:
    //!
    //! Constructor
    //! @param [in] fileName Name of a file somewhere in the file system.
    //!
    QtlFileSystemInfo(const QString& fileName = QString());

    //!
    //! Get the list of all mounted filesystems in the system.
    //! @return The list of all root directories.
    //!
    static QStringList getAllRoots();

    //!
    //! Get the list of all mounted filesystems in the system.
    //! @return The list of all file systems.
    //!
    static QList<QtlFileSystemInfo> getAllFileSystems();

    //!
    //! Get the root of the file system.
    //! @return The root of the file system.
    //!
    QString rootName() const
    {
        return _rootName;
    }

    //!
    //! Get the total size in bytes of the file system.
    //! @return The total size in bytes of the file system or -1 on error.
    //!
    qlonglong totalBytes() const
    {
        return byteSize(true);
    }

    //!
    //! Get the free size in bytes of the file system.
    //! @return The free size in bytes of the file system or -1 on error.
    //!
    qlonglong freeBytes() const
    {
        return byteSize(false);
    }

private:
    QString _rootName; //!< Path name of the root.

    //!
    //! Get either the total size or free size in bytes of the file system.
    //! @param [in] total If true, return the total size. If false, return the free size.
    //! @return The requested size in bytes.
    //!
    qlonglong byteSize(bool total) const;
};

#endif // QTLFILESYSTEMINFO_H
