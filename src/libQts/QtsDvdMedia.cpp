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
// Qts, the Qt MPEG Transport Stream library.
// Define the class QtsDvdMedia.
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

#include "QtsDvdMedia.h"
#include "QtsDvdDataPull.h"
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

QtsDvdMedia::QtsDvdMedia(const QString& fileName, QtlLogger* log, QObject* parent, bool useMaxReadSpeed) :
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
    _rootDirectory(),
    _allFiles(),
    _currentFile(_allFiles.end())
{
    if (!fileName.isEmpty()) {
        openFromFile(fileName, useMaxReadSpeed);
    }
}

QtsDvdMedia::~QtsDvdMedia()
{
    close();
}


//----------------------------------------------------------------------------
// Close a DVD media.
//----------------------------------------------------------------------------

void QtsDvdMedia::close()
{
    const bool wasOpen = _isOpen;
    _isOpen = false;
    _deviceName.clear();
    _rootName.clear();
    _volumeId.clear();
    _volumeSize = 0;
    _nextSector = 0;
    _rootDirectory.clear();
    _allFiles.clear();
    _currentFile = _allFiles.end();

    if (_dvdcss != 0) {
        dvdcss_close(_dvdcss);
        _dvdcss = 0;
    }

    if (wasOpen) {
        emit closed();
    }
}


//----------------------------------------------------------------------------
// A function that can be used as message logger by libdvdcss.
// Require a modified version of libdvdcss.
// The logParam must be a QtlLogger*.
//----------------------------------------------------------------------------

extern "C" void dvdMessageLogger(void* logParam, int debug, const char* message)
{
    QtlLogger* log = reinterpret_cast<QtlLogger*>(logParam);
    if (log != 0 && message != 0) {
        if (debug != 0) {
            log->debug(QStringLiteral("libdvdcss: %1").arg(message));
        }
        else {
            log->line(QStringLiteral("libdvdcss: %1").arg(message));
        }
    }
}


//----------------------------------------------------------------------------
// Check if the DVD media is encrypted.
//----------------------------------------------------------------------------

bool QtsDvdMedia::isEncrypted() const
{
    return _dvdcss != 0 && dvdcss_is_scrambled(_dvdcss);
}


//----------------------------------------------------------------------------
// Open and load the description of a DVD media starting from a file.
//----------------------------------------------------------------------------

bool QtsDvdMedia::openFromFile(const QString& fileName, bool useMaxReadSpeed)
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
    if (!openFromDevice(deviceName, useMaxReadSpeed)) {
        return false;
    }

    // Save the root directory / mount point.
    _rootName = QtlFile::absoluteNativeFilePath(fs.rootPath());
    return true;
}


//----------------------------------------------------------------------------
// Open and load the description of a DVD media starting from its device name.
//----------------------------------------------------------------------------

