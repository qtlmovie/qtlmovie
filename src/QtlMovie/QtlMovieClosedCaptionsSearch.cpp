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
// Define the class QtlMovieClosedCaptionsSearch.
//
//----------------------------------------------------------------------------

#include "QtlMovieClosedCaptionsSearch.h"
#include "QtlMovieCcExtractorProcess.h"
#include "QtlStringList.h"


//----------------------------------------------------------------------------
// Create a new instance. Enforce object creation on the heap.
//----------------------------------------------------------------------------

QtlMovieClosedCaptionsSearch* QtlMovieClosedCaptionsSearch::newInstance(const QString& fileName,
                                                                        const QtlMovieSettings* settings,
                                                                        QtlLogger* log,
                                                                        QObject* parent)
{
    return new QtlMovieClosedCaptionsSearch(fileName, settings, log, parent);
}


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlMovieClosedCaptionsSearch::QtlMovieClosedCaptionsSearch(const QString& fileName,
                                                           const QtlMovieSettings* settings,
                                                           QtlLogger* log,
                                                           QObject* parent) :
    QtlMovieProcess(settings->ccextractor(), QStringList(), false, settings, log, parent),
    _fileName(fileName)
{
    // No need to report in search phase.
    setSilent(true);

    // Build CCExtractor command line..
    setArguments(QtlStringList("-out=report", fileName));
}


//----------------------------------------------------------------------------
// Process one text line from standard error or standard output.
//----------------------------------------------------------------------------

void QtlMovieClosedCaptionsSearch::processOutputLine(QProcess::ProcessChannel channel, const QString& textLine)
{
    // Invoke superclass. In silent mode, the line is output in debug mode only.
    QtlMovieProcess::processOutputLine(channel, textLine);

    // We filter these lines:
    //
    //   EIA-608: Yes/No
    //   CC1: Yes/No
    //   CC2: Yes/No
    //   CC3: Yes/No
    //   CC4: Yes/No
    //   CEA-708: Yes/No
    //   Services: 1 2 3 ...

    int cc;
    QStringList fields(textLine.toLower().trimmed().split(QRegExp("\\s*:\\s*"), QString::SkipEmptyParts));
    if (fields.size() >= 2 &&
        fields[1] == "yes" &&
        fields[0].size() >= 3 &&
        fields[0].startsWith("cc") &&
        (cc = fields[0].mid(2).toInt()) >= 1 &&
        cc <= 4)
    {
        // Found an EIA-608 stream (CC1 to CC4).
        emitFoundClosedCaptions(cc);
    }
    else if (fields.size() >= 2 && fields[0] == "services") {
        // Found a list of CEA-708 streams.
        foreach (const QString& num, fields[1].split(QRegExp("[\\s,]+"), QString::SkipEmptyParts)) {
            if ((cc = num.toInt()) >= 1) {
                emitFoundClosedCaptions(4 + cc);
            }
        }
    }
}


//----------------------------------------------------------------------------
// Emit the completed() signal.
//----------------------------------------------------------------------------

void QtlMovieClosedCaptionsSearch::emitCompleted(bool success, const QString& message)
{
    debug(tr("Closed Captions search completed"));

    // Notify the completion vie super-class.
    QtlMovieAction::emitCompleted(success, message);

    // Finally deallocate ourselves upon return to event loop.
    deleteLater();
}


//----------------------------------------------------------------------------
// Emit the foundClosedCaptions() signal.
//----------------------------------------------------------------------------

void QtlMovieClosedCaptionsSearch::emitFoundClosedCaptions(int ccNumber)
{
    // Compute Close Caption number.
    debug(tr("Found Closed Caption #%1").arg(ccNumber));

    // Build the stream information.
    const QtlMediaStreamInfoPtr info(new QtlMediaStreamInfo());
    info->setStreamType(QtlMediaStreamInfo::Subtitle);
    info->setSubtitleType(QtlMediaStreamInfo::SubCc);
    info->setCcNumber(ccNumber);

    // Send the new stream information to clients.
    emit foundClosedCaptions(info);
}
