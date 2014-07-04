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
// Define the class QtlMovieInputFile.
//
//----------------------------------------------------------------------------

#include "QtlMovieInputFile.h"
#include "QtlMovieFFmpeg.h"
#include "QtlMovieDvd.h"
#include "QtlMovieTeletextSearch.h"
#include "QtlMovieClosedCaptionsSearch.h"
#include "QtlMovie.h"
#include "QtlProcess.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieInputFile::QtlMovieInputFile(const QString& fileName,
                                     const QtlMovieSettings* settings,
                                     QtlLogger* log,
                                     QObject *parent) :
    QtlFile(fileName, parent),
    _log(log),
    _settings(settings),
    _ffmpegInput(),
    _ffInfo(),
    _streams(),
    _dvdIfoStreams(),
    _dvdPalette(),
    _teletextSearch(0),
    _ffprobeInProgress(false),
    _ccSearchCount(0),
    _selectedVideoStreamIndex(-1),
    _selectedAudioStreamIndex(-1),
    _selectedSubtitleStreamIndex(-1),
    _externalSubtitleFileName(),
    _isTs(false),
    _isM2ts(false)
{
    Q_ASSERT(log != 0);
    Q_ASSERT(settings != 0);

    // Update media info when the file name is changed.
    connect(this, &QtlMovieInputFile::fileNameChanged, this, &QtlMovieInputFile::updateMediaInfo);

    // Initial update of the media info.
    updateMediaInfo(this->fileName());
}

QtlMovieInputFile::QtlMovieInputFile(const QtlMovieInputFile& other, QObject* parent) :
    QtlFile(other, parent),
    _log(other._log),
    _settings(other._settings),
    _ffmpegInput(other._ffmpegInput),
    _ffInfo(other._ffInfo),
    _streams(other._streams),
    _dvdIfoStreams(other._dvdIfoStreams),
    _dvdPalette(other._dvdPalette),
    _teletextSearch(0),        // don't copy
    _ffprobeInProgress(false), // don't copy
    _ccSearchCount(0),         // don't copy
    _selectedVideoStreamIndex(other._selectedVideoStreamIndex),
    _selectedAudioStreamIndex(other._selectedAudioStreamIndex),
    _selectedSubtitleStreamIndex(other._selectedSubtitleStreamIndex),
    _externalSubtitleFileName(other._externalSubtitleFileName),
    _isTs(other._isTs),
    _isM2ts(other._isM2ts)
{
    // Update media info when the file name is changed.
    connect(this, &QtlMovieInputFile::fileNameChanged, this, &QtlMovieInputFile::updateMediaInfo);
}


//----------------------------------------------------------------------------
// Invoked when the file name has changed, update file info.
//----------------------------------------------------------------------------

