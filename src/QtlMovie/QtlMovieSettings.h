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
//! @file QtlMovieSettings.h
//!
//! Declare the class QtlMovieSettings, the description of the global settings
//! of the application.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIESETTINGS_H
#define QTLMOVIESETTINGS_H

#include <QtCore>
#include "QtlMovieExecFile.h"
#include "QtlLogger.h"
#include "QtlMovie.h"

//!
//! Description of the global settings of the application.
//!
class QtlMovieSettings : public QObject
{
    Q_OBJECT

public:
    //!
    //! Constructor
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlMovieSettings(QtlLogger* log, QObject* parent = 0);

    //!
    //! Load the settings from an XML file.
    //! @param [in] fileName Name of file to load. Use default name if empty.
    //! @return True on success, false on error.
    //!
    bool load(const QString& fileName = "");

    //!
    //! Save the settings in an XML file.
    //! @param fileName Name of file to create. Use default name if empty.
    //! @return True on success, false on error.
    //!
    bool save(const QString& fileName = "");

    //!
    //! Get an error message for the last error on file load or save.
    //! @return Last error message from file load or save.
    //!
    QString lastFileError() const
    {
        return _fileError;
    }

    //!
    //! Check if settings are modified (ie. not saved on disk).
    //! @return True if the settings have unsaved changes.
    //!
    bool isModified() const
    {
        return _isModified;
    }

    //!
    //! Get the explicit FFmpeg executable path.
    //! This is the path that was manually entered into the settings.
    //! @return FFmpeg executable path.
    //!
    QString ffmpegExplicitExecutable() const
    {
        return _ffmpegExplicit->fileName();
    }

    //!
    //! Get the default FFmpeg executable path.
    //! This is the executable which was automatically detected in the system.
    //! @return FFmpeg executable path.
    //!
    QString ffmpegDefaultExecutable() const
    {
        return _ffmpegDefault->fileName();
    }

    //!
    //! Get the actual FFmpeg executable.
    //! This is the command to execute.
    //! @return Actual FFmpeg executable, never null.
    //!
    const QtlMovieExecFile* ffmpeg() const
    {
        return _ffmpegExplicit->isSet() ? _ffmpegExplicit : _ffmpegDefault;
    }

    //!
    //! Set the explicit FFmpeg executable path.
    //! Empty by default, the executable is automatically located.
    //! @param [in] ffmpegExecutable FFmpeg executable path.
    //!
    void setFFmpegExplicitExecutable(const QString& ffmpegExecutable);

    //!
    //! Get the explicit FFprobe executable path.
    //! This is the path that was manually entered into the settings.
    //! @return FFprobe executable path.
    //!
    QString ffprobeExplicitExecutable() const
    {
        return _ffprobeExplicit->fileName();
    }

    //!
    //! Get the default FFprobe executable path.
    //! This is the executable which was automatically detected in the system.
    //! @return FFprobe executable path.
    //!
    QString ffprobeDefaultExecutable() const
    {
        return _ffprobeDefault->fileName();
    }

    //!
    //! Get the actual FFprobe executable.
    //! This is the command to execute.
    //! @return Actual FFprobe executable, never null.
    //!
    const QtlMovieExecFile* ffprobe() const
    {
        return _ffprobeExplicit->isSet() ? _ffprobeExplicit : _ffprobeDefault;
    }

    //!
    //! Set the explicit FFprobe executable path.
    //! Empty by default, the executable is automatically located.
    //! @param [in] ffprobeExecutable FFprobe executable path.
    //!
    void setFFprobeExplicitExecutable(const QString& ffprobeExecutable);

    //!
    //! Get the HTML description of the actual FFmpeg and FFprobe products.
    //! @return HTML description.
    //!
    QString ffmpegHtmlDescription() const;

    //!
    //! Get the explicit DvdAuthor executable path.
    //! This is the path that was manually entered into the settings.
    //! @return DvdAuthor executable path.
    //!
    QString dvdauthorExplicitExecutable() const
    {
        return _dvdauthorExplicit->fileName();
    }

    //!
    //! Get the default DvdAuthor executable path.
    //! This is the executable which was automatically detected in the system.
    //! @return DvdAuthor executable path.
    //!
    QString dvdauthorDefaultExecutable() const
    {
        return _dvdauthorDefault->fileName();
    }

    //!
    //! Get the actual DvdAuthor executable.
    //! This is the command to execute.
    //! @return Actual DvdAuthor executable, never null.
    //!
    const QtlMovieExecFile* dvdauthor() const
    {
        return _dvdauthorExplicit->isSet() ? _dvdauthorExplicit : _dvdauthorDefault;
    }

