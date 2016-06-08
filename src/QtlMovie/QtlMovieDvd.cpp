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
//
// Define some DVD utilities.
//
//----------------------------------------------------------------------------

#include "QtlMovieDvd.h"
#include "QtlFile.h"

//
// Layout of IFO file.
// See http://dvd.sourceforge.net/dvdinfo/ifo.html
//
#define DVD_SECTOR_SIZE                 0x0800
#define DVD_IFO_START_VALUE     "DVDVIDEO-VTS"
#define DVD_IFO_HEADER_SIZE             0x03D8
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
// Build the list of DVD title set files for a given selected file.
//----------------------------------------------------------------------------

bool QtlMovieDvd::buildFileNames(const QString& fileName, QString& ifoFileName, QStringList& vobFileNames, QtlLogger* log)
{
    // Clear returned info.
    ifoFileName.clear();
    vobFileNames.clear();

    // Collect info on input file path.
    const QFileInfo info(fileName);
    const QString dir(QtlFile::absoluteNativeFilePath(info.path()));
    const QString name(info.completeBaseName());
    const QString suffix(info.suffix().toUpper());
    const bool isIfo = suffix == "IFO";

    // Only .IFO and .VOB are DVD structures.
    if (!isIfo && suffix != "VOB") {
        // Not a DVD structure at all, not an error.
        return true;
    }

    // A "standard" DVD structure contains one or more "video title set" (VTS).
    // Each VTS is a movie, documentary, bonus, etc. The VTS are numbered on
    // two digits "nn" (01, 02, etc.) Each VTS contains the following files:
    //   VTS_nn_0.IFO  <- information file.
    //   VTS_nn_0.VOB  <- optional title set menu, not part of the movie.
    //   VTS_nn_1.VOB  <- first MPEG-PS file in the movie
    //   VTS_nn_2.VOB
    //   ... etc ...
    // The complete MPEG-PS content of the movie is simply the concatenation
    // of all .VOB file. There are at most 9 VOB files per VTS (1GB per VOB).

    // Get the base name of the VTS: "VTS_nn".
    QString vtsName(name);
    if (isIfo) {
        // The IFO must end in "_0".
        if (!vtsName.endsWith("_0")) {
            log->line(QObject::tr("Invalid DVD structure, the .IFO name %1 does not end in _0").arg(name)), QColor("red");
            return false;
        }
        vtsName.chop(2); // Remove trailing "_0"
    }
    else {
        // The VOB name must end in "_n".
        vtsName.remove(QRegExp("_\\d$"));
        if (vtsName == name) {
            // Could not find a valid ending.
            log->line(QObject::tr("Invalid DVD structure, the .VOB name %1 does not end in _n").arg(name), QColor("red"));
            return false;
        }
    }

    // Locate all .VOB files.
    for (int i = 1; i <= 9; ++i) {
        const QString vobFile(QStringLiteral("%1%2%3_%4.VOB").arg(dir).arg(QDir::separator()).arg(vtsName).arg(i));
        if (QFile(vobFile).exists()) {
            vobFileNames << vobFile;
        }
    }

    // There must be at least one VOB file, otherwise there is no video.
    if (vobFileNames.isEmpty()) {
        log->line(QObject::tr("No VOB file for %1").arg(fileName), QColor("red"));
        return false;
    }

    // Return the IFO file name
    ifoFileName = dir + QDir::separator() + vtsName + "_0.IFO";
    if (!QFile(ifoFileName).exists()) {
        // No IFO file: not an error but no language info available.
        log->line(QObject::tr("DVD IFO file not found, audio and subtitle languages are unknown"));
        ifoFileName.clear();
    }

    return true;
}


//----------------------------------------------------------------------------
// Read an IFO file and load the descriptioos of its streams.
//----------------------------------------------------------------------------

bool QtlMovieDvd::readIfoFile(const QString& fileName, QtlMediaStreamInfoPtrVector& streams, QtlByteBlock& palette, QtlLogger* log)
{
    // Reset returned content.
    streams.clear();
    palette.clear();

    // Open IFO file. Will be closed by destructor.
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        log->line(QObject::tr("Error opening %1").arg(fileName));
        return false;
    }

    // Read the IFO header. Must start with "DVDVIDEO-VTS".
    QtlByteBlock header;
    bool valid =
        QtlFile::readBinary(file, header, DVD_IFO_HEADER_SIZE) &&
        header.size() == DVD_IFO_HEADER_SIZE &&
        ::memcmp(header.data(), DVD_IFO_START_VALUE, ::strlen(DVD_IFO_START_VALUE)) == 0;
    if (!valid) {
        log->line(QObject::tr("%1 is not a valid VTS IFO").arg(fileName));
        return false;
    }

    // There is always one video stream.
    QtlMediaStreamInfoPtr video(new QtlMediaStreamInfo());
    streams.append(video);
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
    case 0: // NTSC
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
    case 1: // PAL
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
    default:
        log->line(QObject::tr("Unknown video standard value %1").arg(videoStandard));
        break;
    }

    // Display aspect ratio.
    switch (videoDisplayAspectRatio) {
    case 0: // 4:3
        video->setDisplayAspectRatio(4.0 / 3.0);
        break;
    case 3: // 16:9
        video->setDisplayAspectRatio(16.0 / 9.0);
        break;
    default: // reserved, assume pixel aspect ratio = 1:1
        video->setDisplayAspectRatio(video->height() == 0 ? 0.0 : float(video->width()) / float(video->height()));
        break;
    }

    // Get audio attributes.
    // See http://dvd.sourceforge.net/dvdinfo/ifo.html#audatt
    const int audioCount = qMin<int>(header.fromBigEndian<quint16>(DVD_IFO_AUDIO_COUNT_OFFSET), DVD_IFO_AUDIO_MAX_COUNT);
    log->debug(QObject::tr("IFO: %1 audio streams").arg(audioCount));

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
        streams.append(audio);
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
    log->debug(QObject::tr("IFO: %1 subtitle streams").arg(subpicCount));

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
        streams.append(subtitle);
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
        QtlFile::readBigEndianAt(file,
                                 DVD_IFO_VTS_PGCI_OFFSET,  // Offset IFO header
                                 pgci) &&
        QtlFile::readBigEndianAt(file,
                                 (DVD_SECTOR_SIZE * pgci) + DVD_IFO_VTS_PGC_OFFSET_IN_PGCI,
                                 pgc) &&
        QtlFile::readBinaryAt(file,
                              (DVD_SECTOR_SIZE * pgci) + pgc + DVD_IFO_PALETTE_OFFSET_IN_PGC,
                              palette,
                              paletteSize) &&
        palette.size() == paletteSize;

    if (!valid) {
        palette.clear();
        log->line(QObject::tr("Error reading palette from %1").arg(fileName));
    }
    return valid;
}


//----------------------------------------------------------------------------
// Compare two QtlMediaStreamInfoPtr for DVD stream ordering.
// We reorder stream in "DVD order" for user convenience.
//----------------------------------------------------------------------------

bool QtlMovieDvd::lessThan(const QtlMediaStreamInfoPtr& p1, const QtlMediaStreamInfoPtr& p2)
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
// Convert a YUV palette into RGB.
//----------------------------------------------------------------------------

void QtlMovieDvd::convertPaletteYuvToRgb(QtlByteBlock& palette, QtlLogger* log)
{
    if (palette.size() % 4 != 0) {
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
        if (palette[base] != 0) {
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