void QtlMovieInputFile::updateMediaInfo(const QString& fileName)
{
    // By default, the ffmpeg input spec is the file name.
    _ffmpegInput = fileName;

    // Clear all previous media info.
    const bool wasNone = _streams.isEmpty();
    _streams.clear();
    _ffInfo.clear();

    // If the file is an ISO image, there is nothing to analyze.
    if (isIsoImage()) {
        emit mediaInfoChanged();
        return;
    }

    // If there was some info, they changed (we just cleared them).
    if (!wasNone) {
        emit mediaInfoChanged();
    }

    // If no file specified or not existent or no ffprobe available, nothing to analyze.
    const QString ffprobe(_settings->ffprobe()->fileName());
    if (fileName.isEmpty() || !QFile(fileName).exists() || ffprobe.isEmpty()) {
        return;
    }

    // Check if the file belongs to a DVD structure and collect file names.
    QString ifoFileName;
    QStringList vobFiles;
    if (!QtlMovieDvd::buildFileNames(fileName, ifoFileName, vobFiles, _log)) {
        // Found an invalid DVD structure.
        return;
    }

    // Load DVD .IFO file if one was found.
    _dvdIfoStreams.clear();
    _dvdPalette.clear();
    if (!ifoFileName.isEmpty()) {
        if (!QtlMovieDvd::readIfoFile(ifoFileName, _dvdIfoStreams, _dvdPalette, _log)) {
            _log->line(tr("DVD IFO file not correctly analyzed, audio and subtitle languages are unknown"));
        }
        else {
            // Convert palette to RGB format.
            QtlMovieDvd::convertPaletteYuvToRgb(_dvdPalette, _log);
        }
    }

    // Build the ffmpeg input file specification for multiple DVD VOB files.
    if (vobFiles.size() == 1) {
        // Only one file to transcode. Specify it since fileName was maybe the IFO file.
        _ffmpegInput = vobFiles.first();
    }
    else if (!vobFiles.isEmpty()) {
        // More than one VOB file are present.
        // Use the "concat" protocol to specify a list of file to concatenate as input.
        // Example: ffmpeg -i "concat:vts_01_1.vob|vts_01_2.vob|vts_01_3.vob"
        _ffmpegInput = "concat:" + vobFiles.join('|');
    }

    // Create the process object. It will automatically delete itself after completion.
    const QStringList args(QtlMovieFFmpeg::ffprobeArguments(_settings, _ffmpegInput));
    _log->debug(ffprobe + " " + args.join(' '));
    QtlProcess* process = QtlProcess::newInstance(ffprobe, // command
                                                  args,    // command line arguments
                                                  1000 * _settings->ffprobeExecutionTimeout(),
                                                  40000,   // max output size: 40 kB
                                                  this);   // parent object

    // Get notified of process termination and starts the process.
    connect(process, &QtlProcess::terminated, this, &QtlMovieInputFile::ffprobeTerminated);
    _ffprobeInProgress = true;
    process->start();

    // Look for Closed Captions on channels 1 and 2.
    startClosedCaptionsSearch(1);
    startClosedCaptionsSearch(2);
}


//----------------------------------------------------------------------------
// Start a Closed Caption search.
//----------------------------------------------------------------------------

void QtlMovieInputFile::startClosedCaptionsSearch(int ccChannel)
{
    // Create a new instance of CC search.
    QtlMovieClosedCaptionsSearch* cc = QtlMovieClosedCaptionsSearch::newInstance(fileName(), ccChannel, _settings, _log, this);
    connect(cc, &QtlMovieClosedCaptionsSearch::foundClosedCaptions, this, &QtlMovieInputFile::foundClosedCaptions);
    connect(cc, &QtlMovieClosedCaptionsSearch::completed, this, &QtlMovieInputFile::closedCaptionsSearchTerminated);

    // Start it.
    if (cc->start()) {
        _ccSearchCount++;
    }
    else {
        delete cc;
    }
}


//----------------------------------------------------------------------------
// Invoked when the ffprobe process completes.
//----------------------------------------------------------------------------

