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
// Define the class QtlMovieJob.
//
//----------------------------------------------------------------------------

#include "QtlMovieJob.h"
#include "QtlMovie.h"
#include "QtlMovieDeleteAction.h"
#include "QtlMovieFFmpeg.h"
#include "QtlMovieFFmpegProcess.h"
#include "QtlMovieDvdAuthorProcess.h"
#include "QtlMovieMkisofsProcess.h"
#include "QtlMovieGrowisofsProcess.h"
#include "QtlStringList.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieJob::QtlMovieJob(const QtlMovieInputFile* inputFile,
                         const QtlMovieOutputFile* outputFile,
                         const QtlMovieSettings* settings,
                         QtlLogger* log,
                         QObject *parent) :
    QtlMovieAction(settings, log, parent),
    _inputFile(inputFile),
    _outputFile(outputFile),
    _outSeconds(0),
    _tempDir(),
    _actionList()
{
    Q_ASSERT(inputFile != 0);
    Q_ASSERT(outputFile != 0);
}


//----------------------------------------------------------------------------
// Start the transcoding job.
//----------------------------------------------------------------------------

bool QtlMovieJob::start()
{
    // Do not start twice.
    if (isStarted()) {
        return false;
    }

    // Cleanup action list.
    while (!_actionList.isEmpty()) {
        delete _actionList.takeFirst();
    }

    // Check that input/output files are specified.
    if (_inputFile == 0 || !_inputFile->isSet()) {
        return errorFalse(tr("No input file selected."));
    }
    if (_outputFile == 0 || !_outputFile->isSet()) {
        return errorFalse(tr("No output file selected."));
    }

    // If the output file already exists...
    QFile out(_outputFile->fileName());
    if (out.exists()) {
        // Ask for confirmation to overwrite it.
        if (!qtlConfirm(this, tr("File %1 already exists.\nOverwrite it?").arg(_outputFile->fileName()))) {
            // Don't overwrite, give up.
            return false;
        }
        // Delete the previous output file to avoid using it by mistake if the conversion fails.
        if (!out.remove()) {
            // Failed to delete it. Continue anyway, will be overwritten by converter.
            log()->line(tr("Failed to delete %1").arg(_outputFile->fileName()));
        }
    }

    // If intermediate files needs to be created, we will use a temporary directory
    // in the same directory as the output file.
    _tempDir = QtlFile::absoluteNativeFilePath(_outputFile->fileName() + ".qtlmovie.temp");

    // Build the list of actions to execute.
    if (!buildScenario()) {
        return false;
    }

    // Create the temporary directory.
    QDir dir;
    if (!dir.mkpath(_tempDir)) {
        return errorFalse(tr("Cannot create temporary directory %1").arg(_tempDir));
    }

    // Notify of the job start.
    if (!QtlMovieAction::start()) {
        return false;
    }

    // Start the first action.
    if (!startNextAction()) {
        emitCompleted(false, tr("Error starting process"));
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
// Abort the transcoding job.
//----------------------------------------------------------------------------

void QtlMovieJob::abort()
{
    // Is there anything to abort?
    if (!isStarted() || _actionList.isEmpty()) {
        return;
    }

    // Cleanup all future actions.
    while (_actionList.size() > 1 || !_actionList.first()->isStarted()) {
        delete _actionList.takeLast();
    }

    if (_actionList.isEmpty()) {
        // No action will notify the completion, we need to do it now.
        emitCompleted(false, "Transcoding aborted");
    }
    else {
        // Kill the current process.
        _actionList.first()->abort();
        // The completion of the aborted process will trigger the completion of the job.
    }
}


//----------------------------------------------------------------------------
// Emit the completed() signal.
//----------------------------------------------------------------------------

void QtlMovieJob::emitCompleted(bool success, const QString& message)
{
    // Cleanup all remaining actions (in case of error).
    while (!_actionList.isEmpty()) {
        _actionList.takeFirst()->deleteLater();
    }

    // Delete temporary directory and its content.
    // If the option "keep intermediate files" is present, we delete it only if empty.
    QDir dir(_tempDir);
    const bool exists = dir.exists();
    const bool empty = !exists || dir.entryList().isEmpty();
    if (exists && (empty || !settings()->keepIntermediateFiles()) && !dir.removeRecursively()) {
        log()->line(tr("Error deleting %1").arg(_tempDir));
    }

    // Notify superclass.
    QtlMovieAction::emitCompleted(success, message);
}


//----------------------------------------------------------------------------
// Start the next action in the list.
//----------------------------------------------------------------------------

bool QtlMovieJob::startNextAction()
{
    // Check that there is something to start.
    if (_actionList.isEmpty()) {
        log()->line(tr("Internal error, no more process to start"));
        return false;
    }

    // Get notifications from next process.
    QtlMovieAction* next = _actionList.first();
    connect(next, SIGNAL(progress(QString,int,int,int,int)), this, SIGNAL(progress(QString,int,int,int,int)));
    connect(next, SIGNAL(completed(bool)), this, SLOT(actionCompleted(bool)));

    // Start the next process.
    if (!next->start()) {
        log()->line(tr("Failed to start process."));
        actionCompleted(false);
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
// Invoked each time a process completes.
//----------------------------------------------------------------------------

void QtlMovieJob::actionCompleted(bool success)
{
    // Cleanup terminated action.
    Q_ASSERT(!_actionList.isEmpty());
    _actionList.takeFirst()->deleteLater();

    // Process the next step.
    if (success) {
        if (_actionList.isEmpty()) {
            // Job successfully completed.
            emitCompleted(true);
            return;
        }
        else if (startNextAction()) {
            // Next action started.
            return;
        }
    }

    // Notify failure.
    emitCompleted(false, "Transcoding failed, see messages above.");
}


//----------------------------------------------------------------------------
// Check if it is possible to transcode an input file (with its streams
// selections) to an output type.
//----------------------------------------------------------------------------

bool QtlMovieJob::canTranscode(const QtlMovieInputFile* inputFile, QtlMovieOutputFile::OutputType outputType)
{
    // No file, nothing possible.
    if (inputFile == 0) {
        return false;
    }

    // ISO image can only be burnt.
    if (inputFile->isIsoImage()) {
        return outputType == QtlMovieOutputFile::DvdBurn;
    }

    // Subtitle type.
    const QtlMovieStreamInfoPtr subtitleInfo(inputFile->selectedSubtitleStreamInfo());
    const QtlMovieStreamInfo::SubtitleType subtitleType(subtitleInfo.isNull() ? QtlMovieStreamInfo::SubNone : subtitleInfo->subtitleType());

    // Decide what is supported.
    switch (outputType) {
    case QtlMovieOutputFile::DvdFile:
    case QtlMovieOutputFile::DvdImage:
    case QtlMovieOutputFile::DvdBurn:
    case QtlMovieOutputFile::Ipad:
        // Require any audio and video type.
        // Require a supported or no subtitle type.
        return inputFile->selectedAudioStreamIndex() >= 0 &&
                inputFile->selectedVideoStreamIndex() >= 0 &&
                subtitleType != QtlMovieStreamInfo::SubOther;

    case QtlMovieOutputFile::SubRip:
        // Require a subtitle type which can be exported as SubRip.
        return subtitleType == QtlMovieStreamInfo::SubRip ||
                subtitleType == QtlMovieStreamInfo::SubSsa ||
                subtitleType == QtlMovieStreamInfo::SubAss ||
                subtitleType == QtlMovieStreamInfo::SubTeletext;

    case QtlMovieOutputFile::None:
        // It is always possible to do nothing.
        return true;

    default:
        // Unsupported output type.
        return false;
    }
}


//----------------------------------------------------------------------------
// Check if it is possible to insert subtitles from the specified file.
//----------------------------------------------------------------------------

bool QtlMovieJob::canInsertSubtitles(const QString& subtitleFileName)
{
    // We can insert only SRT/SSA/ASS subtitles from an external file
    // and the file must exist.
    switch (QtlMovieStreamInfo::subtitleType(subtitleFileName, true)) {
    case QtlMovieStreamInfo::SubAss:
    case QtlMovieStreamInfo::SubSsa:
    case QtlMovieStreamInfo::SubRip:
        return true;
    default:
        return false;
    }
}


//----------------------------------------------------------------------------
// Build the list of processes to execute to perform the job.
//----------------------------------------------------------------------------

bool QtlMovieJob::buildScenario()
{
    // Duration of the input file in seconds.
    const int maxSeconds = settings()->transcodeComplete() ? 0 : settings()->transcodeSeconds();
    const int totalSeconds = qRound(_inputFile->durationInSeconds());
    _outSeconds = maxSeconds > 0 && maxSeconds < totalSeconds ? maxSeconds : totalSeconds;

    // Main output file type.
    const QtlMovieOutputFile::OutputType outputType = _outputFile->outputType();

    // If the operation is simply a DVD burning, there is nothing to transcode.
    if (_inputFile->isIsoImage() && outputType == QtlMovieOutputFile::DvdBurn) {
        return addBurnDvd(_inputFile->fileName(), settings()->dvdBurner());
    }

    // The input file which will be used for media transcoding.
    // Initially, this is the input file but we may insert intermediate steps later.
    const QtlMovieInputFile* inputForTranscoding = _inputFile;

    // External subtitle file.
    if (!_inputFile->externalSubtitleFileName().isEmpty()) {
        switch (QtlMovieStreamInfo::subtitleType(_inputFile->externalSubtitleFileName(), true)) {
        case QtlMovieStreamInfo::SubAss:
        case QtlMovieStreamInfo::SubSsa:
        case QtlMovieStreamInfo::SubRip:
            // Supported external subtitle file type.
            break;
        default:
            return errorFalse(tr("Non-existent or unsupported external subtitle file."));
        }
    }

    // Subtitle stream in input file.
    const QtlMovieStreamInfoPtr subtitleStream(_inputFile->selectedSubtitleStreamInfo());

    // Do we need to add a first pass to extract subtitles? Yes if:
    // - A subtitle stream is selected in input file.
    // - The final output contains video (into which the subtitles must be burnt).
    // - Not DVD or DVB subtitles. These are directly processed by FFmpeg from the input file.
    //   No need to have a separate first pass to extract them.
    if (!subtitleStream.isNull() &&
        subtitleStream->subtitleType() != QtlMovieStreamInfo::SubDvd &&
        subtitleStream->subtitleType() != QtlMovieStreamInfo::SubDvb &&
        QtlMovieOutputFile::containsVideo(outputType)) {

        // Extract subtitles in an intermediate file.
        // Do not provide a suffix, addExtractSubtitle() will update it for us.
        QString subtitleFile(_tempDir + QDir::separator() + "subtitles");

        // Create a process for subtitle extraction.
        if (!addExtractSubtitle(_inputFile, subtitleFile)) {
            return false;
        }

        // The input file for transcoding has different subtitle characteristics
        // (external subtitle file instead of internal subtitle stream).
        // The new input file object will be deleted with the job instance.
        QtlMovieInputFile* next = new QtlMovieInputFile(*_inputFile, this);
        next->setExternalSubtitleFileName(subtitleFile);
        inputForTranscoding = next;
    }

    // Build the ffmpeg command for the main process.
    bool success = false;
    switch (outputType) {
    case QtlMovieOutputFile::DvdFile: {
        success = addTranscodeToDvdFile(inputForTranscoding, _outputFile->fileName());
        break;
    }
    case QtlMovieOutputFile::DvdImage: {
        success = addTranscodeToDvdIsoImage(inputForTranscoding, _outputFile->fileName());
        break;
    }
    case QtlMovieOutputFile::DvdBurn: {
        success = addTranscodeToDvdIsoImage(inputForTranscoding, _outputFile->fileName()) &&
                addBurnDvd(_outputFile->fileName(), settings()->dvdBurner());
        break;
    }
    case QtlMovieOutputFile::Ipad: {
        success = addTranscodeToIpad(inputForTranscoding, _outputFile->fileName());
        break;
    }
    case QtlMovieOutputFile::SubRip: {
        QString subtitleFile(_outputFile->fileName());
        success = addExtractSubtitle(inputForTranscoding, subtitleFile);
        break;
    }
    default:
        return errorFalse(tr("Unexpected output type %1").arg(outputType));
    }

    // If more than one process is defined, give them a number.
    if (success) {
        const int actionCount = _actionList.size();
        if (actionCount > 1) {
            int index = 0;
            foreach (QtlMovieAction* action, _actionList) {
                action->setDescription(QObject::tr("Step %1/%2 - %3").arg(++index).arg(actionCount).arg(action->description()));
            }
        }
    }
    return success;
}


//----------------------------------------------------------------------------
// Add an FFmpeg process in the process list.
//----------------------------------------------------------------------------

bool QtlMovieJob::addFFmpeg(const QString& description, const QStringList ffmpegArguments)
{
    if (ffmpegArguments.isEmpty()) {
        // Error building argument list.
        return false;
    }
    else {
        QtlMovieFFmpegProcess* process = new QtlMovieFFmpegProcess(ffmpegArguments, _outSeconds, _tempDir, settings(), log(), this);
        process->setDescription(description);
        _actionList.append(process);
        return true;
    }
}


//----------------------------------------------------------------------------
// Compute an FFmpeg video filter for burning subtitles from an external file
// into video.
//----------------------------------------------------------------------------

bool QtlMovieJob::addSubtitleFileVideoFilter(QString& videoFilters, int widthIn, int heightIn, const QString& subtitleFileName)
{
    // If no subtitle file is specified, no error.
    if (subtitleFileName.isEmpty()) {
        return true;
    }

    // Subtitle file type.
    const QtlMovieStreamInfo::SubtitleType subType(QtlMovieStreamInfo::subtitleType(subtitleFileName));
    switch (subType) {

    case QtlMovieStreamInfo::SubRip:
        return addTextSubtitleFileVideoFilter(videoFilters, "subtitles", widthIn, heightIn, subtitleFileName);

    case QtlMovieStreamInfo::SubAss:
    case QtlMovieStreamInfo::SubSsa:
        return addTextSubtitleFileVideoFilter(videoFilters, "ass", widthIn, heightIn, subtitleFileName);

    case QtlMovieStreamInfo::SubDvd:
    case QtlMovieStreamInfo::SubDvb:
    case QtlMovieStreamInfo::SubTeletext:
    case QtlMovieStreamInfo::SubOther:
    case QtlMovieStreamInfo::SubNone:
    default:
        return errorFalse(tr("Unsupported subtitle type"));
    }
}


//----------------------------------------------------------------------------
// Compute an FFmpeg video filter for burning text subtitles
// from an external file into video.
//----------------------------------------------------------------------------

bool QtlMovieJob::addTextSubtitleFileVideoFilter(QString& videoFilters, const QString& filterName, int widthIn, int heightIn, const QString& subtitleFileName)
{
    // Build an "escaped" version of the file name.
    // All :,;'\ are escaped with a \.
    QString file(subtitleFileName);
    file.replace(QRegExp("\\\\"), "\\\\");
    file.replace(QRegExp("([:,;'])"), "\\\\1");

    // Subtitles filter.
    if (!videoFilters.isEmpty()) {
        videoFilters.append(",");
    }
    videoFilters.append(filterName);
    videoFilters.append("=filename='");
    videoFilters.append(file);
    videoFilters.append("'");

    // If the original video size is known, add it as a hint.
    if (settings()->srtUseVideoSizeHint() && widthIn > 0 && heightIn > 0) {
        videoFilters.append(QStringLiteral(":original_size=%1x%2").arg(widthIn).arg(heightIn));
    }
    return true;
}


//----------------------------------------------------------------------------
// Compute an FFmpeg video filter for burning subtitles from a stream of the
// input file into video.
//----------------------------------------------------------------------------

bool QtlMovieJob::addSubtitleStreamVideoFilter(QString& videoFilters, const QtlMovieInputFile* inputFile, int widthOut, int heightOut)
{
    // Selected subtitle and video streams.
    const QtlMovieStreamInfoPtr subtitleStream(inputFile->selectedSubtitleStreamInfo());
    const QtlMovieStreamInfoPtr videoStream(inputFile->selectedVideoStreamInfo());

    // If no video or subtitle stream specified, nothing to do.
    if (subtitleStream.isNull() || videoStream.isNull()) {
        return true; // nothing to do but not an error.
    }

    // Subtitle type.
    switch (subtitleStream->subtitleType()) {

    case QtlMovieStreamInfo::SubDvb:
    case QtlMovieStreamInfo::SubDvd: {
        // Bitmap subtitles are picture frames with a transparent background which
        // shall be displayed on top of the main video frames.
        // We may need to create a complex filter with several branches.
        // One branch resizes the subtitle frames, the other overlays the subtitles on video.
        // First, setup the origin of the video branch.
        QString videoBranchName;
        if (videoFilters.isEmpty()) {
            // No previous filter, use the video stream specification.
            videoBranchName = videoStream->ffSpecifier();
        }
        else {
            // A previous filter exists. Setup a branch name to the previous video branch.
            videoBranchName="presub";
            videoFilters.append("[presub];");
        }

        // If required, setup a branch to resize the subtitle frame.
        // Note: Bitmap subtitles are picture frames with a transparent background which
        // shall be displayed on top of the main video frames. The size of the subtitle
        // frames are reported by versions of ffprobe after 2013/09/06.
        QString subtitleBranchName;
        if (subtitleStream->width() == widthOut && subtitleStream->height() == heightOut) {
            // Video and subtitle frames have the same size.
            // Simply use the subtitle stream specification as name.
            subtitleBranchName = subtitleStream->ffSpecifier();
        }
        else {
            // Video and subtitle frames do not have the same size or the size of the
            // subtitle frames is not known (the size of the subtitle frames is reported
            // by versions of ffprobe after 2013/09/06).
            // Add a branch to resize the subtitle frames.
            // Note that we simply resize the subtitle frame without attempting to
            // preserve its aspect ratio. When the video and subtitle frames do not
            // have the same aspect ratio, this may distort the subtitle characters.
            // However, some tests have shown that preserving the subtitle aspect ratio
            // may lead to big subtitles which eat a large part of the video frame.
            subtitleBranchName = "sub";
            videoFilters.append(QStringLiteral("[%1]scale=width=%2:height=%3[sub];")
                                .arg(subtitleStream->ffSpecifier())
                                .arg(widthOut)
                                .arg(heightOut));
        }

        // Setup the branch which overlays the subtitle frames over the video frames.
        videoFilters.append(QStringLiteral("[%1][%2]overlay").arg(videoBranchName).arg(subtitleBranchName));
        return true;
    }
    case QtlMovieStreamInfo::SubRip:
    case QtlMovieStreamInfo::SubAss:
    case QtlMovieStreamInfo::SubSsa:
    case QtlMovieStreamInfo::SubTeletext:
        // These subtitles were extracted from the file in a previous pass. Ignore them now.
        return true;

    case QtlMovieStreamInfo::SubOther:
    case QtlMovieStreamInfo::SubNone:
    default:
        return errorFalse(tr("Unsupported subtitle type"));
    }
}


//----------------------------------------------------------------------------
// Build a DVD Author option list to create chapters.
//----------------------------------------------------------------------------

QStringList QtlMovieJob::chapterOptions(int totalSeconds, int chapterSeconds)
{
    // Return nothing if durations are unknown or movie shorter than a chapter.
    if (totalSeconds <= 0 || chapterSeconds <= 0 || totalSeconds < chapterSeconds) {
        return QStringList();
    }

    // Build the list of chapters. First chapter starts at the beginning.
    QString list("00:00:00");
    for (int start = chapterSeconds; start <= totalSeconds; start += chapterSeconds) {
        list.append(QStringLiteral(",%1:%2:%3")
                    .arg(start / 3600, 2, 10, QLatin1Char('0'))
                    .arg((start / 60) % 60, 2, 10, QLatin1Char('0'))
                    .arg(start % 60, 2, 10, QLatin1Char('0')));
    }

    // Return the argument list.
    return QtlStringList("-c", list);
}


//----------------------------------------------------------------------------
// Add the processes for simple audio/video transcoding to DVD.
//----------------------------------------------------------------------------

bool QtlMovieJob::addTranscodeAudioVideoToDvd(const QtlMovieInputFile* inputFile, const QString& outputFileName)
{
    // Video and audio stream to transcode.
    const QtlMovieStreamInfoPtr videoStream(inputFile->selectedVideoStreamInfo());
    const QtlMovieStreamInfoPtr audioStream(inputFile->selectedAudioStreamInfo());

    // Start FFmpeg argument lists. There are two argument lists, one for each encoding pass.
    QStringList pass1Args(QtlMovieFFmpeg::inputArguments(settings(), inputFile->ffmpegInputFileSpecification(), inputFile->palette()));
    QStringList pass2Args(pass1Args);

    // Process selected video stream.
    if (!videoStream.isNull()) {
        // Compute video bitrate. Start with value from settings.
        qint64 bitrate = settings()->dvdVideoBitRate();
        if (_outSeconds> 0) {
            // File duration is known.
            // Compute max media (audio + video) size in bytes.
            const qint64 maxMediaSize = (QTL_DVD_ISO_MAX_SIZE * 100) / (100 + QTL_DVD_ISO_OVERHEAD_PERCENT);
            // Compute max allowed video size in bytes.
            const qint64 maxVideoSize = maxMediaSize - (qint64(_outSeconds) * QTL_DVD_AUDIO_BITRATE / 8);
            if (maxVideoSize > 0) {
                // No error so far, compute max video bitrate in bits / second.
                const qint64 maxBitrate = (maxVideoSize * 8) / _outSeconds;
                // Limit the video bitrate to that maximum.
                if (bitrate > maxBitrate) {
                    bitrate = maxBitrate;
                }
                log()->line(tr("Max video bitrate to fit in DVD: %1 b/s, using %2 b/s").arg(maxBitrate).arg(bitrate));
            }
        }

        // Get the characteristics of the input video stream.
        const int width = videoStream->width();
        const int height = videoStream->height();
        const float dar = videoStream->displayAspectRatio();

        // We always adjust the size of the video. We must know the input video size.
        if (width <= 0 || height <= 0) {
            return errorFalse(tr("Unknown video size or aspect ratio, cannot create suitable video for DVD"));
        }

        // Video filter: DVD output is always 720x576, 16:9.
        QString videoFilters;
        QtlMovieFFmpeg::addResizeVideoFilter(videoFilters, width, height, dar, QTL_DVD_VIDEO_WIDTH, QTL_DVD_VIDEO_HEIGHT, QTL_DVD_DAR);

        // Add subtitles processing to video filter.
        if (!addSubtitleFileVideoFilter(videoFilters, width, height, inputFile->externalSubtitleFileName()) ||
            !addSubtitleStreamVideoFilter(videoFilters, inputFile, QTL_DVD_VIDEO_WIDTH, QTL_DVD_VIDEO_HEIGHT)) {
            return false;
        }

        // Common video options.
        QStringList args;
        args << "-map" << videoStream->ffSpecifier()
             << "-codec:v" << "mpeg2video"
             << "-target" << "pal-dvd"  // Select presets for DVD (PAL format).
             << QtlMovieFFmpeg::frameRateOptions(settings(), QtlMovieOutputFile::DvdFile)
             << "-b:v" << QString::number(bitrate)
             << "-aspect" << QTL_DVD_DAR_FFMPEG
             << QtlMovieFFmpeg::videoFilterOptions(videoFilters)
             << "-threads" << "auto"
             << "-passlogfile" << (_tempDir + QDir::separator() + "fflog");

        // Pass 1 and 2 arguments.
        pass1Args << args << "-pass" << "1";
        pass2Args << args << "-pass" << "2";
    }

    // Process selected audio stream.
    if (!audioStream.isNull()) {
        // Common audio options.
        QStringList args;
        args << "-map" << audioStream->ffSpecifier()
             << "-codec:a" << "mp2"  // MPEG-2 audio layer 2
             << "-ac" << "2"         // Remix to 2 channels (stereo)
             << "-ar" << QString::number(QTL_DVD_AUDIO_SAMPLING)
             << "-b:a" << QString::number(QTL_DVD_AUDIO_BITRATE);

        // Pass 1 and 2 arguments.
        pass1Args << args;
        pass2Args << args;
    }

    // The output of the first pass is useless and sent to the null device (only the log is useful).
    pass1Args << QtlMovieFFmpeg::outputArguments(settings(), QTL_NULL_DEVICE, "dvd");
    pass2Args << QtlMovieFFmpeg::outputArguments(settings(), outputFileName, "dvd");

    // Add 2 process for video transcoding.
    return addFFmpeg(tr("Transcoding, pass 1"), pass1Args) && addFFmpeg(tr("Transcoding, pass 2"), pass2Args);
}


//----------------------------------------------------------------------------
// Add the processes for transcoding to a DVD media file.
//----------------------------------------------------------------------------

bool QtlMovieJob::addTranscodeToDvdFile(const QtlMovieInputFile* inputFile, const QString& outputFileName)
{
    // Check if the input file looks like one of our DVD-compliant MPEG files.
    const bool dvdCompliant = inputFile->isDvdCompliant();

    // Video and audio stream to transcode.
    const QtlMovieStreamInfoPtr videoStream(inputFile->selectedVideoStreamInfo());
    const QtlMovieStreamInfoPtr audioStream(inputFile->selectedAudioStreamInfo());

    // Explicit remux of audio and video if required in settings and both audio and video are present.
    // If the file is already DVD-compliant, always perform remux.
    const bool remuxAfterTranscode = dvdCompliant ||
            (settings()->dvdRemuxAfterTranscode() && !videoStream.isNull() && !audioStream.isNull());

    // Output file for transcoding operation.
    QString transcodeOutput;
    if (remuxAfterTranscode) {
        // Transcode to an intermediate file in the temporary directory.
        transcodeOutput = _tempDir + QDir::separator() + "movie-file.mpg";
    }
    else {
        // Transcode to the final file.
        transcodeOutput = outputFileName;
    }

    // Perform the 2-pass audio/video encoding, except for DVD-compliant files.
    if (dvdCompliant) {
        log()->line(tr("Found DVD-compliant audio/video encoding, will not re-encode"));
    }
    else if (!addTranscodeAudioVideoToDvd(inputFile, transcodeOutput)) {
        return false;
    }

    // If no remux is required after transcode, we are done.
    if (!remuxAfterTranscode) {
        return true;
    }

    // Input file for remux operation.
    const QString remuxInput(dvdCompliant ? inputFile->fileName() : transcodeOutput);

    // Remux audio and video in a dvdauthor-compatible way.
    // At that point, we know that there are one audio and one video stream.
    // Add a process to extract audio in a separate file.
    const QString audioFile(_tempDir + QDir::separator() + "audio.mp2");
    QStringList args(QtlMovieFFmpeg::inputArguments(settings()));
    args << "-i" << remuxInput
         << "-vn"  // Drop video stream
         << "-map" << "0:a"
         << "-codec:a" << "copy"
         << QtlMovieFFmpeg::outputArguments(settings(), audioFile, "mp2");

    // Add the process for audio extraction.
    // Note that this never fails since the argument list is not empty.
    addFFmpeg(tr("Extracting audio"), args);

    // Perform the audio/video remux into the final file.
    args.clear();
    args << QtlMovieFFmpeg::inputArguments(settings())
         << "-i" << remuxInput  // Video (+audio) file
         << "-i" << audioFile   // Audio file
         << "-map" << "0:v"
         << "-codec:v" << "copy"
         << "-map" << "1:a"
         << "-codec:a" << "copy"
         << QtlMovieFFmpeg::outputArguments(settings(), outputFileName, "dvd");

    // Add the process for remux.
    // Note that this never fails since the argument list is not empty.
    addFFmpeg(tr("Remuxing audio/video"), args);

    // Add an action to delete the intermediate files now.
    // If the job continues with an ISO image file generation, this save a lot of transient disk space.
    if (!settings()->keepIntermediateFiles()) {
        QtlMovieDeleteAction* eraser = new QtlMovieDeleteAction(QtlStringList(transcodeOutput, audioFile), settings(), log(), this);
        eraser->setDescription(tr("Cleaning up intermediate audio/video files"));
        _actionList.append(eraser);
    }

    return true;
}


//----------------------------------------------------------------------------
// Add the processes for transcoding to a DVD ISO image.
//----------------------------------------------------------------------------

bool QtlMovieJob::addTranscodeToDvdIsoImage(const QtlMovieInputFile* inputFile, const QString& outputFileName)
{
    // Need to transcode to a temporary file first.
    const QString mpegFileName(_tempDir + QDir::separator() + "movie-iso.mpg");

    // Add a process to transcode input file into a DVD-compliant MPEG file.
    if (!addTranscodeToDvdFile(inputFile, mpegFileName)) {
        return false;
    }

    // Temporary directoy where to create the DVD structure.
    const QString dvdRoot(_tempDir + QDir::separator() + "dvd");

    // Use DVD Author to create the DVD structure.
    // First pass: create the main "title" of the DVD (and only one here).
    // Option "-t" creates a title. Option "-O" (uppercase) specifies the output directory
    // and delete any previous content ("-o" - lowercase - does not delete previous content).
    QStringList args;
    args << "-t"               // Create a "title" in the DVD.
         << "-O" << dvdRoot    // Output directory (-O, uppercase, means delete previous content).
         << chapterOptions(int(inputFile->durationInSeconds()), settings()->chapterMinutes() * 60)
         << mpegFileName;      // MPEG file for this title.

    // Add the DVD author process.
    QtlMovieProcess* process = new QtlMovieDvdAuthorProcess(args, settings(), log(), this);
    process->setDescription(tr("Creating DVD structure pass 1: add movie"));
    _actionList.append(process);

    // Second pass: create the table of content of the DVD.
    args.clear();
    args << "-T"               // Create the "table of content" of the DVD.
         << "-o" << dvdRoot;   // Output directory (-o, uppercase, means keep previous content).

    // Add an action to delete the intermediate file now.
    // This save a lot of transient disk space before creating the ISO image.
    if (!settings()->keepIntermediateFiles()) {
        // If the input was not DVD-compliant, the MPEG file was a temporary one.
        QtlMovieDeleteAction* eraser = new QtlMovieDeleteAction(QStringList(mpegFileName), settings(), log(), this);
        eraser->setDescription(tr("Cleaning up intermediate MPEG file"));
        _actionList.append(eraser);
    }

    // Add the second DVD author process.
    process = new QtlMovieDvdAuthorProcess(args, settings(), log(), this);
    process->setDescription(tr("Creating DVD structure pass 2: DVD table of content"));
    _actionList.append(process);

    // Create the DVD ISO image using mkisofs.
    args.clear();
    args << "-dvd-video"             // Specific file placement for DVD video.
         << "-preparer" << "DVD"     // Dummy "preparer id".
         << "-publisher" << "DVD"    // Dummy "publisher id".
         << "-volid" << "DVD"        // Dummy "volume id".
         << "-o" << outputFileName   // Output ISO file.
         << dvdRoot;                 // Root directory of file structure.

    // Add the mkisofs process.
    process = new QtlMovieMkisofsProcess(args, settings(), log(), this);
    process->setDescription(tr("Creating DVD ISO image file"));
    _actionList.append(process);

    return true;
}


//----------------------------------------------------------------------------
// Add the process for burning a DVD ISO image.
//----------------------------------------------------------------------------

bool QtlMovieJob::addBurnDvd(const QString& isoFile, const QString& dvdBurner)
{
    // Build a growisofs command to burn the DVD.
    // Important: In the last argument, use "device=isofile" in one single argument.
    // If the burning device and the ISO file are specified in two separate arguments,
    // growisofs creates a new file system with the ISO file inside. The "=" in the
    // same argument means that the file is an ISO image to burn.
    QStringList args;
    args << "-dvd-compat" << "-Z" << (dvdBurner + "=" + isoFile);

    // Add the growisofs process to the job.
    QtlMovieProcess* process = new QtlMovieGrowisofsProcess(args, settings(), log(), this);
    process->setDescription(tr("Burning DVD ISO image file"));
    _actionList.append(process);

    return true;
}


//----------------------------------------------------------------------------
// Add a process for transcoding to iPad.
//----------------------------------------------------------------------------

bool QtlMovieJob::addTranscodeToIpad(const QtlMovieInputFile* inputFile, const QString& outputFileName)
{
    // Video and audio stream to transcode.
    const QtlMovieStreamInfoPtr videoStream(inputFile->selectedVideoStreamInfo());
    const QtlMovieStreamInfoPtr audioStream(inputFile->selectedAudioStreamInfo());

    // Start FFmpeg argument list.
    QStringList args(QtlMovieFFmpeg::inputArguments(settings(), inputFile->ffmpegInputFileSpecification(), inputFile->palette()));

    // Process selected video stream.
    if (!videoStream.isNull()) {
        QString videoFilters;

        // Common video options.
        args << "-map" << videoStream->ffSpecifier()
             << "-codec:v" << "libx264"      // H.264 (AVC, Advanced Video Coding, MPEG-4 part 10)
             << QtlMovieFFmpeg::frameRateOptions(settings(), QtlMovieOutputFile::Ipad)
             << "-b:v" << QString::number(QTL_IPAD_VIDEO_BITRATE)
             << "-maxrate" << "10000k"
             << "-bufsize" << "10000k"
             << "-preset" << "slow"
             << "-profile:v" << "baseline"   // AVC profile
             << "-level" << "30"             // AVC level 3.0
             << "-threads" << "auto";        // Autodetect a suitable number of threads to use.

        // Get the characteristics of the video stream.
        const int width = videoStream->width();
        const int height = videoStream->height();
        const float dar = videoStream->displayAspectRatio();

        // If the input video is too large or if the pixel aspect ratio is not 1 ("square" pixels),
        // resize the video. The maximum output size is 1024x768.
        int widthOut = 0;
        int heightOut = 0;
        QtlMovieFFmpeg::addBoundedSizeOptions(args, videoFilters, width, height, dar, QTL_IPAD_VIDEO_WIDTH, QTL_IPAD_VIDEO_HEIGHT, 1.0, widthOut, heightOut);

        // Add subtitles processing.
        if (!addSubtitleFileVideoFilter(videoFilters, width, height, inputFile->externalSubtitleFileName()) ||
            !addSubtitleStreamVideoFilter(videoFilters, inputFile, widthOut, heightOut)) {
            return false;
        }

        // Insert the video filter specification.
        args << QtlMovieFFmpeg::videoFilterOptions(videoFilters);
    }

    // Process selected audio stream.
    if (!audioStream.isNull()) {
        args << "-map" << audioStream->ffSpecifier()
             << "-codec:a" << "aac"         // AAC (Advanced Audio Coding, MPEG-4 part 3)
             << "-strict" << "experimental" // Allow experimental features, aac codec is one.
             << "-ac" << "2"                // Remix to 2 channels (stereo)
             << "-ar" << QString::number(QTL_IPAD_AUDIO_SAMPLING)
             << "-b:a" << QString::number(QTL_IPAD_AUDIO_BITRATE);
    }

    // The FFmpeg argument list ends with the output file name.
    args << QtlMovieFFmpeg::outputArguments(settings(), outputFileName, "mp4");

    // Add the FFmpeg process.
    return addFFmpeg(tr("Transcoding audio/video"), args);
}


//----------------------------------------------------------------------------
// Add a process for extracting subtitles into an SRT file.
//----------------------------------------------------------------------------

bool QtlMovieJob::addExtractSubtitle(const QtlMovieInputFile* inputFile, QString& outputFileName)
{
    // There must be one selected subtitle track.
    const QtlMovieStreamInfoPtr stream(inputFile->selectedSubtitleStreamInfo());
    if (stream.isNull()) {
        return errorFalse(tr("No subtitle stream was selected"));
    }

    // Input subtitle type.
    const QtlMovieStreamInfo::SubtitleType inType = stream->subtitleType();

    // Output subtitle type.
    QtlMovieStreamInfo::SubtitleType outType;
    if (QFileInfo(outputFileName).suffix().isEmpty()) {
        // No file name suffix, compute appropriate output type.
        // Use same as input type, except a few cases.
        switch (inType) {
        case QtlMovieStreamInfo::SubSsa:
            outType = QtlMovieStreamInfo::SubAss;
            break;
        case QtlMovieStreamInfo::SubTeletext:
            outType = QtlMovieStreamInfo::SubRip;
            break;
        default:
            outType = inType;
            break;
        }
        // Update output file name.
        outputFileName.append(QtlMovieStreamInfo::fileExtension(outType));
    }
    else {
        // A file suffix is provided, used corresponding subtitle type.
        outType = QtlMovieStreamInfo::subtitleType(outputFileName);
    }

    // Build the extraction command.
    if (inType == QtlMovieStreamInfo::SubTeletext) {
        // Teletext subtitles are extracted using telxcc, not ffmpeg.
        if (outType != QtlMovieStreamInfo::SubRip) {
            return errorFalse(tr("Teletext subtitles can be extracted as SRT only"));
        }

        // Build telxcc options to extract one subtitle track to SRT.
        QStringList args;
        args << "-n"                                             // No UTF-8 BOM in the SRT file.
             << "-t" << QString::number(stream->streamId())      // PID (-t means transport).
             << "-p" << QString::number(stream->teletextPage())  // Teletext page within PID.
             << "-i" << inputFile->fileName()
             << "-o" << outputFileName;

        // Add the telxcc process.
        QtlMovieProcess* process = new QtlMovieProcess(settings()->telxcc(), args, false, settings(), log(), this);
        process->setDescription(tr("Extracting Teletext subtitles as SRT"));
        _actionList.append(process);
        return true;
    }
    else {
        // Other types of subtitles are extracted using ffmpeg.
        // Determine codec type and output file format.
        QString codecName, fileFormat;
        switch (outType) {
        case QtlMovieStreamInfo::SubRip:
            codecName = "srt";
            fileFormat = "srt";
            break;
        case QtlMovieStreamInfo::SubAss:
            codecName = "ass";
            fileFormat = "ass";
            break;
        default:
            return errorFalse(tr("This type of subtitles cannot be extracted by ffmpeg"));
        }

        // Build FFmpeg options to extract one subtitle track to SRT.
        QStringList args(QtlMovieFFmpeg::inputArguments(settings(), inputFile->ffmpegInputFileSpecification()));
        args << "-vn"   // Suppress video streams.
             << "-an"   // Suppress audio streams.
             << "-codec:s" << codecName
             << "-map" << stream->ffSpecifier()
             << QtlMovieFFmpeg::outputArguments(settings(), outputFileName, fileFormat);

        // Add the FFmpeg process.
        return addFFmpeg(tr("Extracting subtitles"), args);
    }
}
