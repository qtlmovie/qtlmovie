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
// Define the class QtlMovieJob.
//
//----------------------------------------------------------------------------

#include "QtlMovieJob.h"
#include "QtlMovie.h"
#include "QtlMovieDeleteAction.h"
#include "QtlMovieFFmpeg.h"
#include "QtlMovieFFmpegProcess.h"
#include "QtlMovieFFmpegVolumeDetect.h"
#include "QtlMovieDvdAuthorProcess.h"
#include "QtlMovieMkisofsProcess.h"
#include "QtlMovieGrowisofsProcess.h"
#include "QtlMovieCcExtractorProcess.h"
#include "QtlMovieTeletextExtract.h"
#include "QtlMovieCleanupSubtitles.h"
#include "QtlMovieConvertSubStationAlpha.h"
#include "QtlOpticalDrive.h"
#include "QtlStringList.h"
#include "QtlSysInfo.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieJob::QtlMovieJob(QtlMovieTask* task, const QtlMovieSettings* settings, QtlLogger* log, QObject *parent) :
    QtlMovieAction(settings, log, parent),
    _task(task),
    _outSeconds(0),
    _actionCount(0),
    _tempDir(),
    _actionList(),
    _variables()
{
    Q_ASSERT(task != 0);
    Q_ASSERT(task->inputFile() != 0);
    Q_ASSERT(task->outputFile() != 0);

    // Notify the task when the state changes.
    connect(this, &QtlMovieJob::started, task, &QtlMovieTask::setStarted);
    connect(this, &QtlMovieJob::completed, task, &QtlMovieTask::setCompleted);
}


//----------------------------------------------------------------------------
// Store / retrieve a variable in the job.
//----------------------------------------------------------------------------

void QtlMovieJob::setVariable(const QString& name, const QStringList& value)
{
    _variables.insert(name, value);
}

QStringList QtlMovieJob::getVariable(const QString& name) const
{
    return _variables.value(name);
}


//----------------------------------------------------------------------------
// Report an error during start() and abort the job.
//----------------------------------------------------------------------------

bool QtlMovieJob::abortStart(const QString& message, bool result)
{
    qtlError(this, message);
    emitCompleted(false, message);
    return result;
}


//----------------------------------------------------------------------------
// Start the transcoding job.
//----------------------------------------------------------------------------

