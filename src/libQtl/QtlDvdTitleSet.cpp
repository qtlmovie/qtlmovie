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
// Define the class QtlDvdTitleSet.
//
// Relevant documentation: The structure of a Video DVD is not public but
// some useful information can be found on the Internet.
//
// - http://dvd.sourceforge.net/dvdinfo/
//   Information about DVD metadata, contents of VMG and VTS .IFO files.
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

#include "QtlDvdTitleSet.h"
#include "QtlFile.h"
#include "QtlHexa.h"
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

//
// Layout of IFO file for title sets (VTS_nn_0.IFO).
// See http://dvd.sourceforge.net/dvdinfo/ifo.html
//
#define DVD_IFO_START_VALUE     "DVDVIDEO-VTS"
#define DVD_IFO_HEADER_SIZE             0x03D8
#define DVD_IFO_LAST_SECTOR             0x000C
#define DVD_IFO_START_SECTOR            0x00C4
#define DVD_IFO_VTS_PGCI_OFFSET         0x00CC
#define DVD_IFO_VTS_PGC_OFFSET_IN_PGCI  0x000C
#define DVD_IFO_PALETTE_OFFSET_IN_PGC   0x00A4
#define DVD_IFO_PALETTE_ENTRY_SIZE           4
#define DVD_IFO_PALETTE_ENTRY_COUNT         16
#define DVD_IFO_VIDEO_ATTR_OFFSET       0x0200
#define DVD_IFO_AUDIO_COUNT_OFFSET      0x0202
#define DVD_IFO_AUDIO_ATTR_OFFSET       0x0204
#define DVD_IFO_AUDIO_ATTR_SIZE              8
#define DVD_IFO_AUDIO_MAX_COUNT              8
#define DVD_IFO_SUBPIC_COUNT_OFFSET     0x0254
#define DVD_IFO_SUBPIC_ATTR_OFFSET      0x0256
#define DVD_IFO_SUBPIC_ATTR_SIZE             6
#define DVD_IFO_SUBPIC_MAX_COUNT            32


//----------------------------------------------------------------------------
// Constructor & destructor.
//----------------------------------------------------------------------------

QtlDvdTitleSet::QtlDvdTitleSet(const QString& fileName, QtlLogger* log) :
    _nullLog(),
    _log(log != 0 ? log : &_nullLog),
    _deviceName(),
    _volumeId(),
    _isEncrypted(false),
    _dvdcss(0),
    _vtsNumber(-1),
    _ifoFileName(),
    _vobFileNames(),
    _vobSize(0),
    _vobStartSector(-1),
    _nextSector(0),
    _palette(),
    _streams()
{
    if (!fileName.isEmpty()) {
        open(fileName);
    }
}

QtlDvdTitleSet::~QtlDvdTitleSet()
{
    close();
}


//----------------------------------------------------------------------------
// Close a title set.
//----------------------------------------------------------------------------

void QtlDvdTitleSet::close()
{
    _deviceName.clear();
    _volumeId.clear();
    _isEncrypted = false;
    _vtsNumber = -1;
    _ifoFileName.clear();
    _vobFileNames.clear();
    _vobSize = 0;
    _vobStartSector = -1;
    _nextSector = 0;
    _palette.clear();
    _streams.clear();

    if (_dvdcss != 0) {
        dvdcss_close(_dvdcss);
        _dvdcss = 0;
    }
}


//----------------------------------------------------------------------------
// Get the color palette of the title set in RGB format.
//----------------------------------------------------------------------------

QtlByteBlock QtlDvdTitleSet::rgbPalette() const
{
    QtlByteBlock palette(_palette);
    convertPaletteYuvToRgb(palette, _log);
    return palette;
}


//----------------------------------------------------------------------------
// Load the description of a title set.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::open(const QString& fileName)
{
    close();
    const bool ok =
            buildFileNames(fileName) &&
            buildDeviceName() &&
            readVtsIfo() &&
            readDvdStructure();
    if (!ok) {
        close();
    }
    return ok;
}


//----------------------------------------------------------------------------
// Check if a file is a .IFO or .VOB.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::isDvdTitleSetFileName(const QString& fileName)
{
    const QFileInfo info (fileName);
    const QString suffix(info.suffix().toUpper());
    const QString name(info.completeBaseName());

    if (suffix == "IFO") {
        // The IFO must end in "_0".
        return name.endsWith("_0");
    }
    else if (suffix == "VOB") {
        // The VOB name must end in "_n".
        const int len = name.length();
        return len > 2 && name[len-2] == QChar('_') && name[len-1] >= QChar('0') && name[len-1] <= QChar('9');
    }
    else {
        return false;
    }
}


