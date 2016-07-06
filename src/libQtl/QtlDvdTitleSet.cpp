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
//----------------------------------------------------------------------------

#include "QtlDvdTitleSet.h"
#include "QtlDvdDataPull.h"
#include "QtlFileDataPull.h"
#include "QtlFile.h"
#include "QtlStringUtils.h"

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
#define DVD_IFO_PLAYBACK_OFFSET_IN_PGC  0x0004
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

QtlDvdTitleSet::QtlDvdTitleSet(const QString& fileName, QtlLogger* log, QObject* parent) :
    QObject(parent),
    _nullLog(),
    _log(log != 0 ? log : &_nullLog),
    _deviceName(),
    _volumeId(),
    _volumeSectors(0),
    _isEncrypted(false),
    _vtsNumber(-1),
    _duration(0),
    _ifoFileName(),
    _vobFileNames(),
    _vobSizeInBytes(0),
    _vobStartSector(-1),
    _palette(),
    _streams()
{
    if (!fileName.isEmpty()) {
        load(fileName);
    }
}

QtlDvdTitleSet::QtlDvdTitleSet(const QtlDvdTitleSet& other, QObject* parent) :
    QObject(parent),
    _nullLog(),
    _log(other._log != 0 && other._log != &other._nullLog ? other._log : &_nullLog),
    _deviceName(other._deviceName),
    _volumeId(other._volumeId),
    _volumeSectors(other._volumeSectors),
    _isEncrypted(other._isEncrypted),
    _vtsNumber(other._vtsNumber),
    _duration(other._duration),
    _ifoFileName(other._ifoFileName),
    _vobFileNames(other._vobFileNames),
    _vobSizeInBytes(other._vobSizeInBytes),
    _vobStartSector(other._vobStartSector),
    _palette(other._palette),
    _streams(other._streams)
{
}


//----------------------------------------------------------------------------
// Clear object content.
//----------------------------------------------------------------------------

void QtlDvdTitleSet::clear()
{
    _deviceName.clear();
    _volumeId.clear();
    _volumeSectors = 0;
    _isEncrypted = false;
    _vtsNumber = -1;
    _ifoFileName.clear();
    _vobFileNames.clear();
    _vobSizeInBytes = 0;
    _vobStartSector = -1;
    _palette.clear();
    _streams.clear();
}


