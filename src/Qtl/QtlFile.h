//----------------------------------------------------------------------------
//
// Copyright (c) 2013, Thierry Lelegard
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
//! @file QtlFile.h
//!
//! Declare the class QtlFile.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLFILE_H
#define QTLFILE_H

#include <QtCore>
#include "QtlFilePathFilterInterface.h"
#include "QtlByteBlock.h"

//!
//! Executable files suffix.
//!
#if defined(Q_OS_WIN)
#define QTL_EXE_SUFFIX ".exe"
#else
#define QTL_EXE_SUFFIX ""
#endif

//!
//! Null file device.
//!
#if defined(Q_OS_WIN)
#define QTL_NULL_DEVICE "nul"
#else
#define QTL_NULL_DEVICE "/dev/null"
#endif

//!
//! CLSID for "My Computer" on Windows.
//! Can be used in a QFileDialog for instance.
//!
#if defined(Q_OS_WIN)
#define QTL_WIN_MYCOMPUTER "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"
#endif

//!
//! Base class of a file description.
//!
class QtlFile : public QObject
{
#if !defined (DOXYGEN)
    Q_OBJECT
#endif

public:
    //!
    //! Constructor.
    //! @param [in] fileName File name.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlFile(const QString& fileName, QObject *parent = 0);

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy (except parent).
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlFile(const QtlFile& other, QObject *parent = 0);

    //!
    //! Get the file name.
    //! @return Absolute file path.
    //!
    QString fileName() const
    {
        return _fileName;
    }

    //!
    //! Get the directory name of the file.
    //! @return The directory name of the file.
    //!
    QString directoryName() const;

    //!
    //! Check if the file name is set.
    //! @return True if the file is set, false otherwise.
    //!
    bool isSet() const
    {
        return !_fileName.isEmpty();
    }

    //!
    //! Set the file name.
    //! @param [in] fileName File path.
    //! @return True if the file name was modified, false if it was the same file name.
    //!
    virtual bool setFileName(const QString& fileName);

    //!
    //! Build an absolute file path with native directory separators.
    //! @param [in] path The file path to transform.
    //! @return Resulting absolute path.
    //!
    static QString absoluteNativeFilePath(const QString& path);

    //!
    //! Return the list of directories in the system search path.
    //! @return Directories in the system search path.
    //!
    static QStringList commandSearchPath();

    //!
    //! Search a file in a list of directories.
    //! @param [in] baseName Base file name.
    //! If @a baseName contains a directory specification, it is not searched into the list of directories.
    //! @param [in] searchPath List of directories to search.
    //! @param [in] extension If @a baseName does not end with @a extension, the file is searched with
    //! this extension.
    //! @param [in] permissions Keep only files having these permissions.
    //! @return File path of first file in the search list. Empty if not found.
    //!
    static QString search(const QString& baseName,
                          const QStringList& searchPath,
                          const QString& extension = QString(),
                          QFile::Permissions permissions = QFile::Permissions());

    //!
    //! Search an executable file in a list of directories.
    //! @param [in] baseName Base file name.
    //! If @a baseName contains a directory specification, it is not searched into the list of directories.
    //! Do not add platform-specific extension such as ".exe".
    //! @param [in] searchPath List of directories to search.
    //! @return File path of first file in the search list. Empty if not found.
    //!
    static QString searchExecutable(const QString& baseName, const QStringList& searchPath)
    {
        return search(baseName, searchPath, QTL_EXE_SUFFIX, QFile::ExeUser);
    }

    //!
    //! Expand a file path containing wildcards to all existing files matching the specification.
    //! The standard Unix wildcards '*' and '?' are recognized. Additionally, the non-standard
    //! wildcard "**", when used as a directory component, means all subdirectories.
    //! @param [in] path The path containing wildcards.
    //! @param [in] filter Optional filter. If null, all files are matched.
    //! @return An list of all existing files matching @a path.
    //!
    static QStringList expandFilePath(const QString& path, QtlFilePathFilterInterface* filter = 0);

    //!
    //! Get the absolute file path of the parent directory of a file.
    //! @param [in] path The path of the file.
    //! @param [in] upLevels The number of parent levels to climb. Default to 1.
    //! @return The parent path.
    //!
    static QString parentPath(const QString& path, int upLevels = 1);

    //!
    //! Read the content of a binary file.
    //! Error reporting is minimal.
    //! @param [in] fileName Name of file to read.
    //! @param [in] maxSize Maximum size to read.
    //! If the file is larger than @a maxSize, skip the rest of the file.
    //! If negative, read the entire file.
    //! @return A byte array containing the file or en empty array on error.
    //! It is not possible to differentiate an empty file and an error.
    //!
    static QtlByteBlock readBinaryFile(const QString& fileName, int maxSize = -1);

    //!
    //! Read a portion of a binary file at the current position.
    //! @param [in,out] device I/O device to read.
    //! @param [out] data Returned byte array containing the read data.
    //! @param [in] maxSize Maximum size to read.
    //! If the file is larger than @a maxSize, skip the rest of the file.
    //! If negative, read the entire file.
    //! @return True on success, false on error.
    //!
    static bool readBinary(QIODevice& device, QtlByteBlock& data, int maxSize = -1);

    //!
    //! Read a portion of a binary file at a given position.
    //! @param [in,out] device I/O device to read.
    //! @param [in] position Position to seek in the device before reading.
    //! @param [out] data Returned byte array containing the read data.
    //! @param [in] maxSize Maximum size to read.
    //! If the file is larger than @a maxSize, skip the rest of the file.
    //! If negative, read the entire file.
    //! @return True on success, false on error.
    //!
    static bool readBinaryAt(QIODevice& device, qint64 position, QtlByteBlock& data, int maxSize = -1)
    {
        return device.seek(position) && readBinary(device, data, maxSize);
    }

    //!
    //! Read a big endian integer in a binary file at a given position.
    //! @tparam T Integer type.
    //! @param [in,out] device I/O device to read.
    //! @param [in] position Position to seek in the device before reading.
    //! @param [out] data Read data.
    //! @return True on success, false on error.
    //!
    template <typename T>
    static bool readBigEndianAt(QIODevice& device, qint64 position, T& data)
    {
        QtlByteBlock bytes;
        const bool valid = readBinaryAt(device, position, bytes, sizeof(data)) && bytes.size() == sizeof(data);
        if (valid) {
            data = qFromBigEndian<T>(bytes.data());
        }
        return valid;
    }

signals:
    //!
    //! Emitted when the file name has changed.
    //! @param [in] fileName Absolute file path.
    //!
    void fileNameChanged(const QString& fileName);

private:
    QString _fileName;   //!< File name.

    // Unaccessible operations.
    QtlFile() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlFile)
};

#endif // QTLFILE_H
