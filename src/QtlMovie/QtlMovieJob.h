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
//! @file QtlMovieJob.h
//!
//! Declare the class QtlMovieJob.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEJOB_H
#define QTLMOVIEJOB_H

#include <QtCore>
#include "QtlMovieAction.h"
#include "QtlMovieTask.h"
#include "QtlMessageBoxUtils.h"

//!
//! A complete transcoding job.
//! Involves one or more QtlMovieAction.
//!
class QtlMovieJob : public QtlMovieAction
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] task The task to process.
    //! @param [in] settings Application settings.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieJob(QtlMovieTask* task, const QtlMovieSettings* settings, QtlLogger* log, QObject *parent = 0);

    //!
    //! Start the transcoding job.
    //! @return False if already started. True otherwise.
    //!
    virtual bool start();

    //!
    //! Abort the transcoding job.
    //! If the job was started, the signal completed() will be emitted when all actions actually terminate.
    //!
    virtual void abort();

    //!
    //! Get the task to process.
    //! @return The task to process.
    //!
    QtlMovieTask* task() const
    {
        return _task;
    }

    //!
    //! Get the total number of actions in this job.
    //! Available after start.
    //! @return The total number of actions in this job.
    //!
    int totalActionCount() const
    {
        return isStarted() ? _actionCount : 0;
    }

    //!
    //! Get the index of currently executing action.
    //! Available after start.
    //! @return The index of currently executing action.
    //!
    int currentActionCount() const
    {
        return isStarted() ? _actionCount - _actionList.size() : 0;
    }

    //!
    //! Get an indication of the total progress within the job.
    //! Given an estimation of the progress within the current action, we return
    //! an estimation of the progress in the entire job. We assume that all actions
    //! have the same duration, which is of course wrong.
    //! @param [in] currentActionProgress Current progress in the current action.
    //! Shall be in the range 0 to @a currentActionMaximum.
    //! @param [in] currentActionMaximum Value for completion of the current action.
    //! @param [in] jobMaximum Value for completion of the entire job.
    //! @return A value between 0 and @a jobMaximum indicating the progression in the entire job.
    //!
    int currentProgress(int currentActionProgress, int currentActionMaximum = 100, int jobMaximum = 100) const;

    //!
    //! Store a variable in the job.
    //! @param [in] name Variable name.
    //! @param [in] value Variable value as a list of strings.
    //!
    void setVariable(const QString& name, const QStringList& value);

    //!
    //! Get the value of a variable in the job.
    //! @param [in] name Variable name.
    //! @return Variable value as a list of strings.
    //!
    QStringList getVariable(const QString& name) const;

    //!
    //! Check if it is possible to transcode an input file (with its streams selections) to an output type.
    //! @param [in] inputFile The input file.
    //! @param [in] outputType The target output type.
    //! @return True if transcoding is allowed, false otherwise.
    //!
    static bool canTranscode(const QtlMovieInputFile* inputFile, QtlMovieOutputFile::OutputType outputType);

    //!
    //! Check if it is possible to insert subtitles from the specified file.
    //! @param [in] subtitleFileName The subtitle file.
    //! @return True if subtitle insertion is possible, false otherwise.
    //!
    static bool canInsertSubtitles(const QString& subtitleFileName);

protected:
    //!
    //! Emit the completed() signal.
    //! @param [in] success True when the action completed successfully, false otherwise.
    //! @param [in] message Optional error message to log.
    //!
    virtual void emitCompleted(bool success, const QString& message = QString());

private slots:
    //!
    //! Invoked each time an action completes.
    //! @param [in] success Indicates whether the action succeeded or failed.
    //!
    void actionCompleted(bool success);