//----------------------------------------------------------------------------
// Build the IFO and VOB file names for the VTS.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::buildFileNames(const QString& fileName)
{
    // Only .IFO and .VOB are DVD structures.
    if (!isDvdTitleSetFileName(fileName)) {
        _log->line(QObject::tr("%1 is not a valid DVD title set file").arg(fileName));
        return false;
    }

    // Collect info on input file path.
    const QFileInfo info(fileName);
    const QString dir(QtlFile::absoluteNativeFilePath(info.path()));
    QString name(info.completeBaseName());

    // Get the base name of the VTS: "VTS_nn".
    // We already checked that the trailing "_n" exists, remove it.
    name.chop(2);

    // Get the title set number from "VTS_nn". Don't be too strict on format.
    const int uscore = name.lastIndexOf(QLatin1Char('_'));
    if (uscore >= 0) {
        _vtsNumber = qtlToInt(name.mid(uscore + 1), -1, 0, 99, 10);
    }

    // Locate all .VOB files.
    _vobFileNames.clear();
    for (int i = 1; i <= 9; ++i) {
        const QString vobFile(QStringLiteral("%1%2%3_%4.VOB").arg(dir).arg(QDir::separator()).arg(name).arg(i));
        if (!QFile(vobFile).exists()) {
            break;
        }
        _vobFileNames << vobFile;
        _vobSize += QFileInfo(vobFile).size();
    }

    // There must be at least one VOB file, otherwise there is no video.
    if (_vobFileNames.isEmpty()) {
        _log->line(QObject::tr("No VOB file for %1").arg(fileName), QColor("red"));
        return false;
    }

    // Build the IFO file name
    _ifoFileName = dir + QDir::separator() + name + "_0.IFO";
    if (!QFile(_ifoFileName).exists()) {
        // No IFO file: not an error but no language info available.
        _log->line(QObject::tr("DVD IFO file not found: %1").arg(_ifoFileName));
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
// Build device name for libdvdcss from IFO file name.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::buildDeviceName()
{
#if defined(Q_OS_WIN)

    // On Windows, if we are on a DVD readed, the IFO file name starts with a drive name.
    if (_ifoFileName.length() > 2 && _ifoFileName[1] == ':') {
        _deviceName = _ifoFileName.left(2);
        return true;
    }
    else {
        return false;
    }

#else
#error "unsupported OS"
#endif
}


//----------------------------------------------------------------------------
// Read the content of the VTS IFO.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::readVtsIfo()
{
    // Open IFO file. Will be closed by destructor.
    QFile ifo(_ifoFileName);
    if (!ifo.open(QFile::ReadOnly)) {
        _log->line(QObject::tr("Error opening %1").arg(_ifoFileName));
        return false;
    }

    // Read the IFO header. Must start with "DVDVIDEO-VTS".
    QtlByteBlock header;
    bool valid =
        QtlFile::readBinary(ifo, header, DVD_IFO_HEADER_SIZE) &&
        header.size() == DVD_IFO_HEADER_SIZE &&
        ::memcmp(header.data(), DVD_IFO_START_VALUE, ::strlen(DVD_IFO_START_VALUE)) == 0;
    if (!valid) {
        _log->line(QObject::tr("%1 is not a valid VTS IFO").arg(_ifoFileName));
        return false;
    }

    // There is always one video stream.
    QtlMediaStreamInfoPtr video(new QtlMediaStreamInfo());
    _streams.append(video);
    video->setStreamType(QtlMediaStreamInfo::Video);

    // Stream id: maybe 0xE0 (according to various web resources) or 0x01E0 (according to ffprobe).
    video->setStreamId(0x01E0);

    // Get video attributes.
    // See http://dvd.sourceforge.net/dvdinfo/ifo.html#vidatt
    const quint8 videoStandard = (header[DVD_IFO_VIDEO_ATTR_OFFSET] >> 4) & 0x03; // PAL vs. NTSC
    const quint8 videoDisplayAspectRatio = (header[DVD_IFO_VIDEO_ATTR_OFFSET] >> 2) & 0x03;
    const quint8 videoResolution = (header[DVD_IFO_VIDEO_ATTR_OFFSET + 1] >> 3) & 0x07;

    // Video frame size.
    switch (videoStandard) {
        case 0: { // NTSC
            switch (videoResolution) {
                case 0:
                    video->setWidth(720);
                    video->setHeight(480);
                    break;
                case 1:
                    video->setWidth(704);
                    video->setHeight(480);
                    break;
                case 2:
                    video->setWidth(352);
                    video->setHeight(480);
                    break;
                case 3:
                    video->setWidth(352);
                    video->setHeight(240);
                    break;
            }
            break;
        }
        case 1: { // PAL
            switch (videoResolution) {
                case 0:
                    video->setWidth(720);
                    video->setHeight(576);
                    break;
                case 1:
                    video->setWidth(704);
                    video->setHeight(576);
                    break;
                case 2:
                    video->setWidth(352);
                    video->setHeight(576);
                    break;
                case 3:
                    video->setWidth(352);
                    video->setHeight(288);
                    break;
            }
            break;
        }
        default: {
            _log->line(QObject::tr("Unknown video standard value %1 in DVD IFO file").arg(videoStandard));
            break;
        }
    }

    // Display aspect ratio.
    switch (videoDisplayAspectRatio) {
        case 0: // 4:3
            video->setDisplayAspectRatio(QTL_DAR_4_3);
            break;
        case 3: // 16:9
            video->setDisplayAspectRatio(QTL_DAR_16_9);
            break;
        default: // reserved, assume pixel aspect ratio = 1:1
            video->setDisplayAspectRatio(video->height() == 0 ? 0.0 : float(video->width()) / float(video->height()));
            break;
    }

    // Get audio attributes.
    // See http://dvd.sourceforge.net/dvdinfo/ifo.html#audatt
    const int audioCount = qMin<int>(header.fromBigEndian<quint16>(DVD_IFO_AUDIO_COUNT_OFFSET), DVD_IFO_AUDIO_MAX_COUNT);
    _log->debug(QObject::tr("IFO: %1 audio streams").arg(audioCount));

    for (int i = 0; i < audioCount; ++i) {

        // Point to the audio stream description in IFO header.
        const int start = DVD_IFO_AUDIO_ATTR_OFFSET + i * DVD_IFO_AUDIO_ATTR_SIZE;
        const quint8 codingMode = header[start] >> 5;
        const int channelCount = int(header[start + 1] & 0x07) + 1;
        const bool languagePresent = (header[start] & 0x0C) == 0x04;

        // Stream type:
        // 0=not specified, 1=normal, 2=for visually impaired, 3=director comments, 4=alternate director comments
        const quint8 type = header[start + 5];

        // Create the audio stream description.
        QtlMediaStreamInfoPtr audio(new QtlMediaStreamInfo());
        _streams.append(audio);
        audio->setStreamType(QtlMediaStreamInfo::Audio);

        // Compute stream id.
        // For audio, the stream ids depend on the codec.
        // Using n as index of audio stream, the stream id is;
        // AC-3 = 0x80 + n, DTS  = 0x88 + n, SDDS = 0x90 + n, LPCM = 0xA0 + n, MP2  = 0xC0 + n, MPEG2-ext = 0xC8 + n
        switch (codingMode) {
            case 0: // AC-3
                audio->setStreamId(0x80 + i);
                break;
            case 2: // MPEG-1 layer 2
                audio->setStreamId(0xC0 + i);
                break;
            case 3: // MPEG-2 ext
                audio->setStreamId(0xC8 + i);
                break;
            case 4: // LPCM
                audio->setStreamId(0xA0 + i);
                break;
            case 6: // DTS
                audio->setStreamId(0x88 + i);
                break;
        }

        // Other attributes.
        audio->setAudioChannels(channelCount);
        audio->setImpaired(type == 2);
        audio->setCommentary(type == 3 || type == 4);
        if (languagePresent) {
            audio->setLanguage(header.getLatin1(start + 2, 2));
        }
    }

    // Get subpicture (subtitle) attributes.
    // See http://dvd.sourceforge.net/dvdinfo/ifo.html#spatt
    const int subpicCount = qMin<int>(header.fromBigEndian<quint16>(DVD_IFO_SUBPIC_COUNT_OFFSET), DVD_IFO_SUBPIC_MAX_COUNT);
    _log->debug(QObject::tr("IFO: %1 subtitle streams").arg(subpicCount));

    for (int i = 0; i < subpicCount; ++i) {

        // Point to stream description in IFO header.
        const int start = DVD_IFO_SUBPIC_ATTR_OFFSET + i * DVD_IFO_SUBPIC_ATTR_SIZE;
        const bool languagePresent = (header[start] & 0x03) == 0x01;

        // Stream type:
        // 0=not specified, 1=normal, 2=large, 3=children, 5=normal captions, 6=large captions, 7=childrens captions,
        // 9=forced, 13=director comments, 14=large director comments, 15=director comments for children
        const quint8 type = header[start + 5];

        // Create the subtitle stream description.
        QtlMediaStreamInfoPtr subtitle(new QtlMediaStreamInfo());
        _streams.append(subtitle);
        subtitle->setStreamType(QtlMediaStreamInfo::Subtitle);
        subtitle->setSubtitleType(QtlMediaStreamInfo::SubDvd);

        // Compute stream id.
        // For DVD subpictures (=subtitle), the stream id is always 0x20 + index of
        // the subpicture stream in the IFO table of subpicture stream.
        subtitle->setStreamId(0x20 + i);

        // Other attributes.
        // Normally, type values 1,2,3 are normal,large,children, while 0 means unspecified.
        // But the experience shows that 1 is sometimes used for the hearing impaired while 0 is used for normal subtitle.
        subtitle->setImpaired(type >= 1 && type <= 7);
        subtitle->setCommentary(type == 13 || type == 14 || type == 15);
        subtitle->setForced(type == 9);
        if (languagePresent) {
            subtitle->setLanguage(header.getLatin1(start + 2, 2));
        }
    }

    // Read a chain of pointers and data:
    // - pointer to the VTS_PGCI (Title Program Chain table).
    // - offset to VTS_PGC relative to VTS_PGCI.
    // - palette in VTS_PGC
    quint32 pgci = 0;
    quint32 pgc = 0;
    const int paletteSize = DVD_IFO_PALETTE_ENTRY_SIZE * DVD_IFO_PALETTE_ENTRY_COUNT;
    valid =
        QtlFile::readBigEndianAt(ifo, DVD_IFO_VTS_PGCI_OFFSET, pgci) &&
        QtlFile::readBigEndianAt(ifo, (DVD_SECTOR_SIZE * pgci) + DVD_IFO_VTS_PGC_OFFSET_IN_PGCI, pgc) &&
        QtlFile::readBinaryAt(ifo, (DVD_SECTOR_SIZE * pgci) + pgc + DVD_IFO_PALETTE_OFFSET_IN_PGC, _palette, paletteSize) &&
        _palette.size() == paletteSize;

    if (!valid) {
        _log->line(QObject::tr("Error reading palette from %1").arg(_ifoFileName));
    }
    return valid;
}


//----------------------------------------------------------------------------
// Open DVD structure and check if the media is encrypted.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::readDvdStructure()
{
    // Initialize libdvdcss.
    const QByteArray name(_deviceName.toUtf8());
    _dvdcss = dvdcss_open(name.data());

    // Unsuccessful open is not an error. The file is simply probably not on a DVD media.
    if (_dvdcss == 0) {
        _log->debug(QObject::tr("cannot initialize libdvdcss on %1, probably not a DVD media").arg(_deviceName));
        return true;
    }

    // Read primary volume descriptor.
    // ECMA-119, section 6.7.1: The volume descriptor set starts at sector 16.
    // Each volume descriptor uses one sector. There must be one primary volume
    // descriptor and the list is terminated by one volume descriptor set terminator.
    // Here, we do not read more than 8 sectors (not standard, but highly probable).
    QtlByteBlock data(DVD_SECTOR_SIZE);
    int sectorNumber = DVD_VOLUME_DESCRIPTORS_SECTOR;
    int rootDirSector = -1;
    int rootDirSize = -1;
    for (int count = 8; count > 0 && readDvdSectors(data.data(), 1, sectorNumber++, false); --count) {
        // Volume descriptor type is in first byte.
        const int type = data[0];
        // Volume descriptor standard identified is in the next 5 bytes.
        const QString id = data.getLatin1(1, sizeof(DVD_VOLUME_DESCRIPTOR_ID) - 1);
        if (id != DVD_VOLUME_DESCRIPTOR_ID) {
            // Not a valid volume descriptor.
            _log->debug(QObject::tr("Invalid volume descriptor, id=\"%1\" instead of \"%2\"").arg(id).arg(DVD_VOLUME_DESCRIPTOR_ID));
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

    // If the DVD is not scrambled, no need to use libdvdcss.
    _isEncrypted = dvdcss_is_scrambled(_dvdcss);
    if (!_isEncrypted) {
        _log->debug(QObject::tr("DVD %1 is not encryted, not using libdvdcss").arg(_deviceName));
        dvdcss_close(_dvdcss);
        _dvdcss = 0;
        return true;
    }

    // We now need to find the file VTS_nn_1.VOB
    // If root directory not found, cannot read DVD content.
    if (rootDirSector <= 0 || rootDirSize <= 0) {
        _log->line(QObject::tr("Cannot find root directory of DVD %1").arg(_deviceName));
        return false;
    }

    // To be on a DVD, the files must be on a directory named "VIDEO_TS", itself being on top level directory.
    const QString firstVobName(QFileInfo(_vobFileNames.first()).fileName());
    const QString vobDirectory(QFileInfo(QtlFile::parentPath(_vobFileNames.first())).fileName());

    // Locate the first VOB on DVD.
    int vobDirSector = 0;
    int vobDirSize = 0;
    bool vobDirValid = false;
    int firstVobSize = 0;
    bool firstVobIsDir = false;
    const bool ok =
            locateDirectoryEntry(rootDirSector, rootDirSize, vobDirectory, vobDirSector, vobDirSize, vobDirValid) &&
            vobDirValid &&
            locateDirectoryEntry(vobDirSector, vobDirSize, firstVobName, _vobStartSector, firstVobSize, firstVobIsDir) &&
            !firstVobIsDir &&
            _vobStartSector > 0;

    if (ok) {
        _log->debug(QObject::tr("VOB first sector: %1").arg(_vobStartSector));
    }
    else {
        _log->line(QObject::tr("Error locating %1 on DVD media").arg(vobDirectory + QDir::separator() + firstVobName));
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
// Read a given number of sectors from the MPEG content of the VTS.
//----------------------------------------------------------------------------

int QtlDvdTitleSet::readSectors(void* buffer, int count, int position)
{
    // After last sector of VOB's:
    const int endSector = _vobSize / DVD_SECTOR_SIZE;

    // Reject non-open object, unencrypted DVD, wrong file position.
    if (_dvdcss == 0 || _nextSector < 0 || _nextSector > endSector || buffer == 0 || count < 0 || position > endSector) {
        return -1;
    }

    // Read at given position.
    if (position >= 0) {
        _nextSector = position;
    }

    // Max number of sectors to read.
    if (count > endSector - _nextSector) {
        count = endSector - _nextSector;
    }

    // Now read the sectors.
    if (count == 0 || readDvdSectors(buffer, count, _vobStartSector + _nextSector, true)) {
        _nextSector += count;
        return count;
    }
    else {
        return -1;
    }
}


//----------------------------------------------------------------------------
// Read exactly N sectors from the DVD.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::readDvdSectors(void* buffer, int sectorCount, int seekSector, bool vobContent)
{
    // Optionally seek first.
    if (seekSector >= 0) {
        const int count = dvdcss_seek(_dvdcss, seekSector, vobContent ? DVDCSS_SEEK_MPEG : DVDCSS_NOFLAGS);
        if (count < 0) {
            _log->line(QObject::tr("Error seeking DVD to sector %1, seek returned %2").arg(seekSector).arg(count));
            return false;
        }
    }

    // Loop on sectors read.
    char* buf = reinterpret_cast<char*>(buffer);
    while (sectorCount > 0) {
        const int count = dvdcss_read(_dvdcss, buf, sectorCount, vobContent ? DVDCSS_READ_DECRYPT : DVDCSS_NOFLAGS);
        if (count <= 0) {
            _log->line(QObject::tr("Error reading DVD media"));
            return false;
        }
        sectorCount -= count;
        buf += count * DVD_SECTOR_SIZE;
    }

    return true;
}


//----------------------------------------------------------------------------
// Locate an entry inside a directory.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::locateDirectoryEntry(int dirSector, int dirSize, const QString& entryName, int& entrySector, int& entrySize, bool& isDirectory)
{
    // Reset output data.
    entrySector = entrySize = 0;
    isDirectory = false;

    // Read directory content.
    const int dirSectorCount = dirSize / DVD_SECTOR_SIZE + int(dirSize % DVD_SECTOR_SIZE != 0);
    QtlByteBlock data(dirSectorCount * DVD_SECTOR_SIZE);
    if (!readDvdSectors(data.data(), dirSectorCount, dirSector, false)) {
        _log->line(QObject::tr("Error reading DVD directory information at sector %1").arg(dirSector));
        return false;
    }

    // Loop on all directory entries. Format: see ECMA-119, section 9.1.
    int length = 0;
    for (int index = 0; (length = data[index]) >= 34 && index + length <= dirSize; index += length) {

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

        // Check expected entry name.
        if (name.compare(entryName, Qt::CaseInsensitive) == 0) {
            // Entry found.
            isDirectory = (data[index + 25] & 0x02) != 0;
            entrySector = data.fromLittleEndian<quint32>(index + 2);
            entrySize = data.fromLittleEndian<quint32>(index + 10);
            return true;
        }
    }

    // Entry not found.
    return false;
}


//----------------------------------------------------------------------------
// Convert a YUV palette into RGB.
//----------------------------------------------------------------------------

void QtlDvdTitleSet::convertPaletteYuvToRgb(QtlByteBlock& palette, QtlLogger* log)
{
    if (palette.size() % 4 != 0 && log != 0) {
        log->line(QObject::tr("Palette conversion: palette size is %1 bytes, not a multiple of 4").arg(palette.size()));
    }

    // On input, each entry contains 4 bytes: (0, Y, Cr, Cb).
    // On output, each entry contains 4 bytes: (0, R, G, B).
    //
    // The real formula is the following;
    //    r = y + 1.402 * (cr - 128)
    //    g = y - 0.34414 * (cb - 128) - 0.71414 * (cr - 128)
    //    b = y + 1.722 * (cb - 128)
    //
    // The following algorithm is suggested in http://en.wikipedia.org/wiki/YUV.
    //    cr = cr - 128
    //    cb = cb - 128
    //    r = y + cr + (cr >> 2) + (cr >> 3) + (cr >> 5)
    //    g = y - ((cb >> 2) + (cb >> 4) + (cb >> 5)) - ((cr >> 1) + (cr >> 3) + (cr >> 4) + (cr >> 5))
    //    b = y + cb + (cb >> 1) + (cb >> 2) + (cb >> 6)
    // which gives the following values:
    //    r = y + 1.40625 * (cr - 128)
    //    g = y - 0.34375 * (cb - 128) - 0.71875 * (cr - 128)
    //    b = y + 1.765625 * (cb - 128)

    for (int base = 0; base + 4 <= palette.size(); base += 4) {
        if (palette[base] != 0 && log != 0) {
            log->line(QObject::tr("Palette conversion: unexpected value 0x%1, should be 0").arg(int(palette[base]), 2, 16, QChar('0')));
        }
        int y = int(palette[base+1]);
        int cr = int(palette[base+2]) - 128;
        int cb = int(palette[base+3]) - 128;
        int r = y + cr + (cr >> 2) + (cr >> 3) + (cr >> 5);
        int g = y - ((cb >> 2) + (cb >> 4) + (cb >> 5)) - ((cr >> 1) + (cr >> 3) + (cr >> 4) + (cr >> 5));
        int b = y + cb + (cb >> 1) + (cb >> 2) + (cb >> 6);
        palette[base+1] = quint8(qBound(0, r, 255));
        palette[base+2] = quint8(qBound(0, g, 255));
        palette[base+3] = quint8(qBound(0, b, 255));
    }
}


//----------------------------------------------------------------------------
// Compare two QtlMediaStreamInfoPtr for DVD stream ordering.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::lessThan(const QtlMediaStreamInfoPtr& p1, const QtlMediaStreamInfoPtr& p2)
{
    // A null pointer is always less than a non null pointer.
    if (p1.isNull() || p2.isNull()) {
        return !p2.isNull();
    }

    // When the streams have distinct types, use enum order (Video < Audio < Subtitle < Other).
    if (p1->streamType() != p2->streamType()) {
        return p1->streamType() < p2->streamType();
    }

    // Same type, get their stream id in the MPEG-TS.
    int id1 = p1->streamId();
    int id2 = p2->streamId();

    // At most 8 audio streams, the logical order in the low-order 3 bits.
    if (p1->streamType() == QtlMediaStreamInfo::Audio) {
        id1 &= 0x07;
        id2 &= 0x07;
    }

    // Return order of stream on DVD.
    return id1 < id2;
}