void QtlMovieInputFile::ffprobeTerminated(const QtlProcessResult& result)
{
    // FFprobe terminated.
    _ffprobeInProgress = false;

    // Filter ffprobe process execution.
    if (result.hasError()) {
        _log->line(tr("FFprobe error: %1").arg(result.errorMessage()));
    }

    // The standard output from ffprobe contains the media info in the form of "key=value".
    // Fill the _ffInfo data with the parsed info.
    _ffInfo.loadFFprobeOutput(result.standardOutput());

    // Build the stream information from the ffprobe output.
    _ffInfo.buildStreamInfo(_streams);

    // Post-processing when the input has a DVD structure.
    if (!_dvdIfoStreams.isEmpty()) {

        // Merge the info which were previously collected in
        // the .IFO file with the stream info from ffprobe.
        QtlMovieStreamInfo::merge(_streams, _dvdIfoStreams);

        // Sort streams in the DVD order for user's convenience.
        qSort(_streams.begin(), _streams.end(), QtlMovieDvd::lessThan);
    }

    // Is the file a transport stream?
    _isTs = _ffInfo.value("format.format_name").toLower() == "mpegts";

    // If Teletext subtitles are detected, ffprobe does not report the Teletext page value
    // and we need to analyze the file. Sometimes, ffprobe does not even detect that the
    // subtitles are Teletext (unknown subtitle type) and we need to analyze the file as well.
    bool searchTeletext = false;
    if (_isTs) {
        foreach (const QtlMovieStreamInfoPtr& stream, _streams) {
            if (!stream.isNull() &&
                stream->streamType() == QtlMovieStreamInfo::Subtitle &&
                ((stream->subtitleType() == QtlMovieStreamInfo::SubTeletext && stream->teletextPage() < 0) || stream->subtitleType() == QtlMovieStreamInfo::SubOther)) {
                searchTeletext = true;
                break;
            }
        }
    }

    // Start TS analysis when unknown subtitle types are found.
    if (searchTeletext) {

        // Cleanup previous search (should not be any).
        if (_teletextSearch != 0) {
            delete _teletextSearch;
        }

        // Create a search action for teletext.
        // Will be deleted no later than this object.
        _teletextSearch = new QtlMovieTeletextSearch(fileName(), _settings, _log, this);

        // Get notifications from the Teletext searcher.
        connect(_teletextSearch, &QtlMovieTeletextSearch::foundTeletextSubtitles, this, &QtlMovieInputFile::foundTeletextSubtitles);
        connect(_teletextSearch, &QtlMovieTeletextSearch::completed, this, &QtlMovieInputFile::teletextSearchTerminated);

        // Start the search.
        if (!_teletextSearch->start()) {
            _log->line(tr("Failed to start search for Teletext subtitles"));
            searchTeletext = false;
            delete _teletextSearch;
            _teletextSearch = 0;
        }
    }

    // Notify the new media information only when nothing more to do.
    newMediaInfo();
}


//----------------------------------------------------------------------------
// Invoked when a Teletext subtitle stream is found.
//----------------------------------------------------------------------------

void QtlMovieInputFile::foundTeletextSubtitles(QtlMovieStreamInfoPtr stream)
{
    _log->debug(tr("Found one Teletext subtitle stream, page %1").arg(stream->teletextPage()));

    // The first time a Teletext stream is found on a PID, there should be
    // one stream which was built from the ffprobe output with that PID
    // with unknown subtitle type. We need to remove it.
    if (stream->streamId() >= 0) {
        // We know the PID of the stream in the TS file (should be always the case).
        // Look for previous streams with the same PID.
        for (QtlMovieStreamInfoPtrVector::Iterator it = _streams.begin(); it != _streams.end(); ++it) {
            const QtlMovieStreamInfoPtr& s(*it);
            if (s->streamId() == stream->streamId()) {
                // Found a previous stream with same PID.
                // If the new stream's ffmpeg index is unknown, get it from the previous stream on same PID.
                if (stream->ffIndex() < 0) {
                    stream->setFFIndex(s->ffIndex());
                }
                // If the previous stream is unknown subtitle or teletext with unknown page, remove it
                // since we know have a better characterization of the stream.
                if (s->streamType() == QtlMovieStreamInfo::Subtitle &&
                    (s->subtitleType() == QtlMovieStreamInfo::SubOther || (s->subtitleType() == QtlMovieStreamInfo::SubTeletext && s->teletextPage() < 0))) {
                    _streams.erase(it);
                    // We can stop now and we must since our iterator is broken by erase().
                    break;
                }
            }
        }
    }

    // Append the new stream in the input file.
    _streams.append(stream);
}


//----------------------------------------------------------------------------
// Invoked when a Closed Captions stream is found.
//----------------------------------------------------------------------------

void QtlMovieInputFile::foundClosedCaptions(QtlMovieStreamInfoPtr stream)
{
    // Append the new stream in the input file.
    _streams.append(stream);
}


//----------------------------------------------------------------------------
// Invoked when the search for Teletext subtitles completes.
//----------------------------------------------------------------------------

void QtlMovieInputFile::teletextSearchTerminated(bool success)
{
    // Cleanup the teletext searcher.
    if (_teletextSearch != 0) {
        _log->debug(tr("Search for Teletext subtitles completed"));

        // We have analyzed part of the file. Is this an M2TS file?
        _isM2ts = _teletextSearch->isM2tsFile();

        // Cleanup the search object.
        _teletextSearch->deleteLater();
        _teletextSearch = 0;
    }

    // Notify the new media information.
    newMediaInfo();
}


