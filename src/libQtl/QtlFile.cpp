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
//
// Qtl, Qt utility library.
// Define the class QtlFile.
//
//----------------------------------------------------------------------------

#include "QtlFile.h"
#include "QtlStringUtils.h"
#include <QProcessEnvironment>
#include <QFileInfo>
#include <QDir>

#if defined(Q_OS_WIN)
#include <windows.h>
#endif


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlFile::QtlFile(const QString& fileName, QObject *parent) :
    QObject(parent),
    _fileName(fileName == "" ? "" : absoluteNativeFilePath(fileName))
{
}

QtlFile::QtlFile(const QtlFile& other, QObject* parent) :
    QObject(parent),
    _fileName(other._fileName)
{
}


//----------------------------------------------------------------------------
// Get the directory name of the file.
//----------------------------------------------------------------------------

QString QtlFile::directoryName() const
{
    if (_fileName.isEmpty()) {
        return "";
    }
    else {
        const QFileInfo info(_fileName);
        return absoluteNativeFilePath(info.path());
    }
}


//----------------------------------------------------------------------------
// Set the file name.
//----------------------------------------------------------------------------

bool QtlFile::setFileName(const QString& fileName)
{
    const QString path(fileName.isEmpty() ? "" : absoluteNativeFilePath(fileName));
    if (path != _fileName) {
        // Set a new file name.
        _fileName = path;
        emit fileNameChanged(_fileName);
        return true;
    }
    else {
        // The absolute path remains unchanged.
        return false;
    }
}


//----------------------------------------------------------------------------
// Build an absolute file path with native directory separators.
//----------------------------------------------------------------------------

QString QtlFile::absoluteNativeFilePath(const QString& path, bool removeSymLinks)
{
    // Qt conversion and clean path operates on '/' only.
    // Make sure that the Windows separators are converted into slashes.
    QString normPath(path);
    normPath.replace('\\', '/');

    // Build an absolute file path, remove redundant "." and "..", convert directory separators to "/".
    const QFileInfo info(normPath);
    QString clean;
    if (removeSymLinks) {
        // Remove symbolic links and redundant "." or "..".
        // If the file does not exist, return an empty string.
        clean = info.canonicalFilePath();
    }
    if (clean.isEmpty()) {
        // Never return an ampty string but does not remove redundant "." or "..".
        clean = info.absoluteFilePath();
    }
    // Always remove redundant "." or ".." but does not remove symbolic links.
    clean = QDir::cleanPath(clean);

    // Collapse multiple slashes. Note that "//" shall remain at the beginning of the string (Windows share).
#if defined(Q_OS_WIN)
    const bool winShare = clean.startsWith("//");
#endif
    clean.replace(QRegExp("/+"), "/");
#if defined(Q_OS_WIN)
    if (winShare) {
        clean.insert(0, '/');
    }
#endif

    // Remove trailing slashes (unless on "/").
    if (clean.length() > 1) {
        clean.replace(QRegExp("/*$"), "");
    }

    // Convert directory separators to native.
    return QDir::toNativeSeparators(clean);
}


//----------------------------------------------------------------------------
// Return the list of directories in the system search path.
//----------------------------------------------------------------------------

QStringList QtlFile::commandSearchPath()
{
    // Get the process environment.
    const QProcessEnvironment env(QProcessEnvironment::systemEnvironment());

    // Get and split the search path.
    return env.value(QTL_PATH_VARIABLE_NAME).split(QTL_SEARCH_PATH_SEPARATOR, QString::SkipEmptyParts);
}


//----------------------------------------------------------------------------
// Search a file in a list of directories.
//----------------------------------------------------------------------------

QString QtlFile::search(const QString& baseName,
                        const QStringList& searchPath,
                        const QString& extension,
                        QFile::Permissions permissions)
{
    // Filter out empty name.
    if (baseName.isEmpty()) {
        return "";
    }

    // Check if the file ends with the specified extension.
    // On Windows, use non-case-sensitive search.
    QString fileName(baseName);
    if (!baseName.endsWith(extension, QTL_FILE_NAMES_CASE_SENSITIVE)) {
        // Specified extension not present, add it.
        fileName.append(extension);
    }

    // If fileName contains a directory separator, do not search in other directories.
    if (fileName.contains(QChar('/')) || fileName.contains(QChar('\\'))) {
        return absoluteNativeFilePath(fileName);
    }

    // Search file in all directories.
    foreach (const QString& dir, searchPath) {
        const QString path(dir + QDir::separator() + fileName);
        const QFileInfo info(path);
        if (info.exists() && info.permission(permissions) && !info.isDir()) {
            // Found.
            return path;
        }
    }

    // File not found.
    return "";
}


