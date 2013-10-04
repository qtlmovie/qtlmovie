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
// Define the class QtlMovieMkisofsProcess.
//
//----------------------------------------------------------------------------

#include "QtlMovieMkisofsProcess.h"
#include "QtlUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieMkisofsProcess::QtlMovieMkisofsProcess(const QStringList& arguments,
                                               const QtlMovieSettings* settings,
                                               QtlLogger* log,
                                               QObject* parent) :
    QtlMovieProcess(settings->mkisofs(), arguments, false, settings, log, parent)
{
}


//----------------------------------------------------------------------------
// Process one text line from standard error or output.
//----------------------------------------------------------------------------

void QtlMovieMkisofsProcess::processOutputLine(QProcess::ProcessChannel channel, const QString& line)
{
    // Mkisofs outputs lines like this:
    //  58.01% done, estimate finish Sat Sep 28 00:00:10 2013
    // Try to extract a percentage from the line.
    int percent = -1;
    QString s(line.trimmed());
    int sep = s.indexOf("%");
    if (sep >= 0) {
        s.remove(sep, s.size());
        // If a "." is present, remove decimals.
        sep = s.indexOf(".");
        if (sep >= 0) {
            s.remove(sep, s.size());
        }
        // Decode remaining integer value.
        percent = qtlToInt(s, -1, 0, 100);
    }

    if (percent < 0) {
        // None is found, there is no progress indicator. Delegate to superclass.
        QtlMovieProcess::processOutputLine(channel, line);
    }
    else {
        // Report progression.
        emitProgress(percent, 100);
        // Simply echo a dot in the log.
        log()->text(".");
    }
}

