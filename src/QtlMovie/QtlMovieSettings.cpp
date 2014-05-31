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
#include "QtlStringList.h"
#include "QtlMovie.h"
#include "QtlUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieSettings::QtlMovieSettings(QtlLogger* log, QObject* parent) :
    QObject(parent),
    _isModified(false),
    _log(log),
    _defaultFileName(QDir::homePath() + QDir::separator() + ".qtlmovie"),
    _fileError(),
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
    _dvddecrypterExplicit(new QtlMovieExecFile(*_dvddecrypterDefault, "", this)),
    _maxLogLines(1000),
    _initialInputDir(),
    _defaultOutDirIsInput(true),
    _defaultOutDir(),
    _dvdBurner(),
    _audienceLanguages(),
    _transcodeComplete(true),
    _transcodeSeconds(0),
    _dvdVideoBitRate(QTL_DVD_DEFAULT_VIDEO_BITRATE),
    _ipadVideoBitRate(QTL_IPAD_DEFAULT_VIDEO_BITRATE),
    _iphoneVideoBitRate(QTL_IPHONE_DEFAULT_VIDEO_BITRATE),
    _keepIntermediateFiles(false),
    _ffmpegProbeSeconds(200),
    _srtUseVideoSizeHint(true),
    _chapterMinutes(5),
    _dvdRemuxAfterTranscode(true),
    _createPalDvd(true),
    _ipadScreenSize(Ipad12Size),
    _iphoneScreenSize(Iphone4Size),
    _forceDvdTranscode(false),
    _newVersionCheck(true),
    _aviVideoBitRate(QTL_AVI_DEFAULT_VIDEO_BITRATE),
    _aviMaxVideoWidth(QTL_AVI_DEFAULT_MAX_VIDEO_WIDTH),
    _aviMaxVideoHeight(QTL_AVI_DEFAULT_MAX_VIDEO_HEIGHT),
    _audioNormalize(QTL_DEFAULT_AUDIO_NORMALIZE),
    _audioNormalizeMean(QTL_DEFAULT_AUDIO_MEAN_LEVEL),
    _audioNormalizePeak(QTL_DEFAULT_AUDIO_PEAK_LEVEL),
    _audioNormalizeMode(Compress),
    _autoRotateVideo(true),
    _playSoundOnCompletion(false),
    _widgetsGeometry()
{
    Q_ASSERT(log != 0);

    // Default audience is French-speaking.
    _audienceLanguages << "fr" << "fre" << "fra" << "french";
    normalize(_audienceLanguages);
}


//----------------------------------------------------------------------------
// Write an XML element with a "value" integer attribute.
//----------------------------------------------------------------------------

void QtlMovieSettings::setIntAttribute(QXmlStreamWriter& xml, const QString& name, int value)
{
    xml.writeStartElement(name);
    xml.writeAttribute("value", QString::number(value));
    xml.writeEndElement();
}


//----------------------------------------------------------------------------
// Write an XML element with a "value" string attribute.
//----------------------------------------------------------------------------

void QtlMovieSettings::setStringAttribute(QXmlStreamWriter& xml, const QString& name, const QString& value, const QString& type)
{
    xml.writeStartElement(name);
    if (!type.isEmpty()) {
        xml.writeAttribute("type", type);
    }
    xml.writeAttribute("value", value);
    xml.writeEndElement();
}


//----------------------------------------------------------------------------
// Write an XML element with a "value" boolean attribute.
//----------------------------------------------------------------------------

void QtlMovieSettings::setBoolAttribute(QXmlStreamWriter& xml, const QString& name, bool value)
{
    xml.writeStartElement(name);
    xml.writeAttribute("value", value ? "true": "false");
    xml.writeEndElement();
}


//----------------------------------------------------------------------------
// Write an XML element with geometry attributes.
//----------------------------------------------------------------------------

void QtlMovieSettings::setGeometryAttribute(QXmlStreamWriter& xml, const QString& name, const QString& objectName, const QRect& geometry)
{
    xml.writeStartElement(name);
    xml.writeAttribute("widget", objectName);
    xml.writeAttribute("x", QString::number(geometry.x()));
    xml.writeAttribute("y", QString::number(geometry.y()));
    xml.writeAttribute("width", QString::number(geometry.width()));
    xml.writeAttribute("height", QString::number(geometry.height()));
    xml.writeEndElement();
}


