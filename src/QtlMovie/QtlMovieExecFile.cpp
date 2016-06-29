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
// Define the QtlMovieExecFile class.
//
//----------------------------------------------------------------------------

#include "QtlMovieExecFile.h"
#include "QtlProcess.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QRegExp>


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieExecFile::QtlMovieExecFile(const QString& name,
                                   const QString& homePage,
                                   const QString& windowsBuilds,
                                   const QString& fileName,
                                   const QStringList& versionOptions,
                                   const QString& versionStart,
                                   const QString& versionEnd,
                                   QtlLogger* log,
                                   QObject *parent) :
    QtlFile(searchExecutable(fileName, movieExecSearchPath()), parent),
    _log(log),
    _name(name),
    _homePage(homePage),
    _windowsBuilds(windowsBuilds),
    _versionOptions(versionOptions),
    _versionStart(versionStart),
    _versionEnd(versionEnd),
    _version()
{
    Q_ASSERT(log != 0);
    startGetVersion();
}


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieExecFile::QtlMovieExecFile(const QtlMovieExecFile& other, const QString& fileName, QObject* parent) :
    QtlFile(searchExecutable(fileName, movieExecSearchPath()), parent),
    _log(other._log),
    _name(other._name),
    _homePage(other._homePage),
    _windowsBuilds(other._windowsBuilds),
    _versionOptions(other._versionOptions),
    _versionStart(other._versionStart),
    _versionEnd(other._versionEnd),
    _version()
{
}


//----------------------------------------------------------------------------
// Build the search path of QtlMovie utilities.
//----------------------------------------------------------------------------

QStringList QtlMovieExecFile::movieExecSearchPath()
{
    QStringList dirs;

    // Start with the application directory.
    const QString appDir(absoluteNativeFilePath(QCoreApplication::applicationDirPath()));
    dirs << appDir;

    // On Windows and Mac, the media tools are in a local subdirectory.
#if defined(Q_OS_WIN)
    const QString toolDir(QStringLiteral("wintools"));
#else
    const QString toolDir(QStringLiteral("mactools"));
#endif

#if defined(Q_OS_WIN) || defined(Q_OS_DARWIN)
    // On Windows and Mac, add the local tools subdirectory.
    // When running the application from the build tree, also add the tools
    // sudirectory from some level upward from the application executable.
    const QString toolPath(QtlFile::searchParentSubdirectory(appDir, toolDir));
    if (!toolPath.isEmpty()) {
        dirs << toolPath;
    }
#endif

#if defined(Q_OS_WIN)
    // On Windows, add various standard locations for media tools.
    dirs << "C:\\Program Files\\FFmpeg\\bin"
         << "C:\\Program Files (x86)\\FFmpeg\\bin";
#endif

    // End up with the standard executable search path.
    dirs << commandSearchPath();
    return dirs;
}


//----------------------------------------------------------------------------
// Set the file name. Reimplemented from QtlFile.
//----------------------------------------------------------------------------

bool QtlMovieExecFile::setFileName(const QString& fileName)
{
    // Check if the file is a valid executable.
    bool isExecutable = false;
    if (!fileName.isEmpty()) {
        const QFileInfo info(fileName);
        isExecutable = info.exists() && info.isExecutable() && !info.isDir();
    }

    if (!isExecutable) {
        // Not an executable, clear all.
        _version.clear();
        return QtlFile::setFileName("");
    }
    else if (QtlFile::setFileName(fileName)) {
        // A new executable has been found, recompute version.
        _version.clear();
        startGetVersion();
        return true;
    }
    else {
        // Same executable file as before, do not recompute version.
        return false;
    }
}


//----------------------------------------------------------------------------
// Start determining the executable version.
//----------------------------------------------------------------------------

void QtlMovieExecFile::startGetVersion()
{
    // If the executable is not found or no version option is specified, cannot get version.
    if (!isSet() || _versionOptions.isEmpty()) {
        return;
    }

    // Set the command search path to include executable's directory first.
    QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
    env.insert(QTL_PATH_VARIABLE_NAME, directoryName() + QTL_SEARCH_PATH_SEPARATOR + env.value(QTL_PATH_VARIABLE_NAME));

    // Start a process which requests the executable versions.
    QtlProcess* process =
        QtlProcess::newInstance(fileName(),       // command
                                _versionOptions,  // arguments
                                5000,             // timeout after 5 seconds.
                                32768,            // max output size in bytes
                                this,             // parent object
                                env);

    // Get notification at process termination.
    connect(process, &QtlProcess::terminated, this, &QtlMovieExecFile::getVersionTerminated);

    // Start the process.
    process->start();
}


//----------------------------------------------------------------------------
// Locate a marker into a string.
//----------------------------------------------------------------------------

int QtlMovieExecFile::markerPosition(const QString &marker, const QString &str)
{
    return marker.isEmpty() ? -1 : str.indexOf(marker, 0, Qt::CaseInsensitive);
}


//----------------------------------------------------------------------------
// Invoked when the "get version" process completes.
//----------------------------------------------------------------------------

void QtlMovieExecFile::getVersionTerminated(const QtlProcessResult& result)
{
    // Report termination error.
    if (result.hasError()) {
        _log->line(tr("Error getting version for %1: %2").arg(_name).arg(result.errorMessage()));
    }

    // If both standard output and standard error are non-empty, shall we use one? Both? Concatenated? In which order?
    // If either the version start or end marker is non empty and are found in only one of standard output or error,
    // we use the one where a marker is found. Otherwise, we use a contenation of both.
    _version.clear();
    if (!result.standardOutput().isEmpty() && !result.standardError().isEmpty()) {
        const bool markerInOutput = markerPosition(_versionStart, result.standardOutput()) >= 0 || markerPosition(_versionEnd, result.standardOutput()) >= 0;
        const bool markerInError = markerPosition(_versionStart, result.standardError()) >= 0 || markerPosition(_versionEnd, result.standardError()) >= 0;
        if (markerInError && !markerInOutput) {
            _version = result.standardError();
        }
        else if (!markerInError && markerInOutput) {
            _version = result.standardOutput();
        }
    }
    if (_version.isEmpty()) {
        // Build version string as the concatenation of standard output and error.
        _version = result.standardOutput();
        _version.append(result.standardError());
    }

    // Cleanup line terminators, remove blanks lines.
    _version.replace(QRegExp("[\\r\\n]+"), "\n");

    // Extract version between markers, if any specified.
    const int startPosition = markerPosition(_versionStart, _version);
    if (startPosition > 0) {
        _version.remove(0, startPosition);
    }
    const int endPosition = markerPosition(_versionEnd, _version);
    if (endPosition >= 0) {
        _version.truncate(endPosition);
    }

    // Remove leading and trailing new-lines.
    _version.replace(QRegExp("^\\n+"), "");
    _version.replace(QRegExp("\\n+$"), "");
}


//----------------------------------------------------------------------------
// A description of the executable in HTML format.
//----------------------------------------------------------------------------

QString QtlMovieExecFile::htmlDescription() const
{
    if (isSet()) {
        QString text("<p><b>");
        text.append(_name);
        text.append("</b>: ");
        text.append(fileName().toHtmlEscaped());
        text.append("</p>");
        if (!_version.isEmpty()) {
            text.append("<p>");
            text.append(_version.toHtmlEscaped().replace("\n", "<br/>"));
            text.append("</p>");
        }
        return text;
    }
    else {
        return QString("<p><b>%1</b> : %2</p>").arg(_name).arg(tr("Not found").toHtmlEscaped());
    }
}
