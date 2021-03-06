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
// Define the QtlMovieSettings class, the description of the global settings
// of the application.
//
//----------------------------------------------------------------------------

#include "QtlMovieSettings.h"
#include "QtlMovieSettingsMigration.h"
#include "QtlMovieDeviceProfile.h"
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
    _ccextractorDefault(new QtlMovieExecFile("CCExtractor",
                                             "http://www.ccextractor.org/",
                                             "http://sourceforge.net/projects/ccextractor/files/ccextractor/",
                                             "ccextractor",
                                             QStringList("-null"),
                                             QString(),
                                             "----",
                                             log,
                                             this)),
    _ccextractorExplicit(new QtlMovieExecFile(*_ccextractorDefault, "", this))
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

QtlMovieDeviceProfile QtlMovieSettings::iPad() const
{
    return QtlMovieDeviceProfile::iPadModels.value(iPadScreenSize());
}

QtlMovieDeviceProfile QtlMovieSettings::iPhone() const
{
    return QtlMovieDeviceProfile::iPhoneModels.value(iPhoneScreenSize());
}

QtlMovieDeviceProfile QtlMovieSettings::android() const
{
    return QtlMovieDeviceProfile::androidModels.value(androidScreenSize());
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
    if (!ccextractor()->isSet()) {
        _log->line(tr("Error searching for CCExtractor, install it or set explicit path in settings."));
    }
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
QTLMOVIE_SETTINGS_EXEC_DEFINITIONS(ccextractor, CCextractor)