    //!
    //! Set the explicit DvdAuthor executable path.
    //! Empty by default, the executable is automatically located.
    //! @param [in] dvdauthorExecutable DvdAuthor executable path.
    //!
    void setDvdAuthorExplicitExecutable(const QString& dvdauthorExecutable);

    //!
    //! Get the HTML description of the actual DvdAuthor product.
    //! @return HTML description.
    //!
    QString dvdauthorHtmlDescription() const
    {
        return _dvdauthorExplicit->isSet() ? _dvdauthorExplicit->htmlDescription() : _dvdauthorDefault->htmlDescription();
    }

    //!
    //! Get the explicit Mkisofs executable path.
    //! This is the path that was manually entered into the settings.
    //! @return Mkisofs executable path.
    //!
    QString mkisofsExplicitExecutable() const
    {
        return _mkisofsExplicit->fileName();
    }

    //!
    //! Get the default Mkisofs executable path.
    //! This is the executable which was automatically detected in the system.
    //! @return Mkisofs executable path.
    //!
    QString mkisofsDefaultExecutable() const
    {
        return _mkisofsDefault->fileName();
    }

    //!
    //! Get the actual Mkisofs executable.
    //! This is the command to execute.
    //! @return Actual Mkisofs executable, never null.
    //!
    const QtlMovieExecFile* mkisofs() const
    {
        return _mkisofsExplicit->isSet() ? _mkisofsExplicit : _mkisofsDefault;
    }

    //!
    //! Set the explicit Mkisofs executable path.
    //! Empty by default, the executable is automatically located.
    //! @param [in] mkisofsExecutable Mkisofs executable path.
    //!
    void setMkisofsExplicitExecutable(const QString& mkisofsExecutable);

    //!
    //! Get the HTML description of the actual Mkisofs product.
    //! @return HTML description.
    //!
    QString mkisofsHtmlDescription() const
    {
        return _mkisofsExplicit->isSet() ? _mkisofsExplicit->htmlDescription() : _mkisofsDefault->htmlDescription();
    }

    //!
    //! Get the explicit Growisofs executable path.
    //! This is the path that was manually entered into the settings.
    //! @return Growisofs executable path.
    //!
    QString growisofsExplicitExecutable() const
    {
        return _growisofsExplicit->fileName();
    }

    //!
    //! Get the default Growisofs executable path.
    //! This is the executable which was automatically detected in the system.
    //! @return Growisofs executable path.
    //!
    QString growisofsDefaultExecutable() const
    {
        return _growisofsDefault->fileName();
    }

    //!
    //! Get the actual Growisofs executable.
    //! This is the command to execute.
    //! @return Actual Growisofs executable, never null.
    //!
    const QtlMovieExecFile* growisofs() const
    {
        return _growisofsExplicit->isSet() ? _growisofsExplicit : _growisofsDefault;
    }

    //!
    //! Set the explicit Growisofs executable path.
    //! Empty by default, the executable is automatically located.
    //! @param [in] growisofsExecutable Growisofs executable path.
    //!
    void setGrowisofsExplicitExecutable(const QString& growisofsExecutable);

    //!
    //! Get the HTML description of the actual Growisofs product.
    //! @return HTML description.
    //!
    QString growisofsHtmlDescription() const
    {
        return _growisofsExplicit->isSet() ? _growisofsExplicit->htmlDescription() : _growisofsDefault->htmlDescription();
    }

    //!
    //! Get the explicit Telxcc executable path.
    //! This is the path that was manually entered into the settings.
    //! @return Telxcc executable path.
    //!
    QString telxccExplicitExecutable() const
    {
        return _telxccExplicit->fileName();
    }

    //!
    //! Get the default Telxcc executable path.
    //! This is the executable which was automatically detected in the system.
    //! @return Telxcc executable path.
    //!
    QString telxccDefaultExecutable() const
    {
        return _telxccDefault->fileName();
    }

    //!
    //! Get the actual Telxcc executable.
    //! This is the command to execute.
    //! @return Actual Telxcc executable, never null.
    //!
    const QtlMovieExecFile* telxcc() const
    {
        return _telxccExplicit->isSet() ? _telxccExplicit : _telxccDefault;
    }

    //!
    //! Set the explicit Telxcc executable path.
    //! Empty by default, the executable is automatically located.
    //! @param [in] telxccExecutable Telxcc executable path.
    //!
    void setTelxccExplicitExecutable(const QString& telxccExecutable);

