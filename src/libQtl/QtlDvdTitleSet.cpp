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
#define DVD_IFO_HEADER_SIZE             0x03D8
#define DVD_IFO_LAST_SECTOR             0x000C
#define DVD_IFO_START_SECTOR            0x00C4
#define DVD_IFO_VTS_PGCI_OFFSET         0x00CC
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
    _volumeSectors(0),
    _isEncrypted(false),
    _vtsNumber(-1),
    _ifoFileName(),
    _vobFileNames(),
    _vobSizeInBytes(0),
    _vobStartSector(-1),
    _streams(),
    _pgcs()
{
    if (!fileName.isEmpty()) {
        load(fileName);
    }
}

QtlDvdTitleSet::QtlDvdTitleSet(const QtlDvdTitleSet& other) :
    _nullLog(),
    _log(other._log != 0 && other._log != &other._nullLog ? other._log : &_nullLog),
    _deviceName(other._deviceName),
    _volumeId(other._volumeId),
    _volumeSectors(other._volumeSectors),
    _isEncrypted(other._isEncrypted),
    _vtsNumber(other._vtsNumber),
    _ifoFileName(other._ifoFileName),
    _vobFileNames(other._vobFileNames),
    _vobSizeInBytes(other._vobSizeInBytes),
    _vobStartSector(other._vobStartSector),
    _streams(other._streams),
    _pgcs(other._pgcs)
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
    _streams.clear();
    _pgcs.clear();
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
            _log->line(QObject::tr("Cannot find start sector for %1 on %2").arg(_vobFileNames.first()).arg(_deviceName));
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
        _log->debug(QObject::tr("%1 is not a valid DVD title set file").arg(fileName));
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
        _log->line(QObject::tr("No VOB file for %1").arg(fileName), QColor(Qt::red));
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
// Read the content of the VTS IFO.
//----------------------------------------------------------------------------

