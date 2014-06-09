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
#include <QWidget>
#include "QtlMovieExecFile.h"
#include "QtlGeometrySettingsInterface.h"
#include "QtlLogger.h"
#include "QtlMovie.h"

//!
//! Description of the global settings of the application.
//! Refer the QtlMovie user guide for the definition of each setting.
//!
class QtlMovieSettings : public QObject, public QtlGeometrySettingsInterface
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
    //! Make sure that all settings are properly saved on disk immediately.
    //! This is automatically done by Qt from time to time anyway.
    //!
    void sync()
    {
        _settings.sync();
    }

    //!
    //! Report missing media tools in the log.
    //!
    void reportMissingTools() const;

    //!
    //! Define the screen size of an iPad.
    //!
    enum IpadScreenSize {
        Ipad12Size,  //!< iPad 1, 2, mini (1024x768)
        Ipad34Size   //!< iPad 3, 4 (2048x1536)
    };

    //!
    //! Define the screen size of an iPhone.
    //!
    enum IphoneScreenSize {
        Iphone3Size,  //!< iPhone 1/3G/3GS (480x320)
        Iphone4Size,  //!< iPhone 4/4S (960x640)
        Iphone5Size   //!< iPhone 5/5S (1136x640)
    };

    //!
    //! Define how to normalize the audio when the input dymanic range is too large.
    //!
    enum AudioNormalizeMode {
        Compress,   //!< Align to target mean level, compress the dynamic range.
        AlignPeak,  //!< Align to target peak level, mean level will be lower than the target value.
        Clip        //!< Align to target mean level, clip high audio,
    };

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
    QStringList audienceLanguages() const;

    //!
    //! Set the list of language codes for the intended audience.
    //! @param [in] audienceLanguages List of language codes for the intended audience.
    //!
    void setAudienceLanguages(const QStringList& audienceLanguages);

    //!
    //! Get the video width of created DVD.
    //! @return The video width of created DVD.
    //!
    int dvdVideoWidth() const;

    //!
    //! Get the video height of created DVD.
    //! @return The video height of created DVD.
    //!
    int dvdVideoHeight() const;

    //!
    //! Get the video width of iPad.
    //! @return The video width of iPad.
    //!
    int ipadVideoWidth() const;

    //!
    //! Get the video height of iPad.
    //! @return The video height of iPad.
    //!
    int ipadVideoHeight() const;

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
    //! Save the geometry of a widget.
    //! Implementation of QtlGeometrySettingsInterface.
    //! @param [in] widget The widget to save the geometry of.
    //!
    virtual void saveGeometry(const QWidget* widget);

    //!
    //! Restore the geometry of a widget.
    //! Implementation of QtlGeometrySettingsInterface.
    //! @param [in,out] widget The widget to restore the geometry.
    //!
    virtual void restoreGeometry(QWidget* widget);

    //
    // Inlined definitions of the basic getters and setters.
    // These accessors simply load and store the option without further processing.
    // There is one macro per option. The macro defines the getter and the setter.
    //