    //!
    //! Get the HTML description of the actual Telxcc product.
    //! @return HTML description.
    //!
    QString telxccHtmlDescription() const
    {
        return _telxccExplicit->isSet() ? _telxccExplicit->htmlDescription() : _telxccDefault->htmlDescription();
    }

    //!
    //! Get the explicit CCExtractor executable path.
    //! This is the path that was manually entered into the settings.
    //! @return CCExtractor executable path.
    //!
    QString ccextractorExplicitExecutable() const
    {
        return _ccextractorExplicit->fileName();
    }

    //!
    //! Get the default CCExtractor executable path.
    //! This is the executable which was automatically detected in the system.
    //! @return CCExtractor executable path.
    //!
    QString ccextractorDefaultExecutable() const
    {
        return _ccextractorDefault->fileName();
    }

    //!
    //! Get the actual CCExtractor executable.
    //! This is the command to execute.
    //! @return Actual CCExtractor executable, never null.
    //!
    const QtlMovieExecFile* ccextractor() const
    {
        return _ccextractorExplicit->isSet() ? _ccextractorExplicit : _ccextractorDefault;
    }

    //!
    //! Set the explicit CCExtractor executable path.
    //! Empty by default, the executable is automatically located.
    //! @param [in] ccextractorExecutable CCExtractor executable path.
    //!
    void setCcextractorExplicitExecutable(const QString& ccextractorExecutable);

    //!
    //! Get the HTML description of the actual CCExtractor product.
    //! @return HTML description.
    //!
    QString ccextractorHtmlDescription() const
    {
        return _ccextractorExplicit->isSet() ? _ccextractorExplicit->htmlDescription() : _ccextractorDefault->htmlDescription();
    }

    //!
    //! Get the explicit DvdDecrypter executable path.
    //! This is the path that was manually entered into the settings.
    //! @return DvdDecrypter executable path.
    //!
    QString dvddecrypterExplicitExecutable() const
    {
        return _dvddecrypterExplicit->fileName();
    }

    //!
    //! Get the default DvdDecrypter executable path.
    //! This is the executable which was automatically detected in the system.
    //! @return DvdDecrypter executable path.
    //!
    QString dvddecrypterDefaultExecutable() const
    {
        return _dvddecrypterDefault->fileName();
    }

    //!
    //! Get the actual DvdDecrypter executable.
    //! This is the command to execute.
    //! @return Actual DvdDecrypter executable, never null.
    //!
    const QtlMovieExecFile* dvddecrypter() const
    {
        return _dvddecrypterExplicit->isSet() ? _dvddecrypterExplicit : _dvddecrypterDefault;
    }

    //!
    //! Set the explicit DvdDecrypter executable path.
    //! Empty by default, the executable is automatically located.
    //! @param [in] dvddecrypterExecutable DvdDecrypter executable path.
    //!
    void setDvdDecrypterExplicitExecutable(const QString& dvddecrypterExecutable);

    //!
    //! Get the HTML description of the actual DvdDecrypter product.
    //! @return HTML description.
    //!
    QString dvddecrypterHtmlDescription() const
    {
        return _dvddecrypterExplicit->isSet() ? _dvddecrypterExplicit->htmlDescription() : _dvddecrypterDefault->htmlDescription();
    }

    //!
    //! Get the maximum lines in the log window.
    //! @return Maximum lines in the log window.
    //!
    int maxLogLines() const
    {
        return _maxLogLines;
    }

    //!
    //! Set the maximum lines in the log window.
    //! @param [in] maxLogLines Maximum lines in the log window.
    //!
    void setMaxLogLines(int maxLogLines);

    //!
    //! Get the initial input directory.
    //! @return Initial input directory. Never empty (the default is user's home directory).
    //!
    QString initialInputDir() const;

    //!
    //! Set the initial input directory.
    //! @param [in] initialInputDir Set the initial input directory.
    //! Specify an empty string to reset to the default.
    //!
    void setInitialInputDir(const QString& initialInputDir);

    //!
    //! Check if the default output directory shall be the same as the input directory.
    //! @return True if the default output directory shall be the same as the input directory.
    //! False if defaultOutputDir() shall be called to get the output directory for a given
    //! output type.
    //!
    bool defaultOutputDirIsInput() const
    {
        return _defaultOutDirIsInput;
    }

    //!
    //! Specify if the default output directory shall be the same as the input directory.
    //! @param [in] defaultOutDirIsInput If true, the default output directory shall be the
    //! same as the input directory. If false, defaultOutputDir() shall be called to get the
    //! output directory for a given output type.
    //!
    void setDefaultOutputDirIsInput(bool defaultOutDirIsInput);