//----------------------------------------------------------------------------
// Invoked when a search for Closed Captions completes.
//----------------------------------------------------------------------------

void QtlMovieInputFile::closedCaptionsSearchTerminated(bool success)
{
    // Decrement the number of searches.
    if (_ccSearchCount > 0) {
        _ccSearchCount--;
    }

    // Notify the new media information when no more operation in progress
    // but do not change the selection.
    if (_ccSearchCount <= 0) {
        emit mediaInfoChanged();
    }
}


//----------------------------------------------------------------------------
// Report that new media info has been found.
//----------------------------------------------------------------------------

void QtlMovieInputFile::newMediaInfo()
{
    // Notify the new media information when no more operation in progress.
    // Do no take search for Closed Captions into account for the following reasons:
    // - If no CC is present, the "search duration" of CCExtractor is ineffective
    //   and CCExtractor will read the entire input file. This can take a lot of
    //   time which delays the display of the other streams and will not bring
    //   any new information since there is no CC
    // - CC have no metadata and will never affect the default stream selection.
    //   So, there is no inconvenient to notify mediaInfoChanged before CC search
    //   is completed. If some CC is finally found, we will emit it again and only
    //   the new CC streams will appear.

    if (!_ffprobeInProgress && _teletextSearch == 0) {
        // Select the "best" default video/audio/subtitles streams.
        selectDefaultStreams(_settings->audienceLanguages());

        // No more operation in progress, notify the new media information.
        emit mediaInfoChanged();
    }
}


//----------------------------------------------------------------------------
// Get the number of streams in the file matching a given type.
//----------------------------------------------------------------------------

int QtlMovieInputFile::streamCount(QtlMovieStreamInfo::StreamType streamType) const
{
    int count = 0;
    foreach (const QtlMovieStreamInfoPtr& s, _streams) {
        if (!s.isNull() && s->streamType() == streamType) {
            count++;
        }
    }
    return count;
}


//----------------------------------------------------------------------------
// Get the information about the first stream in the file matching a given type.
//----------------------------------------------------------------------------

QtlMovieStreamInfoPtr QtlMovieInputFile::firstStream(QtlMovieStreamInfo::StreamType streamType) const
{
    foreach (const QtlMovieStreamInfoPtr& s, _streams) {
        if (!s.isNull() && s->streamType() == streamType) {
            return s;
        }
    }
    return 0;
}


//----------------------------------------------------------------------------
// Get the information about a stream in input file.
//----------------------------------------------------------------------------

QtlMovieStreamInfoPtr QtlMovieInputFile::streamInfo(int streamIndex) const
{
    return streamIndex >= 0 && streamIndex < _streams.size() ? _streams[streamIndex] : 0;
}


//----------------------------------------------------------------------------
// Try to evaluate the duration of the media file in seconds.
//----------------------------------------------------------------------------

float QtlMovieInputFile::durationInSeconds() const
{
    // Try duration of file.
    float duration = _ffInfo.floatValue("format.duration");

    // If not found, try all streams duration until one is found.
    const int count = _ffInfo.intValue("format.nb_streams");
    for (int si = 0; duration < 0.001 && si < count; ++si) {
        duration = _ffInfo.floatValueOfStream(si, "duration");
    }

    return duration;
}


//----------------------------------------------------------------------------
// Check if the file seems to be DVD-compliant.
//----------------------------------------------------------------------------

bool QtlMovieInputFile::isDvdCompliant() const
{
    // The DVD-compliant files we generate have the following characteristics:
    // - MPEG-PS file format.
    // - Exactly 2 streams: one MPEG-2 video and one AC-3 audio.
    // - Same video size and aspect ratio.

    const QtlMovieStreamInfoPtr audioStream(firstStream(QtlMovieStreamInfo::Audio));

    return  streamCount(QtlMovieStreamInfo::Video) == 1 &&
            streamCount(QtlMovieStreamInfo::Audio) == 1 &&
            streamCount(QtlMovieStreamInfo::Subtitle) == 0 &&
            selectedVideoStreamIsDvdCompliant() &&
            !audioStream.isNull() &&
            _ffInfo.value("format.format_name") == "mpeg" &&
            _ffInfo.valueOfStream(audioStream->ffIndex(), "codec_name") == "ac3";
}


