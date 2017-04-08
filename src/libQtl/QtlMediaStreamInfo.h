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
//!
//! @file QtlMediaStreamInfo.h
//!
//! Declare the class QtlMediaStreamInfo.
//!
//----------------------------------------------------------------------------

#ifndef QTLMEDIASTREAMINFO_H
#define QTLMEDIASTREAMINFO_H

#include <QtCore>
#include "QtlSmartPointer.h"

#define QTL_DAR_4_3   (float(4.0)/float(3.0))  //!< Display aspect ratio value for 4:3.
#define QTL_DAR_16_9 (float(16.0)/float(9.0))  //!< Display aspect ratio value for 16:9.
#define QTL_DVD_DAR              QTL_DAR_16_9  //!< Display aspect ratio for DVD.
#define QTL_DVD_DAR_FFMPEG             "16:9"  //!< Same as QTL_DVD_DAR, used with ffmpeg -aspect option.

//
// Forward definition of the class in this file.
//
class QtlMediaStreamInfo;

//!
//! Smart pointer to a QtlMediaStreamInfo (non thread-safe).
//!
typedef QtlSmartPointer<QtlMediaStreamInfo,QtlNullMutexLocker> QtlMediaStreamInfoPtr;

//!
//! List of smart pointers to QtlMediaStreamInfo (non thread-safe).
//!
typedef QList<QtlMediaStreamInfoPtr> QtlMediaStreamInfoList;

//!
//! Description of a stream (video, audio, subtitle) in a multimedia file.
//!
class QtlMediaStreamInfo
{
public:
    //!
    //! Default constructor.
    //!
    QtlMediaStreamInfo();

    //!
    //! Type of stream in a multimedia file.
    //!
    enum StreamType {
        Video,    //!< Video stream.
        Audio,    //!< Audio stream.
        Subtitle, //!< Subtitle stream.
        Other     //!< Other type of stream (not to be transcoded)
    };

    //!
    //! Convert a StreamType into a string.
    //! @param [in] type Stream type.
    //! @return Human readable string.
    //!
    static QString streamTypeName(StreamType type);

    //!
    //! Get the stream type.
    //! @return The stream type.
    //!
    StreamType streamType() const
    {
        return _streamType;
    }

    //!
    //! Get the stream type as a string.
    //! @return The stream type as a string.
    //!
    QString streamTypeName() const
    {
        return streamTypeName(_streamType);
    }

    //!
    //! Set the stream type.
    //! @param [in] streamType The stream type.
    //!
    void setStreamType(const StreamType& streamType)
    {
        _streamType = streamType;
    }

    //!
    //! Get a human readable description of the stream.
    //! @param [in] compact If true, return a more compact string, for short-width display.
    //! @return A human readable description of the stream.
    //!
    QString description(bool compact) const;

    //!
    //! Get the free format description of the stream.
    //! @return The free format description of the stream.
    //!
    QString title() const
    {
        return _title;
    }

    //!
    //! Set the free format description of the stream.
    //! @param [in] title The free format description of the stream.
    //!
    void setTitle(const QString& title);

    //!
    //! Get the codec name of the stream.
    //! @return The codec name of the stream.
    //!
    QString codecName() const
    {
        return _codecName;
    }

    //!
    //! Set the codec name of the stream.
    //! @param [in] codecName The codec name of the stream.
    //!
    void setCodecName(const QString& codecName);

    //!
    //! Get the stream language (audio, subtitle).
    //! @return The stream language (audio, subtitle).
    //! No specific encoding, depends on input file metadata.
    //! The returned string is always lowercase and trimmed.
    //!
    QString language() const
    {
        return _language;
    }

    //!
    //! Set the stream language (audio, subtitle).
    //! @param [in] language The stream language.
    //!
    void setLanguage(const QString& language);

    //!
    //! Get the stream index as allocated by FFmpeg/FFprobe.
    //! @return The stream index as allocated by FFmpeg/FFprobe or -1 if unknown.
    //!
    int ffIndex() const
    {
        return _ffIndex;
    }

    //!
    //! Get the stream specifier for FFmpeg/FFprobe command line.
    //! @param [in] fileIndex Optional file index.
    //! @return The stream specifier for FFmpeg/FFprobe command line.
    //!
    QString ffSpecifier(int fileIndex = 0) const;