    //!
    //! Get the default output directory for a given output type.
    //! @param [in] outputType Output type.
    //! @param [in] force When true, return the stored directory value,
    //! even if defaultOutputDirIsInput() is true.
    //! @return The default output directory for the specified output type.
    //! If empty, use the same directory as the input file. Note that an empty
    //! string is always returned when defaultOutputDirIsInput() is true
    //! and @a force is false.
    //!
    QString defaultOutputDir(const QString& outputType, bool force = false) const;

    //!
    //! Set the default output directory for a given output type.
    //! @param [in] outputType Output type.
    //! @param [in] defaultOutDir Default output directory for the specified output type.
    //! If empty, the same directory as the input file will be used for the specified output type.
    //!
    void setDefaultOutputDir(const QString& outputType, const QString& defaultOutDir);

    //!
    //! Get the list of language codes for the intended audience.
    //! @return A list of language codes for the intended audience.
    //!
    QStringList audienceLanguages() const
    {
        return _audienceLanguages;
    }

    //!
    //! Set the list of language codes for the intended audience.
    //! @param [in] audienceLanguages List of language codes for the intended audience.
    //!
    void setAudienceLanguages(const QStringList& audienceLanguages);

    //!
    //! Check if the full video shall be transcoded.
    //! @return If true, the full video shall be transcoded.
    //! If false, use transcodeSeconds() to get the duration to transcode.
    //!
    bool transcodeComplete() const
    {
        return _transcodeComplete;
    }

    //!
    //! Set if the full video shall be transcoded.
    //! @param [in] transcodeComplete If true, the full video shall be transcoded.
    //! If false, use transcodeSeconds() to get the duration to transcode.
    //!
    void setTranscodeComplete(bool transcodeComplete);

    //!
    //! Get the duration of video to transcode.
    //! @return The number of seconds to transcode.
    //!
    int transcodeSeconds() const
    {
        return _transcodeSeconds;
    }

    //!
    //! Set the duration of video to transcode.
    //! @param [in] transcodeSeconds The number of seconds to transcode.
    //!
    void setTranscodeSeconds(int transcodeSeconds);

    //!
    //! Get the video bitrate for DVD output in bits / second.
    //! @return Video bitrate for DVD output in bits / second.
    //!
    int dvdVideoBitRate() const
    {
        return _dvdVideoBitRate;
    }

    //!
    //! Set the video bitrate for DVD output in bits / second.
    //! @param [in] dvdVideoBitRate Video bitrate for DVD output in bits / second.
    //!
    void setDvdVideoBitRate(int dvdVideoBitRate);

    //!
    //! Get the video bitrate for iPad output in bits / second.
    //! @return Video bitrate for iPad output in bits / second.
    //!
    int ipadVideoBitRate() const
    {
        return _ipadVideoBitRate;
    }

    //!
    //! Set the video bitrate for iPad output in bits / second.
    //! @param [in] ipadVideoBitRate Video bitrate for iPad output in bits / second.
    //!
    void setIpadVideoBitRate(int ipadVideoBitRate);

    //!
    //! Get the video bitrate for iPhone output in bits / second.
    //! @return Video bitrate for iPhone output in bits / second.
    //!
    int iphoneVideoBitRate() const
    {
        return _iphoneVideoBitRate;
    }

    //!
    //! Set the video bitrate for iPhone output in bits / second.
    //! @param [in] iphoneVideoBitRate Video bitrate for iPhone output in bits / second.
    //!
    void setIphoneVideoBitRate(int iphoneVideoBitRate);

    //!
    //! Check if the intermediate files shall be kept after completion.
    //! @return If true, the intermediate files shall be kept after completion.
    //! If false, they are automatically deleted.
    //!
    bool keepIntermediateFiles() const
    {
        return _keepIntermediateFiles;
    }

    //!
    //! Set if the intermediate files shall be kept after completion.
    //! @param [in] keepIntermediateFiles If true, the intermediate files shall be kept after completion.
    //! If false, they are automatically deleted.
    //!
    void setKeepIntermediateFiles(bool keepIntermediateFiles);

    //!
    //! Get the initial probe size in media playout seconds for ffprobe / ffmpeg.
    //! @return The initial probe size in media playout seconds for ffprobe / ffmpeg.
    //!
    int ffmpegProbeSeconds() const
    {
        return _ffmpegProbeSeconds;
    }

    //!
    //! Set the initial probe size in media playout seconds for ffprobe / ffmpeg.
    //! @param [in] ffmpegProbeSeconds The initial probe size in media playout seconds for ffprobe / ffmpeg.
    //!
    void setFFmpegProbeSeconds(int ffmpegProbeSeconds);

