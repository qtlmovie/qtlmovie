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
// Define the class QtlMovieGrowisofsProcess.
//
//----------------------------------------------------------------------------

#include "QtlMovieGrowisofsProcess.h"
#include "QtlUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieGrowisofsProcess::QtlMovieGrowisofsProcess(const QStringList& arguments,
                                               const QtlMovieSettings* settings,
                                               QtlLogger* log,
                                               QObject* parent) :
    QtlMovieProcess(settings->growisofs(), arguments, false, settings, log, parent)
{
}


//----------------------------------------------------------------------------
// Process one text line from standard error or output.
//----------------------------------------------------------------------------

void QtlMovieGrowisofsProcess::processOutputLine(QProcess::ProcessChannel channel, const QString& line)
{
    // Growisofs outputs lines like this:
    //   2801795072/4182333440 (67.0%) @6.5x, remaining 3:26 RBU 100.0%
    // Try to current/maximum.

    qint64 current = -1;
    qint64 maximum = -1;
    QString s(line.trimmed());
    s.remove(QRegExp(" .*"));
    const QStringList fields(s.split(QChar('/'), QString::SkipEmptyParts));
    if (fields.size() == 2) {
        current = qtlToInt64(fields.first(), -1);
        maximum = qtlToInt64(fields.last(), -1);
    }

    if (current < 0 || maximum < 0) {
        // Not found, there is no progress indicator. Delegate to superclass.
        QtlMovieProcess::processOutputLine(channel, line);
    }
    else {
        // Report progression.
        // We use displayed values / 1000 to avoid integer overflow.
        emitProgress(int(current / 1000), int(maximum / 1000));
    }
}