private:
    QtlMovieTask*             _task;           //!< Task to process.
    int                       _outSeconds;     //!< Output file duration in seconds.
    int                       _actionCount;    //!< Number of actions to execute on start.
    QString                   _tempDir;        //!< Directory of temporary files, to delete after completion.
    QList<QtlMovieAction*>    _actionList;     //!< List of actions to execute.
    QMap<QString,QStringList> _variables;      //!< Set of job variables.

    //!
    //! Cleanup the job environment.
    //!
    void cleanup();

    //!
    //! Build the list of actions to execute to perform the job.
    //! @return True on success, false on error (job aborted).
    //!
    bool buildScenario();

    //!
    //! Report an error during @a start() and abort the job.
    //! The error message is reported both in a message box and in the log.
    //! @param [in] message Message to display.
    //! @param [in] result The value to return, false by default.
    //! @return The specified result, false by default.
    //!
    bool abortStart(const QString& message, bool result = false);

    //!
    //! Start the next action in the list.
    //! @return True on success, false on error (not started).
    //!
    bool startNextAction();

    //!
    //! Add an FFmpeg process in the process list.
    //! @param [in] description Description of this process.
    //! @param [in] ffmpegArguments FFmpeg arguments, empty meaning error.
    //! @return True on success, false on error.
    //!
    bool addFFmpeg(const QString& description, const QStringList ffmpegArguments);

    //!
    //! Compute an FFmpeg video filter for burning subtitles from an external file into video.
    //! @param [in,out] videoFilters A video filter string which is updated.
    //! @param [in] widthIn Input video width in pixels.
    //! @param [in] heightIn Input video height in pixels.
    //! @param [in] subtitleFileName Name of the subtitle file.
    //! @return True is success, false on error (message already reported).
    //!
    bool addSubtitleFileVideoFilter(QString& videoFilters,
                                    int widthIn,
                                    int heightIn,
                                    const QString& subtitleFileName);

    //!
    //! Compute an FFmpeg video filter for burning text subtitles from an external file into video.
    //! @param [in,out] videoFilters A video filter string which is updated.
    //! @param [in] filterName Filter name, usually "subtitles" or "ass".
    //! @param [in] widthIn Input video width in pixels.
    //! @param [in] heightIn Input video height in pixels.
    //! @param [in] subtitleFileName Name of the subtitle file.
    //! @return True is success, false on error (message already reported).
    //!
    bool addTextSubtitleFileVideoFilter(QString& videoFilters,
                                        const QString& filterName,
                                        int widthIn,
                                        int heightIn,
                                        const QString& subtitleFileName);

    //!
    //! Compute an FFmpeg video filter for burning subtitles from a stream of the input file into video.
    //! @param [in,out] videoFilters A video filter string which is updated.
    //! @param [in] inputFile Input file with one selected subtitle stream.
    //! @param [in] widthOut Output video width in pixels.
    //! @param [in] heightOut Output video height in pixels.
    //! @return True is success, false on error (message already reported).
    //!
    bool addSubtitleStreamVideoFilter(QString& videoFilters, const QtlMovieInputFile* inputFile, int widthOut, int heightOut);

    //!
    //! Build a DVD Author option list to create chapters.
    //! @param [in] totalSeconds Total duration of the movie in seconds or zero if unknown.
    //! @param [in] chapterSeconds Duration of a chapter in seconds or zero if none.
    //! @return DVD Author option list. Empty if either movie or chapter duration is unknown.
    //!
    QStringList chapterOptions(int totalSeconds, int chapterSeconds);

    //!
    //! Add the processes for simple audio/video transcoding to DVD.
    //! @param [in] inputFile The input file.
    //! @param [in] outputFileName The output file name.
    //! @return True on success, false on error.
    //!
    bool addTranscodeAudioVideoToDvd(const QtlMovieInputFile* inputFile, const QString& outputFileName);

    //!
    //! Add the processes for transcoding to a final DVD media file.
    //! @param [in] inputFile The input file.
    //! @param [in] outputFileName The output file name.
    //! @return True on success, false on error.
    //!
    bool addTranscodeToDvdFile(const QtlMovieInputFile* inputFile, const QString& outputFileName);

    //!
    //! Add the processes for transcoding to a DVD ISO image.
    //! @param [in] inputFile The input file.
    //! @param [in] outputFileName The output file name.
    //! @return True on success, false on error.
    //!
    bool addTranscodeToDvdIsoImage(const QtlMovieInputFile* inputFile, const QString& outputFileName);

    //!
    //! Add the process for burning a DVD ISO image.
    //! @param [in] isoFile The input ISO image.
    //! @param [in] dvdBurner The DVD burner drive name (see QtlOpticalDrive::name()).
    //! @return True on success, false on error.
    //!
    bool addBurnDvd(const QString& isoFile, const QString& dvdBurner);

    //!
    //! Add a process for transcoding to MP4 (iPad, iPhone).
    //! @param [in] inputFile The input file.
    //! @param [in] outputFileName The output file name.
    //! @param [in] outputType Output file type (either iPad or iPhone).
    //! @return True on success, false on error.
    //!
    bool addTranscodeToMp4(const QtlMovieInputFile* inputFile,
                           const QString& outputFileName,
                           QtlMovieOutputFile::OutputType outputType);

    //!
    //! Add a process for transcoding to AVI.
    //! @param [in] inputFile The input file.
    //! @param [in] outputFileName The output file name.
    //! @return True on success, false on error.
    //!
    bool addTranscodeToAvi(const QtlMovieInputFile* inputFile, const QString& outputFileName);

    //!
    //! Add a process for extracting subtitles into a file.
    //! The supported input subtitle types are SRT, SSA, ASS and Teletext (MPEG-TS input file only).
    //! @param [in] inputFile The input file.
    //! @param [in,out] outputFileName The output subtitle file name. Its suffix, when present,
    //! is used to determine the output subtitle format. Without suffix, the appropriate
    //! type is automatically selected and the file name is updated.
    //! @return True on success, false on error (unsupported subtitle type for instance).
    //!
    bool addExtractSubtitle(const QtlMovieInputFile* inputFile, QString& outputFileName);

    // Unaccessible operations.
    QtlMovieJob() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieJob)
};

#endif // QTLMOVIEJOB_H
