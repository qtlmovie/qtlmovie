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
//!
//! @file QtlMovieInputFile.h
//!
//! Declare the class QtlMovieInputFile.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEINPUTFILE_H
#define QTLMOVIEINPUTFILE_H

#include "QtlFile.h"
#include "QtlMovieStreamInfo.h"
#include "QtlMovieSettings.h"
#include "QtlMovieFFprobeTags.h"
#include "QtlMovieTeletextSearch.h"
#include "QtlProcessResult.h"
#include <QMap>
#include <QStringList>

//!
//! Describes an input video file.
//!
class QtlMovieInputFile : public QtlFile
{
#if !defined (DOXYGEN)
    Q_OBJECT
#endif

public:
    //!
    //! Constructor.
    //! @param [in] fileName Input file name.
    //! @param [in] settings Application settings.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //!
    QtlMovieInputFile(const QString& fileName,
                      const QtlMovieSettings* settings,
                      QtlLogger* log,
                      QObject *parent = 0);

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy (except parent).
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlMovieInputFile(const QtlMovieInputFile& other, QObject *parent = 0);

    //!
    //! Get the name to specify as input to ffmpeg when this object is used as input to ffmpeg.
    //! This may differ from the file name when the input file is a DVD IFO or VOB.
    //! @return A string to specify as input to ffmpeg. This may not be a valid file name.
    //!
    QString ffmpegInputFileSpecification() const
    {
        return _ffmpegInput;
    }

    //!
    //! Get the number of streams in the file.
    //! @return Number of streams in the file.
    //!
    int streamCount() const
    {
        return _streams.size();
    }

    //!
    //! Get the number of streams in the file matching a given type.
    //! @param [in] streamType Stream type to look for.
    //! @return Number of streams in the file for that type.
    //!
    int streamCount(QtlMovieStreamInfo::StreamType streamType) const;

    //!
    //! Get the information about the first stream in the file matching a given type.
    //! @param [in] streamType Stream type to look for.
    //! @return A smart pointer to the stream information data or a null pointer if not found.
    //!
    QtlMovieStreamInfoPtr firstStream(QtlMovieStreamInfo::StreamType streamType) const;

    //!
    //! Get the information about a stream in input file.
    //! @param [in] streamIndex Index of the stream to query, from 0 to streamCount()-1.
    //! @return A smart pointer to the stream information data or a null pointer on error.
    //!
    QtlMovieStreamInfoPtr streamInfo(int streamIndex) const;

    //!
    //! Try to evaluate the duration of the media file in seconds.
    //! @return Duration of media in seconds or zero if unknown.
    //!
    float durationInSeconds() const;

    //!
    //! Check if the file seems to be DVD-compliant.
    //!
    //! More precisely, check if the file looks like one of our DVD-compliant MPEG files,
    //! one of those we generate for output type QtlMovieOutputFile::DvdFile.
    //! Note that other MPEG files may also be DVD-compliant but it is difficult to check.
    //! We simply want to filter a simple use case: We first transcoded to a DVD MPEG file
    //! to see the result and we later want to create the ISO image without retranscoding.
    //!
    //! @return True if the file seems DVD-compliant, false otherwise.
    //!
    bool isDvdCompliant() const;

    //!
    //! Check if the file seems to be a DVD ISO image.
    //! In fact, check that the file name ends in ".iso".
    //! @return True if the file seems to be a DVD ISO image.
    //!
    bool isIsoImage() const;

    //!
    //! Get the DVD palette in RGB format.
    //! @return The DVD palette in RGB format or an empty array if the file is not a DVD.
    //!
    QtlByteBlock palette() const
    {
        return _dvdPalette;
    }

    //!
    //! Get the index of video stream to transcode.
    //! @return Index of video stream to transcode or -1 if none.
    //!
    int selectedVideoStreamIndex() const;

    //!
    //! Get the information about the selected video stream.
    //! @return A smart pointer to the stream information data or a null pointer if no stream is selected.
    //!
    QtlMovieStreamInfoPtr selectedVideoStreamInfo() const
    {
        return streamInfo(_selectedVideoStreamIndex);
    }

    //!
    //! Set the index of video stream to transcode.
    //! @param [in] index Index of video stream to transcode or -1 if none.
    //!
    void setSelectedVideoStreamIndex(int index);

    //!
    //! Get the index of audio stream to transcode.
    //! @return Index of audio stream to transcode or -1 if none.
    //!
    int selectedAudioStreamIndex() const;

    //!
    //! Get the information about the selected audio stream.
    //! @return A smart pointer to the stream information data or a null pointer if no stream is selected.
    //!
    QtlMovieStreamInfoPtr selectedAudioStreamInfo() const
    {
        return streamInfo(_selectedAudioStreamIndex);
    }

    //!
    //! Set the index of audio stream to transcode.
    //! @param [in] index Index of audio stream to transcode or -1 if none.
    //!
    void setSelectedAudioStreamIndex(int index);

