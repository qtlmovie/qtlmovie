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
//! @file QtlFile.h
//!
//! Declare the class QtlFile.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLFILE_H
#define QTLFILE_H

#include "QtlCore.h"
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
//! Case sensitivity of the file system, as a Qt::CaseSensitivity value.
//!
#if defined(Q_OS_WIN)
#define QTL_FILE_NAMES_CASE_SENSITIVE Qt::CaseInsensitive
#else
#define QTL_FILE_NAMES_CASE_SENSITIVE Qt::CaseSensitive
#endif

//!
//! Name of environment variable holding the command seach path.
//!
#if defined(Q_OS_WIN)
#define QTL_PATH_VARIABLE_NAME "Path"
#else
#define QTL_PATH_VARIABLE_NAME "PATH"
#endif

//!
//! Separator string for search paths.
//!
#if defined(Q_OS_WIN)
#define QTL_SEARCH_PATH_SEPARATOR ";"
#else
#define QTL_SEARCH_PATH_SEPARATOR ":"
#endif

//!
//! Base class of a file description.
//!
class QtlFile : public QObject
{
    Q_OBJECT

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
    //! @param [in] removeSymLinks If true, remove symbolic links.
    //! @return Resulting absolute path.
    //!
    static QString absoluteNativeFilePath(const QString& path, bool removeSymLinks = false);

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
    //! Create a directory and all parent directories if necessary.
    //! @param [in] path Directory path to create.
    //! @param [in] createOnly If true, the directory must be created, meaning that
    //! the operation fails if the directory already exists.
    //! @return True on success, false on error.
    //!
    static bool createDirectory(const QString& path, bool createOnly = false);

    //!
    //! Search a subdirectory in the parent path.
    //!
    //! First the subdirectory @a subdir is searched in the directory @a dir.
    //! If not found, it is searched in the parent of @a dir and then in its
    //! upper level, etc., until @a subdir is found or the root directory is reached.
    //! @param [in] dir Start searching at @a dir.
    //! @param [in] subdir Name of the subdirectory to search.
    //! @param [in] maxLevels Maximum number of levels to search.
    //! @return Full path of the first subdirectory found or an empty string if none is found.
    //!
    static QString searchParentSubdirectory(const QString& dir, const QString& subdir, int maxLevels = 128);

    //!
    //! Get the "short path name" of a file path.
    //! On Windows platforms, return the path name in DOS 8.3 convention.
    //! On other platforms, return the input @a path.
    //! @param [in] path The path of the file or directory.
    //! If the file or directory does not exist, the result is unpredictable.
    //! @param [in] keepOnError If true and the conversion fails, return
    //! the same @a path.
    //! @return The equivalent short path name or the empty string on error
    //! and @a keepOnError is false.
    //!
    static QString shortPath(const QString& path, bool keepOnError);

    //!
    //! Get the "short path name" of the file.
    //! On Windows platforms, return the path name in DOS 8.3 convention.
    //! On other platforms, return the input @a path.
    //! @param [in] keepOnError If true and the conversion fails, return
    //! the same as fileName().
    //! @return The equivalent short path name or the empty string on error
    //! and @a keepOnError is false.
    //!
    QString shortPath(bool keepOnError) const
    {
        return shortPath(_fileName, keepOnError);
    }