#define QTLMOVIE_SETTINGS_INT(getter, setter, defaultValue) \
    int getter() const {return intValue(#getter, defaultValue);} \
    void setter(int x) {_settings.setValue(#getter, x);}

#define QTLMOVIE_SETTINGS_ENUM(getter, setter, enumType, defaultValue) \
    enumType getter() const {return static_cast<enumType>(intValue(#getter, static_cast<int>(defaultValue)));} \
    void setter(enumType x) {_settings.setValue(#getter, static_cast<int>(x));}

#define QTLMOVIE_SETTINGS_BOOL(getter, setter, defaultValue) \
    bool getter() const {return boolValue(#getter, defaultValue);} \
    void setter(bool x) {_settings.setValue(#getter, x);}

#define QTLMOVIE_SETTINGS_STRING(getter, setter, defaultValue)      \
    QString getter() const {return _settings.value(#getter, defaultValue).toString();} \
    void setter(const QString& x) {_settings.setValue(#getter, x);}

    QTLMOVIE_SETTINGS_INT(maxLogLines, setMaxLogLines, QTL_MAX_LOG_LINE)
    QTLMOVIE_SETTINGS_BOOL(defaultOutputDirIsInput, setDefaultOutputDirIsInput, QTL_DEFAULT_OUTDIR_INPUT)
    QTLMOVIE_SETTINGS_BOOL(transcodeComplete, setTranscodeComplete, QTL_TRANSCODE_COMPLETE)
    QTLMOVIE_SETTINGS_INT(transcodeSeconds, setTranscodeSeconds, QTL_TRANSCODE_SECONDS)
    QTLMOVIE_SETTINGS_INT(dvdVideoBitRate, setDvdVideoBitRate, QTL_DVD_DEFAULT_VIDEO_BITRATE)
    QTLMOVIE_SETTINGS_INT(ipadVideoBitRate, setIpadVideoBitRate, QTL_IPAD_DEFAULT_VIDEO_BITRATE)
    QTLMOVIE_SETTINGS_INT(iphoneVideoBitRate, setIphoneVideoBitRate, QTL_IPHONE_DEFAULT_VIDEO_BITRATE)
    QTLMOVIE_SETTINGS_BOOL(keepIntermediateFiles, setKeepIntermediateFiles, QTL_KEEP_INTERMEDIATE_FILES)
    QTLMOVIE_SETTINGS_INT(ffmpegProbeSeconds, setFFmpegProbeSeconds, QTL_FFMPEG_PROBE_SECONDS)
    QTLMOVIE_SETTINGS_BOOL(srtUseVideoSizeHint, setSrtUseVideoSizeHint, QTL_SRT_USE_VIDEO_SIZE_HINT)
    QTLMOVIE_SETTINGS_INT(chapterMinutes, setChapterMinutes, QTL_CHAPTER_MINUTES)
    QTLMOVIE_SETTINGS_BOOL(dvdRemuxAfterTranscode, setDvdRemuxAfterTranscode, QTL_DVD_REMUX_AFTER_TRANSCODE)
    QTLMOVIE_SETTINGS_STRING(dvdBurner, setDvdBurner, "")
    QTLMOVIE_SETTINGS_BOOL(createPalDvd, setCreatePalDvd, QTL_CREATE_PAL_DVD)
    QTLMOVIE_SETTINGS_ENUM(ipadScreenSize, setIpadScreenSize, IpadScreenSize, QTL_IPAD_SCREEN_SIZE)
    QTLMOVIE_SETTINGS_ENUM(iphoneScreenSize, setIphoneScreenSize, IphoneScreenSize, QTL_IPHONE_SCREEN_SIZE)
    QTLMOVIE_SETTINGS_BOOL(forceDvdTranscode, setForceDvdTranscode, QTL_FORCE_DVD_TRANSCODE)
    QTLMOVIE_SETTINGS_BOOL(newVersionCheck, setNewVersionCheck, QTL_NEW_VERSION_CHECK)
    QTLMOVIE_SETTINGS_INT(aviVideoBitRate, setAviVideoBitRate, QTL_AVI_DEFAULT_VIDEO_BITRATE)
    QTLMOVIE_SETTINGS_INT(aviMaxVideoWidth, setAviMaxVideoWidth, QTL_AVI_DEFAULT_MAX_VIDEO_WIDTH)
    QTLMOVIE_SETTINGS_INT(aviMaxVideoHeight, setAviMaxVideoHeight, QTL_AVI_DEFAULT_MAX_VIDEO_HEIGHT)
    QTLMOVIE_SETTINGS_BOOL(audioNormalize, setAudioNormalize, QTL_DEFAULT_AUDIO_NORMALIZE)
    QTLMOVIE_SETTINGS_INT(audioNormalizeMean, setAudioNormalizeMean, QTL_DEFAULT_AUDIO_MEAN_LEVEL)
    QTLMOVIE_SETTINGS_INT(audioNormalizePeak, setAudioNormalizePeak, QTL_DEFAULT_AUDIO_PEAK_LEVEL)
    QTLMOVIE_SETTINGS_ENUM(audioNormalizeMode, setAudioNormalizeMode, AudioNormalizeMode, QTL_AUDIO_NORMALIZE_MODE)
    QTLMOVIE_SETTINGS_BOOL(autoRotateVideo, setAutoRotateVideo, QTL_AUTO_ROTATE_VIDEO)
    QTLMOVIE_SETTINGS_BOOL(playSoundOnCompletion, setPlaySoundOnCompletion, QTL_PLAY_SOUND_ON_COMPLETION)
    QTLMOVIE_SETTINGS_BOOL(clearLogBeforeTranscode, setClearLogBeforeTranscode, QTL_CLEAR_LOG_BEFORE_TRANSCODE)
    QTLMOVIE_SETTINGS_BOOL(saveLogAfterTranscode, setSaveLogAfterTranscode, QTL_SAVE_LOG_AFTER_TRANSCODE)
    QTLMOVIE_SETTINGS_STRING(logFileExtension, setLogFileExtension, QTL_LOG_FILE_EXTENSION)
    QTLMOVIE_SETTINGS_INT(ffprobeExecutionTimeout, setFFprobeExecutionTimeout, QTL_FFPROBE_EXECUTION_TIMEOUT)

    //
    // Inlined definitions of the getters and setters for media tools executable.
    // For each tool, there are three getters: default executable, explicitly set executable
    // and actual executable from the previous two. There is only one setter, for the explicitly set executable.
    //
#define QTL_SETTINGS_EXEC(getName,setName) \
    QString getName##ExplicitExecutable() const; \
    void set##setName##ExplicitExecutable(const QString& getName##Executable); \
    QString getName##DefaultExecutable() const {return _##getName##Default->fileName();} \
    const QtlMovieExecFile* getName() const {getName##ExplicitExecutable(); return _##getName##Explicit->isSet() ? _##getName##Explicit : _##getName##Default;}

    QTL_SETTINGS_EXEC(ffmpeg, FFmpeg)
    QTL_SETTINGS_EXEC(ffprobe, FFprobe)
    QTL_SETTINGS_EXEC(dvdauthor, DvdAuthor)
    QTL_SETTINGS_EXEC(mkisofs, Mkisofs)
    QTL_SETTINGS_EXEC(growisofs, Growisofs)
    QTL_SETTINGS_EXEC(telxcc, Telxcc)
    QTL_SETTINGS_EXEC(ccextractor, CCextractor)
    QTL_SETTINGS_EXEC(dvddecrypter, DvdDecrypter)

private:
    QtlLogger*        _log;                   //!< Where to log errors.
    QSettings         _settings;              //!< Settings in Qt format.
    QtlMovieExecFile* _ffmpegDefault;         //!< FFmpeg default executable description.
    QtlMovieExecFile* _ffmpegExplicit;        //!< FFmpeg explicit executable description.
    QtlMovieExecFile* _ffprobeDefault;        //!< FFprobe default executable description.
    QtlMovieExecFile* _ffprobeExplicit;       //!< FFprobe explicit executable description.
    QtlMovieExecFile* _dvdauthorDefault;      //!< DvdAuthor default executable description.
    QtlMovieExecFile* _dvdauthorExplicit;     //!< DvdAuthor explicit executable description.
    QtlMovieExecFile* _mkisofsDefault;        //!< Mkisofs default executable description.
    QtlMovieExecFile* _mkisofsExplicit;       //!< Mkisofs explicit executable description.
    QtlMovieExecFile* _growisofsDefault;      //!< Growisofs default executable description.
    QtlMovieExecFile* _growisofsExplicit;     //!< Growisofs explicit executable description.
    QtlMovieExecFile* _telxccDefault;         //!< Telxcc default executable description.
    QtlMovieExecFile* _telxccExplicit;        //!< Telxcc explicit executable description.
    QtlMovieExecFile* _ccextractorDefault;    //!< CCExtractor default executable description.
    QtlMovieExecFile* _ccextractorExplicit;   //!< CCExtractor explicit executable description.
    QtlMovieExecFile* _dvddecrypterDefault;   //!< DvdDecrypter default executable description.
    QtlMovieExecFile* _dvddecrypterExplicit;  //!< DvdDecrypter explicit executable description.

    //!
    //! Get an integer value from the settings.
    //! @param [in] key Key in the QSettings
    //! @param [in] defaultValue Default value if undefined or incorrectly defined.
    //! @return The final value.
    //!
    int intValue(const QString& key, int defaultValue) const;

    //!
    //! Get a boolean value from the settings.
    //! @param [in] key Key in the QSettings
    //! @param [in] defaultValue Default value if undefined or incorrectly defined.
    //! @return The final value.
    //!
    bool boolValue(const QString& key, bool defaultValue) const;

    //!
    //! "Normalize" a string list: lower case, sorted.
    //! @param list List of strings to normalize.
    //!
    static void normalize(QStringList& list);
};

#endif // QTLMOVIESETTINGS_H
