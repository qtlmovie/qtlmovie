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
// Define the class QtlMovieFFmpegProcess.
//
//----------------------------------------------------------------------------

#include "QtlMovieFFmpegProcess.h"
#include "QtlStringUtils.h"

//
// We build a dedicated fontconfig for subtitling.
//
// We locate the application-specific font directory.
// This directory shall contain a file named fonts.conf.template
// and font files. In fonts.conf.template, the string {FONTDIR}
// shall be replaced with the font directory path (it is a shame
// that fontconfig does not support environment variables in
// configuration files). Since the font directory may not be
// be writeable by the user, we copy fonts.conf.template as
// fonts.conf in the temporary directory, after substituting
// all occurences of {FONTDIR}.
//
#define QTLFC_FONT_DIR_NAME "fonts"
#define QTLFC_TEMPLATE_NAME "fonts.conf.template"
#define QTLFC_ACTUAL_NAME   "fonts.conf"
#define QTLFC_FONTDIR_MARK  "{FONTDIR}"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieFFmpegProcess::QtlMovieFFmpegProcess(const QStringList& ffmpegArguments,
                                             int inputDurationInSeconds,
                                             const QString& temporaryDirectory,
                                             const QtlMovieSettings* settings,
                                             QtlLogger* log,
                                             QObject *parent,
                                             QtlDataPull* dataPull) :
    QtlMovieProcess(settings->ffmpeg(), ffmpegArguments, true, settings, log, parent, dataPull),
    _inputDurationInSeconds(inputDurationInSeconds),
    _tempDir(temporaryDirectory)
{
}


//----------------------------------------------------------------------------
// Start the process.
//----------------------------------------------------------------------------

bool QtlMovieFFmpegProcess::start()
{
    debug(tr("Starting ffmpeg transcode, playback duration: %1 seconds (%2)").arg(_inputDurationInSeconds).arg(qtlSecondsToString(_inputDurationInSeconds)));

    // Try to get reporting from the QtlDataPull instead of the ffmpeg process.
    // This avoid a disconcerting freeze when transcoding from a DVD (see QtlMovie user guide).
    useDataPullProgressReport(true);

    // Continue in the superclass.
    return QtlMovieProcess::start();
}


//----------------------------------------------------------------------------
// Update a QProcessEnvironment for the target FFmpeg process.
//----------------------------------------------------------------------------

void QtlMovieFFmpegProcess::updateEnvironment(QProcessEnvironment& env)
{

    const QChar sep(QDir::separator());
    const QString actualPath(_tempDir + sep + QStringLiteral(QTLFC_ACTUAL_NAME));
    const QString applicationDirectory(QCoreApplication::applicationDirPath());

    // Search fonts.conf.template in "fonts" subdirectory from application.
    // When running the application from the build tree, also add "fonts" upwards
    // from the application executable. This is a sort of hack which should not be
    // too dangerous when run into a real installation.
    const QString fontDir(QtlFile::searchParentSubdirectory(applicationDirectory, QStringLiteral(QTLFC_FONT_DIR_NAME)));
    const QString fontConfigTemplate(fontDir + sep + QStringLiteral(QTLFC_TEMPLATE_NAME));
    const QFileInfo info(fontConfigTemplate);

    if (fontDir.isEmpty() || !info.exists()) {
        line(tr("Cannot find %1, may be a problem to insert subtitles").arg(QTLFC_TEMPLATE_NAME));
    }
    else if (_tempDir.isEmpty()) {
        line(tr("Cannot create %1, no temporary directory defined, may be a problem to insert subtitles").arg(QTLFC_ACTUAL_NAME));
    }
    else if (createFontConfig(fontConfigTemplate, actualPath)) {
        // Define environment variables.
        env.insert("FC_CONFIG_DIR", _tempDir);
        env.insert("FONTCONFIG_PATH", _tempDir);
        env.insert("FONTCONFIG_FILE", actualPath);
    }
}


//----------------------------------------------------------------------------
// Create the temporary fontconfig configuration file.
//----------------------------------------------------------------------------

bool QtlMovieFFmpegProcess::createFontConfig(const QString& templateFile, const QString& actualFile)
{
    // Open input and output files.
    QFile inFile(templateFile);
    QFile outFile(actualFile);
    if (!inFile.open(QFile::ReadOnly)) {
        line(tr("Cannot open %1, may be a problem to insert subtitles").arg(inFile.fileName()));
        return false;
    }
    if (!outFile.open(QFile::WriteOnly)) {
        line(tr("Cannot create %1, may be a problem to insert subtitles").arg(outFile.fileName()));
        return false;
    }

    // The font directory is the directory of the template file.
    const QString fontDir(QtlFile::parentPath(templateFile));

    // Copy file with substitutions of {FONTDIR}
    QTextStream in(&inFile);
    QTextStream out(&outFile);
    QString line;
    while (!(line = in.readLine()).isNull()) {
        line.replace(QTLFC_FONTDIR_MARK, fontDir);
        out << line << "\n";
    }
    return true;
}


//----------------------------------------------------------------------------
// Process one text line from standard error.
//----------------------------------------------------------------------------

void QtlMovieFFmpegProcess::processOutputLine(QProcess::ProcessChannel channel, const QString& line)
{
    // Ignore output lines which are generated by FFmpeg in large quantities and fill the log.
    // This depends on the FFmpeg version.
    if (QRegExp("\\s*Past duration [\\d\\.]* too large[\\s\\.]*").exactMatch(line) ||
        QRegExp("\\s*Last message repeated [\\d]* times[\\s\\.]*").exactMatch(line))
    {
        // debug(line); // debug deactivated since these messages are really, really numerous...
        return;
    }

    // From a progress line, we can extract a time in seconds.
    int timeInSeconds = -1;

    // Look for " time=" in a copy of the line.
    QString line1(line);
    const QString timeName(QStringLiteral(" time="));
    line1.replace(QRegExp("time\\s*=\\s*"), timeName);
    const int timeIndex = line1.indexOf(timeName, Qt::CaseInsensitive);

    // Extract time in format "hh:mm:ss.hh".
    if (timeIndex >= 0) {
        line1.remove(0, timeIndex + timeName.length());
        line1.replace(QRegExp("\\s.*$"), "");
        const QStringList fields(line1.split(QRegExp("[:\\.]")));
        if (fields.size() == 4) {
            const int hours = qtlToInt(fields[0], -1, 0, 99);
            const int minutes = qtlToInt(fields[1], -1, 0, 59);
            const int seconds = qtlToInt(fields[2], -1, 0, 59);
            const int hundredths = qtlToInt(fields[3], -1, 0, 99);
            if (hours >= 0 && minutes >= 0 && seconds >= 0 && hundredths >= 0) {
                timeInSeconds = (hours * 3600) + (minutes * 60) + seconds;
                if (hundredths > 50) {
                    timeInSeconds++;
                }
            }
        }
    }

    if (timeInSeconds < 0) {
        // None is found, there is no progress indicator. Delegate to superclass.
        QtlMovieProcess::processOutputLine(channel, line);
    }
    else {
        // Report progression if not done in QtlDataPull.
        if (!useDataPullProgressReport()) {
            emitProgress(timeInSeconds, _inputDurationInSeconds);
        }
        // Uncomment this to log FFmpeg frame indicators (but this is a lot of output).
        // debug(line);
    }
}
