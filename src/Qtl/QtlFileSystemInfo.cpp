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
//
// Qtl, Qt utility library.
// Define the class QtlFileSystemInfo.
//
//----------------------------------------------------------------------------

#include "QtlFileSystemInfo.h"
#include "QtlFile.h"
#include "QtlStringUtils.h"

#if defined(Q_OS_WIN)
#include <windows.h>
#elif defined(Q_OS_LINUX)
#include <sys/statvfs.h>
#endif


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlFileSystemInfo::QtlFileSystemInfo(const QString& fileName) :
    _rootName()
{
    if (!fileName.isEmpty()) {

        // Get full path name in native format.
        const QString pathName(QtlFile::absoluteNativeFilePath(fileName));

        // Now get the actual root of the file system.
#if defined(Q_OS_WIN)
        // On Windows, just keep the drive letter.
        if (pathName.length() >= 2 && pathName[1] == QLatin1Char(':')) {
            _rootName = pathName.mid(0, 2).toUpper();
        }
#else
        // On Linux, get the longest mount point in the path of the file.
        const QStringList roots(getAllRoots());
        foreach (const QString& root, roots) {
            if (pathName.startsWith(root) && root.length() > _rootName.length()) {
                _rootName = root;
            }
        }
#endif
    }
}


//----------------------------------------------------------------------------
// Get either the total size or free size in bytes of the file system.
//----------------------------------------------------------------------------

qlonglong QtlFileSystemInfo::byteSize(bool total) const
{
#if defined(Q_OS_WIN)

    // Windows version.
    const QVector<wchar_t> wstr(qtlToWCharVector(_rootName));
    ::ULARGE_INTEGER totalUser, freeDisk;
    totalUser.QuadPart = freeDisk.QuadPart = 0;
    if (::GetDiskFreeSpaceEx(&wstr[0], NULL, &totalUser, &freeDisk) == 0) {
        return -1;
    }
    else if (total) {
        return totalUser.QuadPart;
    }
    else {
        return freeDisk.QuadPart;
    }

#else

    // Linux version.
    const QByteArray name(_rootName.toUtf8());
    struct statvfs vfs;
    if (::statvfs(name.constData(), &vfs) < 0) {
        return -1;
    }
    else if (total) {
        return qlonglong(vfs.f_blocks) * qlonglong(vfs.f_frsize);
    }
    else {
        return qlonglong(vfs.f_bfree) * qlonglong(vfs.f_frsize);
    }

#endif
}


//----------------------------------------------------------------------------
// Get the list of all mounted filesystems in the system.
//----------------------------------------------------------------------------

QStringList QtlFileSystemInfo::getAllRoots()
{
    QStringList roots;

#if defined(Q_OS_WIN)

    // On Windows, get all drives.
    const ::DWORD mask = ::GetLogicalDrives();
    for (int i = 0; i < 26; ++i) {
        if ((mask & (1 << i)) != 0) {
            roots << QStringLiteral("%1:").arg(QLatin1Char('A' + i));
        }
    }

#else

    // On Linux, read the content of /proc/mounts.
    const QStringList lines(QtlFile::readTextLinesFile("/proc/mounts", 50000));
    foreach (const QString& line, lines) {
        // Split lines in space-separated fields.
        const QStringList fields(line.split(QRegExp("\\s+"), QString::SkipEmptyParts));
        // Field 0: device path, ignore "auto.*"
        // Field 1: mount point
        // Field 2: file system type
        if (fields.size() >= 2 && !fields[0].startsWith("auto.")) {
            roots << fields[1];
        }
    }

#endif

    roots.sort(Qt::CaseInsensitive);
    roots.removeDuplicates();
    return roots;
}


//----------------------------------------------------------------------------
// Get the list of all mounted filesystems in the system.
//----------------------------------------------------------------------------

QList<QtlFileSystemInfo> QtlFileSystemInfo::getAllFileSystems()
{
    QList<QtlFileSystemInfo> fs;
    const QStringList roots(getAllRoots());
    foreach (const QString& root, roots) {
        fs << QtlFileSystemInfo(root);
    }
    return fs;
}
