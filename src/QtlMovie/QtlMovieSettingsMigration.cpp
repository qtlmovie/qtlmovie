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
// Define the class QtlMovieSettingsMigration.
//
//----------------------------------------------------------------------------

#include "QtlMovieSettingsMigration.h"
#include "QtlStringUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieSettingsMigration::QtlMovieSettingsMigration(QtlMovieSettings* settings, QtlLogger* log) :
    _settings(settings),
    _xml()
{
    // XML file containing the old configuration.
    const QString oldFileName(QDir::homePath() + QDir::separator() + ".qtlmovie");

    // If no old file was found, there is nothing to do.
    if (!QFileInfo(oldFileName).exists()) {
        return;
    }

    // Start the migration of the setting.
    log->line(QObject::tr("Migrating the QtlMovie settings to the new format"));

    // Open the old configuration file.
    QFile oldFile(oldFileName);
    if (!oldFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        log->line(QObject::tr("Error opening %1, migration not done, old settings are ignored").arg(oldFileName), QColor("red"));
        return;
    }

    // Read the XML document.
    _xml.setDevice(&oldFile);
    if (_xml.readNextStartElement()) {
        // Check the value of the root element.
        if (_xml.name() != "QtlMovieSettings" || _xml.attributes().value("version") != "1.0") {
            _xml.raiseError(QObject::tr("The file is not a valid QtlMovie settings file"));
        }
        else {
            // Read all settings.
            while (_xml.readNextStartElement()) {
                if (!getString  ("ffmpegExecutable",       &QtlMovieSettings::setFFmpegExplicitExecutable) &&
                    !getString  ("ffprobeExecutable",      &QtlMovieSettings::setFFprobeExplicitExecutable) &&
                    !getString  ("dvdauthorExecutable",    &QtlMovieSettings::setDvdAuthorExplicitExecutable) &&
                    !getString  ("mkisofsExecutable",      &QtlMovieSettings::setMkisofsExplicitExecutable) &&
                    !getString  ("growisofsExecutable",    &QtlMovieSettings::setGrowisofsExplicitExecutable) &&
                    !getString  ("ccextractorExecutable",  &QtlMovieSettings::setCCextractorExplicitExecutable) &&
                    !getString  ("dvddecrypterExecutable", &QtlMovieSettings::setDvdDecrypterExplicitExecutable) &&
                    !getInt     ("maxLogLines",            &QtlMovieSettings::setMaxLogLines) &&
                    !getString  ("initialInputDir",        &QtlMovieSettings::setInitialInputDir) &&
                    !getBool    ("defaultOutDirIsInput",   &QtlMovieSettings::setDefaultOutputDirIsInput) &&
                    !getString2 ("defaultOutDir",          &QtlMovieSettings::setDefaultOutputDir) &&
                    !getString  ("dvdBurner",              &QtlMovieSettings::setDvdBurner) &&
                    !getList    ("audienceLanguages",      &QtlMovieSettings::setAudienceLanguages) &&
                    !getBool    ("transcodeComplete",      &QtlMovieSettings::setTranscodeComplete) &&
                    !getInt     ("transcodeSeconds",       &QtlMovieSettings::setTranscodeSeconds) &&
                    !getInt     ("dvdVideoBitRate",        &QtlMovieSettings::setDvdVideoBitRate) &&
                    !getBool    ("keepIntermediateFiles",  &QtlMovieSettings::setKeepIntermediateFiles) &&
                    !getInt     ("ffmpegProbeSeconds",     &QtlMovieSettings::setFFmpegProbeSeconds) &&
                    !getBool    ("srtUseVideoSizeHint",    &QtlMovieSettings::setSrtUseVideoSizeHint) &&
                    !getInt     ("chapterMinutes",         &QtlMovieSettings::setChapterMinutes) &&
                    !getBool    ("dvdRemuxAfterTranscode", &QtlMovieSettings::setDvdRemuxAfterTranscode) &&
                    !getBool    ("createPalDvd",           &QtlMovieSettings::setCreatePalDvd) &&
                    !getInt     ("ipadScreenSize",         &QtlMovieSettings::setIpadScreenSize) &&
                    !getInt     ("iphoneScreenSize",       &QtlMovieSettings::setIphoneScreenSize) &&
                    !getBool    ("forceDvdTranscode",      &QtlMovieSettings::setForceDvdTranscode) &&
                    !getBool    ("newVersionCheck",        &QtlMovieSettings::setNewVersionCheck) &&
                    !getInt     ("aviMaxVideoWidth",       &QtlMovieSettings::setAviMaxVideoWidth) &&
                    !getInt     ("aviMaxVideoHeight",      &QtlMovieSettings::setAviMaxVideoHeight) &&
                    !getBool    ("audioNormalize",         &QtlMovieSettings::setAudioNormalize) &&
                    !getInt     ("audioNormalizeMean",     &QtlMovieSettings::setAudioNormalizeMean) &&
                    !getInt     ("audioNormalizePeak",     &QtlMovieSettings::setAudioNormalizePeak) &&
                    !getInt     ("audioNormalizeMode",     &QtlMovieSettings::setAudioNormalizeMode) &&
                    !getBool    ("autoRotateVideo",        &QtlMovieSettings::setAutoRotateVideo) &&
                    !getBool    ("playSoundOnCompletion",  &QtlMovieSettings::setPlaySoundOnCompletion) &&
                    !getGeometry("geometry",               &QtlMovieSettings::saveGeometry) &&
                    !_xml.error()) {
                    // Unexpected element, ignore it.
                    _xml.skipCurrentElement();
                }
            }
        }
    }

    // Make sure the new settings are saved.
    _settings->sync();

    // Process end of XML stream.
    oldFile.close();
    if (_xml.error()) {
        // Format an error string.
        log->line(QObject::tr("Error loading %1\n%2\nLine %3, column %4")
                  .arg(oldFileName)
                  .arg(_xml.errorString())
                  .arg(_xml.lineNumber())
                  .arg(_xml.columnNumber()),
                  QColor("red"));
        return;
    }

    // End of migration, rename the old file.
    const QString newFileName(oldFileName + ".old");
    if (QFile(oldFileName).rename(newFileName)) {
        log->line(QObject::tr("Old settings file was saved as %1 but you may remove it").arg(newFileName));
    }
    else {
        log->line(QObject::tr("Error renaming the old settings file %1 but you may remove it").arg(oldFileName), QColor("red"));
    }
}


