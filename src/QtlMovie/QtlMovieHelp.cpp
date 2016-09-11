//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2016, Thierry Lelegard
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
// Helper functions for displaying help.
//
//----------------------------------------------------------------------------

#include "QtlMovieHelp.h"
#include "QtlMessageBoxUtils.h"
#include "QtlFile.h"
#include "QtlTranslator.h"


//----------------------------------------------------------------------------
// Constructor and destructor.
//----------------------------------------------------------------------------

QtlMovieHelp::QtlMovieHelp() :
    _tempDir(0)
{
}

QtlMovieHelp::~QtlMovieHelp()
{
    // Cleanup temporary files.
    if (_tempDir != 0) {
        _tempDir->remove();
        delete _tempDir;
        _tempDir = 0;
    }
}


//----------------------------------------------------------------------------
// Display a help HTML file in the external browser.
//----------------------------------------------------------------------------

void QtlMovieHelp::showHelp(QObject* parent, const QString& fragment)
{
    // Open the file in the external browser.
    const QUrl url(urlOf(parent, fragment));
    if (url.isValid() && !QDesktopServices::openUrl(url)) {
        qtlError(parent, QObject::tr("Error opening browser.\nSee %1").arg(url.toString()));
    }
}


//----------------------------------------------------------------------------
// Build the URL for a help HTML file.
//----------------------------------------------------------------------------

QUrl QtlMovieHelp::urlOf(QObject* parent, const QString& fragment)
{
    // Name of the main help file.
    const QString fileName("qtlmovie-user.html");

    // Search the directory where help files are stored.
    // Start with the application directory.
    const QString appDir(QtlFile::absoluteNativeFilePath(QCoreApplication::applicationDirPath()));
    QStringList dirs(appDir);

#if defined(Q_OS_WIN) || defined(Q_OS_DARWIN)
    // On Windows and Mac, add the local help subdirectory.
    // When running the application from the build tree, also add the help
    // subdirectory from some level upward from the application executable.
    const QString toolPath(QtlFile::searchParentSubdirectory(appDir, "help"));
    if (!toolPath.isEmpty()) {
        dirs << toolPath;
    }
#endif

#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    // On UNIX, add various standard locations for help files.
    dirs << "/usr/share/doc/qtlmovie"
         << "/usr/share/help/qtlmovie"
         << "/usr/local/doc/qtlmovie"
         << "/usr/local/help/qtlmovie";
#endif

    // Now search the QtlMovie help file there.
    QString htmlFile(QtlFile::search(fileName, dirs));
    if (htmlFile.isEmpty()) {
        // File not found.
        qtlError(parent, QObject::tr("Cannot find help files.\nTry to reinstall QtlMovie."));
        return QUrl();
    }

    // Find a locale variant if it exists.
    htmlFile = QtlTranslator::searchLocaleFile(htmlFile);

    // Build URL.
    QUrl url(QUrl::fromLocalFile(htmlFile));
    if (fragment.isEmpty()) {
        // No "#fragment", URL is complete.
        return url;
    }

    // Set the "#fragment" part of the URL.
    url.setFragment(fragment);

#if QT_VERSION < QT_VERSION_CHECK(5,7,2)

    // Warning: Qt bug #55300 (https://bugreports.qt.io/browse/QTBUG-55300)
    // When there is a fragment in a file:// URL, QDesktopServices::openUrl()
    // fails to set the fragment in the actual URL. This bug has been reported
    // for Qt 5.7.0. We do not know yet when this bug is fixed. Currently, we
    // assume it will be fixed in 5.7.2, if this version ever exists.

    // Workaround for Qt bug: We create a temporary HTML file containing
    // a redirect directive to the target URL. In case of error, we simply
    // return the URL as we built it, directly on the final target file.
    // Because of the Qt bug, the anchor will be ignored and the HTML file
    // will be opened at the beginning, which is better than nothing.

    // Create a temporary directory for temporary HTML files.
    if (_tempDir == 0) {
        _tempDir = new QTemporaryDir();
    }

    // Does the temporary directory exist?
    if (!_tempDir->isValid()) {
        // Cannot create temporary files.
        return url;
    }

    // Temporary file name to create.
    const QString tempFile(_tempDir->path() +
                           QDir::separator() +
                           QFileInfo(htmlFile).completeBaseName() +
                           QStringLiteral("-anchor-") +
                           fragment +
                           ".html");

    // The file may already exist if we already accessed this page.
    // Create the file only if it does not exists.
    if (!QFile(tempFile).exists()) {
        // File content: HTTP redirection to target file and anchor.
        QStringList lines;
        lines << "<html><head>"
              << ("<meta http-equiv=\"refresh\" content=\"0;URL='" + url.toString() + "'\"/> ")
              << "</head><body><p>"
              << ("Click <a href=\"" + url.toString() + "\">here</a>.")
              << "</p></body></html>";

        if (!QtlFile::writeTextLinesFile(tempFile, lines)) {
            // Error while creating the temporary file.
            return url;
        }
    }

    // Replace the URL content with the temporary file, without fragment.
    url = QUrl::fromLocalFile(tempFile);

#endif

    return url;
}