//----------------------------------------------------------------------------
// Check if the selected video stream is DVD-compliant.
//----------------------------------------------------------------------------

bool QtlMovieInputFile::selectedVideoStreamIsDvdCompliant() const
{
    const QtlMovieStreamInfoPtr videoStream(selectedVideoStreamInfo());

    return  !videoStream.isNull() &&
            videoStream->width() == _settings->dvdVideoWidth() &&
            videoStream->height() == _settings->dvdVideoHeight() &&
            qAbs(videoStream->displayAspectRatio() - QTL_DVD_DAR) < 0.001 &&
            _ffInfo.valueOfStream(videoStream->ffIndex(), "codec_name") == "mpeg2video";
}


//----------------------------------------------------------------------------
// Check if the file seems to be a DVD ISO image.
//----------------------------------------------------------------------------

bool QtlMovieInputFile::isIsoImage() const
{
    QFileInfo info(fileName());
    return info.suffix().toLower() == "iso";
}


//----------------------------------------------------------------------------
// Select the default video, audio and subtitle streams.
//----------------------------------------------------------------------------

void QtlMovieInputFile::selectDefaultStreams(const QStringList& audienceLanguages)
{
    // Reset selected streams.
    _selectedVideoStreamIndex = -1;
    _selectedAudioStreamIndex = -1;
    _selectedSubtitleStreamIndex = -1;

    int highestFrameSize = -1;  // Largest video frame size (width x height).
    int firstAudio = -1;        // First audio stream index.
    int originalAudio = -1;     // First original audio stream index (and not impaired).
    int notAudienceAudio = -1;  // First audio stream index not in intended audience languages (and not impaired).
    int notImpairedAudio = -1;  // First audio stream index not for hearing/visual impaired.
    int firstSubtitle = -1;     // First subtitles in intended audience languages.
    int completeSubtitle = -1;  // First complete subtitles in intended audience languages.
    int forcedSubtitle = -1;    // First forced subtitles in intended audience languages.
    int impairedSubtitle = -1;  // First subtitles for impaired in intended audience languages.

    // Loop on all streams.
    const int streamMax = streamCount();
    for (int streamIndex = 0; streamIndex < streamMax; streamIndex++) {

        // Stream description.
        const QtlMovieStreamInfoPtr& stream(_streams[streamIndex]);

        // Is this stream in the intended audience languages?
        const bool inAudienceLanguages = audienceLanguages.contains(stream->language(), Qt::CaseInsensitive);

        // Type-specific processing.
        switch (stream->streamType()) {
        case QtlMovieStreamInfo::Video: {
            // Compute video frame size (width x height). Zero if width or height undefined.
            const int frameSize = stream->width() * stream->height();
            if (frameSize > highestFrameSize) {
                // Largest frame size so far or first video stream, keep it.
                _selectedVideoStreamIndex = streamIndex;
                highestFrameSize = frameSize;
            }
            break;
        }

        case QtlMovieStreamInfo::Audio: {
            if (firstAudio < 0) {
                // Found first audio track.
                firstAudio = streamIndex;
            }
            if (originalAudio < 0 && !stream->impaired() && stream->isOriginalAudio() && !stream->isDubbedAudio()) {
                // Found the first original audio track.
                originalAudio = streamIndex;
            }
            if (notAudienceAudio < 0 && !stream->impaired() && !inAudienceLanguages) {
                // Found the first audio track not in the audience languages.
                notAudienceAudio = streamIndex;
            }
            if (notImpairedAudio < 0 && !stream->impaired()) {
                // Found the first audio track for non-impaired.
                notImpairedAudio = streamIndex;
            }
            break;
        }

        case QtlMovieStreamInfo::Subtitle: {
            if (inAudienceLanguages) {
                // Keep only subtitles for intended audience.
                if (firstSubtitle < 0) {
                    // Found first subtitles.
                    firstSubtitle = streamIndex;
                }
                if (forcedSubtitle < 0 && stream->forced()) {
                    // Found first forced subtitle.
                    forcedSubtitle = streamIndex;
                }
                if (impairedSubtitle < 0 && stream->impaired()) {
                    // Found first subtitle stream for visual/hearing impaired.
                    impairedSubtitle = streamIndex;
                }
                if (completeSubtitle < 0 && !stream->forced() && !stream->impaired()) {
                    // Found first complete subtitle.
                    completeSubtitle = streamIndex;
                }
            }
            break;
        }

        default:
            // Other stream, ignore it.
            break;
        }
    }

    // Select the default audio stream.
    if (originalAudio >= 0) {
        // Use original audio stream.
        _selectedAudioStreamIndex = originalAudio;
    }
    else if (notAudienceAudio >= 0) {
        // Use first audio stream index not in intended audience languages.
        _selectedAudioStreamIndex = notAudienceAudio;
    }
    else if (notImpairedAudio >= 0) {
        // Use first audio stream index not for hearing/visual impaired.
        _selectedAudioStreamIndex = notImpairedAudio;
    }
    else {
        // Use first audio stream index (if any).
        _selectedAudioStreamIndex = firstAudio;
    }

    // Check if the selected audio stream is in a language for the intended audience.
    // This will impact the selection of the subtitle stream.
    const bool audioInAudienceLanguages =
            _selectedAudioStreamIndex >= 0 &&
            audienceLanguages.contains(_streams[_selectedAudioStreamIndex]->language(), Qt::CaseInsensitive);

    // Select the default subtitle stream.
    if (audioInAudienceLanguages) {
        // Use only "forced" subtitles (if any) when audio is for the intended audience.
        _selectedSubtitleStreamIndex = forcedSubtitle;
    }
    else if (completeSubtitle >= 0) {
        // Use complete subtitles.
        _selectedSubtitleStreamIndex = completeSubtitle;
    }
    else if (impairedSubtitle >= 0) {
        // Use subtitles for hearing/visual impaired.
        _selectedSubtitleStreamIndex = impairedSubtitle;
    }
    else {
        // Use first subtitles stream (if any).
        _selectedSubtitleStreamIndex = firstSubtitle;
    }

    // Debug display.
    _log->debug(tr("Default streams: video: %1, audio: %2, subtitles: %3")
                .arg(_selectedVideoStreamIndex)
                .arg(_selectedAudioStreamIndex)
                .arg(_selectedSubtitleStreamIndex));
}