//----------------------------------------------------------------------------
// Decode an XML element with a "value" integer attribute.
//----------------------------------------------------------------------------

bool QtlMovieSettings::getIntAttribute(QXmlStreamReader& xml, const QString& name, int& value)
{
    if (xml.name() != name) {
        // Not the expected element.
        return false;
    }

    // Get the attribute value, skip the rest of the element.
    const QString stringValue(xml.attributes().value("value").toString().trimmed());
    xml.skipCurrentElement();

    // Decode the attribute value (if one was found).
    bool ok = false;
    const int intValue = stringValue.toInt(&ok, 0);
    if (stringValue.isEmpty()) {
        xml.raiseError(tr("No attribute \"%1\" in element <%2>").arg("value").arg(name));
    }
    else if (ok) {
        value = intValue;
    }
    else {
        xml.raiseError(tr("Invalid integer value: %1").arg(stringValue));
    }
    return ok;
}


//----------------------------------------------------------------------------
// Decode an XML element with a "value" string attribute.
//----------------------------------------------------------------------------

bool QtlMovieSettings::getStringAttribute(QXmlStreamReader& xml, const QString& name, QString& value)
{
    if (xml.name() != name) {
        // Not the expected element.
        return false;
    }

    // Get the attribute value, skip the rest of the element.
    value = xml.attributes().value("value").toString();
    xml.skipCurrentElement();
    return true;
}


//----------------------------------------------------------------------------
// Decode an XML element with "type" and "value" string attributes.
//----------------------------------------------------------------------------

bool QtlMovieSettings::getStringAttribute(QXmlStreamReader& xml, const QString& name, QMap<QString, QString>& valueMap)
{
    if (xml.name() != name) {
        // Not the expected element.
        return false;
    }

    // Get the attributes, skip the rest of the element.
    const QString type(xml.attributes().value("type").toString());
    const QString value(xml.attributes().value("value").toString());
    xml.skipCurrentElement();

    // Check that the type attribute is defined.
    if (type.isEmpty()) {
        xml.raiseError(tr("No attribute \"%1\" in element <%2>").arg("type").arg(name));
        return false;
    }
    else {
        valueMap[type] = value;
        return true;
    }
}


//----------------------------------------------------------------------------
// Decode an XML element with a "value" boolean attribute.
//----------------------------------------------------------------------------

bool QtlMovieSettings::getBoolAttribute(QXmlStreamReader& xml, const QString& name, bool& value)
{
    if (xml.name() != name) {
        // Not the expected element.
        return false;
    }

    // Get the attribute value in lowercase, skip the rest of the element.
    const QString stringValue(xml.attributes().value("value").toString().toLower().trimmed());
    xml.skipCurrentElement();

    // Decode the attribute value (if one was found).
    if (stringValue.isEmpty()) {
        xml.raiseError(tr("No attribute \"%1\" in element <%2>").arg("value").arg(name));
        return false;
    }
    else if (stringValue == "true") {
        value = true;
        return true;
    }
    else if (stringValue == "false") {
        value = false;
        return true;
    }
    else {
        xml.raiseError(tr("Invalid boolean value: %1").arg(stringValue));
        return false;
    }
}


//----------------------------------------------------------------------------
// Decode an XML element with geometry attributes.
//----------------------------------------------------------------------------

bool QtlMovieSettings::getGeometryAttribute(QXmlStreamReader& xml, const QString& name, QMap<QString, QRect>& valueMap)
{
    if (xml.name() != name) {
        // Not the expected element.
        return false;
    }

    // Get the attributes, skip the rest of the element.
    const QString objectName(xml.attributes().value("widget").toString());
    const int x = qtlToInt(xml.attributes().value("x").toString().trimmed());
    const int y = qtlToInt(xml.attributes().value("y").toString().trimmed());
    const int width = qtlToInt(xml.attributes().value("width").toString().trimmed());
    const int height = qtlToInt(xml.attributes().value("height").toString().trimmed());
    xml.skipCurrentElement();

    // Check that the geometry is valid.
    if (objectName.isEmpty() || x < 0 || y < 0 || width < 0 || height < 0) {
        xml.raiseError(tr("Invalid geometry syntax in element <%1>").arg(name));
        return false;
    }
    else {
        valueMap[objectName] = QRect(x, y, width, height);
        return true;
    }
}


//----------------------------------------------------------------------------
// Save the settings in an XML file.
//----------------------------------------------------------------------------

