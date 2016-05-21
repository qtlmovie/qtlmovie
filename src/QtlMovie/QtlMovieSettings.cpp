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
// Define the QtlMovieSettings class, the description of the global settings
// of the application.
//
//----------------------------------------------------------------------------

#include "QtlMovieSettings.h"
#include "QtlMovieSettingsMigration.h"
#include "QtlStringList.h"
#include "QtlOpticalDrive.h"
#include "QtlMovie.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieSettings::QtlMovieSettings(QtlLogger* log, QObject* parent) :
    QtlSettings("QtlMovie", "QtlMovie", parent),
    _log(log),
    _ffmpegDefault(new QtlMovieExecFile("FFmpeg",
                                        "http://ffmpeg.org/",
                                        "http://ffmpeg.zeranoe.com/builds/",
                                        "ffmpeg",
                                        QStringList("-version"),
                                        QString(),
                                        QString(),
                                        log,
                                        this)),
    _ffmpegExplicit(new QtlMovieExecFile(*_ffmpegDefault, "", this)),
    _ffprobeDefault(new QtlMovieExecFile("FFprobe",
                                         "http://ffmpeg.org/",
                                         "http://ffmpeg.zeranoe.com/builds/",
                                         "ffprobe",
                                         QStringList("-version"),
                                         QString(),
                                         QString(),
                                         log,
                                         this)),
    _ffprobeExplicit(new QtlMovieExecFile(*_ffprobeDefault, "", this)),
    _dvdauthorDefault(new QtlMovieExecFile("DvdAuthor",
                                           "http://dvdauthor.sourceforge.net/",
                                           "http://www.paehl.com/open_source/?DVDAuthor",
                                           "dvdauthor",
                                           QStringList("-h"),
                                           QString(),
                                           "INFO: ",
                                           log,
                                           this)),
    _dvdauthorExplicit(new QtlMovieExecFile(*_dvdauthorDefault, "", this)),
    _mkisofsDefault(new QtlMovieExecFile("Mkisofs",
                                         "http://cdrecord.berlios.de/",
                                         "http://fy.chalmers.se/~appro/linux/DVD+RW/tools/win32/",
                                         "mkisofs",
                                         QStringList("-version"),
                                         QString(),
                                         QString(),
                                         log,
                                         this)),
    _mkisofsExplicit(new QtlMovieExecFile(*_mkisofsDefault, "", this)),
    _growisofsDefault(new QtlMovieExecFile("Growisofs",
                                           "http://freecode.com/projects/dvdrw-tools",
                                           "http://fy.chalmers.se/~appro/linux/DVD+RW/tools/win32/",
                                           "growisofs",
                                           QStringList("-version"),
                                           QString(),
                                           QString(),
                                           log,
                                           this)),
    _growisofsExplicit(new QtlMovieExecFile(*_growisofsDefault, "", this)),
    _telxccDefault(new QtlMovieExecFile("Telxcc",
                                        "https://github.com/forers/telxcc",
                                        "https://forers.com/download/telxcc/telxcc-windows-x86-v2.5.1.zip",
                                        "telxcc",
                                        QStringList("-h"),
                                        QString(),
                                        "Usage:",
                                        log,
                                        this)),
    _telxccExplicit(new QtlMovieExecFile(*_telxccDefault, "", this)),
    _ccextractorDefault(new QtlMovieExecFile("CCExtractor",
                                             "http://ccextractor.sourceforge.net/",
                                             "http://sourceforge.net/projects/ccextractor/files/ccextractor/",
                                             "ccextractor",
                                             QStringList("-null"),
                                             QString(),
                                             "----",
                                             log,
                                             this)),
    _ccextractorExplicit(new QtlMovieExecFile(*_ccextractorDefault, "", this)),
    _dvddecrypterDefault(new QtlMovieExecFile("DVD Decrypter",
                                              "http://www.videohelp.com/tools/DVD-Decrypter",
                                              "http://www.videohelp.com/download/SetupDVDDecrypter_3.5.4.0.exe",
                                              "dvddecrypter",
                                              QStringList(),
                                              QString(),
                                              QString(),
                                              log,
                                              this)),
    _dvddecrypterExplicit(new QtlMovieExecFile(*_dvddecrypterDefault, "", this))
{
    Q_ASSERT(log != 0);

    // Convert old-style XML file configuration if one exists.
    QtlMovieSettingsMigration(this, log);

    // If no DVD burner is defined, use the first one in the system by default.
    if (dvdBurner().isEmpty()) {
        setDvdBurner(QtlOpticalDrive::firstDvdWriter().name());
    }
}