    //!
    //! Check if the insertion of SRT/SSA/ASS subtitles shall use the original video size as a hint.
    //! @return If true, the video size is used as a hint. If false, the video size is ignored.
    //!
    bool srtUseVideoSizeHint() const
    {
        return _srtUseVideoSizeHint;
    }

    //!
    //! Set if the insertion of SRT/SSA/ASS subtitles shall use the original video size as a hint.
    //! @param [in] srtUseVideoSizeHint If true, the video size is used as a hint. If false, the video size is ignored.
    //!
    void setSrtUseVideoSizeHint(bool srtUseVideoSizeHint);

    //!
    //! Get the duration of chapters to create.
    //! @return The duration of chapters to create in minutes.
    //! If zero, do not create chapters.
    //!
    int chapterMinutes() const
    {
        return _chapterMinutes;
    }

    //!
    //! Set the duration of chapters to create.
    //! @param [in] chapterMinutes The duration of chapters to create in minutes.
    //! If zero, do not create chapters.
    //!
    void setChapterMinutes(int chapterMinutes);

    //!
    //! Report missing media tools in the log.
    //!
    void reportMissingTools() const;

    //!
    //! Check if audio/video must be remuxed after transcode for DVD output.
    //! The initial audio/video transcoding is performed by ffmpeg. The resulting
    //! file may produce synchronization errors when processed by DVD Author.
    //! Using this option, the transcoded file is remuxed by ffmpeg, which
    //! suprinsingly removes the synchronization errors.
    //! @return True if audio/video must be remuxed after transcode for DVD output.
    //!
    bool dvdRemuxAfterTranscode() const
    {
        return _dvdRemuxAfterTranscode;
    }

    //!
    //! Set if audio/video must be remuxed after transcode for DVD output.
    //! @param [in] dvdRemuxAfterTranscode True if audio/video must be remuxed after transcode for DVD output.
    //!
    void setDvdRemuxAfterTranscode(bool dvdRemuxAfterTranscode);

    //!
    //! Get the DVD burner device name.
    //! @return The DVD burner device name.
    //!
    QString dvdBurner() const
    {
        return _dvdBurner;
    }

    //!
    //! Set the DVD burner device name.
    //! @param [in] dvdBurner The DVD burner device name.
    //!
    void setDvdBurner(const QString& dvdBurner);

    //!
    //! Check if the format of created DVD is PAL or NTSC.
    //! @return True if DVD shall be created in PAL format, false in NTSC format.
    //!
    bool createPalDvd() const
    {
        return _createPalDvd;
    }

    //!
    //! Set the format of created DVD (PAL or NTSC).
    //! @param [in] createPalDvd True if DVD shall be created in PAL format, false in NTSC format.
    //!
    void setCreatePalDvd(bool createPalDvd);

    //!
    //! Get the video width of created DVD.
    //! @return The video width of created DVD.
    //!
    int dvdVideoWidth() const
    {
        return _createPalDvd ? QTL_DVD_PAL_VIDEO_WIDTH : QTL_DVD_NTSC_VIDEO_WIDTH;
    }

    //!
    //! Get the video height of created DVD.
    //! @return The video height of created DVD.
    //!
    int dvdVideoHeight() const
    {
        return _createPalDvd ? QTL_DVD_PAL_VIDEO_HEIGHT : QTL_DVD_NTSC_VIDEO_HEIGHT;
    }

    //!
    //! Define the screen size of an iPad.
    //!
    enum IpadScreenSize {
        Ipad12Size,  //!< iPad 1, 2, mini (1024x768)
        Ipad34Size   //!< iPad 3, 4 (2048x1536)
    };

    //!
    //! Get the iPad screen size.
    //! @return The iPad screen size.
    //!
    IpadScreenSize ipadScreenSize() const
    {
        return _ipadScreenSize;
    }

    //!
    //! Set the iPad screen size.
    //! @param [in] ipadScreenSize The iPad screen size.
    //!
    void setIpadScreenSize(IpadScreenSize ipadScreenSize);

    //!
    //! Get the video width of iPad.
    //! @return The video width of iPad.
    //!
    int ipadVideoWidth() const
    {
        return _ipadScreenSize == Ipad12Size ? QTL_IPAD12_VIDEO_WIDTH : QTL_IPAD34_VIDEO_WIDTH;
    }

    //!
    //! Get the video height of iPad.
    //! @return The video height of iPad.
    //!
    int ipadVideoHeight() const
    {
        return _ipadScreenSize == Ipad12Size ? QTL_IPAD12_VIDEO_HEIGHT : QTL_IPAD34_VIDEO_HEIGHT;
    }

