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
#include "QtlDataPull.h"
#include "QtsDvdTitleSet.h"
#include "QtlProcess.h"
#include "QtlMediaStreamInfo.h"
#include "QtlMovieSettings.h"
#include "QtlMovieFFprobeTags.h"
#include "QtlMovieTeletextSearch.h"

//!
//! Describes an input video file.
//!
class QtlMovieInputFile : public QtlFile
{
    Q_OBJECT

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
                      QObject* parent = 0);

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy (except parent).
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlMovieInputFile(const QtlMovieInputFile& other, QObject* parent = 0);

    //!
    //! Get the associated error logger.
    //! @return The associated error logger.
    //!
    QtlLogger* log() const
    {
        return _log;
    }

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
    //! Get the name to specify as input file format to ffmpeg when this object is used as input to ffmpeg.
    //! @return A string to specify as input file format to ffmpeg. Empty is ffmpeg is able to figure it out.
    //!
    QString ffmpegInputFileFormat() const
    {
        return _ffmpegFormat;
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
    int streamCount(QtlMediaStreamInfo::StreamType streamType) const;

    //!
    //! Get the information about the first stream in the file matching a given type.
    //! @param [in] streamType Stream type to look for.
    //! @return A smart pointer to the stream information data or a null pointer if not found.
    //!
    QtlMediaStreamInfoPtr firstStream(QtlMediaStreamInfo::StreamType streamType) const;

    //!
    //! Get the information about a stream in input file.
    //! @param [in] streamIndex Index of the stream to query, from 0 to streamCount()-1.
    //! @return A smart pointer to the stream information data or a null pointer on error.
    //!
    QtlMediaStreamInfoPtr streamInfo(int streamIndex) const;

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
    //! Check if the selected video stream is DVD-compliant.
    //! To generate a DVD from this kind of file, we can simply copy the audio
    //! stream without re-encoding. This saves much time.
    //! @return True if the selected video stream is DVD-compliant, false otherwise.
    //!
    bool selectedVideoStreamIsDvdCompliant() const;

    //!
    //! Check if the file seems to be a DVD ISO image.
    //! In fact, check that the file name ends in ".iso".
    //! @return True if the file seems to be a DVD ISO image.
    //!
    bool isIsoImage() const;

    //!
    //! Check if the input file is an MPEG transport stream.
    //! This include TS and M2TS files.
    //! @return True if the input file is an MPEG transport stream.
    //!
    bool isTsFile() const
    {
        return _isTs;
    }

    //!
    //! Check if the input file has M2TS format.
    //! @return True if the input file has M2TS format.
    //!
    bool isM2tsFile() const
    {
        return _isM2ts;
    }

    //!
    //! Check if the input file is a pure subtitle file.
    //! This means that the file is for instance a .srt or .ass file,
    //! not included into some container file format.
    //! @return True if the input file is a pure subtitle file.
    //!
    bool isSubtitleFile() const
    {
        return _isSubtitle;
    }

    //!
    //! Check if the input file content must be piped from QtlMovie.
    //!
    //! Some input files cannot be read directly from the file system.
    //! This is the case for encrypted DVD's: you can browse files, you can
    //! read metadata files (.IFO) but you cannot read the content of video
    //! files (.VOB). For that kind of files, ffmpeg or ffprobe cannot
    //! directly read the file. They must read the content from their standard
    //! input and QtlMovie will pipe the extracted file content into it.
    //!
    //! @return True if the input file content must be piped from QtlMovie.
    //! @see dataPull()
    //! @see ffmpegInputFileSpecification()
    //!
    bool pipeInput() const
    {
        return _pipeInput;
    }

    //!
    //! Get an instance of QtlDataPull to transfer the content of the input file.
    //! @return An instance of QtlDataPull which can transfer the content of the input
    //! file into a QIODevice. This object will delete itself at the end of the transfer.
    //! Return a null pointer if the file shall be read directly from the file system.
    //! @param parent Optional parent object of the QtlDataPull instance.
    //! @see pipeInput()
    //!
    QtlDataPull* dataPull(QObject* parent = 0) const;

    //!
    //! Get the DVD palette in RGB format.
    //! @return The DVD palette in RGB format or an empty array if the file is not a DVD.
    //!
    QtlByteBlock palette() const;

    //!
    //! Get the index of video stream to transcode.
    //! @return Index of video stream to transcode or -1 if none.
    //!
    int selectedVideoStreamIndex() const;

    //!
    //! Get the information about the selected video stream.
    //! @return A smart pointer to the stream information data or a null pointer if no stream is selected.
    //!
    QtlMediaStreamInfoPtr selectedVideoStreamInfo() const
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
    QtlMediaStreamInfoPtr selectedAudioStreamInfo() const
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
    QtlMediaStreamInfoPtr selectedSubtitleStreamInfo() const
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
    void foundTeletextSubtitles(QtlMediaStreamInfoPtr stream);
    //!
    //! Invoked when a Closed Captions stream is found.
    //! @param [in] stream A smart pointer to the stream info data.
    //!
    void foundClosedCaptions(QtlMediaStreamInfoPtr stream);
    //!
    //! Invoked when the search for Teletext subtitles completes.
    //! @param [in] success True on success, false on error.
    //!
    void teletextSearchTerminated(bool success);
    //!
    //! Invoked when a search for Closed Captions completes.
    //! @param [in] success True on success, false on error.
    //!
    void closedCaptionsSearchTerminated(bool success);

private:
    QtlLogger*              _log;            //!< Where to log errors.
    const QtlMovieSettings* _settings;       //!< Application settings.
    QString                 _ffmpegInput;    //!< Name to specify as input to ffmpeg.
    QString                 _ffmpegFormat;   //!< Name to specify as input file format to ffmpeg.
    QtlMovieFFprobeTags     _ffInfo;         //!< Media info in ffprobe flat format.
    QtlMediaStreamInfoList  _streams;        //!< Stream information.
    QtsDvdTitleSet          _dvdTitleSet;    //!< DVD title set access (when the input file comes from a DVD).
    QtsDvdProgramChainPtr   _dvdPgc;         //!< Smart pointer to PGC inside the DVD VTS.
    QtlMovieTeletextSearch* _teletextSearch; //!< Search for Teletext subtitles in MPEG-TS files.
    int     _ffprobeCount;                       //!< Number of ffprobe in progress.
    int     _ccSearchCount;                      //!< Number of Closed Captions research in progress.
    int     _selectedVideoStreamIndex;           //!< Index of video stream to transcode.
    int     _selectedAudioStreamIndex;           //!< Index of audio stream to transcode.
    int     _selectedSubtitleStreamIndex;        //!< Index of subtitle stream to transcode.
    bool    _selectedVideoExplicit;              //!< Video stream explicitly selected (vs. default choice).
    bool    _selectedAudioExplicit;              //!< Audio stream explicitly selected (vs. default choice).
    bool    _selectedSubtitleExplicit;           //!< Subtitle stream explicitly selected (vs. default choice).
    QString _externalSubtitleFileName;           //!< Subtitle file name, can be empty.
    bool    _isTs;                               //!< File is a transport stream (TS or M2TS format).
    bool    _isM2ts;                             //!< File has M2TS format.
    bool    _isSubtitle;                         //!< File is a pure subtitle file, no container format.
    bool    _pipeInput;                          //!< Input file content shall be piped from QtlMovie.

    //!
    //! Report that new media info has been found.
    //! If no more operation in progress, emit mediaInfoChanged().
    //!
    void newMediaInfo();

    //!
    //! Create and start a ffprobe process.
    //! @param [in] probeTimeDivisor If positive, this value is used to reduce the probe time duration.
    //! The actual probe time is the value from @a settings, divided by @a probeTimeDivisor.
    //! @param [in] ffprobeTimeout Timeout of process execution in seconds.
    //! @return A new instance of QtlProcess.
    //!
    QtlProcess* ffprobeProcess(int probeTimeDivisor, int ffprobeTimeout);

    // Unaccessible operations.
    QtlMovieInputFile() Q_DECL_EQ_DELETE;
};

#endif // QTLMOVIEINPUTFILE_H
