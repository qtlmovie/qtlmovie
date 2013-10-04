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
// Define the class QtlMovieStreamInfo.
//
//----------------------------------------------------------------------------

#include "QtlMovieStreamInfo.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieStreamInfo::QtlMovieStreamInfo() :
    _streamType(Other),
    _title(),
    _codecName(),
    _language(),
    _ffIndex(-1),
    _streamId(-1),
    _subtitleType(SubNone),
    _teletextPage(-1),
    _width(0),
    _height(0),
    _dar(1.0),
    _forced(false),
    _impaired(false),
    _bitRate(0),
    _frameRate(0.0),
    _originalAudio(false),
    _dubbedAudio(false),
    _audioChannels(0),
    _samplingRate(0),
    _commentary(false)
{
}


//----------------------------------------------------------------------------
// Get the type of a subtitle file, based on its extension.
//----------------------------------------------------------------------------

QtlMovieStreamInfo::SubtitleType QtlMovieStreamInfo::subtitleType(const QString& fileName, bool checkExistence)
{
    // No file specified => not a subtitle file.
    if (fileName.isEmpty()) {
        return SubNone;
    }

    const QFileInfo info(fileName);
    const QString ext(info.suffix().toLower());

    if (checkExistence && (!info.exists() || !info.isFile())) {
        // File does not exist and we requested it to exist.
        return SubNone;
    }
    else if (ext == "srt") {
        return SubRip;
    }
    else if (ext == "ssa") {
        return SubSsa;
    }
    else if (ext == "ass") {
        return SubAss;
    }
    else if (ext == "sub") {
        // Typically DVD subtitles but there must be an associated .idx file.
        if (checkExistence) {
            const QFileInfo idx(info.path() + QDir::separator() + info.completeBaseName() + ".idx");
            if (!idx.exists() || !idx.isFile()) {
                // Associated .idx file is missing.
                return SubNone;
            }
        }
        return SubDvd;
    }
    else {
        return SubOther;
    }
}


//----------------------------------------------------------------------------
// Get the usual file extension for subtitle.
//----------------------------------------------------------------------------

QString QtlMovieStreamInfo::fileExtension(SubtitleType type)
{
    switch (type) {
    case SubRip:
        return ".srt";
    case SubSsa:
        return ".ssa";
    case SubAss:
        return ".ass";
    case SubDvd:
        return ".sub"; // there is also an associated ".idx" file.
    case SubDvb:
    case SubTeletext:
    case SubOther:
    case SubNone:
    default:
        return "";
    }
}


//----------------------------------------------------------------------------
// Merge two lists of stream informations.
//----------------------------------------------------------------------------

void QtlMovieStreamInfo::merge(QtlMovieStreamInfoPtrVector& destination, const QtlMovieStreamInfoPtrVector& source)
{
    // Loop on all destination streams.
    foreach (const QtlMovieStreamInfoPtr& dest, destination) {

        // If the stream has no stream id, cannot find its counterpart in source.
        if (dest.isNull() || dest->_streamId < 0) {
            continue;
        }

        // Search its counterpart in source list.
        QtlMovieStreamInfoPtr src;
        foreach (const QtlMovieStreamInfoPtr& s, source) {
            if (!s.isNull() && s->_streamId == dest->_streamId) {
                src = s;
                break;
            }
        }

        // If no counterpart is found in source, there is nothing to merge.
        if (src.isNull()) {
            continue;
        }

        // Merge the missing info in *dest from *src.
        if (dest->_streamType == Other) {
            dest->_streamType = src->_streamType;
        }
        if (dest->_title.isEmpty()) {
            dest->_title = src->_title;
        }
        if (dest->_codecName.isEmpty()) {
            dest->_codecName = src->_codecName;
        }
        if (dest->_language.isEmpty()) {
            dest->_language = src->_language;
        }
        if (dest->_ffIndex < 0) {
            dest->_ffIndex = src->_ffIndex;
        }
        if (dest->_subtitleType == SubOther && src->_subtitleType != SubNone) {
            dest->_subtitleType = src->_subtitleType;
        }
        if (dest->_teletextPage < 0) {
            dest->_teletextPage = src->_teletextPage;
        }
        if (dest->_width <= 0) {
            dest->_width = src->_width;
        }
        if (dest->_height <= 0) {
            dest->_height = src->_height;
        }
        if (qAbs(dest->_dar - 1.0) < 0.001) {
            dest->_dar = src->_dar;
        }
        if (!dest->_forced) {
            dest->_forced = src->_forced;
        }
        if (!dest->_impaired) {
            dest->_impaired = src->_impaired;
        }
        if (dest->_bitRate <= 0) {
            dest->_bitRate = src->_bitRate;
        }
        if (dest->_frameRate < 0.001) {
            dest->_frameRate = src->_frameRate;
        }
        if (!dest->_originalAudio) {
            dest->_originalAudio = src->_originalAudio;
        }
        if (!dest->_dubbedAudio) {
            dest->_dubbedAudio = src->_dubbedAudio;
        }
        if (dest->_audioChannels <= 0) {
            dest->_audioChannels = src->_audioChannels;
        }
        if (dest->_samplingRate <= 0) {
            dest->_samplingRate = src->_samplingRate;
        }
        if (!dest->_commentary) {
            dest->_commentary = src->_commentary;
        }
    }
}