    //!
    //! Define the screen size of an iPhone.
    //!
    enum IphoneScreenSize {
        Iphone3Size,  //!< iPhone 1/3G/3GS (480x320)
        Iphone4Size,  //!< iPhone 4/4S (960x640)
        Iphone5Size   //!< iPhone 5/5S (1136x640)
    };

    //!
    //! Get the iPhone screen size.
    //! @return The iPhone screen size.
    //!
    IphoneScreenSize iphoneScreenSize() const
    {
        return _iphoneScreenSize;
    }

    //!
    //! Set the iPhone screen size.
    //! @param [in] iphoneScreenSize The iPhone screen size.
    //!
    void setIphoneScreenSize(IphoneScreenSize iphoneScreenSize);

    //!
    //! Get the video width of iPhone.
    //! @return The video width of iPhone.
    //!
    int iphoneVideoWidth() const;

    //!
    //! Get the video height of iPhone.
    //! @return The video height of iPhone.
    //!
    int iphoneVideoHeight() const;

    //!
    //! Do we force transcoding even if input file is already DVD-compliant.
    //! @return True to force transcoding even if input file is already DVD-compliant.
    //!
    bool forceDvdTranscode() const
    {
        return _forceDvdTranscode;
    }

    //!
    //! Force transcoding even if input file is already DVD-compliant.
    //! @param [in] forceDvdTranscode True to force transcoding even if input file is already DVD-compliant.
    //!
    void setForceDvdTranscode(bool forceDvdTranscode);

    //!
    //! Do we check for a new version of QtlMovie on startup.
    //! @return True to check for a new version of QtlMovie on startup.
    //!
    bool newVersionCheck() const
    {
        return _newVersionCheck;
    }

    //!
    //! Check for a new version of QtlMovie on startup.
    //! @param [in] newVersionCheck True to check for a new version of QtlMovie on startup.
    //!
    void setNewVersionCheck(bool newVersionCheck);

    //!
    //! Get the video bitrate for AVI output in bits / second.
    //! @return The video bitrate for AVI output in bits / second.
    //!
    int aviVideoBitRate() const
    {
        return _aviVideoBitRate;
    }

    //!
    //! Set the video bitrate for AVI output in bits / second.
    //! @param [in] aviVideoBitRate The video bitrate for AVI output in bits / second.
    //!
    void setAviVideoBitRate(int aviVideoBitRate);

    //!
    //! Get the maximum video width for AVI output in pixels.
    //! @return The maximum video width for AVI output in pixels.
    //!
    int aviMaxVideoWidth() const
    {
        return _aviMaxVideoWidth;
    }

    //!
    //! Set the maximum video width for AVI output in pixels.
    //! @param [in] aviMaxVideoWidth The maximum video width for AVI output in pixels.
    //!
    void setAviMaxVideoWidth(int aviMaxVideoWidth);

    //!
    //! Get the maximum video height for AVI output in pixels.
    //! @return The maximum video height for AVI output in pixels.
    //!
    int aviMaxVideoHeight() const
    {
        return _aviMaxVideoHeight;
    }

    //!
    //! Set the maximum video height for AVI output in pixels.
    //! @param [in] aviMaxVideoHeight The maximum video height for AVI output in pixels.
    //!
    void setAviMaxVideoHeight(int aviMaxVideoHeight);

    //!
    //! Check if audio level shall be normalized.
    //! @return True if audio level shall be normalized.
    //!
    bool audioNormalize() const
    {
        return _audioNormalize;
    }

    //!
    //! Set if audio level shall be normalized.
    //! @param [in] audioNormalize True if audio level shall be normalized.
    //!
    void setAudioNormalize(bool audioNormalize);

    //!
    //! Get the target mean level in dBFS for audio normalization.
    //! @return The target mean level in dBFS for audio normalization.
    //!
    int audioNormalizeMean() const
    {
        return _audioNormalizeMean;
    }

    //!
    //! Set the target mean level in dBFS for audio normalization.
    //! @param [in] audioNormalizeMean The target mean level in dBFS for audio normalization.
    //!
    void setAudioNormalizeMean(int audioNormalizeMean);

    //!
    //! Get the target peak level in dBFS for audio normalization.
    //! @return The target peak level in dBFS for audio normalization.
    //!
    int audioNormalizePeak() const
    {
        return _audioNormalizePeak;
    }

    //!
    //! Set the target peak level in dBFS for audio normalization.
    //! @param [in] audioNormalizePeak The target peak level in dBFS for audio normalization.
    //!
    void setAudioNormalizePeak(int audioNormalizePeak);

