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
//
// Define the class QtlMovieInputFile.
//
//----------------------------------------------------------------------------

#include "QtlMovieInputFile.h"
#include "QtlMovieFFmpeg.h"
#include "QtlMovieTeletextSearch.h"
#include "QtlMovieClosedCaptionsSearch.h"
#include "QtlMovie.h"
#include "QtlNumUtils.h"
#include "QtlFileDataPull.h"
#include "QtsDvdDataPull.h"
#include "QtsDvdProgramChainDemux.h"


//----------------------------------------------------------------------------
// Constructors and destructors.
//----------------------------------------------------------------------------

QtlMovieInputFile::QtlMovieInputFile(const QString& fileName,
                                     const QtlMovieSettings* settings,
                                     QtlLogger* log,
                                     QObject* parent) :
    QtlFile(fileName, parent),
    _log(log),
    _settings(settings),
    _ffmpegInput(),
    _ffmpegFormat(),
    _ffInfo(),
    _streams(),
    _dvdTitleSet(fileName, log),
    _dvdPgc(),
    _teletextSearch(0),
    _ffprobeCount(0),
    _ccSearchCount(0),
    _selectedVideoStreamIndex(-1),
    _selectedAudioStreamIndex(-1),
    _selectedSubtitleStreamIndex(-1),
    _selectedVideoExplicit(false),
    _selectedAudioExplicit(false),
    _selectedSubtitleExplicit(false),
    _externalSubtitleFileName(),
    _isTs(false),
    _isM2ts(false),
    _isSubtitle(false),
    _pipeInput(false),
    _dvdTranscodeRawVob(settings->dvdTranscodeRawVob()),
    _dvdProgramChain(settings->dvdProgramChain()),
    _dvdAngle(settings->dvdAngle())
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
    _ffmpegFormat(other._ffmpegFormat),
    _ffInfo(other._ffInfo),
    _streams(other._streams),
    _dvdTitleSet(other._dvdTitleSet),
    _dvdPgc(other._dvdPgc),
    _teletextSearch(0),  // don't copy
    _ffprobeCount(0),    // don't copy
    _ccSearchCount(0),   // don't copy
    _selectedVideoStreamIndex(other._selectedVideoStreamIndex),
    _selectedAudioStreamIndex(other._selectedAudioStreamIndex),
    _selectedSubtitleStreamIndex(other._selectedSubtitleStreamIndex),
    _selectedVideoExplicit(false),     // don't copy
    _selectedAudioExplicit(false),     // don't copy
    _selectedSubtitleExplicit(false),  // don't copy
    _externalSubtitleFileName(other._externalSubtitleFileName),
    _isTs(other._isTs),
    _isM2ts(other._isM2ts),
    _isSubtitle(other._isSubtitle),
    _pipeInput(other._pipeInput),
    _dvdTranscodeRawVob(other._dvdTranscodeRawVob),
    _dvdProgramChain(other._dvdProgramChain),
    _dvdAngle(other._dvdAngle)
{
    // Update media info when the file name is changed.
    connect(this, &QtlMovieInputFile::fileNameChanged, this, &QtlMovieInputFile::updateMediaInfo);
}


//----------------------------------------------------------------------------
// Create and start a ffprobe process.
//----------------------------------------------------------------------------

QtlBoundProcess* QtlMovieInputFile::ffprobeProcess(int probeTimeDivisor, int ffprobeTimeout)
{
    // ffprobe executable.
    const QString ffprobe(_settings->ffprobe()->fileName());

    // Build ffprobe arguments
    QStringList args;
    args << "-loglevel" << QTL_FFPROBE_LOGLEVEL
         << QtlMovieFFmpeg::probeArguments(_settings, probeTimeDivisor);
    if (!_ffmpegFormat.isEmpty()) {
        args << "-f" << _ffmpegFormat;
    }
    args << _ffmpegInput << "-print_format" << "flat" << "-show_format" << "-show_streams";

    // Create the process object.
    _log->debug(ffprobe + " " + args.join(' '));
    QtlBoundProcess* process =
            QtlBoundProcess::newInstance(ffprobe,
                                         args,
                                         1000 * ffprobeTimeout,
                                         65536,   // max output size: 64 kB
                                         this,    // parent object
                                         QProcessEnvironment(),
                                         _pipeInput);

    // Get notified of process termination and starts the process.
    connect(process, &QtlBoundProcess::terminated, this, &QtlMovieInputFile::ffprobeTerminated);
    process->start();
    _ffprobeCount++;
    return process;
}