    //!
    //! Set the stream index as allocated by FFmpeg/FFprobe.
    //! @param [in] ffIndex The stream index as allocated by FFmpeg/FFprobe.
    //!
    void setFFIndex(int ffIndex);

    //!
    //! Get the physical stream id in the input file.
    //! Its semantic depends on the input file format.
    //! For MPEG-TS files, this is the PID.
    //! For MPEG-PS files, this is the stream id.
    //! @return The stream id or -1 if unknown.
    //!
    int streamId() const
    {
        return _streamId;
    }

    //!
    //! Set the physical stream id in the input file.
    //! Do no filtering on the value since the semantic is unknown.
    //! @param [in] id The stream id.
    //!
    void setStreamId(int id)
    {
        _streamId = id < 0 ? -1 : id;
    }

    //!
    //! Get the Teletext page number (Teletext subtitles only).
    //! @return The Teletext page number or -1 if unknown.
    //!
    int teletextPage() const
    {
        return _teletextPage;
    }

    //!
    //! Set the Teletext page number (Teletext subtitles only).
    //! @param [in] teletextPage The Teletext page number.
    //!
    void setTeletextPage(int teletextPage);

    //!
    //! Get the Close Captions number (CC subtitles only).
    //! The CC are numbered as follow:
    //! - 1: EIA-608, channel 1, field 1
    //! - 2: EIA-608, channel 1, field 2
    //! - 3: EIA-608, channel 2, field 1
    //! - 4: EIA-608, channel 2, field 2
    //! - 5: CEA-708, service 1
    //! - 6: CEA-708, service 2
    //! - etc.
    //! This may not be ideal. Feedback from US users required.
    //! @return The Close Captions number or -1 if unknown.
    //!
    int ccNumber() const
    {
        return _ccNumber;
    }

    //!
    //! Set the Close Captions number (CC subtitles only).
    //! @param [in] ccNumber The Close Captions number.
    //! @see ccNumber()
    //!
    void setCcNumber(int ccNumber);

    //!
    //! Get the video frame width in pixels (video and DVD subtitles).
    //! @return The video frame width in pixels or zero if unknown.
    //!
    int width() const
    {
        return _width;
    }

    //!
    //! Set the video frame width in pixels (video and DVD subtitles).
    //! @param [in] width The video frame width.
    //!
    void setWidth(int width);

    //!
    //! Get the video frame height in pixels (video and DVD subtitles).
    //! @return The video frame height in pixels or zero if unknown.
    //!
    int height() const
    {
        return _height;
    }

    //!
    //! Set the video frame height in pixels (video and DVD subtitles).
    //! @param [in] height The video frame height in pixels.
    //!
    void setHeight(int height);

    //!
    //! Get the video frame display aspect ratio.
    //! @param [in] original If true, ignore the forced DAR (if specified) and
    //! always return the original DAR from the input file.
    //! @return The video frame display aspect ratio or 0.0 if unknown.
    //!
    float displayAspectRatio(bool original = false) const;

    //!
    //! Get the video frame display aspect ratio as a string.
    //! @param [in] original If true, ignore the forced DAR (if specified) and
    //! always return the original DAR from the input file.
    //! @param [in] validFloat If true, always return a string which can be interpreted
    //! as a float using qtlToFloat().
    //! @return A string representing the video frame display aspect ratio, empty if unknown.
    //!
    QString displayAspectRatioString(bool original = false, bool validFloat = false) const;

    //!
    //! Set the original video frame display aspect ratio.
    //! @param [in] dar The video frame display aspect ratio.
    //!
    void setDisplayAspectRatio(float dar);

    //!
    //! Force the video frame display aspect ratio to a value not identical to the original one.
    //! @param [in] dar The forced video frame display aspect ratio. If zero, there is no more
    //! forced DAR and the original DAR from the input file is used.
    //!
    void setForcedDisplayAspectRatio(float dar);

    //!
    //! Get the video frame rotation in degrees.
    //! @return The video frame rotation in degrees.
    //!
    int rotation() const
    {
        return _rotation;
    }

    //!
    //! Set the video frame rotation in degrees.
    //! @param [in] rotation The video frame rotation in degrees.
    //!
    void setRotation(int rotation);

