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
// Define the class QtlMovieDvdAuthorProcess.
//
//----------------------------------------------------------------------------

#include "QtlMovieDvdAuthorProcess.h"
#include "QtlUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieDvdAuthorProcess::QtlMovieDvdAuthorProcess(const QStringList& arguments,
                                                   const QtlMovieSettings* settings,
                                                   QtlLogger* log,
                                                   QObject* parent) :
    QtlMovieProcess(settings->dvdauthor(), arguments, false, settings, log, parent),
    _fileSize(-1)
{
}


//----------------------------------------------------------------------------
// Start the process.
//----------------------------------------------------------------------------

bool QtlMovieDvdAuthorProcess::start()
{
    // Invoke superclass.
    const bool success = QtlMovieProcess::start();

    // To evaluate the progression of pass 1, we need to evaluate the input movie
    // file size. We assume that the last argument could be the input file path.
    // At that point, the file should exist.
    if (success) {
        const QStringList args(arguments());
        if (!args.isEmpty()) {
            QFileInfo info(args.last());
            if (info.exists() && info.isFile() && !info.isDir()) {
                _fileSize = info.size();
            }
        }
    }

    return success;
}


//----------------------------------------------------------------------------
// Process one text line from standard error or output.
//----------------------------------------------------------------------------

void QtlMovieDvdAuthorProcess::processOutputLine(QProcess::ProcessChannel channel, const QString& line)
{
    // When adding a file, DVD Author outputs lines like this:
    // First pass:
    //   STAT: VOBU 752 at 196MB, 1 PGCs
    // Second pass:
    //   STAT: fixing VOBU at 2083MB (6497/10193, 63%)
    // We base the progression of first pass on file size and
    // second pass on percentage. We assume that both passes
    // have the same duration.

    const int totalMb = _fileSize < 0 ? -1 : int(_fileSize / (1024 * 1024));
    int mb = -1;
    int percent = -1;
    QString s(line.trimmed().toLower());

    if (s.startsWith("stat:") && s.endsWith("pgcs")) {
        // Progression line in pass 1.
        int sep = s.indexOf("mb,");
        if (sep >= 0) {
            s.remove(sep, s.size()); // truncate at "mb,"
            sep = s.lastIndexOf(" ");
            if (sep >= 0) {
                s.remove(0, sep + 1); // remove up to last space
            }
            mb = qtlToInt(s, -1);
        }
    }
    else if (s.startsWith("stat:") && s.endsWith("%)")) {
        // Progression line in pass 1.
        s.chop(2); // remove trailing "%)"
        int sep = line.lastIndexOf(" ");
        if (sep >= 0) {
            s.remove(0, sep + 1); // remove up to last space
        }
        percent = qtlToInt(s, -1, 0, 100);
    }

    // Progression info extracted or not found, now report the progression.
    if (mb >= 0 && totalMb >= 0) {
        // Progression indicator in pass 1. We double the size of the total
        // so that full progression is 50% of the total.
        emitProgress(mb, 2 * totalMb);
        log()->text(".");
    }
    else if (percent >= 0) {
        // Progression indicator in pass 2.
        // Adjust so that 0% of pass 2 means 50% of the total.
        emitProgress(100 + percent, 200);
        log()->text(".");
    }
    else {
        // None is found, there is no progress indicator. Delegate to superclass.
        QtlMovieProcess::processOutputLine(channel, line);
    }
}


//----------------------------------------------------------------------------
// Update a QProcessEnvironment for the target DVD Author process.
//----------------------------------------------------------------------------

void QtlMovieDvdAuthorProcess::updateEnvironment(QProcessEnvironment& env)
{
    // Ensure that a default video format is defined.
    // Otherwise, DVD Author complains.
    env.insert("VIDEO_FORMAT", settings()->createPalDvd() ? "PAL" : "NTSC");
}
