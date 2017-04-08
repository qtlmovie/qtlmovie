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
// Define the class QtlMovieAboutMediaTools.
//
//----------------------------------------------------------------------------

#include "QtlMovieAboutMediaTools.h"
#include "QtlStringUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieAboutMediaTools::QtlMovieAboutMediaTools(QtlMovieSettings* settings, QWidget *parent) :
    QtlDialog(parent),
    _settings(settings)
{
    // Build the UI as defined in Qt Designer.
    _ui.setupUi(this);

    // Restore the window geometry from the saved settings.
    setGeometrySettings(_settings, true);

    // Media tools description.
    _ui.ffmpegTitle->setText(title(settings->ffmpeg()));
    _ui.ffmpegDescription->setText(settings->ffmpeg()->htmlDescription() + settings->ffprobe()->htmlDescription());

    _ui.dvdauthorTitle->setText(title(settings->dvdauthor()));
    _ui.dvdauthorDescription->setText(settings->dvdauthor()->htmlDescription());

    _ui.mkisofsTitle->setText(title(settings->mkisofs()));
    _ui.mkisofsDescription->setText(settings->mkisofs()->htmlDescription());

    _ui.growisofsTitle->setText(title(settings->growisofs()));
    _ui.growisofsDescription->setText(settings->growisofs()->htmlDescription());

    _ui.ccextractorTitle->setText(title(settings->ccextractor()));
    _ui.ccextractorDescription->setText(settings->ccextractor()->htmlDescription());
}


//----------------------------------------------------------------------------
// Build an HTML title label for a media tool.
//----------------------------------------------------------------------------

QString QtlMovieAboutMediaTools::title(const QtlMovieExecFile* file)
{
    QString text(tr("<b>%1 home page</b>: %2").arg(file->name().toHtmlEscaped()).arg(qtlHtmlLink(file->homePage())));
#if defined(Q_OS_WIN)
    const QString win(file->windowsBuilds());
    if (!win.isEmpty()) {
        text.append("<br/>");
        text.append(tr("Windows builds: %1").arg(qtlHtmlLink(win)));
    }
#endif
    return text;
}
