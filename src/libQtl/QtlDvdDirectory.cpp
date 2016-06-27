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
//
// Define the class QtlDvdDirectory.
//
//----------------------------------------------------------------------------

#include "QtlDvdDirectory.h"


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlDvdDirectory::QtlDvdDirectory(const QString& name, int startSector, int sizeInBytes) :
    QtlDvdFile(name, startSector, sizeInBytes),
    _subDirectories(),
    _files()
{
}


//----------------------------------------------------------------------------
// Clear object content.
//----------------------------------------------------------------------------

void QtlDvdDirectory::clear()
{
    QtlDvdFile::clear();
    _subDirectories.clear();
    _files.clear();
}


//----------------------------------------------------------------------------
// Search files and directories.
//----------------------------------------------------------------------------

QtlDvdFile QtlDvdDirectory::searchFile(const QString& fileName, Qt::CaseSensitivity cs) const
{
    foreach (const QtlDvdFile& file, _files) {
        if (fileName.compare(file.name(), cs) == 0) {
            return file;
        }
    }
    return QtlDvdFile(fileName);
}

QtlDvdFile QtlDvdDirectory::searchPath(const QString& path, Qt::CaseSensitivity cs) const
{
    return searchPath(path.split(QRegExp("[\\\\/]+"), QString::SkipEmptyParts), cs);
}

QtlDvdFile QtlDvdDirectory::searchPath(const QStringList& path, Qt::CaseSensitivity cs) const
{
    return searchPath(path.begin(), path.end(), cs);
}

QtlDvdFile QtlDvdDirectory::searchPath(QStringList::ConstIterator begin, QStringList::ConstIterator end, Qt::CaseSensitivity cs) const
{
    if (begin == end) {
        // Empty path, no file.
        return QtlDvdFile();
    }
    const QStringList::ConstIterator next(begin + 1);
    if (next == end) {
        // We are at the end of the path, search a file.
        return searchFile(*begin, cs);
    }
    foreach (const QtlDvdDirectory& dir, _subDirectories) {
        if (begin->compare(dir.name(), cs) == 0) {
            return dir.searchPath(next, end, cs);
        }
    }
    // Not found
    return QtlDvdFile(*(end - 1));
}