    //!
    //! Define how to normalize the audio when the input dymanic range is too large.
    //!
    enum AudioNormalizeMode {
        Compress,   //!< Align to target mean level, compress the dynamic range.
        AlignPeak,  //!< Align to target peak level, mean level will be lower than the target value.
        Clip        //!< Align to target mean level, clip high audio,
    };

    //!
    //! Get how to normalize audio when the input dynamic range is too large.
    //! @return How to normalize audio when the input dynamic range is too large.
    //!
    AudioNormalizeMode audioNormalizeMode() const
    {
        return _audioNormalizeMode;
    }

    //!
    //! Set how to normalize audio when the input dynamic range is too large.
    //! @param [in] audioNormalizeMode How to normalize audio when the input dynamic range is too large.
    //!
    void setAudioNormalizeMode(AudioNormalizeMode audioNormalizeMode);

    //!
    //! Check if automatic rotation of video shall be applied.
    //! @return True if automatic rotation of video shall be applied.
    //!
    bool autoRotateVideo() const
    {
        return _autoRotateVideo;
    }

    //!
    //! Set if automatic rotation of video shall be applied.
    //! @param [in] autoRotateVideo True if automatic rotation of video shall be applied.
    //!
    void setAutoRotateVideo(bool autoRotateVideo);

    //!
    //! Check if a sound shall be played on transcoding completion.
    //! @return True if a sound shall be played on transcoding completion.
    //!
    bool playSoundOnCompletion() const
    {
        return _playSoundOnCompletion;
    }

    //!
    //! Set if a sound shall be played on transcoding completion.
    //! @param [in] playSoundOnCompletion True if a sound shall be played on transcoding completion.
    //!
    void setPlaySoundOnCompletion(bool playSoundOnCompletion);

private:
    bool                  _isModified;             //!< Object has unsaved changes.
    QtlLogger*            _log;                    //!< Where to log errors.
    QString               _defaultFileName;        //!< Default XML file name for the settings.
    QString               _fileError;              //!< Last error message from file load or save.
    QtlMovieExecFile*     _ffmpegDefault;          //!< FFmpeg default executable description.
    QtlMovieExecFile*     _ffmpegExplicit;         //!< FFmpeg explicit executable description.
    QtlMovieExecFile*     _ffprobeDefault;         //!< FFprobe default executable description.
    QtlMovieExecFile*     _ffprobeExplicit;        //!< FFprobe explicit executable description.
    QtlMovieExecFile*     _dvdauthorDefault;       //!< DvdAuthor default executable description.
    QtlMovieExecFile*     _dvdauthorExplicit;      //!< DvdAuthor explicit executable description.
    QtlMovieExecFile*     _mkisofsDefault;         //!< Mkisofs default executable description.
    QtlMovieExecFile*     _mkisofsExplicit;        //!< Mkisofs explicit executable description.
    QtlMovieExecFile*     _growisofsDefault;       //!< Growisofs default executable description.
    QtlMovieExecFile*     _growisofsExplicit;      //!< Growisofs explicit executable description.
    QtlMovieExecFile*     _telxccDefault;          //!< Telxcc default executable description.
    QtlMovieExecFile*     _telxccExplicit;         //!< Telxcc explicit executable description.
    QtlMovieExecFile*     _ccextractorDefault;     //!< CCExtractor default executable description.
    QtlMovieExecFile*     _ccextractorExplicit;    //!< CCExtractor explicit executable description.
    QtlMovieExecFile*     _dvddecrypterDefault;    //!< DvdDecrypter default executable description.
    QtlMovieExecFile*     _dvddecrypterExplicit;   //!< DvdDecrypter explicit executable description.
    int                   _maxLogLines;            //!< Maximum lines in the log window.
    QString               _initialInputDir;        //!< Initial input directory.
    bool                  _defaultOutDirIsInput;   //!< Default output directory is same as input.
    QMap<QString,QString> _defaultOutDir;          //!< Default output directory by output type.
    QString               _dvdBurner;              //!< Device name of DVD burner.
    QStringList           _audienceLanguages;      //!< List of intended audience languages.
    bool                  _transcodeComplete;      //!< Transcode complete video file.
    int                   _transcodeSeconds;       //!< Max time to transcode when _transcodeComplete is false.
    int                   _dvdVideoBitRate;        //!< Video bitrate for DVD output in bits / second.
    int                   _ipadVideoBitRate;       //!< Video bitrate for iPad output in bits / second.
    int                   _iphoneVideoBitRate;     //!< Video bitrate for iPhone output in bits / second.
    bool                  _keepIntermediateFiles;  //!< Do not delete intermediate files after transcoding.
    int                   _ffmpegProbeSeconds;     //!< Initial probe size in media playout seconds for ffprobe / ffmpeg.
    bool                  _srtUseVideoSizeHint;    //!< Insertion of SRT/SSA/ASS subtitles uses original video size as a hint.
    int                   _chapterMinutes;         //!< Create chapters of this number of minutes.
    bool                  _dvdRemuxAfterTranscode; //!< Remux audio/video after transcode to DVD.
    bool                  _createPalDvd;           //!< Create DVD in PAL format (false: NTSC).
    IpadScreenSize        _ipadScreenSize;         //!< iPad screen size.
    IphoneScreenSize      _iphoneScreenSize;       //!< iPhone screen size.
    bool                  _forceDvdTranscode;      //!< Force transcoding even if input file is already DVD-compliant.
    bool                  _newVersionCheck;        //!< Automatically check new version on startup.
    int                   _aviVideoBitRate;        //!< Video bitrate for AVI output in bits / second.
    int                   _aviMaxVideoWidth;       //!< Maximum video width for AVI output in pixels.
    int                   _aviMaxVideoHeight;      //!< Maximum video height for AVI output in pixels.
    bool                  _audioNormalize;         //!< Normalize audio level.
    int                   _audioNormalizeMean;     //!< Target mean level in dBFS for audio normalization.
    int                   _audioNormalizePeak;     //!< Target peak level in dBFS for audio normalization.
    AudioNormalizeMode    _audioNormalizeMode;     //!< How to normalize audio when the input dynamic range is too large.
    bool                  _autoRotateVideo;        //!< Use "rotate" metadata to rotate output video.
    bool                  _playSoundOnCompletion;  //!< Play a sound on transcoding completion.

