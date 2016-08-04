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
// Define the class QtlMediaStreamInfo.
//
//----------------------------------------------------------------------------

#include "QtlMediaStreamInfo.h"
#include "QtlIsoLanguages.h"
#include "QtlNumUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMediaStreamInfo::QtlMediaStreamInfo() :
    _streamType(Other),
    _title(),
    _codecName(),
    _language(),
    _ffIndex(-1),
    _streamId(-1),
    _subtitleType(SubNone),
    _teletextPage(-1),
    _ccNumber(-1),
    _width(0),
    _height(0),
    _dar(1.0),
    _forcedDar(0.0),
    _rotation(0),
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
// Convert a StreamType into a string.
//----------------------------------------------------------------------------

QString QtlMediaStreamInfo::streamTypeName(QtlMediaStreamInfo::StreamType type)
{
    switch (type) {
        case Video:    return "Video";
        case Audio:    return "Audio";
        case Subtitle: return "Subtitle";
        case Other:    return "Other";
        default:       return QString::number(int(type));
    }
}


//----------------------------------------------------------------------------
// Get the type of a subtitle file, based on its extension.
//----------------------------------------------------------------------------

QtlMediaStreamInfo::SubtitleType QtlMediaStreamInfo::subtitleType(const QString& fileName, bool checkExistence)
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

QString QtlMediaStreamInfo::fileExtension(SubtitleType type)
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
        case SubCc:
        case SubOther:
        case SubNone:
        default:
            return "";
    }
}


//----------------------------------------------------------------------------
// Check if subtitles files are text files.
//----------------------------------------------------------------------------

bool QtlMediaStreamInfo::isTextFile(QtlMediaStreamInfo::SubtitleType type)
{
    // Remember that we indicate the _file_ format, not the subtitles format.
    // Thus, while Teletext, Closed Captions and some DVB subtitles are texts,
    // they are encoded as binary data. Only SRT/SSA/ASS are plain text files.

    switch (type) {
        case SubRip:
        case SubSsa:
        case SubAss:
            return true;
        case SubDvd:
        case SubDvb:
        case SubTeletext:
        case SubCc:
        case SubOther:
        case SubNone:
        default:
            return false;
    }
}


//----------------------------------------------------------------------------
// Merge two lists of stream informations.
//----------------------------------------------------------------------------

