//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2016, Thierry Lelegard
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
// Define the class QtlDvdMedia.
//
// Relevant documentation: The structure of a Video DVD is not public but
// some useful information can be found on the Internet.
//
// - http://www.osta.org/specs/pdf/udf102.pdf
//   UDF 1.0.2 specifications (Universal Disk Format). UDF is the underlying
//   file system structure in Video DVD's. This specification includes the
//   specificities for Video DVD. Note that more recent versions of this
//   specification are available but not used on Video DVD's.
//
// - http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-119.pdf
//   Volume and File Structure of CDROM for Information Interchange
//
// The structure of a Video DVD has limitations:
// - Sector size = logical block size = 2048 bytes
// - One single volume
// - One single partition
//
//----------------------------------------------------------------------------

#include "QtlDvdMedia.h"
#include "QtlDvdDataPull.h"
#include "QtlByteBlock.h"
#include "QtlStringList.h"
#include "QtlStringUtils.h"
#include "QtlFile.h"
#include "dvdcss.h"

//
// Layout of a DVD volume. See ECMA-119.
//
#define DVD_VOLUME_DESCRIPTORS_SECTOR       16
#define DVD_VOLUME_DESCRIPTOR_ID       "CD001"
#define DVD_VOLDESC_TYPE_PRIMARY             1
#define DVD_VOLDESC_TYPE_TERMINATOR        255
#define DVD_ROOTDIRDESC_OFFSET             156
#define DVD_ROOTDIRDESC_SIZE                34
#define DVD_BAD_SECTOR_RETRY                64


//----------------------------------------------------------------------------
// Constructor & destructor.
//----------------------------------------------------------------------------

QtlDvdMedia::QtlDvdMedia(const QString& fileName, QtlLogger* log, QObject* parent) :
    QObject(parent),
    _nullLog(),
    _log(log != 0 ? log : &_nullLog),
    _isOpen(false),
    _deviceName(),
    _rootName(),
    _volumeId(),
    _volumeSize(0),
    _vtsCount(0),
    _dvdcss(0),
    _nextSector(0),
    _rootDirectory()
{
    if (!fileName.isEmpty()) {
        openFromFile(fileName);
    }
}

QtlDvdMedia::~QtlDvdMedia()
{
    close();
}


//----------------------------------------------------------------------------
// Close a DVD media.
//----------------------------------------------------------------------------

void QtlDvdMedia::close()
{
    const bool wasOpen = _isOpen;
    _isOpen = false;
    _deviceName.clear();
    _rootName.clear();
    _volumeId.clear();
    _volumeSize = 0;
    _nextSector = 0;
    _rootDirectory.clear();

    if (_dvdcss != 0) {
        dvdcss_close(_dvdcss);
        _dvdcss = 0;
    }

    if (wasOpen) {
        emit closed();
    }
}


//----------------------------------------------------------------------------
// Check if the DVD media is encrypted.
//----------------------------------------------------------------------------

bool QtlDvdMedia::isEncrypted() const
{
    return _dvdcss != 0 && dvdcss_is_scrambled(_dvdcss);
}


//----------------------------------------------------------------------------
// Open and load the description of a DVD media starting from a file.
//----------------------------------------------------------------------------

bool QtlDvdMedia::openFromFile(const QString& fileName)
{
    // Close previous media if necessary.
    close();

    // Get the file system for this file. Make sure to use a complete path
    // with all symbolic links removed just in case QStorageInfo is fooled.
    QStorageInfo fs(QtlFile::absoluteNativeFilePath(fileName, true));
    if (!fs.isValid() || !fs.isReady()) {
        return false;
    }

    // Get the device name.
    QString deviceName;
#if defined(Q_OS_WIN)
    // On Windows, if we are on a DVD reader, the file name starts with a drive name.
    // Network shares like \\... cannot be DVD readers.
    if (fileName.length() > 2 && fileName[1] == ':') {
        deviceName = fileName.left(2);
    }
#elif defined(Q_OS_OSX)
    // On Mac OS X, make sure we use /dev/rdiskN, not the block device /dev/diskN.
    deviceName = QString(fs.device());
    deviceName.replace(QRegExp("^/dev/disk"), "/dev/rdisk");
#else
    deviceName = QString(fs.device());
#endif

    // Check the device name.
    if (deviceName.isEmpty()){
        _log->debug(tr("Device name not found for %1").arg(fileName));
        return false;
    }

    // Open the device.
    if (!openFromDevice(deviceName, true)) {
        return false;
    }

    // Save the root directory / mount point.
    _rootName = QtlFile::absoluteNativeFilePath(fs.rootPath());
    return true;
}