    //!
    //! Write an XML element with a "value" integer attribute.
    //! @param [in,out] xml XML stream.
    //! @param [in] name Element name.
    //! @param [in] value Integer value.
    //!
    static void setIntAttribute(QXmlStreamWriter& xml, const QString& name, int value);

    //!
    //! Write an XML element with a "value" string attribute.
    //! @param [in,out] xml XML stream.
    //! @param [in] name Element name.
    //! @param [in] value String value.
    //! @param [in] type Option "type" attribute. Not set if empty.
    //!
    static void setStringAttribute(QXmlStreamWriter& xml, const QString& name, const QString& value, const QString& type = QString());

    //!
    //! Write an XML element with a "value" boolean attribute.
    //! @param [in,out] xml XML stream.
    //! @param [in] name Element name.
    //! @param [in] value Boolean value.
    //!
    static void setBoolAttribute(QXmlStreamWriter& xml, const QString& name, bool value);

    //!
    //! Decode an XML element with a "value" integer attribute.
    //! @param [in] xml XML stream.
    //! @param [in] name Expected element name.
    //! @param [in,out] value Integer value. Unchanged on error.
    //! @return True on success, false on error: not the expected element,
    //! "value" attribute not found, not a valid integer.
    //!
    static bool getIntAttribute(QXmlStreamReader& xml, const QString& name, int& value);

    //!
    //! Decode an XML element with a "value" string attribute.
    //! @param [in] xml XML stream.
    //! @param [in] name Expected element name.
    //! @param [in,out] value String value. Unchanged on error.
    //! @return True on success, false on error: not the expected element, "value" attribute not found.
    //!
    static bool getStringAttribute(QXmlStreamReader& xml, const QString& name, QString& value);

    //!
    //! Decode an XML element with "type" and "value" string attributes.
    //! @param [in] xml XML stream.
    //! @param [in] name Expected element name.
    //! @param [in,out] valueMap String map. Unchanged on error.
    //! @return True on success, false on error: not the expected element, "type" or "value" attribute not found.
    //!
    static bool getStringAttribute(QXmlStreamReader& xml, const QString& name, QMap<QString,QString>& valueMap);

    //!
    //! Decode an XML element with a "value" boolean attribute.
    //! @param [in] xml XML stream.
    //! @param [in] name Expected element name.
    //! @param [in,out] value Boolean value. Unchanged on error.
    //! @return True on success, false on error: not the expected element,
    //! "value" attribute not found, not a valid boolean.
    //!
    static bool getBoolAttribute(QXmlStreamReader& xml, const QString& name, bool& value);

    //!
    //! "Normalize" a string list: lower case, sorted.
    //! @param list List of strings to normalize.
    //!
    void normalize(QStringList& list);
};

#endif // QTLMOVIESETTINGS_H
