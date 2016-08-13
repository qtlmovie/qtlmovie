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
// Display a help HTML file in the external browser.
//----------------------------------------------------------------------------

void QtlMovieHelp::showHelp(QObject* parent, QtlMovieHelp::HelpFileType type, const QString& fragment)
{
    // Name of the main help file.
    QString fileName;
    switch (type) {
        case Home:
            fileName = "qtlmovie.html";
            break;
        case User:
            fileName = "qtlmovie-user.html";
            break;
        case Developer:
            fileName = "qtlmovie-build.html";
            break;
        default:
            qtlError(parent, QObject::tr("Invalid help file"));
            return;
    }

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
        return;
    }

    // Find a locale variant if it exists.
    htmlFile = QtlTranslator::searchLocaleFile(htmlFile);

    // Build URL.
    QUrl url(QUrl::fromLocalFile(htmlFile));
    if (!fragment.isEmpty()) {
        url.setFragment(fragment);
    }

    // Open the file in the external browser.
    if (!QDesktopServices::openUrl(url)) {
        qtlError(parent, QObject::tr("Error opening browser.\nSee %1").arg(url.toString()));
    }
}
