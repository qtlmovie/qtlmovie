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
// Define the class QtlMovieCcExtractorProcess.
//
//----------------------------------------------------------------------------

#include "QtlMovieCcExtractorProcess.h"
#include "QtlStringList.h"
#include "QtlStringUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieCcExtractorProcess::QtlMovieCcExtractorProcess(const QStringList& arguments,
                                                       const QtlMovieSettings* settings,
                                                       QtlLogger* log,
                                                       QObject* parent) :
    QtlMovieProcess(settings->ccextractor(), arguments, false, settings, log, parent)
{
}


//----------------------------------------------------------------------------
// Process one text line from standard error or output.
//----------------------------------------------------------------------------

void QtlMovieCcExtractorProcess::processOutputLine(QProcess::ProcessChannel channel, const QString& line)
{
    // CCExtractor --gui_mode_reports outputs lines starting with ###
    // The progress lines are: ###PROGRESS#percent#minutes#seconds

    if (!line.startsWith("###")) {
        // Not a GUI event. Delegate to superclass.
        QtlMovieProcess::processOutputLine(channel, line);
    }
    else if (line.startsWith("###PROGRESS#")) {
        // This is a progress indicator.
        const QStringList fields(line.split(QChar('#'), QString::SkipEmptyParts));
        if (fields.size() >= 2) {
            const int percent = qtlToInt(fields[1], -1, 0, 100);
            if (percent >= 0) {
                // Valid percentage, report progression.
                emitProgress(percent, 100);
            }
        }
    }
}


//----------------------------------------------------------------------------
// Build a list of options which limit the extraction duration.
//----------------------------------------------------------------------------

QStringList QtlMovieCcExtractorProcess::durationOptions(int totalSeconds)
{
    if (totalSeconds < 0) {
        // No limit, no option.
        return QStringList();
    }
    else {
        // Build extraction duration as HH:MM:SS.
        const int hours = (totalSeconds / 3600) % 100;
        const int minutes = (totalSeconds % 3600) / 60;
        const int seconds = totalSeconds % 60;
        return QtlStringList("-endat", QStringLiteral("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
    }
}