bool QtlMovieSettings::save(const QString& fileName)
{
    // Clear error.
    _fileError.clear();

    // Save the file.
    const QString actualFileName (fileName.isEmpty() ? _defaultFileName : fileName);
    QFile file(actualFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        _fileError = tr("Error creating file %1").arg(actualFileName);
        return false;
    }

    // Write the content as XML.
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    xml.writeStartDocument();
    xml.writeStartElement("QtlMovieSettings");
    xml.writeAttribute("version", "1.0");

    // Add an entry for each options.
    setStringAttribute(xml, "ffmpegExecutable", _ffmpegExplicit->fileName());
    setStringAttribute(xml, "ffprobeExecutable", _ffprobeExplicit->fileName());
    setStringAttribute(xml, "dvdauthorExecutable", _dvdauthorExplicit->fileName());
    setStringAttribute(xml, "mkisofsExecutable", _mkisofsExplicit->fileName());
    setStringAttribute(xml, "growisofsExecutable", _growisofsExplicit->fileName());
    setStringAttribute(xml, "telxccExecutable", _telxccExplicit->fileName());
    setStringAttribute(xml, "ccextractorExecutable", _ccextractorExplicit->fileName());
    setStringAttribute(xml, "dvddecrypterExecutable", _dvddecrypterExplicit->fileName());
    setIntAttribute(xml, "maxLogLines", _maxLogLines);
    setStringAttribute(xml, "initialInputDir", _initialInputDir);
    setBoolAttribute(xml, "defaultOutDirIsInput", _defaultOutDirIsInput);
    foreach (const QString& key, _defaultOutDir.keys()) {
        setStringAttribute(xml, "defaultOutDir", _defaultOutDir[key], key);
    }
    setStringAttribute(xml, "dvdBurner", _dvdBurner);
    setStringAttribute(xml, "audienceLanguages", _audienceLanguages.join(','));
    setBoolAttribute(xml, "transcodeComplete", _transcodeComplete);
    setIntAttribute(xml, "transcodeSeconds", _transcodeSeconds);
    setIntAttribute(xml, "dvdVideoBitRate", _dvdVideoBitRate);
    setIntAttribute(xml, "ipadVideoBitRate", _ipadVideoBitRate);
    setIntAttribute(xml, "iphoneVideoBitRate", _iphoneVideoBitRate);
    setBoolAttribute(xml, "keepIntermediateFiles", _keepIntermediateFiles);
    setIntAttribute(xml, "ffmpegProbeSeconds", _ffmpegProbeSeconds);
    setBoolAttribute(xml, "srtUseVideoSizeHint", _srtUseVideoSizeHint);
    setIntAttribute(xml, "chapterMinutes", _chapterMinutes);
    setBoolAttribute(xml, "dvdRemuxAfterTranscode", _dvdRemuxAfterTranscode);
    setBoolAttribute(xml, "createPalDvd", _createPalDvd);
    setIntAttribute(xml, "ipadScreenSize", int(_ipadScreenSize));
    setIntAttribute(xml, "iphoneScreenSize", int(_iphoneScreenSize));
    setBoolAttribute(xml, "forceDvdTranscode", _forceDvdTranscode);
    setBoolAttribute(xml, "newVersionCheck", _newVersionCheck);
    setIntAttribute(xml, "aviVideoBitRate", _aviVideoBitRate);
    setIntAttribute(xml, "aviMaxVideoWidth", _aviMaxVideoWidth);
    setIntAttribute(xml, "aviMaxVideoHeight", _aviMaxVideoHeight);
    setBoolAttribute(xml, "audioNormalize", _audioNormalize);
    setIntAttribute(xml, "audioNormalizeMean", _audioNormalizeMean);
    setIntAttribute(xml, "audioNormalizePeak", _audioNormalizePeak);
    setIntAttribute(xml, "audioNormalizeMode", int(_audioNormalizeMode));
    setBoolAttribute(xml, "autoRotateVideo", _autoRotateVideo);
    setBoolAttribute(xml, "playSoundOnCompletion", _playSoundOnCompletion);
    foreach (const QString& key, _widgetsGeometry.keys()) {
        setGeometryAttribute(xml, "geometry", key, _widgetsGeometry[key]);
    }

    // Finalize the XML document.
    xml.writeEndElement();
    xml.writeEndDocument();
    file.close();

    // Project is safe on disk.
    _isModified = false;

    return true;
}