    //!
    //! Enforce a suffix in a file name.
    //! If @a path does not end with @a suffix, it is added.
    //! @param [in] path A file path.
    //! @param [in] suffix The suffix to check or add.
    //! @param [in] cs Indicates if the file names are case sensitive.
    //! @return A file path ending with @a suffix.
    //!
    static QString enforceSuffix(const QString& path, const QString& suffix, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

    //!
    //! Format the content of an URL into a file path if the URL scheme is file:
    //! @param [in] url The URL.
    //! @return A file path if the URL scheme is file:, the URL string otherwise.
    //!
    static QString toFileName(const QUrl& url);

    //!
    //! Write the content of a binary file.
    //! Error reporting is minimal.
    //! @param [in] fileName Name of file to write.
    //! @param [in] content Content of the file to write.
    //! @return True on success, false on error.
    //!
    static bool writeBinaryFile(const QString& fileName, const QtlByteBlock& content);

    //!
    //! Write binary data into an open file.
    //! Loop on write operations until all data are written or an error occured.
    //! Error reporting is minimal.
    //! @param [in] file An open file.
    //! @param [in] data Binary data to write.
    //! @return True on success, false on error.
    //!
    static bool writeBinary(QIODevice& file, const QtlByteBlock& data)
    {
        return writeBinary(file, data.data(), data.size());
    }

    //!
    //! Write binary data into an open file.
    //! Loop on write operations until all data are written or an error occured.
    //! Error reporting is minimal.
    //! @param [in] file An open file.
    //! @param [in] data Address of binary data to write.
    //! @param [in] size Size in bytes of binary data to write.
    //! @param [in] processEvents If true, process all pending events before each write operation.
    //! @return True on success, false on error.
    //!
    static bool writeBinary(QIODevice& file, const void* data, int size, bool processEvents = false);

    //!
    //! Write the binary content of the file.
    //! Error reporting is minimal.
    //! @param [in] content Content of the file to write.
    //! @return True on success, false on error.
    //!
    bool writeBinary(const QtlByteBlock& content)
    {
        return writeBinaryFile(_fileName, content);
    }

    //!
    //! Read the content of a binary file.
    //! Error reporting is minimal.
    //! @param [in] fileName Name of file to read.
    //! @param [in] maxSize Maximum size to read.
    //! If the file is larger than @a maxSize, skip the rest of the file.
    //! If negative, read the entire file.
    //! @return A byte array containing the file or an empty array on error.
    //! It is not possible to differentiate an empty file and an error.
    //!
    static QtlByteBlock readBinaryFile(const QString& fileName, int maxSize = -1);

    //!
    //! Read the binary content of the file.
    //! Error reporting is minimal.
    //! @param [in] maxSize Maximum size to read.
    //! If the file is larger than @a maxSize, skip the rest of the file.
    //! If negative, read the entire file.
    //! @return A byte array containing the file or an empty array on error.
    //! It is not possible to differentiate an empty file and an error.
    //!
    QtlByteBlock readBinary(int maxSize = -1) const
    {
        return readBinaryFile(_fileName, maxSize);
    }

    //!
    //! Read the content of a text file.
    //! Error reporting is minimal.
    //! @param [in] fileName Name of file to read.
    //! @param [in] maxSize Maximum number of characters to return.
    //! If the file is larger than @a maxSize, skip the rest of the file.
    //! If negative, read the entire file.
    //! @return A list of strings containing the text lines of the file or an empty list on error.
    //! It is not possible to differentiate an empty file and an error.
    //!
    static QStringList readTextLinesFile(const QString& fileName, int maxSize = -1);

    //!
    //! Read the text content of the file.
    //! Error reporting is minimal.
    //! @param [in] maxSize Maximum number of characters to return.
    //! If the file is larger than @a maxSize, skip the rest of the file
    //! (do not truncate the last line, so the actual number of returned characters may be larger).
    //! If negative, read the entire file.
    //! @return A list of strings containing the text lines of the file or an empty list on error.
    //! It is not possible to differentiate an empty file and an error.
    //!
    QStringList readTextLines(int maxSize = -1) const
    {
        return readTextLinesFile(_fileName, maxSize);
    }

    //!
    //! Read the content of a text file.
    //! Error reporting is minimal.
    //! @param [in] fileName Name of file to read.
    //! @return The text content of the file or an empty string on error.
    //! It is not possible to differentiate an empty file and an error.
    //!
    static QString readTextFile(const QString& fileName);

    //!
    //! Read the text content of the file.
    //! Error reporting is minimal.
    //! @return The text content of the file or an empty string on error.
    //! It is not possible to differentiate an empty file and an error.
    //!
    QString readText() const
    {
        return readTextFile(_fileName);
    }

    //!
    //! Write the content of a text file.
    //! Error reporting is minimal.
    //! @param [in] fileName Name of file to write.
    //! @param [in] lines A list of strings containing the text lines of the file.
    //! @return True on success, false on error.
    //!
    static bool writeTextLinesFile(const QString& fileName, const QStringList& lines);

    //!
    //! Write the text content of the file.
    //! Error reporting is minimal.
    //! @param [in] lines A list of strings containing the text lines of the file.
    //! @return True on success, false on error.
    //!
    bool writeTextLines(const QStringList& lines)
    {
        return writeTextLinesFile(_fileName, lines);
    }

    //!
    //! Write the content of a text file.
    //! Error reporting is minimal.
    //! @param [in] fileName Name of file to write.
    //! @param [in] text The text content of the file.
    //! @return True on success, false on error.
    //!
    static bool writeTextFile(const QString& fileName, const QString& text);

    //!
    //! Write the text content of the file.
    //! Error reporting is minimal.
    //! @param [in] text The text content of the file.
    //! @return True on success, false on error.
    //!
    bool writeText(const QString& text)
    {
        return writeTextFile(_fileName, text);
    }

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
};

#endif // QTLFILE_H