//----------------------------------------------------------------------------
// Expand a file path containing wildcards to all existing files matching
// the specification.
//----------------------------------------------------------------------------

namespace {
    //!
    //! Recursive helper function for qtlExpandFilePath.
    //! @param [in,out] result The list of full file paths to return.
    //! @param [in] baseDir Current directory to explore.
    //! @param [in] currentPart Iterator to current component in the original wildcard specification.
    //! @param [in] endPart Iterator to the end of this list of components.
    //! @param [in] filter User-specified filter.
    //!
    void subExpandFilePath(QStringList& result,
                           const QString& baseDir,
                           const QStringList::const_iterator& currentPart,
                           const QStringList::const_iterator& endPart,
                           QtlFilePathFilterInterface* filter)
    {
        // If there is nothing to search in the base directory, there is nothing to find...
        if (currentPart == endPart) {
            return;
        }

        // Get the current component in the base directory (can be a wildcard).
        QString localPart(*currentPart);
        QStringList::const_iterator nextPart(currentPart + 1);
        bool isLastPart = nextPart == endPart;

        // Base options for filtering files in the directory.
        QDir::Filters filterOptions (
        #if defined (Q_OS_UNIX)
            // Case sensitivity depends on the operating system.
            QDir::CaseSensitive |
        #endif
            // Always filter directories.
            QDir::Dirs |
            // We always skip "." and "..".
            QDir::NoDotAndDotDot);

        // Special case: the local part is **, meaning all subdirectories.
        if (localPart == "**") {

            // Skip contiguous **.
            while (nextPart != endPart && *nextPart == "**") {
                ++nextPart;
            }

            // First, apply _next_ part (without **) to the local subdirectory.
            subExpandFilePath(result, baseDir, nextPart, endPart, filter);

            // Then apply the local scheme (**) on all subdirectories.
            localPart = "*";    // search all local subdirectories.
            --nextPart;         // back on **, applied on all local subdirectories.
            isLastPart = false; // since we stepped back.
        }
        else if (nextPart == endPart) {
            // Last component and not **. We also filter files.
            filterOptions |= QDir::Files;
        }

        // Load files and directories in the base directories using localPart as name filter.
        const QDir dir(baseDir, localPart, QDir::Name | QDir::IgnoreCase | QDir::DirsLast, filterOptions);

        // Loop on all entries in the directory.
        foreach (const QFileInfo& info, dir.entryInfoList()) {

            // Build full path.
            const QString fileName(info.fileName());
            const QString filePath(baseDir + QDir::separator() + fileName);

            // Check if this file path must be kept using the optional filter.
            // We only add paths when we reached the last componement in the path.
            if (isLastPart && (filter == 0 || filter->filePathFilter(info))) {
                result.append(filePath);
            }

            // Check if we need to recurse in a subdirectory.
            if (info.isDir() && !isLastPart && (filter == 0 || filter->recursionFilter(info))) {
                subExpandFilePath(result, filePath, nextPart, endPart, filter);
            }
        }
    }
}

QStringList QtlFile::expandFilePath(const QString& path, QtlFilePathFilterInterface* filter)
{
    // Resolve absolute path.
    QString absPath(absoluteNativeFilePath(path));
    const QChar sep(QDir::separator());

    // The object to return.
    QStringList result;

    // Locate first wildcard character in string.
    int start = absPath.indexOf(QRegExp("[?\\*]"));
    if (start < 0) {
        // No wildcard, point after end of string.
        start = absPath.length();
    }

    // Filter pathological case: a wildcard cannot be at beginning of absolute file path.
    if (start <= 0) {
        return result;
    }

    // Locate last directory separator before first wildcard (or end of string).
    start = absPath.lastIndexOf(sep, start - 1);

    // Longest directory path without wildcard.
    // Use at least "/" when root is the first directory without wildcard.
    QString baseDirectory(absPath.left(qMax(start, 1)));

    // Split all remaining components.
    QStringList parts(absPath.mid(start).split(sep, QString::SkipEmptyParts));

    // Run the exploration
    subExpandFilePath(result, baseDirectory, parts.begin(), parts.end(), filter);

    // Cleanup the list.
    result.sort(Qt::CaseInsensitive);
    result.removeDuplicates();
    return result;
}