//----------------------------------------------------------------------------
// "Normalize" a string list: lower case, sorted.
//----------------------------------------------------------------------------

void QtlMovieSettings::normalize(QStringList& list)
{
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
        *it = it->toLower().trimmed();
    }
    list.sort();
}


//----------------------------------------------------------------------------
// List of language codes for the intended audience.
//----------------------------------------------------------------------------

QStringList QtlMovieSettings::audienceLanguages() const
{
    QStringList languages(_settings.value("audienceLanguages", QTL_DEFAULT_LANGUAGES).toString().trimmed().split(QRegExp("\\s*,\\s*"), QString::SkipEmptyParts));
    normalize(languages);
    return languages;
}

void QtlMovieSettings::setAudienceLanguages(const QStringList& audienceLanguages)
{
    // Apply a normalized list.
    QStringList languages(audienceLanguages);
    normalize(languages);
    _settings.setValue("audienceLanguages", languages.join(","));
}


//----------------------------------------------------------------------------
// Initial input directory.
//----------------------------------------------------------------------------

QString QtlMovieSettings::initialInputDir() const
{
    // The default initial input directory is user's home.
    const QString dir(_settings.value("initialInputDir").toString());
    return dir.isEmpty() ? QtlFile::absoluteNativeFilePath(QDir::homePath()) : dir;
}

void QtlMovieSettings::setInitialInputDir(const QString& initialInputDir)
{
    _settings.setValue("initialInputDir", initialInputDir);
}


//----------------------------------------------------------------------------
// Default output directory for a given output type.
//----------------------------------------------------------------------------

QString QtlMovieSettings::defaultOutputDir(const QString& outputType, bool force) const
{
    return (defaultOutputDirIsInput() && !force) ? "" : _settings.value("defaultOutputDir/" + outputType).toString();
}

void QtlMovieSettings::setDefaultOutputDir(const QString& outputType, const QString& defaultOutDir)
{
    _settings.setValue("defaultOutputDir/" + outputType, defaultOutDir);
}


//----------------------------------------------------------------------------
// Compute a video bitrate from quality indicator and video size.
//----------------------------------------------------------------------------

int QtlMovieSettings::videoBitRate(int qualityIndicator, int width, int height, int frameRate)
{
    // Use 64-bit int intermediate value to avoid overflow.
    return int((qint64(qualityIndicator) * qint64(width) * qint64(height) * qint64(frameRate)) / 100);
}


//----------------------------------------------------------------------------
// Synthetic getters for video width and height.
//----------------------------------------------------------------------------

int QtlMovieSettings::dvdVideoWidth() const
{
    return createPalDvd() ? QTL_DVD_PAL_VIDEO_WIDTH : QTL_DVD_NTSC_VIDEO_WIDTH;
}

int QtlMovieSettings::dvdVideoHeight() const
{
    return createPalDvd() ? QTL_DVD_PAL_VIDEO_HEIGHT : QTL_DVD_NTSC_VIDEO_HEIGHT;
}

int QtlMovieSettings::ipadVideoWidth() const
{
    switch (ipadScreenSize()) {
        case Ipad12Size: return QTL_IPAD12_VIDEO_WIDTH;
        case Ipad34Size: return QTL_IPAD34_VIDEO_WIDTH;
        default: return -1;
    }
}

int QtlMovieSettings::ipadVideoHeight() const
{
    switch (ipadScreenSize()) {
        case Ipad12Size: return QTL_IPAD12_VIDEO_HEIGHT;
        case Ipad34Size: return QTL_IPAD34_VIDEO_HEIGHT;
        default: return -1;
    }
}