    //!
    //! Check if the stream is "forced" (mostly subtitles).
    //! Forced subtitles for a given language shall be displayed when the selected
    //! audio stream has the same language. They typically correspond to fragment
    //! of dialogs which are in a different language.
    //! @return True if the stream is "forced".
    //!
    bool forced() const
    {
        return _forced;
    }

    //!
    //! Set if the stream is "forced" (mostly subtitles).
    //! @param [in] forced True if the stream is "forced".
    //!
    void setForced(bool forced)
    {
        _forced = forced;
    }

    //!
    //! Check if the stream is targeted to hearing or visual impaired audience (audio or subtitle).
    //! @return True if the stream is targeted to hearing or visual impaired audience.
    //!
    bool impaired() const
    {
        return _impaired;
    }

    //!
    //! Set if the stream is targeted to hearing or visual impaired audience (audio or subtitle).
    //! @param [in] impaired True if the stream is targeted to hearing or visual impaired audience.
    //!
    void setImpaired(bool impaired)
    {
        _impaired = impaired;
    }

    //!
    //! Get the bitrate of the stream.
    //! @return The bitrate of the stream in bits/second or zero if unknown.
    //!
    int bitRate() const
    {
        return _bitRate;
    }

    //!
    //! Set the bitrate of the stream.
    //! @param [in] bitRate The bitrate of the stream in bits/second.
    //!
    void setBitRate(int bitRate);

    //!
    //! Get the frame rate of the stream (video).
    //! @return The frame rate of the stream in frames/second or zero if unknown.
    //!
    float frameRate() const
    {
        return _frameRate;
    }

    //!
    //! Set the frame rate of the stream (video).
    //! @param [in] frameRate The frame rate of the stream in frames/second or zero if unknown.
    //!
    void setFrameRate(float frameRate);

    //!
    //! Check if the stream is an original soundtrack.
    //! @return True if the stream is an original soundtrack.
    //!
    bool isOriginalAudio() const
    {
        return _originalAudio;
    }

    //!
    //! Set if the stream is an original soundtrack.
    //! @param [in] originalAudio True if the stream is an original soundtrack.
    //!
    void setOriginalAudio(bool originalAudio)
    {
        _originalAudio = originalAudio;
    }

    //!
    //! Check if the stream is a dubbed soundtrack.
    //! @return True if the stream is a dubbed soundtrack.
    //!
    bool isDubbedAudio() const
    {
        return _dubbedAudio;
    }

    //!
    //! Set if the stream is a dubbed soundtrack.
    //! @param [in] dubbedAudio True if the stream is a dubbed soundtrack.
    //!
    void setDubbedAudio(bool dubbedAudio)
    {
        _dubbedAudio = dubbedAudio;
    }

    //!
    //! Get the number of audio channels.
    //! @return The number of audio channels or zero if unknown.
    //!
    int audioChannels() const
    {
        return _audioChannels;
    }

    //!
    //! Set the number of audio channels.
    //! @param [in] audioChannels The number of audio channels.
    //!
    void setAudioChannels(int audioChannels);

    //!
    //! Get the sampling rate.
    //! @return The sampling rate in samples/seconds or zero if unknown.
    //!
    int samplingRate() const
    {
        return _samplingRate;
    }

    //!
    //! Set the sampling rate.
    //! @param [in] samplingRate The sampling rate in samples/seconds.
    //!
    void setSamplingRate(int samplingRate);

    //!
    //! Check if the stream is a commentary soundtrack.
    //! @return True if the stream is a commentary soundtrack.
    //!
    bool isCommentary() const
    {
        return _commentary;
    }

    //!
    //! Set if the stream is a commentary soundtrack.
    //! @param [in] commentary True if the stream is a commentary soundtrack.
    //!
    void setCommentary(bool commentary)
    {
        _commentary = commentary;
    }

    //!
    //! Type of encoding for subtitles.
    //!
    enum SubtitleType {
        SubRip,      //!< SubRip format, .srt file.
        SubSsa,      //!< SubStation Alpha, .ssa files.
        SubAss,      //!< Advanced SubStation Alpha, SSA v4+, .ass files.
        SubDvd,      //!< DVD subtitles or VobSub, .sub + .idx files.
        SubDvb,      //!< DVB subtitles (in transport streams only).
        SubTeletext, //!< Teletext subtitles (in transport streams only).
        SubCc,       //!< CEA-608/708 Closed Captions.
        SubOther,    //!< Other type of subtitles.
        SubNone      //!< Not a subtitle stream or file.
    };