//----------------------------------------------------------------------------
// Get or set the various stream index to transcode.
//----------------------------------------------------------------------------

int QtlMovieInputFile::selectedVideoStreamIndex() const
{
    return _selectedVideoStreamIndex < streamCount() ? _selectedVideoStreamIndex : -1;
}

void QtlMovieInputFile::setSelectedVideoStreamIndex(int selectedVideoStreamIndex)
{
    _selectedVideoStreamIndex = selectedVideoStreamIndex;
}

int QtlMovieInputFile::selectedAudioStreamIndex() const
{
    return _selectedAudioStreamIndex < streamCount() ? _selectedAudioStreamIndex : -1;
}

void QtlMovieInputFile::setSelectedAudioStreamIndex(int selectedAudioStreamIndex)
{
    _selectedAudioStreamIndex = selectedAudioStreamIndex;
}

int QtlMovieInputFile::selectedSubtitleStreamIndex() const
{
    return _selectedSubtitleStreamIndex < streamCount() ? _selectedSubtitleStreamIndex : -1;
}

void QtlMovieInputFile::setSelectedSubtitleStreamIndex(int selectedSubtitleStreamIndex)
{
    _selectedSubtitleStreamIndex = selectedSubtitleStreamIndex;
}

QString QtlMovieInputFile::externalSubtitleFileName() const
{
    return selectedSubtitleStreamIndex() < 0 ? _externalSubtitleFileName : QString();
}

void QtlMovieInputFile::setExternalSubtitleFileName(const QString& subtitleFileName)
{
    _externalSubtitleFileName = subtitleFileName;
    if (!subtitleFileName.isEmpty()) {
        _selectedSubtitleStreamIndex = -1;
    }
}