//----------------------------------------------------------------------------
// Open and load the description of a DVD media starting from its device name.
//----------------------------------------------------------------------------

bool QtlDvdMedia::openFromDevice(const QString& deviceName, bool loadFileStructure)
{
    // Close previous media if necessary.
    close();

    // Initialize libdvdcss.
    const QByteArray name(deviceName.toUtf8());
    _dvdcss = dvdcss_open(name.data());
    if (_dvdcss == 0) {
        _log->debug(tr("Cannot initialize libdvdcss on %1").arg(deviceName));
        close();
        return false;
    }

    // Keep device name.
    _deviceName = deviceName;

    // Position and size of the root directory.
    int rootDirSector = -1;
    int rootDirSize = -1;

    // Read primary volume descriptor.
    // ECMA-119, section 6.7.1: The volume descriptor set starts at sector 16.
    if (seekSector(DVD_VOLUME_DESCRIPTORS_SECTOR, false)) {
        // Each volume descriptor uses one sector. There must be one primary volume
        // descriptor and the list is terminated by one volume descriptor set terminator.
        // Here, we do not read more than 8 sectors (not standard, but highly probable).
        QtlByteBlock data(DVD_SECTOR_SIZE);
        for (int count = 8; count > 0 && readSectors(data.data(), 1, -1, false, false) == 1; --count) {
            // Volume descriptor type is in first byte.
            const int type = data[0];
            // Volume descriptor standard identified is in the next 5 bytes.
            const QString id = data.getLatin1(1, sizeof(DVD_VOLUME_DESCRIPTOR_ID) - 1);
            if (id != DVD_VOLUME_DESCRIPTOR_ID) {
                // Not a valid volume descriptor.
                _log->debug(tr("Invalid DVD volume descriptor \"%1\" instead of \"%2\"").arg(id).arg(DVD_VOLUME_DESCRIPTOR_ID));
                break;
            }
            else if (type == DVD_VOLDESC_TYPE_TERMINATOR) {
                // Reached the "volume description set terminator", primary volume descriptor not found.
                break;
            }
            else if (type == DVD_VOLDESC_TYPE_PRIMARY) {
                // Found the "primary volume descriptor".
                // Format: see ECMA-119, section 8.4.
                _volumeId = data.getLatin1(40, 32).trimmed();
                _volumeSize = data.fromLittleEndian<quint32>(80);
                // The "Directory Record for Root Directory" is at offset 156, 34 bytes long.
                // Format: see ECMA-119, section 9.1. Size of record is in first byte.
                if (data[DVD_ROOTDIRDESC_OFFSET] == DVD_ROOTDIRDESC_SIZE) {
                    // Sector number at offset 2, size in bytes at offset 10.
                    rootDirSector = data.fromLittleEndian<quint32>(DVD_ROOTDIRDESC_OFFSET + 2);
                    rootDirSize = data.fromLittleEndian<quint32>(DVD_ROOTDIRDESC_OFFSET + 10);
                }
                break;
            }
        }
    }

    // Load the file structure if requested.
    if (loadFileStructure) {

        // Check the detection of the root directory.
        if (rootDirSector < 0 || rootDirSize < 0) {
            _log->debug(tr("Cannot locate root directory on %1").arg(_deviceName));
            close();
            return false;
        }

        // Read the complete file structure.
        _rootDirectory = QtlDvdDirectory(QString(), rootDirSector, rootDirSize);
        if (!readDirectoryStructure(_rootDirectory, 0, true, false)) {
            close();
            return false;
        }
    }

    // DVD media now successfully open.
    _isOpen = true;
    emit newMedia(_volumeId);
    return true;
}


//----------------------------------------------------------------------------
// Set the position of the next sector to read.
//----------------------------------------------------------------------------