    //!
    //! Get the subtitle type of the stream.
    //! @return The subtitle type of the stream.
    //!
    SubtitleType subtitleType() const
    {
        return _subtitleType;
    }

    //!
    //! Set the subtitle type of the stream.
    //! @param [in] subtitleType The subtitle type of the stream.
    //!
    void setSubtitleType(const SubtitleType& subtitleType)
    {
        _subtitleType = subtitleType;
    }

    //!
    //! Get the type of a subtitle file, based on its extension.
    //! @param [in] fileName File name. The file name extension is used to determine the subtitle type.
    //! @param [in] checkExistence If true, also check that the file exists. If false, only check
    //! the file name extension.
    //! @return Subtitle type. If @a checkExistence is true and the file does not exist, return SubNone.
    //!
    static SubtitleType subtitleType(const QString& fileName, bool checkExistence = false);

    //!
    //! Get the usual file extension for subtitle.
    //! @param [in] type Subtitle type.
    //! @return File extension (including the dot) or empty if unknown.
    //!
    static QString fileExtension(SubtitleType type);

    //!
    //! Check if subtitles files are text files.
    //! @param [in] type Subtitle type to check.
    //! @return True if subtitle files for the given @a type are text files, false if they are binary files.
    //!
    static bool isTextFile(SubtitleType type);

    //!
    //! Check if subtitles files are text files.
    //! @param [in] fileName Subtitle file name. The file name extension is used to determine the subtitle type.
    //! @return True if subtitle files for the given file are text files, false if they are binary files.
    //!
    static inline bool isTextFile(const QString& fileName)
    {
        return isTextFile(subtitleType(fileName, false));
    }

    //!
    //! Merge two lists of stream informations.
    //! @param [in,out] destination All existing streams in this list are merged
    //! with their counterpart in @a source. For each stream in @a destination, if
    //! a stream with the same streamId() exists in @a source, the missing information
    //! are added in the stream in @a destination.
    //! @param [in] source A list of stream information.
    //!
    static void merge(QtlMediaStreamInfoList& destination, const QtlMediaStreamInfoList& source);

private:
    StreamType   _streamType;       //!< Stream type (audio, video, etc.)
    QString      _title;            //!< Free format readable description.
    QString      _codecName;        //!< Codec name (free format).
    QString      _language;         //!< Stream language (audio, subtitle).
    int          _ffIndex;          //!< Stream index as allocated by FFmpeg/FFprobe.
    int          _streamId;         //!< Physical stream id in input file.
    SubtitleType _subtitleType;     //!< Subtitle type.
    int          _teletextPage;     //!< Teletext page number (Teletext subtitles only).
    int          _ccNumber;         //!< Closed Captions channel and field.
    int          _width;            //!< Video frame width in pixels.
    int          _height;           //!< Video frame height in pixels.
    float        _dar;              //!< Video frame display aspect ratio.
    float        _forcedDar;        //!< Manually forced video frame display aspect ratio.
    int          _rotation;         //!< Video frame rotation in degrees.
    bool         _forced;           //!< Forced stream.
    bool         _impaired;         //!< For hearing/visual impaired.
    int          _bitRate;          //!< Bitrate (bits/second).
    float        _frameRate;        //!< Frame rate (frames/second).
    bool         _originalAudio;    //!< Original soundtrack.
    bool         _dubbedAudio;      //!< Dubbed soundtrack.
    int          _audioChannels;    //!< Audio channel count.
    int          _samplingRate;     //!< Sampling rate (samples/second).
    bool         _commentary;       //!< Commentary soundtrack.

    //!
    //! Append a separator to a string if it is non empty and a new item.
    //! @param [in,out] str String to update.
    //! @param [in] item Item to add.
    //! @param [in] separator Separator to append if @a str is not empty.
    //!
    static void add(QString& str, const QString& item, const QString& separator = ", ");
};

#endif // QTLMEDIASTREAMINFO_H