//----------------------------------------------------------------------------
// Load the settings from an XML file.
//----------------------------------------------------------------------------

bool QtlMovieSettings::load(const QString& fileName)
{
    // Clear error.
    _fileError.clear();

    // Open the file.
    const QString actualFileName(fileName.isEmpty() ? _defaultFileName : fileName);
    QFile file(actualFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        _fileError = tr("Error opening file %1").arg(actualFileName);
        return false;
    }

    // Default values.
    QString ffmpegExecutable;
    QString ffprobeExecutable;
    QString dvdauthorExecutable;
    QString mkisofsExecutable;
    QString growisofsExecutable;
    QString telxccExecutable;
    QString ccextractorExecutable;
    QString dvddecrypterExecutable;
    int maxLogLines =_maxLogLines;
    QString initialInputDir(_initialInputDir);
    bool defaultOutDirIsInput = _defaultOutDirIsInput;
    QMap<QString,QString> defaultOutDir(_defaultOutDir);
    QString dvdBurner(_dvdBurner);
    QString audienceLanguages(_audienceLanguages.join(','));
    bool transcodeComplete = _transcodeComplete;
    int transcodeSeconds = _transcodeSeconds;
    int dvdVideoBitRate = _dvdVideoBitRate;
    int ipadVideoBitRate = _ipadVideoBitRate;
    int iphoneVideoBitRate = _iphoneVideoBitRate;
    bool keepIntermediateFiles = _keepIntermediateFiles;
    int ffmpegProbeSeconds = _ffmpegProbeSeconds;
    bool srtUseVideoSizeHint = _srtUseVideoSizeHint;
    int chapterMinutes = _chapterMinutes;
    bool dvdRemuxAfterTranscode = _dvdRemuxAfterTranscode;
    bool createPalDvd = _createPalDvd;
    int ipadScreenSize = int(_ipadScreenSize);
    int iphoneScreenSize = int(_iphoneScreenSize);
    bool forceDvdTranscode = _forceDvdTranscode;
    bool newVersionCheck = _newVersionCheck;
    int aviVideoBitRate = _aviVideoBitRate;
    int aviMaxVideoWidth = _aviMaxVideoWidth;
    int aviMaxVideoHeight = _aviMaxVideoHeight;
    bool audioNormalize = _audioNormalize;
    int audioNormalizeMean = _audioNormalizeMean;
    int audioNormalizePeak = _audioNormalizePeak;
    int audioNormalizeMode = int(_audioNormalizeMode);
    bool autoRotateVideo = _autoRotateVideo;
    bool playSoundOnCompletion = _playSoundOnCompletion;
    QMap<QString,QRect> widgetsGeometry(_widgetsGeometry);

    // Read the XML document.
    QXmlStreamReader xml(&file);
    if (xml.readNextStartElement()) {
        // Check the value of the root element.
        if (xml.name() != "QtlMovieSettings" || xml.attributes().value("version") != "1.0") {
            xml.raiseError(tr("The file is not a valid QtlMovie settings file"));
        }
        else {
            // Read all settings.
            while (xml.readNextStartElement()) {
                if (!getStringAttribute(xml, "ffmpegExecutable", ffmpegExecutable) &&
                    !getStringAttribute(xml, "ffprobeExecutable", ffprobeExecutable) &&
                    !getStringAttribute(xml, "dvdauthorExecutable", dvdauthorExecutable) &&
                    !getStringAttribute(xml, "mkisofsExecutable", mkisofsExecutable) &&
                    !getStringAttribute(xml, "growisofsExecutable", growisofsExecutable) &&
                    !getStringAttribute(xml, "telxccExecutable", telxccExecutable) &&
                    !getStringAttribute(xml, "ccextractorExecutable", ccextractorExecutable) &&
                    !getStringAttribute(xml, "dvddecrypterExecutable", dvddecrypterExecutable) &&
                    !getIntAttribute(xml, "maxLogLines", maxLogLines) &&
                    !getStringAttribute(xml, "initialInputDir", initialInputDir) &&
                    !getBoolAttribute(xml, "defaultOutDirIsInput", defaultOutDirIsInput) &&
                    !getStringAttribute(xml, "defaultOutDir", defaultOutDir) &&
                    !getStringAttribute(xml, "dvdBurner", dvdBurner) &&
                    !getStringAttribute(xml, "audienceLanguages", audienceLanguages) &&
                    !getBoolAttribute(xml, "transcodeComplete", transcodeComplete) &&
                    !getIntAttribute(xml, "transcodeSeconds", transcodeSeconds) &&
                    !getIntAttribute(xml, "dvdVideoBitRate", dvdVideoBitRate) &&
                    !getIntAttribute(xml, "ipadVideoBitRate", ipadVideoBitRate) &&
                    !getIntAttribute(xml, "iphoneVideoBitRate", iphoneVideoBitRate) &&
                    !getBoolAttribute(xml, "keepIntermediateFiles", keepIntermediateFiles) &&
                    !getIntAttribute(xml, "ffmpegProbeSeconds", ffmpegProbeSeconds) &&
                    !getBoolAttribute(xml, "srtUseVideoSizeHint", srtUseVideoSizeHint) &&
                    !getIntAttribute(xml, "chapterMinutes", chapterMinutes) &&
                    !getBoolAttribute(xml, "dvdRemuxAfterTranscode", dvdRemuxAfterTranscode) &&
                    !getBoolAttribute(xml, "createPalDvd", createPalDvd) &&
                    !getIntAttribute(xml, "ipadScreenSize", ipadScreenSize) &&
                    !getIntAttribute(xml, "iphoneScreenSize", iphoneScreenSize) &&
                    !getBoolAttribute(xml, "forceDvdTranscode", forceDvdTranscode) &&
                    !getBoolAttribute(xml, "newVersionCheck", newVersionCheck) &&
                    !getIntAttribute(xml, "aviVideoBitRate", aviVideoBitRate) &&
                    !getIntAttribute(xml, "aviMaxVideoWidth", aviMaxVideoWidth) &&
                    !getIntAttribute(xml, "aviMaxVideoHeight", aviMaxVideoHeight) &&
                    !getBoolAttribute(xml, "audioNormalize", audioNormalize) &&
                    !getIntAttribute(xml, "audioNormalizeMean", audioNormalizeMean) &&
                    !getIntAttribute(xml, "audioNormalizePeak", audioNormalizePeak) &&
                    !getIntAttribute(xml, "audioNormalizeMode", audioNormalizeMode) &&
                    !getBoolAttribute(xml, "autoRotateVideo", autoRotateVideo) &&
                    !getBoolAttribute(xml, "playSoundOnCompletion", playSoundOnCompletion) &&
                    !getGeometryAttribute(xml, "geometry", widgetsGeometry) &&
                    !xml.error()) {
                    // Unexpected element, ignore it.
                    xml.skipCurrentElement();
                }
            }
        }
    }

    // Process end of XML stream.
    const bool ok = !xml.error();
    if (ok) {
        // Project is safe on disk.
        _isModified = false;

        // Apply values.
        setFFmpegExplicitExecutable(ffmpegExecutable);
        setFFprobeExplicitExecutable(ffprobeExecutable);
        setDvdAuthorExplicitExecutable(dvdauthorExecutable);
        setMkisofsExplicitExecutable(mkisofsExecutable);
        setGrowisofsExplicitExecutable(growisofsExecutable);
        setTelxccExplicitExecutable(telxccExecutable);
        setCcextractorExplicitExecutable(ccextractorExecutable);
        setDvdDecrypterExplicitExecutable(dvddecrypterExecutable);
        setMaxLogLines(maxLogLines);
        setInitialInputDir(initialInputDir);
        setDefaultOutputDirIsInput(defaultOutDirIsInput);
        setAudienceLanguages(audienceLanguages.split(',', QString::SkipEmptyParts));
        foreach (const QString& type, defaultOutDir.keys()) {
            setDefaultOutputDir(type, defaultOutDir[type]);
        }
        setDvdBurner(dvdBurner);
        setTranscodeComplete(transcodeComplete);
        setTranscodeSeconds(transcodeSeconds);
        setDvdVideoBitRate(dvdVideoBitRate);
        setIpadVideoBitRate(ipadVideoBitRate);
        setIphoneVideoBitRate(iphoneVideoBitRate);
        setKeepIntermediateFiles(keepIntermediateFiles);
        setFFmpegProbeSeconds(ffmpegProbeSeconds);
        setSrtUseVideoSizeHint(srtUseVideoSizeHint);
        setChapterMinutes(chapterMinutes);
        setDvdRemuxAfterTranscode(dvdRemuxAfterTranscode);
        setCreatePalDvd(createPalDvd);
        setIpadScreenSize(IpadScreenSize(ipadScreenSize));
        setIphoneScreenSize(IphoneScreenSize(iphoneScreenSize));
        setForceDvdTranscode(forceDvdTranscode);
        setNewVersionCheck(newVersionCheck);
        setAviVideoBitRate(aviVideoBitRate);
        setAviMaxVideoWidth(aviMaxVideoWidth);
        setAviMaxVideoHeight(aviMaxVideoHeight);
        setAudioNormalize(audioNormalize);
        setAudioNormalizeMean(audioNormalizeMean);
        setAudioNormalizePeak(audioNormalizePeak);
        setAudioNormalizeMode(AudioNormalizeMode(audioNormalizeMode));
        setAutoRotateVideo(autoRotateVideo);
        setPlaySoundOnCompletion(playSoundOnCompletion);
        foreach (const QString& widgetName, widgetsGeometry.keys()) {
            setGeometry(widgetName, widgetsGeometry[widgetName]);
        }
    }
    else {
        // Format an error string.
        _fileError = tr("Error loading %1\n%2\nLine %3, column %4")
                .arg(actualFileName)
                .arg(xml.errorString())
                .arg(xml.lineNumber())
                .arg(xml.columnNumber());
    }

    file.close();
    return ok;
}