bool QtlDvdMedia::seekSector(int position, bool vobContent)
{
    // Check that the device is at least partially open.
    if (_dvdcss == 0 || position < 0 || (_isOpen && position >= _volumeSize)) {
        return false;
    }

    // Let libdvdcss do the job.
    const int count = dvdcss_seek(_dvdcss, position, vobContent ? DVDCSS_SEEK_MPEG : DVDCSS_NOFLAGS);
    if (count < 0) {
        _log->line(tr("Error seeking DVD to sector %1, seek returned %2").arg(position).arg(count));
        return false;
    }

    _nextSector = position;
    return true;
}


//----------------------------------------------------------------------------
// Read a given number of sectors from the DVD media.
//----------------------------------------------------------------------------

int QtlDvdMedia::readSectors(void* buffer, int count, int position, bool vobContent, bool skipBadSectors)
{
    // Check that the device is at least partially open.
    if (_dvdcss == 0 || (position >= 0 && !seekSector(position, vobContent))) {
        return -1;
    }

    // Loop on sectors read.
    char* buf = reinterpret_cast<char*>(buffer);
    int result = 0;
    while (count > 0) {

        // Try to read all requested sectors.
        int got = dvdcss_read(_dvdcss, buf, count, vobContent ? DVDCSS_READ_DECRYPT : DVDCSS_NOFLAGS);

        if (got <= 0 && skipBadSectors) {
            // In case of read error, this may be an intentional bad sector, used to fool copy programs.
            // We try to skip this one and try next one, etc.
            const int lastRetry = _nextSector + DVD_BAD_SECTOR_RETRY;
            while (got < 0 && _nextSector <= lastRetry) {
                if (dvdcss_seek(_dvdcss, ++_nextSector, vobContent ? DVDCSS_SEEK_MPEG : DVDCSS_NOFLAGS) > 0) {
                    got = dvdcss_read(_dvdcss, buf, 1, vobContent ? DVDCSS_READ_DECRYPT : DVDCSS_NOFLAGS);
                }
            }
        }

        if (got <= 0) {
            // Unrecoverable error.
            _log->line(tr("Error reading sector %1 on DVD media in %2").arg(_nextSector).arg(_deviceName));
            return result > 0 ? result : -1;
        }

        result += got;
        _nextSector += got;
        count -= got;
        buf += got * DVD_SECTOR_SIZE;
    }

    return result;
}


//----------------------------------------------------------------------------
// Read the file structure under the specified directory.
//----------------------------------------------------------------------------

bool QtlDvdMedia::readDirectoryStructure(QtlDvdDirectory& dir, int depth, bool inRoot, bool inVideoTs)
{
    // Avoid loops in the file system, limit the depth.
    if (depth > 256) {
        _log->line(tr("Too many directory levels in DVD structure, probably incorrect media"));
        return false;
    }

    // Read directory content.
    const int dirSectorCount = dir.sizeInBytes() / DVD_SECTOR_SIZE + int(dir.sizeInBytes() % DVD_SECTOR_SIZE != 0);
    QtlByteBlock data(dirSectorCount * DVD_SECTOR_SIZE);
    if (!readSectors(data.data(), dirSectorCount, dir.startSector(), false, false)) {
        _log->line(tr("Error reading DVD directory information at sector %1").arg(dir.startSector()));
        return false;
    }

    // Loop on all directory entries. Format: see ECMA-119, section 9.1.
    int length = 0;
    for (int index = 0; (length = data[index]) >= 34 && index + length <= dir.sizeInBytes(); index += length) {

        // Get entry name.
        // Special entry names have are one byte long with special value for this byte:
        // - 00 = descriptor for current directory
        // - 01 = descriptor for parent directory (or current if at root directory)
        const int nameLength = data[index + 32];
        QString name;
        if (nameLength == 1 && (data[index + 33] == 0x00 || data[index + 33] == 0x01)) {
            continue; // skip this one
        }
        name = data.getLatin1(index + 33, nameLength);

        // Remove version number after ';'.
        name.remove(QRegExp(";.*$"));

        // Is this a directory entry?
        const bool isDirectory = (data[index + 25] & 0x02) != 0;

        // Size and position of the file.
        const int entrySector = data.fromLittleEndian<quint32>(index + 2);
        const int entrySize = data.fromLittleEndian<quint32>(index + 10);

        if (isDirectory) {
            // Add a directory entry.
            dir._subDirectories.append(QtlDvdDirectory(name, entrySector, entrySize));
            // Is this directory the root VIDEO_TS?
            const bool vts = inRoot && name.compare("VIDEO_TS", Qt::CaseInsensitive) == 0;
            // Explore subdirectory tree.
            if (!readDirectoryStructure(dir._subDirectories.last(), depth + 1, false, vts)) {
                return false;
            }
        }
        else {
            // Add a file entry.
            dir._files.append(QtlDvdFile(name, entrySector, entrySize));
            // Is this a video title set? Must be VTS_nn_0.IFO.
            if (inVideoTs && vtsInformationFileNumber(name) >= 0) {
                ++_vtsCount;
            }
        }
    }

    return true;
}