bool QtlDvdTitleSet::readVtsIfo()
{
    // Read IFO file.
    QtlByteBlock ifo(QtlFile::readBinaryFile(_ifoFileName));
    if (ifo.isEmpty()) {
        _log->line(QObject::tr("Error opening %1").arg(_ifoFileName));
        return false;
    }

    // IFO header must start with "DVDVIDEO-VTS".
    bool valid = ifo.size() >= DVD_IFO_HEADER_SIZE && ifo.getLatin1(0, 12) == "DVDVIDEO-VTS";
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
    const quint8 videoStandard = (ifo[DVD_IFO_VIDEO_ATTR_OFFSET] >> 4) & 0x03; // PAL vs. NTSC
    const quint8 videoDisplayAspectRatio = (ifo[DVD_IFO_VIDEO_ATTR_OFFSET] >> 2) & 0x03;
    const quint8 videoResolution = (ifo[DVD_IFO_VIDEO_ATTR_OFFSET + 1] >> 3) & 0x07;

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
    const int audioCount = qMin<int>(ifo.fromBigEndian<quint16>(DVD_IFO_AUDIO_COUNT_OFFSET), DVD_IFO_AUDIO_MAX_COUNT);
    _log->debug(QObject::tr("IFO: %1 audio streams").arg(audioCount));

    for (int i = 0; i < audioCount; ++i) {

        // Point to the audio stream description in IFO header.
        const int start = DVD_IFO_AUDIO_ATTR_OFFSET + i * DVD_IFO_AUDIO_ATTR_SIZE;
        const quint8 codingMode = ifo[start] >> 5;
        const int channelCount = int(ifo[start + 1] & 0x07) + 1;
        const bool languagePresent = (ifo[start] & 0x0C) == 0x04;

        // Stream type:
        // 0=not specified, 1=normal, 2=for visually impaired, 3=director comments, 4=alternate director comments
        const quint8 type = ifo[start + 5];

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
            audio->setLanguage(ifo.getLatin1(start + 2, 2));
        }
    }

    // Get subpicture (subtitle) attributes.
    // See http://dvd.sourceforge.net/dvdinfo/ifo.html#spatt
    const int subpicCount = qMin<int>(ifo.fromBigEndian<quint16>(DVD_IFO_SUBPIC_COUNT_OFFSET), DVD_IFO_SUBPIC_MAX_COUNT);
    _log->debug(QObject::tr("IFO: %1 subtitle streams").arg(subpicCount));

    for (int i = 0; i < subpicCount; ++i) {

        // Point to stream description in IFO header.
        const int start = DVD_IFO_SUBPIC_ATTR_OFFSET + i * DVD_IFO_SUBPIC_ATTR_SIZE;
        const bool languagePresent = (ifo[start] & 0x03) == 0x01;

        // Stream type:
        // 0=not specified, 1=normal, 2=large, 3=children, 5=normal captions, 6=large captions, 7=childrens captions,
        // 9=forced, 13=director comments, 14=large director comments, 15=director comments for children
        const quint8 type = ifo[start + 5];

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
            subtitle->setLanguage(ifo.getLatin1(start + 2, 2));
        }
    }

    // Locate the VTS_PGCI (Title Program Chain table).
    // - pgci: the relative sector VTS_PGCI, make it a byte offset from the beginning of the file.
    // - pgcCount: number of entries in the PGCI.
    // - pgcLastByte: index of last PGC byte, relative to start of PGCI.
    const int pgci = Qtl::DVD_SECTOR_SIZE * ifo.fromBigEndian<quint32>(DVD_IFO_VTS_PGCI_OFFSET);
    int pgcCount = 0;
    int pgcLastByte = 0;

    valid = ifo.size() >= pgci + 8;
    if (valid) {
        pgcCount = ifo.fromBigEndian<quint16>(pgci);
        pgcLastByte = ifo.fromBigEndian<quint32>(pgci + 4);
        valid = pgcCount > 0 && pgcLastByte > 0 && ifo.size() >= pgci + pgcLastByte && ifo.size() >= pgci + 8 + 8* pgcCount;
    }

    // Analyze all Program Chains.
    for (int pgcIndex = 0; valid && pgcIndex < pgcCount; ++pgcIndex) {
        // Check if the PGCI entry is set.
        const int descIndex = pgci + 8 + 8 * pgcIndex;
        if ((ifo[descIndex] & 0x80) == 0) {
            // Entry not set, skip it.
            continue;
        }
        // Read the title number. The first title is #1.
        const int titleNumber = ifo[descIndex] & 0x7F;
        if (titleNumber < 1 || titleNumber > pgcCount) {
            _log->line(QObject::tr("incorrect title number %1 in IFO.PGCI, should be in range 1 to %2").arg(titleNumber).arg(pgcCount));
            continue;
        }
        // Read the starting offset of each PGC, relative to the start of PGCI.
        const int pgcStart = pgci + ifo.fromBigEndian<quint32>(descIndex + 4);
        QtlDvdProgramChainPtr pgc(new QtlDvdProgramChain(ifo, pgcStart, titleNumber));
        valid = !pgc.isNull() && pgc->isValid();
        if (valid) {
            // Make sure the list is large enough.
            // Note that titleNumber is less than 128.
            while (_pgcs.size() < titleNumber) {
                _pgcs << QtlDvdProgramChainPtr();
            }
            // Now set the PGC at the right index.
            _pgcs.replace(titleNumber - 1, pgc);
        }
    }
    if (!valid) {
        _log->line(QObject::tr("Error reading Program Chain Table (PGC) from %1").arg(_ifoFileName));
    }
    return valid;
}


//----------------------------------------------------------------------------
// Get the description of all sequential titles in the title set.
//----------------------------------------------------------------------------

QtlDvdProgramChainPtrList QtlDvdTitleSet::allTitles(int titleNumber) const
{
    // We must not create a loop and add the same title twice, keep track of inserted titles.
    QSet<int> numbers;

    // Get the "main" title.
    QtlDvdProgramChainPtr pgc(title(titleNumber));

    // Build the list containing this PGC.
    QtlDvdProgramChainPtrList result;
    if (!pgc.isNull()) {
        const int next = pgc->nextTitleNumber();

        // Prepend main title and all previous titles.
        while (!pgc.isNull() && !numbers.contains(pgc->titleNumber())) {
            result.prepend(pgc);
            numbers.insert(pgc->titleNumber());
            pgc = title(pgc->previousTitleNumber());
        }

        // Append all next titles.
        pgc = title(next);
        while (!pgc.isNull() && !numbers.contains(pgc->titleNumber())) {
            result.append(pgc);
            numbers.insert(pgc->titleNumber());
            pgc = title(pgc->nextTitleNumber());
        }
    }
    return result;
}


//----------------------------------------------------------------------------
// Get the longest duration of all sets of sequential titles in the title set.
//----------------------------------------------------------------------------