bool QtsDvdMedia::openFromDevice(const QString& deviceName, bool useMaxReadSpeed)
{
    // Close previous media if necessary.
    close();

    // Initialize libdvdcss.
    const QByteArray name(deviceName.toUtf8());
    _dvdcss = dvdcss_open_log(name.data(), dvdMessageLogger, _log, 2);
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
    if (seekSector(DVD_VOLUME_DESCRIPTORS_SECTOR)) {
        // Each volume descriptor uses one sector. There must be one primary volume
        // descriptor and the list is terminated by one volume descriptor set terminator.
        // Here, we do not read more than 8 sectors (not standard, but highly probable).
        // We do not skip bad sectors in this part of the media.
        QtlByteBlock data(QTS_DVD_SECTOR_SIZE);
        for (int count = 8; count > 0 && readSectors(data.data(), 1, -1, Qts::ErrorOnBadSectors) == 1; --count) {
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

    // Check the detection of the root directory.
    if (rootDirSector < 0 || rootDirSize < 0) {
        _log->debug(tr("Cannot locate root directory on %1").arg(_deviceName));
        close();
        return false;
    }

    // Read the complete file structure.
    _rootDirectory = QtsDvdDirectory(QString(), rootDirSector, rootDirSize);
    if (!readDirectoryStructure(_rootDirectory, 0, true, false)) {
        close();
        return false;
    }

    // The list of all file has been built by readDirectoryStructure.
    // Sort the files according to physical placement on DVD.
    std::sort(_allFiles.begin(), _allFiles.end());

    // Walk through the list of files and insert placeholders on non-file space.
    int lastSector = 0;
    for (QList<QtsDvdFilePtr>::Iterator iter = _allFiles.begin(); iter != _allFiles.end(); ++iter) {
        if ((*iter)->startSector() > lastSector) {
            // There is a hole here, fill it with a placeholder.
            QtsDvdFilePtr ph(new QtsDvdFile(QString(), lastSector, ((*iter)->startSector() - lastSector) * QTS_DVD_SECTOR_SIZE));
            // Insert placeholder before current element.
            iter = _allFiles.insert(iter, ph); // iter now points to inserted element
            ++iter; // iter now points back to original current element
        }
        lastSector = (*iter)->endSector();
    }

    // Add placeholder at the end if necessary.
    if (lastSector < _volumeSize) {
        _allFiles << QtsDvdFilePtr(new QtsDvdFile(QString(), lastSector, (_volumeSize - lastSector) * QTS_DVD_SECTOR_SIZE));
    }

    // Set the reader at maximum read speed only now.
    if (useMaxReadSpeed && dvdcss_set_max_speed(_dvdcss) < 0) {
        _log->debug(tr("Failed to set DVD to maximum read speed, using default speed"));
    }

    // Position the current sector at beginning of media.
    _isOpen = true;
    if (!seekSector(0)) {
        close();
        return false;
    }

    // DVD media now successfully open.
    emit newMedia(_volumeId);
    return true;
}


//----------------------------------------------------------------------------
// Set the position of the next sector to read.
//----------------------------------------------------------------------------

bool QtsDvdMedia::seekSector(int position)
{
    // Check that the device is at least partially open.
    if (_dvdcss == 0 || position < 0 || (_isOpen && position >= _volumeSize)) {
        return false;
    }

    // Get flags for libdvdcss.
    int seekFlags = DVDCSS_NOFLAGS;
    QList<QtsDvdFilePtr>::ConstIterator file(_allFiles.end());

    // If not yet fully open, we are still reading the file structure and NOFLAGS is ok.
    if (_isOpen) {

        // Look for the file corresponding to position.
        for (file = _allFiles.begin(); file != _allFiles.end(); ++file) {
            if ((*file)->startSector() <= position && position < (*file)->endSector()) {
                break;
            }
        }
        if (file == _allFiles.end()) {
            _log->line(tr("Error seeking DVD to sector %1, not found in media layout").arg(position));
            return false;
        }
        if (file != _currentFile) {
            _log->debug(tr("Switching to %1 on DVD").arg((*file)->description()));
        }

        // If this is a VOB file, decrypt.
        if ((*file)->isVob()) {
            // Use SEEK_KEY when changing file on encrypted DVD to force a key search.
            seekFlags = file == _currentFile || !dvdcss_is_scrambled(_dvdcss) ? DVDCSS_SEEK_MPEG : DVDCSS_SEEK_KEY;
        }
    }

    // Let libdvdcss do the job.
    const int count = dvdcss_seek(_dvdcss, position, seekFlags);
    if (count < 0) {
        _log->line(tr("Error seeking DVD to sector %1, dvdcss_seek returned %2").arg(position).arg(count));
        return false;
    }

    _currentFile = file;
    _nextSector = position;
    return true;
}


//----------------------------------------------------------------------------
// Read a given number of sectors from the DVD media.
//----------------------------------------------------------------------------

int QtsDvdMedia::readSectors(void* buffer, int count, int position, Qts::BadSectorPolicy badSectorPolicy)
{
    // Check that the device is at least partially open.
    // Seek if requested.
    if (_dvdcss == 0 || (position >= 0 && !seekSector(position))) {
        return -1;
    }

    // If the media is fully open, do not read more than the media size.
    int endSector = _nextSector + count;
    if (_isOpen && endSector > _volumeSize) {
        endSector = _volumeSize;
    }

    // Loop on sectors read.
    char* buf = reinterpret_cast<char*>(buffer);
    int result = 0;
    int badSectorMax = DVD_BAD_SECTOR_RETRY;

    // We need both checks below if the bad sector policy is "skip" since
    // count and _nextSector may not identically advance.
    while (count > 0 && _nextSector < endSector) {

        // See how many sectors we can read in the current file.
        int readFlags = DVDCSS_NOFLAGS;
        int seekFlags = DVDCSS_NOFLAGS;
        int readCount = count;

        // If not yet fully open, we are still reading the file structure.
        // Otherwise, check where we are in the list of files.
        if (_isOpen && _currentFile != _allFiles.end()) {

            // See if we need to change file.
            bool changed = false;
            while (_currentFile != _allFiles.end() && _nextSector >= (*_currentFile)->endSector()) {
                ++_currentFile;
                changed = true;
            }
            if (_currentFile == _allFiles.end()) {
                // Reached the end of media, exit the read loop.
                break;
            }

            // If current file is a VOB, we need to decrypt.
            if ((*_currentFile)->isVob() && dvdcss_is_scrambled(_dvdcss)) {
                readFlags = DVDCSS_READ_DECRYPT;
                seekFlags = DVDCSS_SEEK_KEY;
            }

            // If we changed, do an explicit seek.
            // Not sure it is necessary but enforce an encrypted<=>clear or key switch.
            if (changed) {
                _log->debug(tr("Switching to %1 on DVD").arg((*_currentFile)->description()));
                if (dvdcss_seek(_dvdcss, _nextSector, seekFlags) < 0) {
                    _log->line(tr("Error seeking at first sector of %1").arg((*_currentFile)->description()));
                    return result > 0 ? result : -1;
                }
            }

            // Do not read more than available in current file.
            readCount = qMin(readCount, (*_currentFile)->endSector() - _nextSector);
        }

        // Try to read all requested sectors.
        int got = dvdcss_read(_dvdcss, buf, readCount, readFlags);

        if (got > 0) {
            // dvdcss_read successful, reset bad sector count.
            if (badSectorMax < DVD_BAD_SECTOR_RETRY) {
                _log->line(tr("Skipped %1 bad sectors in %2").arg(DVD_BAD_SECTOR_RETRY - badSectorMax).arg((*_currentFile)->description()));
                badSectorMax = DVD_BAD_SECTOR_RETRY;
            }
        }
        else if (badSectorPolicy != Qts::ErrorOnBadSectors && badSectorMax > 0) {
            // In case of read error, this may be an intentional bad sector, used to fool copy programs.
            // Let's ignore it if we can explicitly seek to next sector.
            if (dvdcss_seek(_dvdcss, _nextSector + 1, seekFlags) > 0) {
                badSectorMax--;
                switch (badSectorPolicy) {
                    case Qts::SkipBadSectors:
                        got = 0;
                        ++_nextSector;
                        break;
                    case Qts::ReadBadSectorsAsZero:
                        got = 1;
                        ::memset(buf, 0, QTS_DVD_SECTOR_SIZE);
                        break;
                    default:
                        _log->line(tr("Internal error, invalid bad sector policy"));
                        break;
                }
            }
        }

        if (got < 0) {
            // Unrecoverable error.
            _log->line(tr("Error reading sector %1 on DVD media in %2").arg(_nextSector).arg(_deviceName));
            return result > 0 ? result : -1;
        }

        _nextSector += got;
        result += got;
        count -= got;
        buf += got * QTS_DVD_SECTOR_SIZE;
    }

    return result;
}


//----------------------------------------------------------------------------
// Read the file structure under the specified directory.
//----------------------------------------------------------------------------

bool QtsDvdMedia::readDirectoryStructure(QtsDvdDirectory& dir, int depth, bool inRoot, bool inVideoTs)
{
    // Avoid loops in the file system, limit the depth.
    if (depth > 256) {
        _log->line(tr("Too many directory levels in DVD structure, probably incorrect media"));
        return false;
    }

    // Prefix for child objects.
    QString parent(dir.path());
    if (!parent.isEmpty()) {
        parent.append(QDir::separator());
    }

    // Read directory content.
    const int dirSectorCount = dir.sizeInBytes() / QTS_DVD_SECTOR_SIZE + int(dir.sizeInBytes() % QTS_DVD_SECTOR_SIZE != 0);
    QtlByteBlock data(dirSectorCount * QTS_DVD_SECTOR_SIZE);
    if (!readSectors(data.data(), dirSectorCount, dir.startSector(), Qts::ErrorOnBadSectors)) {
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
            QtsDvdDirectoryPtr subdir(new QtsDvdDirectory(parent + name, entrySector, entrySize));
            dir._subDirectories.append(subdir);
            // Is this directory the root VIDEO_TS?
            const bool vts = inRoot && name.compare("VIDEO_TS", Qt::CaseInsensitive) == 0;
            // Explore subdirectory tree.
            if (!readDirectoryStructure(*subdir, depth + 1, false, vts)) {
                return false;
            }
        }
        else {
            // Add a file entry.
            QtsDvdFilePtr file(new QtsDvdFile(parent + name, entrySector, entrySize));
            dir._files.append(file);
            _allFiles.append(file);
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

QtsDvdFile QtsDvdMedia::searchFile(const QString& fileName, Qt::CaseSensitivity cs) const
{
    // Can't search file if there is no DVD.
    if (!_isOpen) {
        return QtsDvdFile();
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

QString QtsDvdMedia::vtsInformationFileName(int vtsNumber) const
{
    const QString name(QStringLiteral("VIDEO_TS%1VTS_%2_0.IFO").arg(QDir::separator()).arg(vtsNumber, 2, 10, QLatin1Char('0')));
    return _rootName.isEmpty() ? name : _rootName + QDir::separator() + name;
}


//----------------------------------------------------------------------------
// Build the full path name of a Video Title Set video file on the DVD.
//----------------------------------------------------------------------------

QString QtsDvdMedia::vtsVideoFileName(int vtsNumber, int vobIndex) const
{
    const QString name(QStringLiteral("VIDEO_TS%1VTS_%2_%3.VOB").arg(QDir::separator()).arg(vtsNumber, 2, 10, QLatin1Char('0')).arg(vobIndex));
    return _rootName.isEmpty() ? name : _rootName + QDir::separator() + name;
}


//----------------------------------------------------------------------------
// Get the description of a Video Title Set information file on the DVD.
//----------------------------------------------------------------------------

QtsDvdFile QtsDvdMedia::vtsInformationFile(int vtsNumber) const
{
    const QString name(QStringLiteral("VTS_%1_0.IFO").arg(vtsNumber, 2, 10, QLatin1Char('0')));
    return _rootDirectory.searchPath(QtlStringList("VIDEO_TS", name));
}


//----------------------------------------------------------------------------
// Get the description of a Video Title Set video file on the DVD.
//----------------------------------------------------------------------------

QtsDvdFile QtsDvdMedia::vtsVideoFile(int vtsNumber, int vobIndex) const
{
    const QString name(QStringLiteral("VTS_%1_%2.VOB").arg(vtsNumber, 2, 10, QLatin1Char('0')).arg(vobIndex));
    return _rootDirectory.searchPath(QtlStringList("VIDEO_TS", name));
}


//----------------------------------------------------------------------------
// Check if a file name matches a Video Title Set information file.
//----------------------------------------------------------------------------

int QtsDvdMedia::vtsInformationFileNumber(const QString& fileName)
{
    const QString name(QFileInfo(fileName).fileName());
    // Does name match "VTS_nn_0.IFO"?
    int vts = -1;
    if (name.length() == 12 && name.startsWith("VTS_", Qt::CaseInsensitive) && name.endsWith("_0.IFO", Qt::CaseInsensitive)) {
        vts = qtlToInt(name.mid(4, 2)); // -1 if incorrect
    }
    return vts;
}


//----------------------------------------------------------------------------
// Load all encryption keys into the cache of libdvdcss.
//----------------------------------------------------------------------------

bool QtsDvdMedia::loadAllEncryptionKeys()
{
    // We need to be able to locate all encryption keys.
    if (_dvdcss == 0 || _allFiles.isEmpty()) {
        _log->line(tr("DVD media is not open or its file structure has not been read"));
        return false;
    }

    // Loop on all files.
    bool success = true;
    foreach (const QtsDvdFilePtr& file, _allFiles) {
        // We seek on first VOB's.
        const QString name(file->name());
        if (name.endsWith("_TS.VOB", Qt::CaseInsensitive) || name.endsWith("_0.VOB", Qt::CaseInsensitive) || name.endsWith("_1.VOB", Qt::CaseInsensitive)) {
            _log->debug(tr("Getting CSS key for %1").arg(file->description()));
            if (dvdcss_seek(_dvdcss, file->startSector(), DVDCSS_SEEK_KEY) < 0) {
                _log->line(tr("Error getting CSS key for %1").arg(file->description()));
                success = false;
            }
        }
    }
    return success;
}