//----------------------------------------------------------------------------
// Load the description of a title set.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::load(const QString& fileName, const QtlDvdMedia* dvd)
{
    // Reset previous content.
    clear();

    // Build all VTS file names and read the content of the IFO file
    if (!buildFileNames(fileName) || !readVtsIfo()) {
        clear();
        return false;
    }

    // Check if the files are on a DVD media.
    // Use the provided QtlDvdMedia or open a local one.
    // Opening takes some time, so providing an already open one is faster.
    QtlDvdMedia dvdLocal(QString(), _log);
    if (dvd == 0 || !dvd->isOpen()) {
        dvd = &dvdLocal;
        dvdLocal.openFromFile(fileName);
    }
    if (dvd->isOpen()) {

        // Save generic information on the DVD media.
        _deviceName = dvd->deviceName();
        _volumeId = dvd->volumeId();
        _volumeSectors = dvd->volumeSizeInSectors();
        _isEncrypted = dvd->isEncrypted();

        // Look for the starting sector of the VTS.
        const QtlDvdFile vob(dvd->searchFile(_vobFileNames.first()));
        _vobStartSector = vob.startSector();
        if (_vobStartSector < 0) {
            _log->line(tr("Cannot find start sector for %1 on %2").arg(_vobFileNames.first()).arg(_deviceName));
            clear();
            return false;
        }
    }

    return true;
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
        _log->debug(tr("%1 is not a valid DVD title set file").arg(fileName));
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
        _vobSizeInBytes += QFileInfo(vobFile).size();
    }

    // There must be at least one VOB file, otherwise there is no video.
    if (_vobFileNames.isEmpty()) {
        _log->line(tr("No VOB file for %1").arg(fileName), QColor("red"));
        return false;
    }

    // Build the IFO file name
    _ifoFileName = dir + QDir::separator() + name + "_0.IFO";
    if (!QFile(_ifoFileName).exists()) {
        // No IFO file: not an error but no language info available.
        _log->line(tr("DVD IFO file not found: %1").arg(_ifoFileName));
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
// Read the content of the VTS IFO.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::readVtsIfo()
{
    // Open IFO file. Will be closed by destructor.
    QFile ifo(_ifoFileName);
    if (!ifo.open(QFile::ReadOnly)) {
        _log->line(tr("Error opening %1").arg(_ifoFileName));
        return false;
    }

    // Sizeo of the IFO file.
    const qint64 ifoSize = ifo.size();

    // Read the IFO header. Must start with "DVDVIDEO-VTS".
    QtlByteBlock header;
    bool valid =
        QtlFile::readBinary(ifo, header, DVD_IFO_HEADER_SIZE) &&
        header.size() == DVD_IFO_HEADER_SIZE &&
        ::memcmp(header.data(), DVD_IFO_START_VALUE, ::strlen(DVD_IFO_START_VALUE)) == 0;
    if (!valid) {
        _log->line(tr("%1 is not a valid VTS IFO").arg(_ifoFileName));
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
            _log->line(tr("Unknown video standard value %1 in DVD IFO file").arg(videoStandard));
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
    _log->debug(tr("IFO: %1 audio streams").arg(audioCount));

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
    _log->debug(tr("IFO: %1 subtitle streams").arg(subpicCount));

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

    // Locate the VTS_PGCI (Title Program Chain table).
    // - pgci: the relative sector VTS_PGCI.
    // - pgcCount: number of entries in the PGCI.
    // - pgcLastByte: index of last PGC byte, relative to start of PGCI.
    quint32 pgci = 0;
    quint16 pgcCount = 0;
    quint32 pgcLastByte = 0;
    valid = QtlFile::readBigEndianAt(ifo, DVD_IFO_VTS_PGCI_OFFSET, pgci) &&
            pgci < (ifoSize / QtlDvdMedia::DVD_SECTOR_SIZE) &&
            QtlFile::readBigEndianAt(ifo, (QtlDvdMedia::DVD_SECTOR_SIZE * pgci) + 0, pgcCount) &&
            pgcCount > 0 &&
            QtlFile::readBigEndianAt(ifo, (QtlDvdMedia::DVD_SECTOR_SIZE * pgci) + 4, pgcLastByte) &&
            (QtlDvdMedia::DVD_SECTOR_SIZE * pgci) + pgcLastByte < ifoSize;

    // Make pgci a byte offset from the beginning of the file.
    pgci = pgci * QtlDvdMedia::DVD_SECTOR_SIZE;

    // Size of the color palette in bytes.
    const int paletteSize = DVD_IFO_PALETTE_ENTRY_SIZE * DVD_IFO_PALETTE_ENTRY_COUNT;

    // Clear info to collect.
    _palette.clear();
    _duration = 0;

    // Read interesting information in each PGC.
    for (quint16 pgcIndex = 0; valid && pgcIndex < pgcCount; ++pgcIndex) {
        // Read the starting offset of each PGC, relative to the start of PGCI.
        // Then, read the playback time in the PGC.
        quint32 pgcStart = 0;
        quint32 playbackTime = 0;
        valid = QtlFile::readBigEndianAt(ifo, pgci + 12 + (pgcIndex * 8), pgcStart) &&
                pgcStart < pgcLastByte &&
                QtlFile::readBigEndianAt(ifo, pgci + pgcStart + DVD_IFO_PLAYBACK_OFFSET_IN_PGC, playbackTime);
        if (!valid) {
            break;
        }

        // The playback time is encoded in BCD as: hh:mm:ss:ff (ff = frame count within second)
        // With 2 MSBits of ff indicating frame rate: 11 = 30 fps, 10 = illegal, 01 = 25 fps, 00 = illegal
        // We use a number of seconds and ignore the frame count within last second.
        const int hours   = (((playbackTime >> 28) & 0x0F) * 10) + ((playbackTime >> 24) & 0x0F);
        const int minutes = (((playbackTime >> 20) & 0x0F) * 10) + ((playbackTime >> 16) & 0x0F);
        const int seconds = (((playbackTime >> 12) & 0x0F) * 10) + ((playbackTime >>  8) & 0x0F);

        // Add the duration of this PCG in the title set duration.
        _duration += (hours * 3600) + (minutes * 60) + seconds;

        // We read the palette in the first PGC only. If there are several
        // PGC's, we hope they all have the same palette.
        if (_palette.isEmpty()) {
            valid = QtlFile::readBinaryAt(ifo, pgci + pgcStart + DVD_IFO_PALETTE_OFFSET_IN_PGC, _palette, paletteSize) &&
                    _palette.size() == paletteSize;
        }
    }
    if (!valid) {
        _log->line(tr("Error reading Program Chain Table (PGC) from %1").arg(_ifoFileName));
        return false;
    }

    return valid;
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
// Convert a YUV palette into RGB.
//----------------------------------------------------------------------------

void QtlDvdTitleSet::convertPaletteYuvToRgb(QtlByteBlock& palette, QtlLogger* log)
{
    if (palette.size() % 4 != 0 && log != 0) {
        log->line(tr("Palette conversion: palette size is %1 bytes, not a multiple of 4").arg(palette.size()));
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
            log->line(tr("Palette conversion: unexpected value 0x%1, should be 0").arg(int(palette[base]), 2, 16, QChar('0')));
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


//----------------------------------------------------------------------------
// Create a QtlDataPull to transfer of the video content of the title set.
//----------------------------------------------------------------------------

QtlDataPull* QtlDvdTitleSet::dataPull(QtlLogger* log, QObject* parent, bool useMaxReadSpeed) const
{
    // Create an object that will write the VTS in the background.
    // If the DVD is encrypted, use QtlDvdDataPull.
    // Otherwise, we are not sure the media a DVD, so use file transfer.
    QtlDataPull* writer;

    if (_isEncrypted) {
        // When ripping files, we skip bad sectors.
        writer = new QtlDvdDataPull(_deviceName,
                                    _vobStartSector,
                                    vobSectorCount(),
                                    QtlDvdMedia::SkipBadSectors,
                                    QtlDvdDataPull::DEFAULT_TRANSFER_SIZE,
                                    QtlDvdDataPull::DEFAULT_MIN_BUFFER_SIZE,
                                    log,
                                    parent,
                                    useMaxReadSpeed);
    }
    else {
        writer = new QtlFileDataPull(_vobFileNames,
                                     QtlFileDataPull::DEFAULT_TRANSFER_SIZE,
                                     QtlFileDataPull::DEFAULT_MIN_BUFFER_SIZE,
                                     log,
                                     parent);
    }

    // This object will delete itself upon transfer completion.
    writer->setAutoDelete(true);
    return writer;
}