bool QtlMovieJob::start()
{
    // Do not start twice.
    if (!QtlMovieAction::start()) {
        return false;
    }

    // Cleanup action list.
    while (!_actionList.isEmpty()) {
        delete _actionList.takeFirst();
    }

    // Check that input/output files are specified.
    if (_task == 0 || _task->inputFile() == 0 || !_task->inputFile()->isSet()) {
        return abortStart(tr("No input file selected."), true);
    }
    if (_task->outputFile() == 0 || !_task->outputFile()->isSet()) {
        return abortStart(tr("No output file selected."), true);
    }

    // If intermediate files needs to be created, we will use a temporary directory
    // in the same directory as the output file.
    _tempDir = QtlFile::absoluteNativeFilePath(_task->outputFile()->fileName() + ".qtlmovie.temp");

    // Create the temporary directory.
    QDir dir;
    if (!dir.mkpath(_tempDir)) {
        return abortStart(tr("Cannot create temporary directory %1").arg(_tempDir), true);
    }

    // Being based on the output file name, the temporary directory may contain
    // any type of character. This has proven to be harmless on Unix systems.
    // On Windows, at least two problems were identified:
    // - Non-ASCII characters in the -passlogfile option for ffmpeg.
    //   See the bug report at http://trac.ffmpeg.org/ticket/3056
    // - A single quote in the specification of the subtitle file for the "ass"
    //   and "subtitles" video filters in ffmpeg. Special characters are
    //   escaped but the single quote is special here.
    // To avoid the problem, we use the "short path name" of the temporary directory,
    // ie. the old DOS 8.3 name. The operation is transparent on non-Windows systems.
    _tempDir = QtlFile::shortPath(_tempDir, true);

    // Force the display aspect ratio of the video stream if required.
    const QtlMediaStreamInfoPtr video(_task->inputFile()->selectedVideoStreamInfo());
    if (!video.isNull()) {
        video->setForcedDisplayAspectRatio(_task->outputFile()->forcedDisplayAspectRatio());
    }

    // Build the list of actions to execute.
    if (!buildScenario()) {
        return true;
    }
    _actionCount = _actionList.size();

    // Start the first action.
    if (_actionList.isEmpty()) {
        emitCompleted(false, tr("Nothing to do"));
    }
    else if (!startNextAction()) {
        emitCompleted(false, tr("Error starting process"));
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
// Get an indication of the total progress within the job.
//----------------------------------------------------------------------------

int QtlMovieJob::currentProgress(int currentActionProgress, int currentActionMaximum, int jobMaximum) const
{
    int progressValue = 0;
    if (isStarted() && jobMaximum > 0) {
        // First, compute duration of one action. This assumes that all actions
        // have the same duration, which is of course wrong. But never mind.
        const int actionDuration = _actionCount == 0 ? jobMaximum : jobMaximum / _actionCount;

        // Then, compute initial progress of current action.
        progressValue = actionDuration * (_actionCount - _actionList.size());

        // Finally, add progress within current action, if available.
        if (currentActionMaximum > 0) {
            progressValue += (actionDuration * currentActionProgress) / currentActionMaximum;
        }
    }
    return progressValue;
}


//----------------------------------------------------------------------------
// Emit the completed() signal.
//----------------------------------------------------------------------------

void QtlMovieJob::emitCompleted(bool success, const QString& message)
{
    // Cleanup the job.
    cleanup();

    // Notify superclass.
    QtlMovieAction::emitCompleted(success, message);

    // Final message in the log.
    line(tr("Job completed"), QColor(Qt::darkGreen));
}


//----------------------------------------------------------------------------
// Cleanup the job environment.
//----------------------------------------------------------------------------

void QtlMovieJob::cleanup()
{
    // Cleanup all remaining actions (in case of error).
    while (!_actionList.isEmpty()) {
        _actionList.takeFirst()->deleteLater();
    }

    // Delete temporary directory and its content.
    // If the option "keep intermediate files" is present, we delete it only if empty.
    QDir dir(_tempDir);
    const bool exists = !_tempDir.isEmpty() && dir.exists();
    const bool empty = !exists || dir.entryList().isEmpty();
    if (exists && (empty || !settings()->keepIntermediateFiles())) {
        // We must delete the temporary directory. In some cases, it appears that the
        // deletion fails, probably due to some race condition like a child process not
        // completely terminated, holding an open file inside the temporary directory.
        // As a plausible solution, we retry the deletion every 500 ms during 2 seconds
        // until the directory is actually deleted. If the problem persist, well, we
        // will need to characterize it more closely and find a real solution.
        int remaingMillisec = 2000;
        while (remaingMillisec >= 0 && dir.exists() && !dir.removeRecursively()) {
            // Failed to delete the directory, wait 500 ms.
            QThread::msleep(500);
            remaingMillisec -= 500;
        }
        if (dir.exists()) {
            line(tr("Error deleting %1").arg(_tempDir));
        }
    }
}


//----------------------------------------------------------------------------
// Start the next action in the list.
//----------------------------------------------------------------------------

bool QtlMovieJob::startNextAction()
{
    // Check that there is something to start.
    if (_actionList.isEmpty()) {
        line(tr("Internal error, no more process to start"));
        return false;
    }

    // Get notifications from next process.
    QtlMovieAction* next = _actionList.first();
    connect(next, &QtlMovieAction::progress,  this, &QtlMovieJob::progress);
    connect(next, &QtlMovieAction::completed, this, &QtlMovieJob::actionCompleted);

    // Start the next process.
    if (!next->start()) {
        line(tr("Failed to start process."));
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
    const QtlMediaStreamInfoPtr subtitleInfo(inputFile->selectedSubtitleStreamInfo());
    const QtlMediaStreamInfo::SubtitleType subtitleType(subtitleInfo.isNull() ? QtlMediaStreamInfo::SubNone : subtitleInfo->subtitleType());

    // Decide what is supported.
    switch (outputType) {
        case QtlMovieOutputFile::DvdFile:
        case QtlMovieOutputFile::DvdImage:
        case QtlMovieOutputFile::DvdBurn:
        case QtlMovieOutputFile::Ipad:
        case QtlMovieOutputFile::Iphone:
        case QtlMovieOutputFile::Android:
        case QtlMovieOutputFile::Avi:
            // Require any audio and video type.
            // Require a supported or no subtitle type.
            return inputFile->selectedAudioStreamIndex() >= 0 &&
                    inputFile->selectedVideoStreamIndex() >= 0 &&
                    subtitleType != QtlMediaStreamInfo::SubOther;

        case QtlMovieOutputFile::SubRip:
            // Require a subtitle type which can be exported as SubRip.
            return subtitleType == QtlMediaStreamInfo::SubRip ||
                    subtitleType == QtlMediaStreamInfo::SubSsa ||
                    subtitleType == QtlMediaStreamInfo::SubAss ||
                    subtitleType == QtlMediaStreamInfo::SubTeletext ||
                    subtitleType == QtlMediaStreamInfo::SubCc;

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
    switch (QtlMediaStreamInfo::subtitleType(subtitleFileName, true)) {
        case QtlMediaStreamInfo::SubAss:
        case QtlMediaStreamInfo::SubSsa:
        case QtlMediaStreamInfo::SubRip:
            return true;
        default:
            return false;
    }
}


//----------------------------------------------------------------------------
// Change the name of the external subtitle file.
//----------------------------------------------------------------------------

void QtlMovieJob::setExternalSubtitleFileName(QtlMovieInputFile*& inputFile, const QString& subtitleFile)
{
    if (!subtitleFile.isEmpty()) {
        // Clone the object if it is the original input file.
        if (inputFile == _task->inputFile()) {
            inputFile = new QtlMovieInputFile(*inputFile, this);
        }
        inputFile->setExternalSubtitleFileName(subtitleFile);
    }
}


//----------------------------------------------------------------------------
// Build the list of processes to execute to perform the job.
//----------------------------------------------------------------------------

bool QtlMovieJob::buildScenario()
{
    // Duration of the input file in seconds.
    const int totalSeconds = qRound(_task->inputFile()->durationInSeconds());

    // Duration of output file in seconds.
    const int maxSeconds = settings()->transcodeComplete() ? 0 : settings()->transcodeSeconds();
    _outSeconds = maxSeconds > 0 && maxSeconds < totalSeconds ? maxSeconds : totalSeconds;

    // Main output file type.
    const QtlMovieOutputFile::OutputType outputType = _task->outputFile()->outputType();

    // If the operation is simply a DVD burning, there is nothing to transcode.
    if (_task->inputFile()->isIsoImage() && outputType == QtlMovieOutputFile::DvdBurn) {
        return addBurnDvd(_task->inputFile()->fileName(), settings()->dvdBurner());
    }

    // The input file which will be used for media transcoding.
    // Initially, this is the input file but we may insert intermediate steps later.
    QtlMovieInputFile* inputForTranscoding = _task->inputFile();

    // External subtitle file.
    if (!inputForTranscoding->externalSubtitleFileName().isEmpty()) {
        switch (QtlMediaStreamInfo::subtitleType(_task->inputFile()->externalSubtitleFileName(), true)) {
            case QtlMediaStreamInfo::SubAss:
            case QtlMediaStreamInfo::SubSsa:
            case QtlMediaStreamInfo::SubRip:
                // Supported external subtitle file type.
                break;
            default:
                return abortStart(tr("Non-existent or unsupported external subtitle file."));
        }
    }

    // Subtitle stream in input file.
    const QtlMediaStreamInfoPtr subtitleStream(inputForTranscoding->selectedSubtitleStreamInfo());

    // This boolean will be true when the subtitle file is created by QtlMovie internal code.
    bool internallyCreatedSubtitles = false;

    // Do we need to add an initial pass to extract subtitles? Yes if:
    // - A subtitle stream is selected in input file.
    // - Not DVD or DVB subtitles. These are directly processed by FFmpeg from the input file.
    //   No need to have a separate first pass to extract them.
    // - The final output file is not simply a subtitle extraction.
    if (!subtitleStream.isNull() &&
        subtitleStream->subtitleType() != QtlMediaStreamInfo::SubDvd &&
        subtitleStream->subtitleType() != QtlMediaStreamInfo::SubDvb &&
        outputType != QtlMovieOutputFile::SubRip) {

        // Extract subtitles in an intermediate file.
        // Do not provide a suffix, addExtractSubtitle() will update it for us.
        QString subtitleFile(_tempDir + QDir::separator() + "subtitles");

        // Create a process for subtitle extraction. File suffix is updated.
        // SSA/ASS are converted to SRT if specified in the settings.
        if (!addExtractSubtitle(inputForTranscoding, subtitleFile, internallyCreatedSubtitles, settings()->downgradeSsaToSrt(), false)) {
            return false;
        }

        // Clone the input file with different subtitle characteristics.
        setExternalSubtitleFileName(inputForTranscoding, subtitleFile);
    }

    // Do we need to add a pass to cleanup the subtitles?
    // Note that if the subtitles are created internally, by our code, we trust the output
    // and there is no need to cleanup the subtitles.
    if (!internallyCreatedSubtitles &&
        settings()->cleanupSubtitles() &&
        QtlMediaStreamInfo::isTextFile(inputForTranscoding->externalSubtitleFileName()))
    {
        // Cleanup subtitles into an intermediate file with same suffix.
        const QString subtitleSuffix(QFileInfo(inputForTranscoding->externalSubtitleFileName()).suffix());
        const QString cleanSubtitleFile(_tempDir + QDir::separator() + "clean-subtitles." + subtitleSuffix);

        // Create a phase to cleanup the subtitles.
        QtlMovieCleanupSubtitles* action =
                new QtlMovieCleanupSubtitles(inputForTranscoding->externalSubtitleFileName(),
                                             cleanSubtitleFile,
                                             settings(),
                                             this,
                                             this);
        action->setDescription(tr("Cleanup subtitles file"));
        _actionList.append(action);

        // So now the transcoding should use the cleaned file.
        // Clone the input file with different subtitle characteristics.
        setExternalSubtitleFileName(inputForTranscoding, cleanSubtitleFile);
    }

    // Selected audio stream in input file.
    const QtlMediaStreamInfoPtr audioStream(inputForTranscoding->selectedAudioStreamInfo());

    // Check if we need an initial pass to determine the audio volume of the input file.
    // This is required if audio normalization is requested and both input and output contain audio.
    if (settings()->audioNormalize() && !audioStream.isNull() && outputType != QtlMovieOutputFile::SubRip) {

        // Add an initial pass to evaluate the audio volume.
        QtlMovieFFmpegVolumeDetect* process =
                new QtlMovieFFmpegVolumeDetect(inputForTranscoding->ffmpegInputFileSpecification(),
                                               audioStream->ffSpecifier(),
                                               _outSeconds,
                                               _tempDir,
                                               settings(),
                                               this,
                                               this,
                                               inputForTranscoding->dataPull(this));
        process->setDescription(tr("Evaluate audio level"));
        _actionList.append(process);
    }

    // Build the ffmpeg command for the main process.
    bool success = false;
    switch (outputType) {
        case QtlMovieOutputFile::DvdFile: {
            success = addTranscodeToDvdFile(inputForTranscoding, _task->outputFile()->fileName());
            break;
        }
        case QtlMovieOutputFile::DvdImage: {
            success = addTranscodeToDvdIsoImage(inputForTranscoding, _task->outputFile()->fileName());
            break;
        }
        case QtlMovieOutputFile::DvdBurn: {
            success = addTranscodeToDvdIsoImage(inputForTranscoding, _task->outputFile()->fileName()) &&
                    addBurnDvd(_task->outputFile()->fileName(), settings()->dvdBurner());
            break;
        }
        case QtlMovieOutputFile::Ipad: {
            success = addTranscodeToMp4(inputForTranscoding, _task->outputFile()->fileName(),
                                        settings()->iPad(), settings()->iPadVideoQuality());
            break;
        }
        case QtlMovieOutputFile::Iphone: {
            success = addTranscodeToMp4(inputForTranscoding, _task->outputFile()->fileName(),
                                        settings()->iPhone(), settings()->iPhoneVideoQuality());
            break;
        }
        case QtlMovieOutputFile::Android: {
            success = addTranscodeToMp4(inputForTranscoding, _task->outputFile()->fileName(),
                                        settings()->android(), settings()->androidVideoQuality());
            break;
        }
        case QtlMovieOutputFile::Avi: {
            success = addTranscodeToAvi(inputForTranscoding, _task->outputFile()->fileName());
            break;
        }
        case QtlMovieOutputFile::SubRip: {
            QString subtitleFile(_task->outputFile()->fileName());
            bool internallyCreated = false; // unused
            // Convert SSA/ASS to SRT is all cases, fail if cannot be converted to SRT.
            success = addExtractSubtitle(inputForTranscoding, subtitleFile, internallyCreated, true, true);
            break;
        }
        default: {
            return abortStart(tr("Unexpected output type %1").arg(outputType));
        }
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

bool QtlMovieJob::addFFmpeg(const QString& description, const QStringList ffmpegArguments, bool originalInput)
{
    if (ffmpegArguments.isEmpty()) {
        // Error building argument list.
        return abortStart(tr("Internal error, no FFmpeg option"));
    }
    else {
        // If the FFmpeg input file is the original input file, then use the QtlDataPull is necessary.
        // But if the input file is some intermediate file, always use the file.
        QtlDataPull* dataPull = originalInput ? task()->inputFile()->dataPull(this) : 0;

        QtlMovieFFmpegProcess* process = new QtlMovieFFmpegProcess(ffmpegArguments, _outSeconds, _tempDir, settings(), this, this, dataPull);
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
    const QtlMediaStreamInfo::SubtitleType subType(QtlMediaStreamInfo::subtitleType(subtitleFileName));
    switch (subType) {

        case QtlMediaStreamInfo::SubRip:
            return addTextSubtitleFileVideoFilter(videoFilters, "subtitles", widthIn, heightIn, subtitleFileName);

        case QtlMediaStreamInfo::SubAss:
        case QtlMediaStreamInfo::SubSsa:
            return addTextSubtitleFileVideoFilter(videoFilters, "ass", widthIn, heightIn, subtitleFileName);

        case QtlMediaStreamInfo::SubDvd:
        case QtlMediaStreamInfo::SubDvb:
        case QtlMediaStreamInfo::SubTeletext:
        case QtlMediaStreamInfo::SubCc:
        case QtlMediaStreamInfo::SubOther:
        case QtlMediaStreamInfo::SubNone:
        default:
            return abortStart(tr("Unsupported subtitle type"));
    }
}


//----------------------------------------------------------------------------
// Compute an FFmpeg video filter for burning text subtitles
// from an external file into video.
//----------------------------------------------------------------------------

bool QtlMovieJob::addTextSubtitleFileVideoFilter(QString& videoFilters, const QString& filterName, int widthIn, int heightIn, const QString& subtitleFileName)
{
    // Subtitles filter.
    if (!videoFilters.isEmpty()) {
        videoFilters.append(",");
    }
    videoFilters.append(filterName);
    videoFilters.append("=filename=");
    videoFilters.append(QtlMovieFFmpeg::escapedFilterArgument(subtitleFileName));

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
    const QtlMediaStreamInfoPtr subtitleStream(inputFile->selectedSubtitleStreamInfo());
    const QtlMediaStreamInfoPtr videoStream(inputFile->selectedVideoStreamInfo());

    // If no video or subtitle stream specified, nothing to do.
    if (subtitleStream.isNull() || videoStream.isNull()) {
        return true; // nothing to do but not an error.
    }

    // Subtitle type.
    switch (subtitleStream->subtitleType()) {

        case QtlMediaStreamInfo::SubDvb:
        case QtlMediaStreamInfo::SubDvd: {
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
        case QtlMediaStreamInfo::SubRip:
        case QtlMediaStreamInfo::SubAss:
        case QtlMediaStreamInfo::SubSsa:
        case QtlMediaStreamInfo::SubTeletext:
        case QtlMediaStreamInfo::SubCc:
            // These subtitles were extracted from the file in a previous pass. Ignore them now.
            return true;

        case QtlMediaStreamInfo::SubOther:
        case QtlMediaStreamInfo::SubNone:
        default:
            return abortStart(tr("Unsupported subtitle type"));
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
    // Start FFmpeg argument lists (input file and audio options).
    QStringList args;
    args << QtlMovieFFmpeg::inputArguments(settings(), inputFile)
         << QtlMovieFFmpeg::dvdAudioOptions(settings(), inputFile->selectedAudioStreamInfo());

    // Video stream to transcode.
    const QtlMediaStreamInfoPtr videoStream(inputFile->selectedVideoStreamInfo());

    if (videoStream.isNull()) {
        return abortStart(tr("No selected video stream"));
    }
    else if (!settings()->forceDvdTranscode() &&
             inputFile->selectedVideoStreamIsDvdCompliant() &&
             inputFile->selectedSubtitleStreamIndex() < 0 &&
             inputFile->externalSubtitleFileName().isEmpty()) {
        // Video is DVD compliant, no subtitle to insert, simply copy the video stream.
        args << "-map" << videoStream->ffSpecifier()
             << "-codec:v" << "copy"
             << QtlMovieFFmpeg::outputArguments(settings(), outputFileName, "dvd");
        return addFFmpeg(tr("Transcoding audio only"), args, true);
    }
    else {
        // Transcode video in two passes.
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
                line(tr("Max video bitrate to fit in DVD: %1 b/s, using %2 b/s").arg(maxBitrate).arg(bitrate));
            }
        }

        // Add video rotation options if required.
        int width = 0;
        int height = 0;
        float dar = 1.0;
        QString videoFilters;
        QtlMovieFFmpeg::addRotateOptions(settings(), videoStream, args, videoFilters, width, height, dar);

        // We always adjust the size of the video. We must know the input video size.
        if (width <= 0 || height <= 0) {
            return abortStart(tr("Unknown video size or aspect ratio, cannot create suitable video for DVD"));
        }

        // Video filter: DVD output is always 720x576, 16:9.
        QtlMovieFFmpeg::addResizeVideoFilter(videoFilters,
                                             width,
                                             height,
                                             dar,
                                             settings()->dvdVideoWidth(),
                                             settings()->dvdVideoHeight(),
                                             QTL_DVD_DAR);

        // Add subtitles processing to video filter.
        if (!addSubtitleFileVideoFilter(videoFilters, width, height, inputFile->externalSubtitleFileName()) ||
            !addSubtitleStreamVideoFilter(videoFilters, inputFile, settings()->dvdVideoWidth(), settings()->dvdVideoHeight())) {
            return false;
        }

        // Common video options.
        args << "-map" << videoStream->ffSpecifier()
             << "-codec:v" << "mpeg2video"
             << "-threads" << QString::number(QtlSysInfo::numberOfProcessors(1))
             << "-target" << (settings()->createPalDvd() ? "pal-dvd" : "ntsc-dvd")  // Select presets for DVD.
             << "-b:v" << QString::number(bitrate)
             << "-aspect" << QTL_DVD_DAR_FFMPEG
             << QtlMovieFFmpeg::videoFilterOptions(videoFilters)
             << "-passlogfile" << (_tempDir + QDir::separator() + "fflog");

        // There are two argument lists, one for each encoding pass.
        // The output of the first pass is useless and sent to the null device (only the log is useful).
        QStringList pass1Args;
        QStringList pass2Args;

        pass1Args << args
                  << "-pass" << "1"
                  << QtlMovieFFmpeg::outputArguments(settings(), QProcess::nullDevice(), "dvd");

        pass2Args << args
                  << "-pass" << "2"
                  << QtlMovieFFmpeg::outputArguments(settings(), outputFileName, "dvd");

        // Add 2 processes for video transcoding.
        return addFFmpeg(tr("Transcoding, pass 1"), pass1Args, true) &&
                addFFmpeg(tr("Transcoding, pass 2"), pass2Args, true);
    }
}


//----------------------------------------------------------------------------
// Add the processes for transcoding to a DVD media file.
//----------------------------------------------------------------------------

bool QtlMovieJob::addTranscodeToDvdFile(const QtlMovieInputFile* inputFile, const QString& outputFileName)
{
    // Check if the input file looks like one of our DVD-compliant MPEG files.
    const bool dvdCompliant = !settings()->forceDvdTranscode() && inputFile->isDvdCompliant();

    // Video and audio stream to transcode.
    const QtlMediaStreamInfoPtr videoStream(inputFile->selectedVideoStreamInfo());
    const QtlMediaStreamInfoPtr audioStream(inputFile->selectedAudioStreamInfo());

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
        line(tr("Found DVD-compliant audio/video encoding, will not re-encode"));
    }
    else if (!addTranscodeAudioVideoToDvd(inputFile, transcodeOutput)) {
        return false;
    }

    // If no remux is required after transcode, we are done.
    if (!remuxAfterTranscode) {
        return true;
    }

    // Input file for remux operation.
    const QString remuxInput(dvdCompliant ? inputFile->ffmpegInputFileSpecification() : transcodeOutput);

    // Remux audio and video in a dvdauthor-compatible way.
    // At that point, we know that there are one audio and one video stream.
    // Add a process to extract audio in a separate file.
    const QString audioFile(_tempDir + QDir::separator() + "audio.ac3");
    QStringList args(QtlMovieFFmpeg::inputArguments(settings()));
    args << "-i" << remuxInput
         << "-vn"  // Drop video stream
         << "-map" << "0:a"
         << "-codec:a" << "copy"
         << QtlMovieFFmpeg::outputArguments(settings(), audioFile, "ac3");

    // Add the process for audio extraction.
    // Note that this never fails since the argument list is not empty.
    addFFmpeg(tr("Extracting audio"), args, false);

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
    addFFmpeg(tr("Remuxing audio/video"), args, false);

    // Add an action to delete the intermediate files now.
    // If the job continues with an ISO image file generation, this save a lot of transient disk space.
    if (!settings()->keepIntermediateFiles()) {
        QtlMovieDeleteAction* eraser = new QtlMovieDeleteAction(QtlStringList(transcodeOutput, audioFile), settings(), this, this);
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
         << "-a" << "ac3+2ch"  // Audio is AC-3 stereo.
         << "-O" << dvdRoot    // Output directory (-O, uppercase, means delete previous content).
         << chapterOptions(int(inputFile->durationInSeconds()), settings()->chapterMinutes() * 60)
         << mpegFileName;      // MPEG file for this title.

    // Add the DVD author process.
    QtlMovieProcess* process = new QtlMovieDvdAuthorProcess(args, settings(), this, this);
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
        QtlMovieDeleteAction* eraser = new QtlMovieDeleteAction(QStringList(mpegFileName), settings(), this, this);
        eraser->setDescription(tr("Cleaning up intermediate MPEG file"));
        _actionList.append(eraser);
    }

    // Add the second DVD author process.
    process = new QtlMovieDvdAuthorProcess(args, settings(), this, this);
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
    process = new QtlMovieMkisofsProcess(args, settings(), this, this);
    process->setDescription(tr("Creating DVD ISO image file"));
    _actionList.append(process);

    return true;
}


//----------------------------------------------------------------------------
// Add the process for burning a DVD ISO image.
//----------------------------------------------------------------------------

bool QtlMovieJob::addBurnDvd(const QString& isoFile, const QString& dvdBurner)
{
    // Get the device name for burning.
    const QtlOpticalDrive drive(QtlOpticalDrive::getDrive(dvdBurner));
    if (!drive.isValid()) {
        return abortStart(tr("Cannot find device name for DVD drive name \"%1\"").arg(dvdBurner));
    }
    const QString dev(drive.burnerDeviceName());
    if (dev.isEmpty()) {
        return abortStart(tr("No media in drive \"%1\"").arg(dvdBurner));
    }

    // Build a growisofs command to burn the DVD.
    // Important: In the last argument, use "device=isofile" in one single argument.
    // If the burning device and the ISO file are specified in two separate arguments,
    // growisofs creates a new file system with the ISO file inside. The "=" in the
    // same argument means that the file is an ISO image to burn.
    QStringList args;
    const int dvdBurningSpeed = settings()->dvdBurningSpeed();
    if (dvdBurningSpeed > 0) {
        args << QStringLiteral("-speed=%1").arg(dvdBurningSpeed);
    }
    args << "-dvd-compat" << "-Z" << (dev + "=" + isoFile);

    // Add the growisofs process to the job.
    QtlMovieProcess* process = new QtlMovieGrowisofsProcess(args, settings(), this, this);
    process->setDescription(tr("Burning DVD ISO image file"));
    _actionList.append(process);

    return true;
}


//----------------------------------------------------------------------------
// Add a process for transcoding to iPad/iPhone.
//----------------------------------------------------------------------------

bool QtlMovieJob::addTranscodeToMp4(const QtlMovieInputFile* inputFile,
                                    const QString& outputFileName,
                                    const QtlMovieDeviceProfile& profile,
                                    int videoQuality)
{
    // Video and audio stream to transcode.
    const QtlMediaStreamInfoPtr videoStream(inputFile->selectedVideoStreamInfo());
    const QtlMediaStreamInfoPtr audioStream(inputFile->selectedAudioStreamInfo());

    // Start FFmpeg argument list.
    QStringList args(QtlMovieFFmpeg::inputArguments(settings(), inputFile));

    // Process selected video stream.
    if (!videoStream.isNull()) {
        QString videoFilters;

        // Common video options.
        args << "-map" << videoStream->ffSpecifier()
             << "-codec:v" << "libx264"      // H.264 (AVC, Advanced Video Coding, MPEG-4 part 10)
             << "-threads" << QString::number(QtlSysInfo::numberOfProcessors(1))
             << "-r" << profile.frameRateString()
             << "-maxrate" << "10000k"
             << "-bufsize" << "10000k"
             << "-preset" << "slow"
             << "-profile:v" << "baseline"   // AVC profile
             << "-level" << "30";            // AVC level 3.0

        // Add video rotation options if required.
        int width = 0;
        int height = 0;
        float dar = 1.0;
        QtlMovieFFmpeg::addRotateOptions(settings(), videoStream, args, videoFilters, width, height, dar);

        // If the input video is too large or if the pixel aspect ratio is not 1 ("square" pixels),
        // resize the video. The maximum output size depends on the iPad/iPhone screen size in the settings.
        int widthOut = 0;
        int heightOut = 0;
        QtlMovieFFmpeg::addBoundedSizeOptions(args,
                                              videoFilters,
                                              width,
                                              height,
                                              dar,
                                              profile.width(),
                                              profile.height(),
                                              1.0,
                                              widthOut,
                                              heightOut);

        // Set video bitrate based on actual output video size.
        const int videoBitRate = QtlMovieDeviceProfile::videoBitRate(videoQuality, widthOut, heightOut, profile.frameRate());
        debug(tr("MP4: video quality: %1, width: %2, height: %3, frameRate: %4, bitrate: %5")
              .arg(videoQuality).arg(widthOut).arg(heightOut).arg(profile.frameRate(), 0, 'f', 2).arg(videoBitRate));
        args << "-b:v" << QString::number(videoBitRate);

        // In case of 4:2:2 input chroma format, force a downgrade to 4:2:0
        // since the x264 codec does not support 4:2:2.
        if (inputFile->ffProbeInfo().valueOfStream(videoStream->ffIndex(), "pix_fmt").contains("422")) {
            args << "-pix_fmt" << "yuv420p";
        }

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
             << QTL_AUDIO_FILTER_VARREF     // Potential audio filter will be inserted here.
             << "-codec:a" << "aac"         // AAC (Advanced Audio Coding, MPEG-4 part 3)
             << "-strict" << "experimental" // Allow experimental features, aac codec is one.
             << "-ac" << "2"                // Remix to 2 channels (stereo)
             << "-ar" << QString::number(profile.audioSampling())
             << "-b:a" << QString::number(profile.audioBitRate());
    }

    // The FFmpeg argument list ends with the output file name.
    args << QtlMovieFFmpeg::outputArguments(settings(), outputFileName, "mp4");

    // Add the FFmpeg process.
    return addFFmpeg(tr("Transcoding audio/video"), args, true);
}


//----------------------------------------------------------------------------
// Add a process for transcoding to AVI.
//----------------------------------------------------------------------------

bool QtlMovieJob::addTranscodeToAvi(const QtlMovieInputFile* inputFile, const QString& outputFileName)
{
    // Video and audio stream to transcode.
    const QtlMediaStreamInfoPtr videoStream(inputFile->selectedVideoStreamInfo());
    const QtlMediaStreamInfoPtr audioStream(inputFile->selectedAudioStreamInfo());

    if (videoStream.isNull()) {
        return abortStart(tr("No selected video stream"));
    }

    // Start FFmpeg argument list.
    QStringList args(QtlMovieFFmpeg::inputArguments(settings(), inputFile));

    // Process selected audio stream.
    if (!audioStream.isNull()) {
        args << "-map" << audioStream->ffSpecifier()
             << QTL_AUDIO_FILTER_VARREF     // Potential audio filter will be inserted here.
             << "-codec:a" << "mp3"         // MP3 (MPEG-2 Audio Layer 3)
             << "-ac" << "2"                // Remix to 2 channels (stereo)
             << "-ar" << QString::number(QTL_AVI_AUDIO_SAMPLING)
             << "-b:a" << QString::number(QTL_AVI_AUDIO_BITRATE);
    }

    // Process selected video stream.
    QString videoFilters;

    // Common video options.
    args << "-map" << videoStream->ffSpecifier()
         << "-codec:v" << "mpeg4"      // H.263 (MPEG-4 part 2)
         << "-r" << QTL_STRINGIFY(QTL_AVI_FRAME_RATE);

    // Add video rotation options if required.
    int width = 0;
    int height = 0;
    float dar = 1.0;
    QtlMovieFFmpeg::addRotateOptions(settings(), videoStream, args, videoFilters, width, height, dar);

    // If the input video is too large or if the pixel aspect ratio is not 1 ("square" pixels),
    // resize the video. The maximum output size depends on the screen size in the settings.
    int widthOut = 0;
    int heightOut = 0;
    QtlMovieFFmpeg::addBoundedSizeOptions(args,
                                          videoFilters,
                                          width,
                                          height,
                                          dar,
                                          settings()->aviMaxVideoWidth(),
                                          settings()->aviMaxVideoHeight(),
                                          1.0,
                                          widthOut,
                                          heightOut);

    // Set video bitrate based on actual output video size.
    args << "-b:v" << QString::number(settings()->aviVideoBitrate(widthOut, heightOut));

    // Add subtitles processing.
    if (!addSubtitleFileVideoFilter(videoFilters, width, height, inputFile->externalSubtitleFileName()) ||
        !addSubtitleStreamVideoFilter(videoFilters, inputFile, widthOut, heightOut)) {
        return false;
    }

    // Insert the video filter specification.
    args << QtlMovieFFmpeg::videoFilterOptions(videoFilters)
         << "-passlogfile" << (_tempDir + QDir::separator() + "fflog");

    // There are two argument lists, one for each encoding pass.
    // The output of the first pass is useless and sent to the null device (only the log is useful).
    QStringList pass1Args;
    QStringList pass2Args;

    pass1Args << args
              << "-pass" << "1"
              << QtlMovieFFmpeg::outputArguments(settings(), QProcess::nullDevice(), "avi");

    pass2Args << args
              << "-pass" << "2"
              << QtlMovieFFmpeg::outputArguments(settings(), outputFileName, "avi");

    // Add 2 processes for video transcoding.
    return addFFmpeg(tr("Transcoding, pass 1"), pass1Args, true) &&
            addFFmpeg(tr("Transcoding, pass 2"), pass2Args, true);
}


//----------------------------------------------------------------------------
// Add a process for extracting subtitles into an SRT file.
//----------------------------------------------------------------------------

bool QtlMovieJob::addExtractSubtitle(const QtlMovieInputFile* inputFile,
                                     QString& outputFileName,
                                     bool& internallyCreated,
                                     bool convertToSubRip,
                                     bool srtMandatory)
{
    // By default, we do not create the subtitles file.
    internallyCreated = false;

    // There must be one selected subtitle track.
    const QtlMediaStreamInfoPtr stream(inputFile->selectedSubtitleStreamInfo());
    if (stream.isNull()) {
        return abortStart(tr("No subtitle stream was selected"));
    }

    // Input subtitle type.
    const QtlMediaStreamInfo::SubtitleType inType = stream->subtitleType();

    // Output subtitle type.
    QtlMediaStreamInfo::SubtitleType outType;
    if (QFileInfo(outputFileName).suffix().isEmpty()) {
        // No file name suffix, compute appropriate output type.
        // Use same as input type, except a few cases.
        switch (inType) {
            case QtlMediaStreamInfo::SubSsa:
            case QtlMediaStreamInfo::SubAss:
                outType = convertToSubRip ? QtlMediaStreamInfo::SubRip : QtlMediaStreamInfo::SubAss;
                break;
            case QtlMediaStreamInfo::SubTeletext:
            case QtlMediaStreamInfo::SubCc:
                outType = QtlMediaStreamInfo::SubRip;
                break;
            default:
                outType = inType;
                break;
        }
        // Update output file name.
        outputFileName.append(QtlMediaStreamInfo::fileExtension(outType));
    }
    else {
        // A file suffix is provided, used corresponding subtitle type.
        outType = QtlMediaStreamInfo::subtitleType(outputFileName);
    }

    // If mandatory conversion to SRT, error if we cannot.
    if (srtMandatory && outType != QtlMediaStreamInfo::SubRip) {
        return abortStart(tr("This subtitle type cannot be converted to SRT"));
    }

    // Build the extraction command.
    if (inputFile->isTsFile() && inType == QtlMediaStreamInfo::SubTeletext) {
        // Teletext subtitles are extracted from TS files using internal code, not ffmpeg.
        internallyCreated = true;

        if (outType != QtlMediaStreamInfo::SubRip) {
            return abortStart(tr("Teletext subtitles can be extracted as SRT only"));
        }

        // Build our own action to extract one subtitle track to SRT.
        QtlMovieTeletextExtract* action = new QtlMovieTeletextExtract
                (inputFile->fileName(),
                 stream->streamId(), // PID
                 stream->teletextPage(),
                 outputFileName,
                 settings(),
                 this,               // log
                 this);              // parent

        // Add the extraction to the job.
        action->setDescription(tr("Extracting Teletext subtitles as SRT"));
        _actionList.append(action);
        return true;
    }
    else if (inType == QtlMediaStreamInfo::SubCc) {
        // Closed Caption subtitles are extracted using CCextractor, not ffmpeg.
        if (outType != QtlMediaStreamInfo::SubRip) {
            return abortStart(tr("Closed Captions subtitles can be extracted as SRT only"));
        }

        // Build CCExtractor options.
        const int cc = stream->ccNumber();
        QStringList args;
        args << "--gui_mode_reports"     // Report info to GUI.
             << "-noteletext"            // Closed Captions only, ignore Teletext.
             << "-srt"                   // Output format is SRT.
             << "-utf8"                  // UTF-8 output.
             << "-bom"                   // Append a BOM (Byte Order Mark) to output files.
             << "-lf"                    // Use LF (UNIX) instead of CRLF (DOS, Windows) as line terminator.
             << "-nofc"                  // Don't add font color tags.
             << "-trim";                 // Trim lines.
        if (settings()->capitalizeClosedCaptions()) {
             args << "-sc";              // Sentence capitalization, suppress ALL CAPS in subtitles.
        }
        if (cc > 4) {
            // CEA-708 service.
            args << "-svc" << QString::number(cc - 4);
        }
        else if (cc > 2) {
            // EIA-608 channel 2.
            args << "-cc2" << (cc == 3 ? "-1" : "-2");
        }
        else if (cc > 0) {
            // EIA-608 channel 1.
            args << (cc == 1 ? "-1" : "-2");
        }
        else {
            return abortStart(tr("Invalid Closed Captions number: %1").arg(cc));
        }
        args << "-o" << outputFileName   // SRT output file.
             << inputFile->fileName();   // Input file.

        // Add the CCExtractor process.
        QtlMovieCcExtractorProcess* process = new QtlMovieCcExtractorProcess(args, settings(), this, this);
        process->setDescription(tr("Extracting Closed Captions as SRT"));
        _actionList.append(process);
        return true;
    }
    else {
        // Other types of subtitles are extracted using ffmpeg (unless the input file is already a pure subtitle file).
        // Determine codec type and output file format.
        QString codecName;
        QString fileFormat;
        QString ffmpegOutputFile(outputFileName);
        switch (inType) {
            case QtlMediaStreamInfo::SubRip:
                codecName = "srt";
                fileFormat = "srt";
                convertToSubRip = false; // no need to convert, already SRT
                break;
            case QtlMediaStreamInfo::SubSsa:
            case QtlMediaStreamInfo::SubAss:
                codecName = "ass";
                fileFormat = "ass";
                if (convertToSubRip) {
                    // Will be converted to SRT later, use an intermediate file.
                    ffmpegOutputFile = _tempDir + QDir::separator() + "subtitles.ass";
                    // The final subtitle file will be internally created.
                    internallyCreated = true;
                }
                break;
            default:
                return abortStart(tr("This type of subtitles cannot be extracted by ffmpeg"));
        }

        if (inputFile->isSubtitleFile()) {
            // No need to extract, already a subtitle file.
            ffmpegOutputFile = inputFile->fileName();
        }
        else {
            // Build FFmpeg options to extract one subtitle track to SRT.
            QStringList args(QtlMovieFFmpeg::inputArguments(settings(), inputFile));
            args << "-vn"   // Suppress video streams.
                 << "-an"   // Suppress audio streams.
                 << "-codec:s" << codecName
                 << "-map" << stream->ffSpecifier()
                 << QtlMovieFFmpeg::outputArguments(settings(), ffmpegOutputFile, fileFormat);

            // Add the FFmpeg process.
            if (!addFFmpeg(tr("Extracting subtitles"), args, true)) {
                return false;
            }
        }

        // If SSA/ASS to SRT conversion is required, add a pass to do this.
        if (convertToSubRip) {
            QtlMovieConvertSubStationAlpha * action = new QtlMovieConvertSubStationAlpha(ffmpegOutputFile, outputFileName, settings(), this, this);
            action->setDescription(tr("Convert subtitles to SRT"));
            _actionList.append(action);
        }
        return true;
    }
}