//----------------------------------------------------------------------------
// Get the HTML description of the actual FFmpeg and FFprobe products.
//----------------------------------------------------------------------------

QString QtlMovieSettings::ffmpegHtmlDescription() const
{
    return (_ffmpegExplicit->isSet() ? _ffmpegExplicit->htmlDescription() : _ffmpegDefault->htmlDescription()) +
            (_ffprobeExplicit->isSet() ? _ffprobeExplicit->htmlDescription() : _ffprobeDefault->htmlDescription());
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
    if (!dvddecrypter()->isSet()) {
        _log->line(tr("Error searching for DVD Decrypter, install it or set explicit path in settings."));
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
// Save the geometry of a widget into the settings.
//----------------------------------------------------------------------------

void QtlMovieSettings::setGeometry(const QString& widgetName, const QRect& geometry)
{
    // Get the previous geometry for a widget with the same name.
    const QMap<QString,QRect>::ConstIterator it = _widgetsGeometry.find(widgetName);

    if (it == _widgetsGeometry.end() || it.value() != geometry) {
        // There was one and the geometry changed.
        _widgetsGeometry[widgetName] = geometry;
        _isModified = true;
    }
}

void QtlMovieSettings::saveGeometry(const QWidget* widget)
{
    // Save the new geometry into this object.
    setGeometry(widget->objectName(), widget->geometry());

    // Save the file if required.
    if (_isModified) {
        save();
    }
}


//----------------------------------------------------------------------------
// Restore the geometry of a widget from the settings.
//----------------------------------------------------------------------------

void QtlMovieSettings::restoreGeometry(QWidget* widget)
{
    // Get the previous geometry for a widget with the same name.
    const QMap<QString,QRect>::ConstIterator it = _widgetsGeometry.find(widget->objectName());
    if (it != _widgetsGeometry.end()) {
        // Found one, apply the geometry.
        widget->setGeometry(it.value());
    }
}


//----------------------------------------------------------------------------
// Get/set the various elementary properties.
//----------------------------------------------------------------------------

QString QtlMovieSettings::initialInputDir() const
{
    // The default initial input directory is user's home.
    return _initialInputDir.isEmpty() ? QtlFile::absoluteNativeFilePath(QDir::homePath()) : _initialInputDir;
}

QString QtlMovieSettings::defaultOutputDir(const QString& outputType, bool force) const
{
    QMap<QString,QString>::ConstIterator it = _defaultOutDir.find(outputType);
    return (_defaultOutDirIsInput && !force) || it == _defaultOutDir.end() ? "" : it.value();
}

void QtlMovieSettings::setDefaultOutputDir(const QString& outputType, const QString& defaultOutDir)
{
    if (defaultOutDir != defaultOutputDir(outputType)) {
        _defaultOutDir[outputType] = defaultOutDir;
        _isModified = true;
    }
}

void QtlMovieSettings::setAudienceLanguages(const QStringList& audienceLanguages)
{
    // Apply a normalized list.
    QStringList languages(audienceLanguages);
    normalize(languages);
    if (_audienceLanguages != languages) {
        _audienceLanguages = languages;
        _isModified = true;
    }
}

int QtlMovieSettings::iphoneVideoWidth() const
{
    switch (_iphoneScreenSize) {
        case Iphone3Size: return QTL_IPHONE3_VIDEO_WIDTH;
        case Iphone4Size: return QTL_IPHONE4_VIDEO_WIDTH;
        case Iphone5Size: return QTL_IPHONE5_VIDEO_WIDTH;
        default: return -1;
    }
}

int QtlMovieSettings::iphoneVideoHeight() const
{
    switch (_iphoneScreenSize) {
        case Iphone3Size: return QTL_IPHONE3_VIDEO_HEIGHT;
        case Iphone4Size: return QTL_IPHONE4_VIDEO_HEIGHT;
        case Iphone5Size: return QTL_IPHONE5_VIDEO_HEIGHT;
        default: return -1;
    }
}

#define SETTER(method, type, parameter)           \
    void QtlMovieSettings::method(type parameter) \
    {                                             \
        if (_##parameter != parameter) {          \
            _##parameter = parameter;             \
            _isModified = true;                   \
        }                                         \
    }

SETTER(setMaxLogLines, int, maxLogLines)
SETTER(setInitialInputDir, const QString&, initialInputDir)
SETTER(setDefaultOutputDirIsInput, bool, defaultOutDirIsInput)
SETTER(setTranscodeSeconds, int, transcodeSeconds)
SETTER(setTranscodeComplete, bool, transcodeComplete)
SETTER(setDvdVideoBitRate, int, dvdVideoBitRate)
SETTER(setIpadVideoBitRate, int, ipadVideoBitRate)
SETTER(setIphoneVideoBitRate, int, iphoneVideoBitRate)
SETTER(setKeepIntermediateFiles, bool, keepIntermediateFiles)
SETTER(setFFmpegProbeSeconds, int, ffmpegProbeSeconds)
SETTER(setSrtUseVideoSizeHint, bool, srtUseVideoSizeHint)
SETTER(setChapterMinutes, int, chapterMinutes)
SETTER(setDvdRemuxAfterTranscode, bool, dvdRemuxAfterTranscode)
SETTER(setDvdBurner, const QString&, dvdBurner)
SETTER(setCreatePalDvd, bool, createPalDvd)
SETTER(setIpadScreenSize, QtlMovieSettings::IpadScreenSize, ipadScreenSize)
SETTER(setIphoneScreenSize, QtlMovieSettings::IphoneScreenSize, iphoneScreenSize)
SETTER(setForceDvdTranscode, bool, forceDvdTranscode)
SETTER(setNewVersionCheck, bool, newVersionCheck)
SETTER(setAviVideoBitRate, int, aviVideoBitRate)
SETTER(setAviMaxVideoWidth, int, aviMaxVideoWidth)
SETTER(setAviMaxVideoHeight, int, aviMaxVideoHeight)
SETTER(setAudioNormalize, bool, audioNormalize)
SETTER(setAudioNormalizeMean, int, audioNormalizeMean)
SETTER(setAudioNormalizePeak, int, audioNormalizePeak)
SETTER(setAudioNormalizeMode, AudioNormalizeMode, audioNormalizeMode)
SETTER(setAutoRotateVideo, bool, autoRotateVideo)
SETTER(setPlaySoundOnCompletion, bool, playSoundOnCompletion)

#define EXE_SETTER(method, parameter)                       \
    void QtlMovieSettings::method(const QString& parameter) \
    {                                                       \
        if (_##parameter->setFileName(parameter)) {         \
            _isModified = true;                             \
        }                                                   \
    }

EXE_SETTER(setFFmpegExplicitExecutable, ffmpegExplicit)
EXE_SETTER(setFFprobeExplicitExecutable, ffprobeExplicit)
EXE_SETTER(setDvdAuthorExplicitExecutable, dvdauthorExplicit)
EXE_SETTER(setMkisofsExplicitExecutable, mkisofsExplicit)
EXE_SETTER(setGrowisofsExplicitExecutable, growisofsExplicit)
EXE_SETTER(setTelxccExplicitExecutable, telxccExplicit)
EXE_SETTER(setCcextractorExplicitExecutable, ccextractorExplicit)
EXE_SETTER(setDvdDecrypterExplicitExecutable, dvddecrypterExplicit)