int QtlDvdTitleSet::longestDurationInSeconds() const
{
    int longest = 0;
    for (int i = 1; i <= titleCount(); ++i) {
        longest = qMax(longest, allTitles(i).totalDurationInSeconds());
    }
    return longest;
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
// Get the list of sectors for a given PGC, with fallback.
//----------------------------------------------------------------------------

QtlRangeList QtlDvdTitleSet::titleSectors(int titleNumber, int fallbackTitleNumber, QtlLogger* log) const
{
    // A range of sectors describing all VOB files, using 0 as first sector of first VOB.
    const QtlRange allSectors(0, vobSectorCount() - 1);

    // Without specific PGC, we extract everything.
    if (titleNumber <= 0) {
        // Extract everything.
        return QtlRangeList(allSectors);
    }

    // We want to extract a specific PGC.
    QtlDvdProgramChainPtr pgc(title(titleNumber));
    if (!pgc.isNull()) {
        // The PGC is found, use its sector list.
        log->debug(QObject::tr("Extracting PGC #%1").arg(titleNumber));
        return pgc->sectors();
    }

    // If the fallback is unspecified, extract everything.
    if (fallbackTitleNumber < 1) {
        log->debug(QObject::tr("PGC #%1 not found, extracting all VOB content instead").arg(titleNumber));
        return QtlRangeList(allSectors);
    }

    // Get a specific fallback PGC (maybe does not exist either).
    pgc = title(fallbackTitleNumber);
    if (!pgc.isNull()) {
        // The fallback PGC is found, use its sector list.
        log->debug(QObject::tr("PGC #%1 not found, extracting PGC #%2 instead").arg(titleNumber).arg(fallbackTitleNumber));
        return pgc->sectors();
    }

    log->debug(QObject::tr("PGC #%1 and #%2 (fallback) not found, extracting nothing").arg(titleNumber).arg(fallbackTitleNumber));
    return QtlRangeList();
}


//----------------------------------------------------------------------------
// Create a QtlDataPull to transfer of the video content of the title set.
//----------------------------------------------------------------------------

QtlDataPull* QtlDvdTitleSet::dataPull(QtlLogger* log, QObject* parent, bool useMaxReadSpeed, const QtlRangeList& sectorList) const
{
    QtlDataPull* writer;
    QtlRangeList sectors(sectorList);

    if (_isEncrypted) {
        // If the DVD is encrypted, use QtlDvdDataPull.
        // Sector numbering start a beginning of DVD, add the offset of the first VOB.
        // Remember that all VOB's of a given VTS are contiguous on disk.
        sectors.add(_vobStartSector);
        log->debug(QObject::tr("Extracting DVD sectors %1").arg(sectors.toString()));

        // When ripping files, we skip bad sectors.
        writer = new QtlDvdDataPull(_deviceName,
                                    sectors,
                                    Qtl::SkipBadSectors,
                                    QtlDvdDataPull::DEFAULT_TRANSFER_SIZE,
                                    QtlDvdDataPull::DEFAULT_MIN_BUFFER_SIZE,
                                    log,
                                    parent,
                                    useMaxReadSpeed);
    }
    else {
        // Non-encrypted DVD or not a DVD, use file transfer.
        // Transform sector ranges into bytes ranges.
        sectors.scale(Qtl::DVD_SECTOR_SIZE);

        // Build a list of files to read.
        QtlFileSlicesPtrList files;
        foreach (const QString& name, _vobFileNames) {

            // Get current file size in bytes.
            const qint64 size = QFileInfo(name).size();
            if (size <= 0) {
                log->line(QObject::tr("Error getting size of %1, skipping all subsequent VOB files").arg(name));
                break;
            }

            // Build the file slices within this file.
            QtlRangeList fileSectors(sectors);
            fileSectors.clip(QtlRange(0, size - 1));

            // Add the data to extract from this file.
            files << QtlFileSlicesPtr(new QtlFileSlices(name, fileSectors));
            log->debug(QObject::tr("Extracting %1 %2").arg(name).arg(fileSectors.toString()));

            // Now shift the complete sector list downward to the beginning of next file.
            sectors.add(-size);
        }

        // Build the data pull from the list of files.
        writer = new QtlFileDataPull(files,
                                     QtlFileDataPull::DEFAULT_TRANSFER_SIZE,
                                     QtlFileDataPull::DEFAULT_MIN_BUFFER_SIZE,
                                     log,
                                     parent);
    }

    return writer;
}