//-----------------------------------------------------------------------------
// Get the absolute file path of the parent directory of a file.
//-----------------------------------------------------------------------------

QString QtlFile::parentPath(const QString& path, int upLevels)
{
    QFileInfo info(path);
    while (upLevels-- > 0) {
        info.setFile(info.absolutePath());
    }
    return absoluteNativeFilePath(info.absoluteFilePath());
}


//-----------------------------------------------------------------------------
// Create a directory and all parent directories if necessary.
//-----------------------------------------------------------------------------

bool QtlFile::createDirectory(const QString& path, bool createOnly)
{
    // Avoid looping forever.
    int foolProof = 1024;

    // Iterate over parents until one is found, keep path elements to create.
    QFileInfo info(absoluteNativeFilePath(path));
    QStringList subdirs;
    while (!info.isDir()) {
        // When too many levels of parents, this is suspect, abort.
        // If something with that name exists, this is not a directory, abort.
        // If we reached a root that is not a directory, abort.
        if (--foolProof < 0 || info.exists() || info.isRoot()) {
            return false;
        }
        // Push local name.
        subdirs.prepend(info.fileName());
        // Now examine parent.
        info.setFile(info.path());
    }

    // If the directory already exists and createOnly is true, this is an error.
    if (createOnly && subdirs.isEmpty()) {
        return false;
    }

    // Now create all levels of directory.
    QDir dir(info.filePath());
    while (!subdirs.isEmpty()) {
        const QString name(subdirs.takeFirst());
        if (!dir.mkdir(name)) {
            return false;
        }
        dir.setPath(dir.path() + QDir::separator() + name);
    }
    return true;
}


//-----------------------------------------------------------------------------
// Search a subdirectory in the parent path.
//-----------------------------------------------------------------------------

QString QtlFile::searchParentSubdirectory(const QString &dirName, const QString &subdirName, int maxLevels)
{
    if (!dirName.isEmpty() && !subdirName.isEmpty()) {
        QString dir(absoluteNativeFilePath(dirName));
        QFileInfo info(dir);
        do {
            const QString subdir(dir + QDir::separator() + subdirName);
            if (QDir(subdir).exists()) {
                return subdir;
            }
            info.setFile(parentPath(dir));
            dir = info.absoluteFilePath();
        } while (maxLevels-- > 0 && !dir.isEmpty() && !info.isRoot());
    }
    return "";
}


//-----------------------------------------------------------------------------
// Get the "short path name" of a file path.
//-----------------------------------------------------------------------------

QString QtlFile::shortPath(const QString& path, bool keepOnError)
{
#if defined(Q_OS_WIN)
    // Get the length of the corresponding short path.
    // We specify an empty output buffer. Thus, the returned value is the required
    // length to hold the path name, INCLUDING the terminating nul character.
    const QVector<wchar_t> input(qtlToWCharVector(path));
    int length = ::GetShortPathName(&input[0], NULL, 0);
    if (length <= 0) {
        // Error, typically non-existent file.
        return keepOnError ? path : QString();
    }

    // Get the actual short path. This time, the returned value is the length
    // of the returned path, EXCLUDING the terminating nul character.
    QVector<wchar_t> output(length);
    length = ::GetShortPathName(&input[0], &output[0], length);
    if (length <= 0 || length >= output.size()) {
        // Error, should not happen since the length was previously returned.
        return keepOnError ? path : QString();
    }

    // Convert returned wide string as a QString.
    return QString::fromWCharArray(&output[0], length);

#else
    // On non-Windows platforms, return the input path, unchanged.
    return path;
#endif
}


//-----------------------------------------------------------------------------
// Enforce a suffix in a file name.
//-----------------------------------------------------------------------------

QString QtlFile::enforceSuffix(const QString& path, const QString& suffix, Qt::CaseSensitivity cs)
{
    return path.endsWith(suffix, cs) ? path : path + suffix;
}


//-----------------------------------------------------------------------------
// Format the content of an URL into a file path if the URL scheme is file:
//-----------------------------------------------------------------------------

QString QtlFile::toFileName(const QUrl& url)
{
    // For non-file URL, return the full URL.
    if (url.scheme() != "file") {
        return url.url();
    }

    // Get the local file path.
    QString path(url.url(QUrl::PreferLocalFile));

    // On Windows, remove any leading slash.
#if defined(Q_OS_WIN)
    path.replace(QRegExp("^/*"), "");
#endif

    // Try to make the file path native.
    return QDir::toNativeSeparators(path);
}