int QtlMovieSettings::iphoneVideoWidth() const
{
    switch (iphoneScreenSize()) {
        case Iphone3Size: return QTL_IPHONE3_VIDEO_WIDTH;
        case Iphone4Size: return QTL_IPHONE4_VIDEO_WIDTH;
        case Iphone5Size: return QTL_IPHONE5_VIDEO_WIDTH;
        case Iphone6Size: return QTL_IPHONE6_VIDEO_WIDTH;
        case Iphone6PlusSize: return QTL_IPHONE6P_VIDEO_WIDTH;
        case IphoneSeSize: return QTL_IPHONESE_VIDEO_WIDTH;
        default: return -1;
    }
}

int QtlMovieSettings::iphoneVideoHeight() const
{
    switch (iphoneScreenSize()) {
        case Iphone3Size: return QTL_IPHONE3_VIDEO_HEIGHT;
        case Iphone4Size: return QTL_IPHONE4_VIDEO_HEIGHT;
        case Iphone5Size: return QTL_IPHONE5_VIDEO_HEIGHT;
        case Iphone6Size: return QTL_IPHONE6_VIDEO_HEIGHT;
        case Iphone6PlusSize: return QTL_IPHONE6P_VIDEO_HEIGHT;
        case IphoneSeSize: return QTL_IPHONESE_VIDEO_HEIGHT;
        default: return -1;
    }
}


//----------------------------------------------------------------------------
// Report missing media tools in the log.
//----------------------------------------------------------------------------

void QtlMovieSettings::reportMissingTools() const
{
    if (!ffmpeg()->isSet()) {
        _log->line(tr("Error searching for FFmpeg, install FFmpeg or set explicit path in settings."));
    }
    if (!ffprobe()->isSet()) {
        _log->line(tr("Error searching for FFprobe, install FFmpeg or set explicit path in settings."));
    }
    if (!dvdauthor()->isSet()) {
        _log->line(tr("Error searching for DvdAuthor, install it or set explicit path in settings."));
    }
    if (!mkisofs()->isSet()) {
        _log->line(tr("Error searching for mkisofs, install it or set explicit path in settings."));
    }
    if (!growisofs()->isSet()) {
        _log->line(tr("Error searching for growisofs, install it or set explicit path in settings."));
    }
    if (!telxcc()->isSet()) {
        _log->line(tr("Error searching for telxcc, install it or set explicit path in settings."));
    }
    if (!ccextractor()->isSet()) {
        _log->line(tr("Error searching for CCExtractor, install it or set explicit path in settings."));
    }

    // DVD Decrypter is usually present on Windows only, so don't annoy other OS' users.
#if defined(Q_OS_WIN)
    if (!dvddecrypter()->isSet()) {
        _log->line(tr("Error searching for DVD Decrypter, install it or set explicit path in settings."));
    }
#endif
}


//----------------------------------------------------------------------------
// Media tools executables.
//----------------------------------------------------------------------------

#define QTLMOVIE_SETTINGS_EXEC_DEFINITIONS(getName,setName) \
    QString QtlMovieSettings::getName##ExplicitExecutable() const \
    { \
        _##getName##Explicit->setFileName(_settings.value("tools/" #getName).toString()); \
        return _##getName##Explicit->fileName(); \
    } \
    void QtlMovieSettings::set##setName##ExplicitExecutable(const QString& getName##Executable) { \
        if (_##getName##Explicit->setFileName(getName##Executable)) { \
            _settings.setValue("tools/" #getName, _##getName##Explicit->fileName()); \
        } \
    }

QTLMOVIE_SETTINGS_EXEC_DEFINITIONS(ffmpeg, FFmpeg)
QTLMOVIE_SETTINGS_EXEC_DEFINITIONS(ffprobe, FFprobe)
QTLMOVIE_SETTINGS_EXEC_DEFINITIONS(dvdauthor, DvdAuthor)
QTLMOVIE_SETTINGS_EXEC_DEFINITIONS(mkisofs, Mkisofs)
QTLMOVIE_SETTINGS_EXEC_DEFINITIONS(growisofs, Growisofs)
QTLMOVIE_SETTINGS_EXEC_DEFINITIONS(telxcc, Telxcc)
QTLMOVIE_SETTINGS_EXEC_DEFINITIONS(ccextractor, CCextractor)
QTLMOVIE_SETTINGS_EXEC_DEFINITIONS(dvddecrypter, DvdDecrypter)