//----------------------------------------------------------------------------
// Invoked when the file name has changed, update file info.
//----------------------------------------------------------------------------

void QtlMovieInputFile::updateMediaInfo(const QString& fileName)
{
    // By default, the ffmpeg input spec is the file name.
    _ffmpegInput = fileName;
    _ffmpegFormat.clear();
    _pipeInput = false;
    _dvdPgc.clear();

    // Clear all previous media info.
    const bool wasNone = _streams.isEmpty();
    _streams.clear();
    _ffInfo.clear();
    _selectedVideoStreamIndex = -1;
    _selectedAudioStreamIndex = -1;
    _selectedSubtitleStreamIndex = -1;
    _selectedVideoExplicit = false;
    _selectedAudioExplicit = false;
    _selectedSubtitleExplicit = false;

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

    // Check if the file belongs to a DVD structure and collect relevant information.
    const bool isOnDvd = _dvdTitleSet.load(fileName);
    const bool isOnEncryptedDvd = isOnDvd && _dvdTitleSet.isEncrypted();

    // Collect and save DVD demultiplexing options. Depending on these options, the DVD content
    // which will be sent to FFmpeg can be different. If the user changes the settings
    // between the analysis of the input (now) and the transcoding (later), the stream
    // information could be inconsistent in the two operations. To avoid this, we need
    // to use the same demux options in both cases.
    _dvdTranscodeRawVob = _settings->dvdTranscodeRawVob();
    _dvdProgramChain = _settings->dvdProgramChain();
    _dvdAngle = _settings->dvdAngle();

    // DVD media and file structure requires special treatment.
    int ffprobeTimeout = _settings->ffprobeExecutionTimeout();
    if (isOnDvd) {

        // If the specified PGC in the settings does not exist in this VTS, revert to PGC #1.
        // This is a safe default since most DVD's have only one PGC.
        _dvdPgc = _dvdTitleSet.title(_dvdProgramChain);
        if (_dvdPgc.isNull()) {
            _dvdPgc = _dvdTitleSet.title(1);
        }

        // Give a 4 times longer timeout on DVD devices, they are so slow to start.
        ffprobeTimeout *= 4;

        if (isOnEncryptedDvd || !_dvdTranscodeRawVob) {
            // Encrypted and/or demuxed DVD's shall be read from stdin.
            // We need to specify the file format for ffmpeg.
            _ffmpegInput = "-";
            _ffmpegFormat = "mpeg";
            _pipeInput = true;
        }
        else if (_dvdTitleSet.vobCount() == 1) {
            // Only one file to transcode. Specify it since fileName was maybe the IFO file.
            _ffmpegInput = _dvdTitleSet.vobFileNames().first();
        }
        else {
            // More than one VOB file are present.
            // Use the "concat" protocol to specify a list of file to concatenate as input.
            // Example: ffmpeg -i "concat:vts_01_1.vob|vts_01_2.vob|vts_01_3.vob"
            _ffmpegInput = "concat:" + _dvdTitleSet.vobFileNames().join('|');
        }
    }

    // Create the process object. It will automatically delete itself after completion.
    QtlBoundProcess* process = ffprobeProcess(1, ffprobeTimeout);

    // Here is another trick. Reading an encrypted DVD, or a DVD media in general,
    // is very slow. A typical bitrate is 21 Mb/s. Reading the default probe size
    // duration (200 s. of contents) takes approximately 1 minute. So, you cannot
    // see the audio and subtitle streams before that. This is not user-friendly.
    // To improve the user experience, we start shorter ffprobes in parallel. These
    // shorter ffprobes may not find all streams but will give information much
    // faster. If other streams are found later, they will appear later. This is not
    // perfect but better than waiting 1 minute.

    if (isOnDvd) {
        QtlBoundProcess* process2 = ffprobeProcess(QTL_FFPROBE_DVD_DIVISOR_2, ffprobeTimeout);
        QtlBoundProcess* process3 = ffprobeProcess(QTL_FFPROBE_DVD_DIVISOR_3, ffprobeTimeout);

        // Pipe DVD content into all process inputs at the same time.
        if (_pipeInput) {
            QList<QIODevice*> processInputs;
            processInputs << process->inputDevice() << process2->inputDevice() << process3->inputDevice();
            dataPull(this)->start(processInputs);
            _log->line(tr("Searching audio and subtitles tracks on DVD, please be patient..."), QColor(Qt::darkGreen));
        }
    }
    else {
        // Look for Closed Captions. Create a new instance of CC search.
        // DVD's do not have CC, they have DVD subtitles. So, skip this phase on DVD.
        QtlMovieClosedCaptionsSearch* cc = QtlMovieClosedCaptionsSearch::newInstance(fileName, _settings, _log, this);
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
}


//----------------------------------------------------------------------------
// Invoked when the ffprobe process completes.
//----------------------------------------------------------------------------

void QtlMovieInputFile::ffprobeTerminated(const QtlBoundProcessResult& result)
{
    // FFprobe terminated.
    _ffprobeCount--;

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
    if (_dvdTitleSet.isLoaded()) {

        // Merge the info which were previously collected in
        // the .IFO file with the stream info from ffprobe.
        QtlMediaStreamInfo::merge(_streams, _dvdTitleSet.streams());

        // Sort streams in the DVD order for user's convenience.
        std::sort(_streams.begin(), _streams.end(), QtsDvdTitleSet::lessThan);
    }

    // Is the file a transport stream, a pure subtitle file?
    const QString fileFormat(_ffInfo.value("format.format_name").toLower());
    _isTs = fileFormat == "mpegts";
    _isSubtitle = fileFormat == "srt" || fileFormat == "ass";

    // Is the file?
    _isTs = _ffInfo.value("format.format_name").toLower() == "mpegts";

    // If Teletext subtitles are detected, ffprobe does not report the Teletext page value
    // and we need to analyze the file. Sometimes, ffprobe does not even detect that the
    // subtitles are Teletext (unknown subtitle type) and we need to analyze the file as well.
    bool searchTeletext = false;
    if (_isTs) {
        foreach (const QtlMediaStreamInfoPtr& stream, _streams) {
            if (!stream.isNull() &&
                stream->streamType() == QtlMediaStreamInfo::Subtitle &&
                ((stream->subtitleType() == QtlMediaStreamInfo::SubTeletext && stream->teletextPage() < 0) || stream->subtitleType() == QtlMediaStreamInfo::SubOther)) {
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

void QtlMovieInputFile::foundTeletextSubtitles(QtlMediaStreamInfoPtr stream)
{
    _log->debug(tr("Found one Teletext subtitle stream, page %1").arg(stream->teletextPage()));

    // The first time a Teletext stream is found on a PID, there should be
    // one stream which was built from the ffprobe output with that PID
    // with unknown subtitle type. We need to remove it.
    if (stream->streamId() >= 0) {
        // We know the PID of the stream in the TS file (should be always the case).
        // Look for previous streams with the same PID.
        for (QtlMediaStreamInfoList::Iterator it = _streams.begin(); it != _streams.end(); ++it) {
            const QtlMediaStreamInfoPtr& s(*it);
            if (s->streamId() == stream->streamId()) {
                // Found a previous stream with same PID.
                // If the new stream's ffmpeg index is unknown, get it from the previous stream on same PID.
                if (stream->ffIndex() < 0) {
                    stream->setFFIndex(s->ffIndex());
                }
                // If the previous stream is unknown subtitle or teletext with unknown page, remove it
                // since we know have a better characterization of the stream.
                if (s->streamType() == QtlMediaStreamInfo::Subtitle &&
                    (s->subtitleType() == QtlMediaStreamInfo::SubOther || (s->subtitleType() == QtlMediaStreamInfo::SubTeletext && s->teletextPage() < 0))) {
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

void QtlMovieInputFile::foundClosedCaptions(QtlMediaStreamInfoPtr stream)
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
    if (_teletextSearch == 0) {
        if (_ffprobeCount <= 0 && _pipeInput) {
            _log->line(tr("Searching for audio and subtitles tracks completed"), QColor(Qt::darkGreen));
        }
        selectDefaultStreams(_settings->audienceLanguages());
        emit mediaInfoChanged();
    }
}


//----------------------------------------------------------------------------
// Get an instance of QtlDataPull to transfer the content of the input file.
//----------------------------------------------------------------------------

QtlDataPull* QtlMovieInputFile::dataPull(QObject* parent) const
{
    // If the input is not piped into FFmpeg, no need for a QtlDataPull.
    if (!_pipeInput) {
        return 0;
    }

    QtlDataPull* dp = 0;
    if (!_dvdTranscodeRawVob) {
        // We need to demultiplex one program chain from the VOB files.
        // We have a specific class for that.
        dp = new QtsDvdProgramChainDemux(_dvdTitleSet,
                                         _dvdProgramChain,     // the PGC to demux
                                         _dvdAngle,            // the angle to demux in the PGC
                                         1,                    // fallback PGC number
                                         QTS_DEFAULT_DVD_TRANSFER_SIZE,
                                         QtlDataPull::DEFAULT_MIN_BUFFER_SIZE,
                                         Qts::NavPacksRemoved, // Navigation packs are useless to FFmpeg
                                         _log,
                                         parent,
                                         _settings->dvdUseMaxSpeed());
    }
    else if (_dvdTitleSet.isEncrypted()) {
        // Read the raw VOB content from an encrypted DVD.
        // When ripping files, we skip bad sectors.
        dp = new QtsDvdDataPull(_dvdTitleSet.deviceName(),
                                _dvdTitleSet.vobStartSector(),
                                _dvdTitleSet.vobSectorCount(),
                                Qts::SkipBadSectors,
                                QTS_DEFAULT_DVD_TRANSFER_SIZE,
                                QtlDataPull::DEFAULT_MIN_BUFFER_SIZE,
                                _log,
                                parent,
                                _settings->dvdUseMaxSpeed());
    }
    else {
        // Read the raw VOB content from a non-encrypted file system.
        // Normally, this never happens because it is better handled
        // directly by FFmpeg on the file system.
        dp = new QtlFileDataPull(_dvdTitleSet.vobFileNames(),
                                 QtlFileDataPull::DEFAULT_TRANSFER_SIZE,
                                 QtlDataPull::DEFAULT_MIN_BUFFER_SIZE,
                                 _log,
                                 parent);
    }

    // Make sure the object deletes itself after completion.
    dp->setAutoDelete(true);
    return dp;
}


//----------------------------------------------------------------------------
// Get the DVD palette in RGB format.
//----------------------------------------------------------------------------

QtlByteBlock QtlMovieInputFile::palette() const
{
    return _dvdPgc.isNull() ? QtlByteBlock() : _dvdPgc->rgbPalette();
}


//----------------------------------------------------------------------------
// Get the number of streams in the file matching a given type.
//----------------------------------------------------------------------------

int QtlMovieInputFile::streamCount(QtlMediaStreamInfo::StreamType streamType) const
{
    int count = 0;
    foreach (const QtlMediaStreamInfoPtr& s, _streams) {
        if (!s.isNull() && s->streamType() == streamType) {
            count++;
        }
    }
    return count;
}


//----------------------------------------------------------------------------
// Get the information about the first stream in the file matching a given type.
//----------------------------------------------------------------------------

QtlMediaStreamInfoPtr QtlMovieInputFile::firstStream(QtlMediaStreamInfo::StreamType streamType) const
{
    foreach (const QtlMediaStreamInfoPtr& s, _streams) {
        if (!s.isNull() && s->streamType() == streamType) {
            return s;
        }
    }
    return 0;
}


//----------------------------------------------------------------------------
// Get the information about a stream in input file.
//----------------------------------------------------------------------------

QtlMediaStreamInfoPtr QtlMovieInputFile::streamInfo(int streamIndex) const
{
    return streamIndex >= 0 && streamIndex < _streams.size() ? _streams[streamIndex] : 0;
}


//----------------------------------------------------------------------------
// Try to evaluate the duration of the media file in seconds.
//----------------------------------------------------------------------------

float QtlMovieInputFile::durationInSeconds() const
{
    // If this is a DVD title set, get the title set duration.
    // This information is read from the IFO file, not evaluated by ffprobe
    // but is usually more reliable. First, if the input file is on an encrypted DVD,
    // ffprobe only reads its standard input pipe, it has no global view on the file
    // and cannot evaluate its duration. Second, even on ripped VOB files, ffprobe only
    // reads the last time stamps of the last VOB. But when the VOB files come from
    // a multi-layer DVD, the time stamps are reset at the beginning of each layer.
    // So, the last time stamp of the last file is the duration of the last layer,
    // not the duration of the movie.
    if (_dvdTitleSet.isLoaded() && !_dvdPgc.isNull()) {
        int dvdDuration = _dvdPgc->durationInSeconds();
        if (dvdDuration > 0) {
            return float(dvdDuration);
        }
    }

    // Try duration of file.
    float duration = _ffInfo.floatValue("format.duration");

    // If not found, try all streams duration until one is found.
    const int count = _ffInfo.intValue("format.nb_streams");
    for (int si = 0; qtlFloatZero(duration) && si < count; ++si) {
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

    const QtlMediaStreamInfoPtr audioStream(firstStream(QtlMediaStreamInfo::Audio));

    return  streamCount(QtlMediaStreamInfo::Video) == 1 &&
            streamCount(QtlMediaStreamInfo::Audio) == 1 &&
            streamCount(QtlMediaStreamInfo::Subtitle) == 0 &&
            externalSubtitleFileName().isEmpty() &&
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
    const QtlMediaStreamInfoPtr videoStream(selectedVideoStreamInfo());

    // Compliant if:
    // - Selected video stream exists.
    // - Forced display aspect ratio not specified or equal to original one.
    // - Same display aspect ratio, video size and video format as DVD.

    return  !videoStream.isNull() &&
            qtlFloatEqual(videoStream->displayAspectRatio(true), videoStream->displayAspectRatio(false)) &&
            qtlFloatEqual(videoStream->displayAspectRatio(), QTL_DVD_DAR) &&
            videoStream->width() == _settings->dvdVideoWidth() &&
            videoStream->height() == _settings->dvdVideoHeight() &&
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
    int defaultVideoStreamIndex = -1;
    int defaultAudioStreamIndex = -1;
    int defaultSubtitleStreamIndex = -1;

    int firstVideo = -1;             // First video stream index.
    int highestFrameSize = -1;       // Largest video frame size (width x height).
    int firstAudio = -1;             // First audio stream index.
    int originalAudio = -1;          // First original audio stream index (and not impaired).
    int notAudienceAudio = -1;       // First audio stream index not in intended audience languages (and not impaired).
    int notImpairedAudio = -1;       // First audio stream index not for hearing/visual impaired.
    int firstSubtitle = -1;          // First subtitles stream.
    int firstAudienceSubtitle = -1;  // First subtitles in intended audience languages.
    int completeSubtitle = -1;       // First complete subtitles in intended audience languages.
    int forcedSubtitle = -1;         // First forced subtitles in intended audience languages.
    int impairedSubtitle = -1;       // First subtitles for impaired in intended audience languages.

    // Loop on all streams.
    const int streamMax = streamCount();
    for (int streamIndex = 0; streamIndex < streamMax; streamIndex++) {

        // Stream description.
        const QtlMediaStreamInfoPtr& stream(_streams[streamIndex]);

        // Is this stream in the intended audience languages?
        const bool inAudienceLanguages = audienceLanguages.contains(stream->language(), Qt::CaseInsensitive);

        // Type-specific processing.
        switch (stream->streamType()) {

            case QtlMediaStreamInfo::Video: {
                if (firstVideo < 0) {
                    // Found first video track.
                    firstVideo = streamIndex;
                }
                // Compute video frame size (width x height). Zero if width or height undefined.
                const int frameSize = stream->width() * stream->height();
                if (frameSize > highestFrameSize) {
                    // Largest frame size so far or first video stream, keep it.
                    defaultVideoStreamIndex = streamIndex;
                    highestFrameSize = frameSize;
                }
                break;
            }

            case QtlMediaStreamInfo::Audio: {
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

            case QtlMediaStreamInfo::Subtitle: {
                if (firstSubtitle < 0) {
                    // Found first subtitles.
                    firstSubtitle = streamIndex;
                }
                if (inAudienceLanguages) {
                    // Keep only subtitles for intended audience.
                    if (firstAudienceSubtitle < 0) {
                        // Found first subtitles for audience language.
                        firstAudienceSubtitle = streamIndex;
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

            default: {
                // Other stream, ignore it.
                break;
            }
        }
    }

    // Select the default audio stream.
    if (!_settings->selectOriginalAudio()) {
        // Do not look for original audio track, just use first audio stream.
        defaultAudioStreamIndex = firstAudio;
    }
    else if (originalAudio >= 0) {
        // Use original audio stream.
        defaultAudioStreamIndex = originalAudio;
    }
    else if (notAudienceAudio >= 0) {
        // Use first audio stream index not in intended audience languages.
        defaultAudioStreamIndex = notAudienceAudio;
    }
    else if (notImpairedAudio >= 0) {
        // Use first audio stream index not for hearing/visual impaired.
        defaultAudioStreamIndex = notImpairedAudio;
    }
    else {
        // Use first audio stream index (if any).
        defaultAudioStreamIndex = firstAudio;
    }

    // Check if the selected audio stream is in a language for the intended audience.
    // This will impact the selection of the subtitle stream.
    const bool audioInAudienceLanguages =
            defaultAudioStreamIndex >= 0 &&
            audienceLanguages.contains(_streams[defaultAudioStreamIndex]->language(), Qt::CaseInsensitive);

    // Select the default subtitle stream.
    if (audioInAudienceLanguages) {
        // Use only "forced" subtitles (if any) when audio is for the intended audience.
        defaultSubtitleStreamIndex = forcedSubtitle;
    }
    else if (!_settings->selectTargetSubtitles()) {
        // Do not look for subtitles.
        defaultSubtitleStreamIndex = -1;
    }
    else if (completeSubtitle >= 0) {
        // Use complete subtitles.
        defaultSubtitleStreamIndex = completeSubtitle;
    }
    else if (impairedSubtitle >= 0) {
        // Use subtitles for hearing/visual impaired.
        defaultSubtitleStreamIndex = impairedSubtitle;
    }
    else {
        // Use first subtitles stream (if any).
        defaultSubtitleStreamIndex = firstAudienceSubtitle;
    }

    // If the input has no audio and no video, automatically select a subtitle stream.
    if (firstVideo < 0 && firstAudio < 0 && defaultSubtitleStreamIndex < 0) {
        // Use first subtitles stream (if any).
        defaultSubtitleStreamIndex = firstAudienceSubtitle < 0 ? firstSubtitle : firstAudienceSubtitle;
    }

    // Now set the default streams as selected if not already explicitly selected.
    if (!_selectedVideoExplicit) {
        _selectedVideoStreamIndex = defaultVideoStreamIndex;
    }
    if (!_selectedAudioExplicit) {
        _selectedAudioStreamIndex = defaultAudioStreamIndex;
    }
    if (!_selectedSubtitleExplicit && _externalSubtitleFileName.isEmpty()) {
        _selectedSubtitleStreamIndex = defaultSubtitleStreamIndex;
    }
}


//----------------------------------------------------------------------------
// Get or set the various stream index to transcode.
//----------------------------------------------------------------------------

int QtlMovieInputFile::selectedVideoStreamIndex() const
{
    return _selectedVideoStreamIndex < streamCount() ? _selectedVideoStreamIndex : -1;
}

void QtlMovieInputFile::setSelectedVideoStreamIndex(int index)
{
    _selectedVideoStreamIndex =
            (index >= 0 &&
             index < streamCount() &&
             !_streams[index].isNull() &&
             _streams[index]->streamType() == QtlMediaStreamInfo::Video) ?
            index : -1;
    _selectedVideoExplicit = _selectedVideoStreamIndex >= 0;
}

int QtlMovieInputFile::selectedAudioStreamIndex() const
{
    return _selectedAudioStreamIndex < streamCount() ? _selectedAudioStreamIndex : -1;
}

void QtlMovieInputFile::setSelectedAudioStreamIndex(int index)
{
    _selectedAudioStreamIndex =
            (index >= 0 &&
             index < streamCount() &&
             !_streams[index].isNull() &&
             _streams[index]->streamType() == QtlMediaStreamInfo::Audio) ?
            index : -1;
    _selectedAudioExplicit = _selectedAudioStreamIndex >= 0;
}

int QtlMovieInputFile::selectedSubtitleStreamIndex() const
{
    return _selectedSubtitleStreamIndex < streamCount() ? _selectedSubtitleStreamIndex : -1;
}

void QtlMovieInputFile::setSelectedSubtitleStreamIndex(int index)
{
    _selectedSubtitleStreamIndex =
            (index >= 0 &&
             index < streamCount() &&
             !_streams[index].isNull() &&
             _streams[index]->streamType() == QtlMediaStreamInfo::Subtitle) ?
            index : -1;
    _selectedSubtitleExplicit = _selectedSubtitleStreamIndex >= 0;
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