//----------------------------------------------------------------------------
// Append a separator to a string if it is non empty and a new item.
//----------------------------------------------------------------------------

void QtlMovieStreamInfo::add(QString& str, const QString& item, const QString& separator)
{
    if (!item.isEmpty()) {
        if (!str.isEmpty()) {
            str.append(separator);
        }
        str.append(item);
    }
}


//----------------------------------------------------------------------------
// Get a human readable full description of the stream.
//----------------------------------------------------------------------------

QString QtlMovieStreamInfo::description(bool compact) const
{
    // The description is built here.
    QString result;

    // Start with FFmpeg stream index.
    if (_ffIndex >= 0) {
        add(result, QStringLiteral("#%1").arg(_ffIndex));
    }

    // Specific video options.
    if (_streamType == Video) {
        // Video frame size.
        if (_width > 0 && _height > 0) {
            add(result, QStringLiteral("%1x%2").arg(_width).arg(_height));
        }
        // Display aspect ratio.
        if (_dar > 0.001) {
            // Translate common display aspect ratios.
            if (qAbs(_dar - (float(4) / float(3))) < 0.001) {
                add(result, "4:3");
            }
            else if (qAbs(_dar - (float(16) / float(9))) < 0.001) {
                add(result, "16:9");
            }
            else {
                add(result, QStringLiteral("DAR %1").arg(_dar, 0, 'f', 3));
            }
        }
        // Bit rate.
        if (!compact && _bitRate > 0) {
            add(result, QStringLiteral("%1 b/s").arg(_bitRate));
        }
        // Frame rate.
        if (!compact && _frameRate > 0.001) {
            add(result, QStringLiteral("%1 f/s").arg(_frameRate, 0, 'f', 1));
        }
    }
    else {
        // Not a video stream. Display language.
        add(result, _language);
        if (_forced) {
            add(result, "forced");
        }
    }

    // Specific audio options.
    if (_streamType == Audio) {
        // Bit rate.
        if (!compact && _bitRate > 0) {
            add(result, QStringLiteral("%1 b/s").arg(_bitRate));
        }
        // Number of channels.
        switch (_audioChannels) {
        case 0:
            break;
        case 1:
            add(result, "mono");
            break;
        case 2:
            add(result, "stereo");
            break;
        default:
            add(result, QStringLiteral("%1 channels").arg(_audioChannels));
            break;
        }
        // Sampling rate.
        if (!compact && _samplingRate > 0) {
            add(result, QStringLiteral("@%1 Hz").arg(_samplingRate));
        }
    }

    // Codec name for all types of streams.
    if (!compact) {
        add(result, _codecName);
    }

    // Other non-video options.
    if (_streamType != Video) {
        if (_commentary) {
            add(result, "commentary");
        }
        if (_impaired) {
            add(result, compact ? "impaired" : "visual/hearing impaired");
        }
        if (_teletextPage > 0) {
            add(result, QStringLiteral("Teletext %1%2").arg(compact ? "" : "page ").arg(_teletextPage));
        }
    }

    // End with free format title.
    if (!compact) {
        add(result, _title);
    }

    return result;
}


//----------------------------------------------------------------------------
// Accessors.
//----------------------------------------------------------------------------

void QtlMovieStreamInfo::setTitle(const QString& title)
{
    _title = title.trimmed();
}

void QtlMovieStreamInfo::setCodecName(const QString& codecName)
{
    _codecName = codecName.trimmed();
}

void QtlMovieStreamInfo::setLanguage(const QString& language)
{
    _language = language.trimmed().toLower();
    if (_language == "und") {
        // Code "und" means undefined...
        _language.clear();
    }
}

QString QtlMovieStreamInfo::ffSpecifier(int fileIndex) const
{
    return _ffIndex < 0 ? QString() : QStringLiteral("%1:%2").arg(fileIndex).arg(_ffIndex);
}

void QtlMovieStreamInfo::setFFIndex(int ffIndex)
{
    _ffIndex = ffIndex < 0 ? -1 : ffIndex;
}

void QtlMovieStreamInfo::setTeletextPage(int teletextPage)
{
    _teletextPage = teletextPage < 0 ? -1 : teletextPage;
}

void QtlMovieStreamInfo::setWidth(int width)
{
    _width = width < 0 ? 0 : width;
}

void QtlMovieStreamInfo::setHeight(int height)
{
    _height = height < 0 ? 0 : height;
}

void QtlMovieStreamInfo::setDisplayAspectRatio(float dar)
{
    _dar = dar < 0.001 ? 0.0 : dar;
}

void QtlMovieStreamInfo::setBitRate(int bitRate)
{
    _bitRate = bitRate < 0 ? 0 : bitRate;
}

void QtlMovieStreamInfo::setFrameRate(float frameRate)
{
    _frameRate = frameRate < 0.001 ? 0.0 : frameRate;
}

void QtlMovieStreamInfo::setAudioChannels(int audioChannels)
{
    _audioChannels = audioChannels < 0 ? 0 : audioChannels;
}

void QtlMovieStreamInfo::setSamplingRate(int samplingRate)
{
    _samplingRate = samplingRate < 9 ? 0 : samplingRate;
}
