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
//! @file QtsDvdFile.h
//!
//! Declare the class QtsDvdFile.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDVDFILE_H
#define QTSDVDFILE_H

#include "QtsDvd.h"
#include "QtlSmartPointer.h"

class QtsDvdFile;

//!
//! Smart pointer to QtsDvdFile, non thread-safe.
//!
typedef QtlSmartPointer<QtsDvdFile,QtlNullMutexLocker> QtsDvdFilePtr;
Q_DECLARE_METATYPE(QtsDvdFilePtr)

//!
//! Operator "less than" on QtsDvdFilePtr to sort files.
//! The sort criteria is the placement on the DVD media.
//! @param [in] f1 First instance to compare.
//! @param [in] f2 Second instance to compare.
//! @return True if @a f1 is logically less than @a f2, meaning that
//! @a f1 physically preceeds @a f2 on the DVD media.
//!
bool operator<(const QtsDvdFilePtr& f1, const QtsDvdFilePtr& f2);

//!
//! A class which describes a file on DVD.
//!
class QtsDvdFile
{
public:
    //!
    //! Constructor.
    //! @param [in] path File name with directory from DVD root.
    //! @param [in] startSector First sector on DVD media.
    //! @param [in] sizeInBytes Size in bytes.
    //!
    QtsDvdFile(const QString& path = QString(), int startSector = -1, int sizeInBytes = 0);

    //!
    //! Virtual destructor.
    //!
    virtual ~QtsDvdFile();

    //!
    //! Check if this object contains a valid file description.
    //! @return True if this object contains a valid file description.
    //!
    bool isValid() const
    {
        return _sector >= 0;
    }

    //!
    //! Get the file description.
    //! If the file name is empty, this is the description of a metadata area on DVD.
    //! @return The file description.
    //!
    QString description() const;

    //!
    //! Get the file name, without directory.
    //! @return The file name, without directory.
    //!
    QString name() const;

    //!
    //! Get the file path with directory from DVD root.
    //! @return The file path, with directory.
    //!
    QString path() const
    {
        return _path;
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
    //! Get the end sector (the one after the last sector) on DVD media.
    //! @return The end sector on DVD media.
    //!
    int endSector() const
    {
        return _sector + sectorCount();
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
    int sectorCount() const;

    //!
    //! Check if the file is a VOB one, possibly encrypted.
    //! @return True if the file is a VOB.
    //!
    bool isVob() const;

    //!
    //! Clear the content of this object.
    //!
    virtual void clear();

    //!
    //! Operator "less than" to sort files.
    //! The sort criteria is the placement on the DVD media.
    //! @param [in] other Another instance to compare.
    //! @return True if @a this is logically less than @a other, meaning that
    //! @a this physically preceeds @a other on the DVD media.
    //!
    bool operator<(const QtsDvdFile& other)
    {
        return _sector < other._sector;
    }

protected:
    QString _path;    //!< File name with directory from DVD root.
    int     _sector;  //!< First sector on DVD media.
    int     _size;    //!< Size in bytes.
};

#endif // QTSDVDFILE_H
