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
// Define the class QtlMovieFFprobeTags.
//
//----------------------------------------------------------------------------

#include "QtlMovieFFprobeTags.h"
#include "QtlNumUtils.h"


//----------------------------------------------------------------------------
// Parse the text output of ffprobe and load this object.
//----------------------------------------------------------------------------

void QtlMovieFFprobeTags::loadFFprobeOutput(const QString& ffprobeOutput)
{
    // Clear the returned container.
    clear();

    // Split the output lines and analyze each line.
    foreach (const QString& line, ffprobeOutput.split(QRegExp("\\r*\\n+"), QString::SkipEmptyParts)) {

        // Locate the first '=' in the line.
        const int eq = line.indexOf(QChar('='));
        if (eq > 0) {
            // Key and value.
            const QString key(line.left(eq));
            QString value(line.mid(eq + 1));

            // If the value is surrounded by quotes, decode the string.
            if (value.startsWith(QChar('"')) && value.endsWith(QChar('"'))) {
                // Remove quotes.
                value.remove(0, 1);
                value.chop(1);
                // Unescape characters.
                value.replace(QStringLiteral("\\\\"), QStringLiteral("\\"));
                value.replace(QStringLiteral("\\\""), QStringLiteral("\""));
            }

            // Store the key/value. Keys are forced to lowercase.
            insert(key.trimmed().toLower(), value.trimmed());
        }
    }
}


//----------------------------------------------------------------------------
// Analyze the FFprobe tags and build a list of stream info.
//----------------------------------------------------------------------------

void QtlMovieFFprobeTags::buildStreamInfo(QtlMediaStreamInfoList& streams)
{
    // Loop on all streams.
    const int ffStreamCount = intValue("format.nb_streams");
    for (int index = 0; index < ffStreamCount; ++index) {

        // Search existing stream with same index.
        QtlMediaStreamInfoPtr info;
        foreach (const QtlMediaStreamInfoPtr& p, streams) {
            if (p->ffIndex() == index) {
                info = p;
                break;
            }
         }

        // If none found, use a new one.
        if (info.isNull()) {
            info = new QtlMediaStreamInfo();
            info->setFFIndex(index);
            streams.append(info);
        }

        // Codec description.
        const QString codecType(valueOfStream(index, "codec_type").toLower());
        const QString codecName(valueOfStream(index, "codec_name").toLower());
        const QString profile(valueOfStream(index, "profile"));
        QString codecLongName(valueOfStream(index, "codec_long_name"));
        if (!profile.isEmpty() && profile.compare("unknown", Qt::CaseInsensitive) != 0) {
            codecLongName.append(QStringLiteral(" (%1)").arg(profile));
        }
        info->setCodecName(codecLongName);

        // Stream type.
        if (codecType == "video") {
            info->setStreamType(QtlMediaStreamInfo::Video);
        }
        else if (codecType == "audio") {
            info->setStreamType(QtlMediaStreamInfo::Audio);
        }
        else if (codecType.startsWith("subtitle")) {
            info->setStreamType(QtlMediaStreamInfo::Subtitle);
        }

        // Video size attributes (usually zeroes on non-video streams).
        const int width = intValueOfStream(index, "width");
        const int height = intValueOfStream(index, "height");
        const float dar = floatValueOfStream(index, "display_aspect_ratio");
        info->setWidth(width);
        info->setHeight(height);
        info->setRotation(intValueOfStream(index, "tags.rotate"));

        // If display aspect ratio is not specified, assume pixel aspect ratio = 1
        // (square pixels), hence DAR = width / height.
        info->setDisplayAspectRatio(qtlFloatZero(dar) && height > 0 ? float(width) / float(height) : dar);

        // Various simple stream attributes.
        info->setTitle(valueOfStream(index, "tags.title"));
        info->setBitRate(intValueOfStream(index, "bit_rate"));
        info->setFrameRate(floatValueOfStream(index, "r_frame_rate"));
        info->setLanguage(valueOfStream(index, "tags.language"));
        info->setImpaired(intValueOfStream(index, "disposition.hearing_impaired") != 0 ||
                          intValueOfStream(index, "disposition.visual_impaired") != 0);
        info->setForced(intValueOfStream(index, "disposition.forced") != 0);
        info->setOriginalAudio(intValueOfStream(index, "disposition.original") != 0);
        info->setDubbedAudio(intValueOfStream(index, "disposition.dub") != 0);
        info->setAudioChannels(intValueOfStream(index, "channels"));
        info->setSamplingRate(intValueOfStream(index, "sample_rate"));
        info->setCommentary(intValueOfStream(index, "comment") != 0);
        info->setStreamId(intValueOfStream(index, "id", -1));

        // Subtitle type is based on codec name.
        // Closed Captions are embedded in a video stream and are not detected by ffprobe.
        if (info->streamType() == QtlMediaStreamInfo::Subtitle) {
            if (codecName == "subrip" || codecName == "srt") {
                info->setSubtitleType(QtlMediaStreamInfo::SubRip);
            }
            else if (codecName == "ssa") {
                info->setSubtitleType(QtlMediaStreamInfo::SubSsa);
            }
            else if (codecName == "ass") {
                info->setSubtitleType(QtlMediaStreamInfo::SubAss);
            }
            else if (codecName == "vobsub" || codecName == "dvdsub") {
                info->setSubtitleType(QtlMediaStreamInfo::SubDvd);
            }
            else if (codecName == "dvbsub") {
                info->setSubtitleType(QtlMediaStreamInfo::SubDvb);
            }
            else if (codecName == "teletext" || codecName == "dvb_teletext") {
                info->setSubtitleType(QtlMediaStreamInfo::SubTeletext);
            }
            else if (!codecName.isEmpty()) {
                info->setSubtitleType(QtlMediaStreamInfo::SubOther);
            }
        }
    }
}


//----------------------------------------------------------------------------
// Get the value of a global tag in FFprobe output.
//----------------------------------------------------------------------------

QString QtlMovieFFprobeTags::value(const QString& name, const QString& defaultValue) const
{
    const ConstIterator it(find(name.toLower()));
    return it == end() ? defaultValue : it.value();
}


//----------------------------------------------------------------------------
// Get the integer value of a global tag in FFprobe output.
//----------------------------------------------------------------------------

int QtlMovieFFprobeTags::intValue(const QString& name, int defaultValue) const
{
    const QString sValue(value(name));
    bool ok = false;
    const int iValue = sValue.toInt(&ok, 0);
    return ok ? iValue : defaultValue;
}