    //!
    //! Get the index of subtitle stream to transcode.
    //! @return Index of subtitle stream to transcode or -1 if none.
    //!
    int selectedSubtitleStreamIndex() const;

    //!
    //! Get the information about the selected subtitle stream.
    //! @return A smart pointer to the stream information data or a null pointer if no stream is selected.
    //!
    QtlMovieStreamInfoPtr selectedSubtitleStreamInfo() const
    {
        return streamInfo(_selectedSubtitleStreamIndex);
    }

    //!
    //! Set the index of subtitle stream to transcode.
    //! @param [in] index Index of subtitle stream to transcode or -1 if none.
    //!
    void setSelectedSubtitleStreamIndex(int index);

    //!
    //! Get the external subtitle file name.
    //! @return The subtitle file name. Empty if no subtitle file is specified.
    //!
    QString externalSubtitleFileName() const;

    //!
    //! Set an external subtitle file name.
    //! If not empty, clear the selected stream index.
    //! @param [in] subtitleFileName The subtitle file name. Can be empty.
    //!
    void setExternalSubtitleFileName(const QString& subtitleFileName);

    //!
    //! Select the default video, audio and subtitle streams.
    //!
    //! Default video stream selection:
    //! We try to determine the one with "best quality". In theory, this should
    //! be some evaluation of the combination of size, bitrate, encoding options.
    //! Currently, we just keep the video stream with the largest frame size.
    //! If we cannot compute the frame sizes, keep the first video stream.
    //!
    //! Default audio stream selection:
    //! Try to determine "original version". If not possible, keep first audio
    //! track not matching any language in @a audienceLanguages (supposed to
    //! be the original audio, considering any language in @a audienceLanguages
    //! as dubbing). Otherwise, keep first audio without visual/hearing impaired.
    //!
    //! Default subtitle stream selection:
    //! In all cases, consider only subtitle in @a audienceLanguages. If this
    //! list is empty, there will be no default subtitle. Within the subtitle
    //! in @a audienceLanguages, the following criteria apply.
    //! - If the audio stream is in @a audienceLanguages, keep the first "forced"
    //!   subtitle (the "forced" subtitle are used only when the audio track of
    //!   the same language contains dialogues in another language).
    //! - If the audio stream is @e not in @a audienceLanguages, keep subtitle
    //!   which are either (in order of preference)
    //!   1) complete, 2) for hearing/visual impaired, 3) other.
    //!
    //! @param [in] audienceLanguages List of languages of the intended audience.
    //!
    void selectDefaultStreams(const QStringList& audienceLanguages = QStringList());

    //!
    //! Get the list of tags from FFprobe output.
    //! Low-level method, dependent on ffprobe output.
    //! @return A map of tag/value.
    //!
    QtlMovieFFprobeTags ffProbeInfo() const
    {
        return _ffInfo;
    }

signals:
    //!
    //! Emitted when the media info have changed.
    //!
    void mediaInfoChanged();

private slots:
    //!
    //! Invoked when the file name has changed, update file info.
    //! @param [in] fileName Absolute file path.
    //!
    void updateMediaInfo(const QString& fileName);
    //!
    //! Invoked when the ffprobe process completes.
    //! @param [in] result Process execution results.
    //!
    void ffprobeTerminated(const QtlProcessResult& result);
    //!
    //! Invoked when a Teletext subtitle stream is found.
    //! @param [in] stream A smart pointer to the stream info data.
    //!
    void foundTeletextSubtitles(QtlMovieStreamInfoPtr stream);
    //!
    //! Invoked when the search for Teletext subtitles completes.
    //! @param [in] success True on success, false on error.
    //!
    void teletextSearchTerminated(bool success);

private:
    QtlLogger*                  _log;            //!< Where to log errors.
    const QtlMovieSettings*     _settings;       //!< Application settings.
    QString                     _ffmpegInput;    //!< Name to specify as input to ffmpeg.
    QtlMovieFFprobeTags         _ffInfo;         //!< Media info in ffprobe flat format.
    QtlMovieStreamInfoPtrVector _streams;        //!< Stream information.
    QtlMovieStreamInfoPtrVector _dvdIfoStreams;  //!< Stream info from the DVD .IFO file (when applicable).
    QtlByteBlock                _dvdPalette;     //!< DVD color palette in RGB format.
    QtlMovieTeletextSearch*     _teletextSearch; //!< Search for Teletext subtitles in MPEG-TS files.
    int     _selectedVideoStreamIndex;           //!< Index of video stream to transcode.
    int     _selectedAudioStreamIndex;           //!< Index of audio stream to transcode.
    int     _selectedSubtitleStreamIndex;        //!< Index of subtitle stream to transcode.
    QString _externalSubtitleFileName;           //!< Subtitle file name, can be empty.

    // Unaccessible operations.
    QtlMovieInputFile() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieInputFile)
};

#endif // QTLMOVIEINPUTFILE_H