//----------------------------------------------------------------------------
// Locate a file in the DVD.
//----------------------------------------------------------------------------

QtlDvdFile QtlDvdMedia::searchFile(const QString& fileName, Qt::CaseSensitivity cs) const
{
    // Can't search file if there is no DVD.
    if (!_isOpen) {
        return QtlDvdFile();
    }

    // Full path of the file to search.
    QString path(QtlFile::absoluteNativeFilePath(fileName, true));

    // Check if it starts with the mount point of the DVD. Note that we compare
    // using the case sensitivity of the operating system, not the user-specified one.
    if (path.startsWith(_rootName, QTL_FILE_NAMES_CASE_SENSITIVE)) {
        path.remove(0, _rootName.length());
    }

    // Search the rest of the path from the root directory.
    return _rootDirectory.searchPath(path, cs);
}


//----------------------------------------------------------------------------
// Build the full path name of a Video Title Set information file on the DVD.
//----------------------------------------------------------------------------

QString QtlDvdMedia::vtsInformationFileName(int vtsNumber) const
{
    const QString name(QStringLiteral("VIDEO_TS%1VTS_%2_0.IFO").arg(QDir::separator()).arg(vtsNumber, 2, 10, QLatin1Char('0')));
    return _rootName.isEmpty() ? name : _rootName + QDir::separator() + name;
}


//----------------------------------------------------------------------------
// Build the full path name of a Video Title Set video file on the DVD.
//----------------------------------------------------------------------------

QString QtlDvdMedia::vtsVideoFileName(int vtsNumber, int vobIndex) const
{
    const QString name(QStringLiteral("VIDEO_TS%1VTS_%2_%3.VOB").arg(QDir::separator()).arg(vtsNumber, 2, 10, QLatin1Char('0')).arg(vobIndex));
    return _rootName.isEmpty() ? name : _rootName + QDir::separator() + name;
}


//----------------------------------------------------------------------------
// Get the description of a Video Title Set information file on the DVD.
//----------------------------------------------------------------------------

QtlDvdFile QtlDvdMedia::vtsInformationFile(int vtsNumber) const
{
    const QString name(QStringLiteral("VTS_%1_0.IFO").arg(vtsNumber, 2, 10, QLatin1Char('0')));
    return _rootDirectory.searchPath(QtlStringList("VIDEO_TS", name));
}


//----------------------------------------------------------------------------
// Get the description of a Video Title Set video file on the DVD.
//----------------------------------------------------------------------------

QtlDvdFile QtlDvdMedia::vtsVideoFile(int vtsNumber, int vobIndex) const
{
    const QString name(QStringLiteral("VTS_%1_%2.VOB").arg(vtsNumber, 2, 10, QLatin1Char('0')).arg(vobIndex));
    return _rootDirectory.searchPath(QtlStringList("VIDEO_TS", name));
}


//----------------------------------------------------------------------------
// Check if a file name matches a Video Title Set information file.
//----------------------------------------------------------------------------

int QtlDvdMedia::vtsInformationFileNumber(const QString& fileName)
{
    const QString name(QFileInfo(fileName).fileName());
    // Does name match "VTS_nn_0.IFO"?
    int vts = -1;
    if (name.length() == 12 && name.startsWith("VTS_", Qt::CaseInsensitive) && name.endsWith("_0.IFO", Qt::CaseInsensitive)) {
        vts = qtlToInt(name.mid(4, 2)); // -1 if incorrect
    }
    return vts;
}