void QtlMediaStreamInfo::merge(QtlMediaStreamInfoPtrVector& destination, const QtlMediaStreamInfoPtrVector& source)
{
    // Loop on all destination streams.
    foreach (const QtlMediaStreamInfoPtr& dest, destination) {

        // If the stream has no stream id, cannot find its counterpart in source.
        if (dest.isNull() || dest->_streamId < 0) {
            continue;
        }

        // Search its counterpart in source list.
        QtlMediaStreamInfoPtr src;
        foreach (const QtlMediaStreamInfoPtr& s, source) {
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
        if (qtlFloatEqual(dest->_dar, float(1.0))) {
            dest->_dar = src->_dar;
        }
        if (dest->_rotation == 0) {
            dest->_rotation = src->_rotation;
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
        if (qtlFloatZero(dest->_frameRate)) {
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

void QtlMediaStreamInfo::add(QString& str, const QString& item, const QString& separator)
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

QString QtlMediaStreamInfo::description(bool compact) const
{
    // The description is built here.
    QString result;

    // Start with FFmpeg stream index.
    if (_ffIndex >= 0) {
        add(result, QObject::tr("#%1").arg(_ffIndex));
    }

    // Specific video options.
    if (_streamType == Video) {
        // Video frame size.
        if (_width > 0 && _height > 0) {
            add(result, QObject::tr("%1x%2").arg(_width).arg(_height));
        }
        // Display aspect ratio.
        add(result, displayAspectRatioString(true, false));
        // Video rotation.
        if (_rotation != 0) {
            if (compact) {
                add(result, QObject::tr("%1 degrees").arg(_rotation));
            }
            else {
                add(result, QObject::tr("rotate %1 degrees").arg(_rotation));
            }
        }
        // Bit rate.
        if (!compact && _bitRate > 0) {
            add(result, QObject::tr("%1 b/s").arg(_bitRate));
        }
        // Frame rate.
        if (!compact && !qtlFloatZero(_frameRate)) {
            add(result, QObject::tr("%1 f/s").arg(_frameRate, 0, 'f', 1));
        }
    }
    else {
        // Not a video stream. Display language.
        add(result, QtlIsoLanguages::instance()->languageName(_language));
        if (_forced) {
            add(result, QObject::tr("forced"));
        }
    }

    // Specific audio options.
    if (_streamType == Audio) {
        // Bit rate.
        if (!compact && _bitRate > 0) {
            add(result, QObject::tr("%1 b/s").arg(_bitRate));
        }
        // Number of channels.
        switch (_audioChannels) {
        case 0:
            break;
        case 1:
            add(result, QObject::tr("mono"));
            break;
        case 2:
            add(result, QObject::tr("stereo"));
            break;
        default:
            add(result, QObject::tr("%1 channels").arg(_audioChannels));
            break;
        }
        // Sampling rate.
        if (!compact && _samplingRate > 0) {
            add(result, QObject::tr("@%1 Hz").arg(_samplingRate));
        }
    }

    // Codec name for all types of streams.
    if (!compact) {
        add(result, _codecName);
    }

    // Other non-video options.
    if (_streamType != Video && _commentary) {
        add(result, QObject::tr("commentary"));
    }
    if (_streamType == Audio && _impaired) {
        add(result, QObject::tr("visual impaired"));
    }
    if (_streamType == Subtitle) {
        if (_teletextPage > 0) {
            add(result, QObject::tr("Teletext %1%2").arg(compact ? "" : QObject::tr("page ")).arg(_teletextPage));
        }
        if (_ccNumber > 0) {
            add(result, QObject::tr("Closed Captions"));
            if (_ccNumber > 4) {
                add(result, QObject::tr("service %1").arg(_ccNumber - 4));
            }
            else if (_ccNumber > 2) {
                add(result, QObject::tr("channel 2"));
            }
            if (_ccNumber == 2 || _ccNumber == 4) {
                add(result, QObject::tr("field 2"));
            }
        }
        if (_impaired) {
            add(result, QObject::tr("hearing impaired"));
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

void QtlMediaStreamInfo::setTitle(const QString& title)
{
    _title = title.trimmed();
}

void QtlMediaStreamInfo::setCodecName(const QString& codecName)
{
    _codecName = codecName.trimmed();
}

void QtlMediaStreamInfo::setLanguage(const QString& language)
{
    _language = language.trimmed().toLower();
    if (_language == "und") {
        // Code "und" means undefined...
        _language.clear();
    }
}

QString QtlMediaStreamInfo::ffSpecifier(int fileIndex) const
{
    return _ffIndex < 0 ? QString() : QStringLiteral("%1:%2").arg(fileIndex).arg(_ffIndex);
}

void QtlMediaStreamInfo::setFFIndex(int ffIndex)
{
    _ffIndex = ffIndex < 0 ? -1 : ffIndex;
}

void QtlMediaStreamInfo::setTeletextPage(int teletextPage)
{
    _teletextPage = teletextPage < 0 ? -1 : teletextPage;
}

void QtlMediaStreamInfo::setCcNumber(int ccNumber)
{
    _ccNumber = ccNumber >= 1 ? ccNumber : -1;
}

void QtlMediaStreamInfo::setWidth(int width)
{
    _width = width < 0 ? 0 : width;
}

void QtlMediaStreamInfo::setHeight(int height)
{
    _height = height < 0 ? 0 : height;
}

float QtlMediaStreamInfo::displayAspectRatio(bool original) const
{
    return original || qtlFloatZero(_forcedDar) ? _dar : _forcedDar;
}

QString QtlMediaStreamInfo::displayAspectRatioString(bool original, bool validFloat) const
{
    const float dar = displayAspectRatio(original);
    if (qtlFloatZero(dar)) {
        // Undefined DAR.
        return validFloat ? QStringLiteral("0") : QString();
    }
    // Translate common display aspect ratios.
    else if (qtlFloatEqual(dar, QTL_DAR_4_3)) {
        return QObject::tr("4:3");
    }
    else if (qtlFloatEqual(dar, QTL_DAR_16_9)) {
        return QObject::tr("16:9");
    }
    else if (validFloat) {
        return QObject::tr("%1").arg(dar, 0, 'f', 3);
    }
    else {
        return QObject::tr("DAR %1").arg(dar, 0, 'f', 3);
    }
}

void QtlMediaStreamInfo::setDisplayAspectRatio(float dar)
{
    _dar = qtlFloatZero(dar) ? 0.0 : dar;
}

void QtlMediaStreamInfo::setForcedDisplayAspectRatio(float dar)
{
    _forcedDar = qtlFloatZero(dar) ? 0.0 : dar;
}

void QtlMediaStreamInfo::setRotation(int rotation)
{
    _rotation = rotation % 360;
}

void QtlMediaStreamInfo::setBitRate(int bitRate)
{
    _bitRate = bitRate < 0 ? 0 : bitRate;
}

void QtlMediaStreamInfo::setFrameRate(float frameRate)
{
    _frameRate = qtlFloatZero(frameRate) ? 0.0 : frameRate;
}

void QtlMediaStreamInfo::setAudioChannels(int audioChannels)
{
    _audioChannels = audioChannels < 0 ? 0 : audioChannels;
}

void QtlMediaStreamInfo::setSamplingRate(int samplingRate)
{
    _samplingRate = samplingRate < 9 ? 0 : samplingRate;
}