//-----------------------------------------------------------------------------
// Write the content of a binary file.
//-----------------------------------------------------------------------------

bool QtlFile::writeBinaryFile(const QString& fileName, const QtlByteBlock& content)
{
    // Open the file. Note: the ~QFile() destructor will close it.
    QFile file(fileName);
    return file.open(QFile::WriteOnly) && writeBinary(file, content);
}


//-----------------------------------------------------------------------------
// Write binary data into an open file.
//-----------------------------------------------------------------------------

bool QtlFile::writeBinary(QIODevice& file, const void* data, int size, bool processEvents)
{
    const char* current = reinterpret_cast<const char*>(data);
    qint64 remain = size;
    while (remain > 0) {

        // Process pending events if required before entering a write operation.
        if (processEvents) {
            qApp->processEvents();
        }

        // Write as many bytes as possible.
        const qint64 written = file.write(current, remain);
        if (written <= 0) {
            // Error occured.
            return false;
        }

        // Next chunk.
        current += written;
        remain -= written;
    }
    return true;
}


//-----------------------------------------------------------------------------
// Read the content of a binary file.
//-----------------------------------------------------------------------------

QtlByteBlock QtlFile::readBinaryFile(const QString& fileName, int maxSize)
{
    // Open the file. Note: the ~QFile() destructor will close it.
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return QtlByteBlock();
    }

    // Read the file.
    QtlByteBlock data;
    readBinary(file, data, maxSize);
    return data;
}


//-----------------------------------------------------------------------------
// Read the content of a text file.
//-----------------------------------------------------------------------------

QStringList QtlFile::readTextLinesFile(const QString& fileName, int maxSize)
{
    // Open the file. Note: the ~QFile() destructor will close it.
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return QStringList();
    }

    // Read the file line by line.
    QTextStream text(&file);
    QStringList lines;
    int size = 0;
    while (maxSize < 0 || size < maxSize) {
        const QString line(text.readLine(maxSize < 0 ? 0 : qint64(maxSize - size)));
        if (line.isNull()) { // end of file
            break;
        }
        size += line.length();
        lines << line;
    }

    return lines;
}

QString QtlFile::readTextFile(const QString& fileName)
{
    // Open the file. Note: the ~QFile() destructor will close it.
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return QString();
    }

    // Read the file line by line.
    QTextStream text(&file);
    return text.readAll();
}


//-----------------------------------------------------------------------------
// Write the content of a text file.
//-----------------------------------------------------------------------------

bool QtlFile::writeTextLinesFile(const QString& fileName, const QStringList& lines)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        return false;
    }
    else {
        QTextStream stream(&file);
        foreach (const QString& line, lines) {
            stream << line << endl;
        }
        stream.flush();
        file.close();
        return true;
    }
}

bool QtlFile::writeTextFile(const QString& fileName, const QString& text)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        return false;
    }
    else {
        QTextStream stream(&file);
        stream << text;
        stream.flush();
        file.close();
        return true;
    }
}


//-----------------------------------------------------------------------------
// Read a portion of a binary file at a given position.
//-----------------------------------------------------------------------------

bool QtlFile::readBinary(QIODevice& device, QtlByteBlock& data, int maxSize)
{
    // Clear returned content.
    data.clear();

    // Chunk size for each read operation.
    const int maxChunkSize = 2048;
    const int chunkSize = maxSize < 0 ? maxChunkSize : qMin(maxSize, maxChunkSize);

    // Actual read size.
    int size = 0;
    bool success = true;

    // Read until maxSize or end of file.
    while (maxSize < 0 || size < maxSize) {
        // Resize buffer.
        data.resize(data.size() + chunkSize);

        // Read a chunk.
        const qint64 thisMax = maxSize < 0 || size + chunkSize < maxSize ? chunkSize : maxSize - size;
        const qint64 thisRead = device.read(reinterpret_cast<char*>(data.data() + size), thisMax);
        success = thisRead >= 0;
        if (!success) {
            break;
        }
        size += thisRead;
        if (thisRead < thisMax) {
            break;
        }
    }

    // Resize to actually read size.
    data.resize(size);

    // Do not report error now if some bytes were read.
    return success || size > 0;
}
