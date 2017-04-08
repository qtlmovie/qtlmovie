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
//! @file QtsDvdDirectory.h
//!
//! Declare the class QtsDvdDirectory.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDVDDIRECTORY_H
#define QTSDVDDIRECTORY_H

#include "QtsDvdFile.h"

class QtsDvdDirectory;

//!
//! Smart pointer to QtsDvdDirectory, non thread-safe.
//!
typedef QtlSmartPointer<QtsDvdDirectory,QtlNullMutexLocker> QtsDvdDirectoryPtr;
Q_DECLARE_METATYPE(QtsDvdDirectoryPtr)

//!
//! A class which describes a directory on DVD.
//!
class QtsDvdDirectory : public QtsDvdFile
{
public:
    //!
    //! Constructor.
    //! @param [in] path File name with directory from DVD root.
    //! @param [in] startSector First sector on DVD media.
    //! @param [in] sizeInBytes Size in bytes.
    //!
    QtsDvdDirectory(const QString& path = QString(), int startSector = -1, int sizeInBytes = 0);

    //!
    //! Virtual destructor.
    //!
    virtual ~QtsDvdDirectory();

    //!
    //! Get the list of all subdirectories in this directory.
    //! @return The list of all subdirectories in this directory.
    //!
    QList<QtsDvdDirectoryPtr> subDirectories() const
    {
        return _subDirectories;
    }

    //!
    //! Get the list of all files in this directory.
    //! @return The list of all files in this directory.
    //!
    QList<QtsDvdFilePtr> files() const
    {
        return _files;
    }

    //!
    //! Search a file in the directory.
    //! @param [in] fileName Name of the file to search. Must be a simple name (no directory).
    //! @param [in] cs Case sensitivity when looking for file names.
    //! @return Description of the file. Return an invalid file if not found.
    //!
    QtsDvdFile searchFile(const QString& fileName, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

    //!
    //! Search a file in the directory tree.
    //! @param [in] path Either a simple file or a path with directory components.
    //! @param [in] cs Case sensitivity when looking for file names.
    //! @return Description of the file. Return an invalid file if not found.
    //!
    QtsDvdFile searchPath(const QString& path, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

    //!
    //! Search a file in the directory tree.
    //! @param [in] path A list of directory components, ending with the file name.
    //! @param [in] cs Case sensitivity when looking for file names.
    //! @return Description of the file. Return an invalid file if not found.
    //!
    QtsDvdFile searchPath(const QStringList& path, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

    //!
    //! Search a file in the directory tree.
    //! @param [in] begin Iterator to a list of directory components, ending with the file name.
    //! @param [in] end Iterator past the end of the list.
    //! @param [in] cs Case sensitivity when looking for file names.
    //! @return Description of the file. Return an invalid file if not found.
    //!
    QtsDvdFile searchPath(QStringList::ConstIterator begin, QStringList::ConstIterator end, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

    //!
    //! Clear the content of this object.
    //!
    virtual void clear() Q_DECL_OVERRIDE;

private:
    friend class QtsDvdMedia;
    QList<QtsDvdDirectoryPtr> _subDirectories;  //!< All subdirectories in this directory.
    QList<QtsDvdFilePtr>      _files;           //!< All files in this directory.
};

#endif // QTSDVDDIRECTORY_H