//----------------------------------------------------------------------------
// Decode XML elements with attributes of various types.
//----------------------------------------------------------------------------

bool QtlMovieSettingsMigration::getString(const QString& name, void (QtlMovieSettings::*setter)(const QString&))
{
    if (_xml.name() != name) {
        return false;
    }
    else {
        (_settings->*setter)(_xml.attributes().value("value").toString());
        _xml.skipCurrentElement();
        return true;
    }
}

bool QtlMovieSettingsMigration::getString2(const QString& name, void (QtlMovieSettings::*setter)(const QString&, const QString&))
{
    if (_xml.name() != name) {
        return false;
    }
    else {
        const QString type(_xml.attributes().value("type").toString());
        const QString value(_xml.attributes().value("value").toString());
        _xml.skipCurrentElement();
        if (type.isEmpty()) {
            _xml.raiseError(QObject::tr("No attribute \"%1\" in element <%2>").arg("type").arg(name));
            return false;
        }
        else {
            (_settings->*setter)(type, value);
            return true;
        }
    }
}

bool QtlMovieSettingsMigration::getList(const QString& name, void (QtlMovieSettings::*setter)(const QStringList&))
{
    if (_xml.name() != name) {
        return false;
    }
    else {
        (_settings->*setter)(_xml.attributes().value("value").toString().split(',', QString::SkipEmptyParts));
        _xml.skipCurrentElement();
        return true;
    }
}

bool QtlMovieSettingsMigration::getBool(const QString& name, void (QtlMovieSettings::*setter)(bool))
{
    if (_xml.name() != name) {
        return false;
    }
    else {
        const QString stringValue(_xml.attributes().value("value").toString().toLower().trimmed());
        _xml.skipCurrentElement();
        if (stringValue == "true") {
            (_settings->*setter)(true);
            return true;
        }
        else if (stringValue == "false") {
            (_settings->*setter)(false);
            return true;
        }
        else {
            _xml.raiseError(QObject::tr("Invalid boolean value %1 in element <%2>").arg(stringValue).arg(name));
            return false;
        }
    }
}

bool QtlMovieSettingsMigration::getGeometry(const QString& name, void (QtlMovieSettings::*setter)(const QWidget*))
{
    if (_xml.name() != name) {
        return false;
    }

    // Get the attributes, skip the rest of the element.
    const QString objectName(_xml.attributes().value("widget").toString());
    const int x = qtlToInt(_xml.attributes().value("x").toString().trimmed());
    const int y = qtlToInt(_xml.attributes().value("y").toString().trimmed());
    const int width = qtlToInt(_xml.attributes().value("width").toString().trimmed());
    const int height = qtlToInt(_xml.attributes().value("height").toString().trimmed());
    _xml.skipCurrentElement();

    // Check that the geometry is valid.
    if (objectName.isEmpty() || x < 0 || y < 0 || width < 0 || height < 0) {
        _xml.raiseError(QObject::tr("Invalid geometry syntax in element <%1>").arg(name));
        return false;
    }

    // Create an unused widget with that name and geometry to invoke the setter.
    QWidget widget;
    widget.setObjectName(objectName);
    widget.setGeometry(QRect(x, y, width, height));
    (_settings->*setter)(&widget);
    return true;
}
