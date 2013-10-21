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
// Define the class QtlMovieClosedCaptionsSearch.
//
//----------------------------------------------------------------------------

#include "QtlMovieClosedCaptionsSearch.h"
#include "QtlMovieCcExtractorProcess.h"


//----------------------------------------------------------------------------
// Create a new instance. Enforce object creation on the heap.
//----------------------------------------------------------------------------

QtlMovieClosedCaptionsSearch* QtlMovieClosedCaptionsSearch::newInstance(const QString& fileName,
                                                                        int ccChannel,
                                                                        const QtlMovieSettings* settings,
                                                                        QtlLogger* log,
                                                                        QObject* parent)
{
    return new QtlMovieClosedCaptionsSearch(fileName, ccChannel, settings, log, parent);
}


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlMovieClosedCaptionsSearch::QtlMovieClosedCaptionsSearch(const QString& fileName,
                                                           int ccChannel,
                                                           const QtlMovieSettings* settings,
                                                           QtlLogger* log,
                                                           QObject* parent) :
    QtlMovieProcess(settings->ccextractor(), QStringList(), false, settings, log, parent),
    _fileName(fileName),
    _ccChannel(ccChannel),
    _output1(newFileName()),
    _output2(newFileName())
{
    // No need to report in search phase.
    setSilent(true);

    // Build CCExtractor command line.
    // The analysis duration is identical to the ffmpeg probe duration.
    QStringList args;
    args << "-quiet"              // No information message.
         << "-noteletext"         // Closed Captions only, ignore Teletext.
         << "-srt"                // Output format is SRT.
         << "-utf8"               // UTF-8 output.
         << QtlMovieCcExtractorProcess::durationOptions(settings->ffmpegProbeSeconds())
         << "-12";                // Extract fields 1 and 2.
    if (ccChannel > 1) {
        args << "-cc2";           // Extract from channel 2.
    }
    args << "-o1" << _output1     // SRT file for field 1.
         << "-o2" << _output2     // SRT file for field 2.
         << fileName;             // Input file.
    setArguments(args);
}


//----------------------------------------------------------------------------
// Emit the completed() signal.
//----------------------------------------------------------------------------

void QtlMovieClosedCaptionsSearch::emitCompleted(bool success, const QString& message)
{
    debug(tr("Closed Captions search completed on channel %1").arg(_ccChannel));

    // Report CC if the resulting SRT file exists and is non empty.
    QFile file1(_output1);
    QFile file2(_output2);
    if (file1.exists() && file1.size() > 0) {
        emitFoundClosedCaptions(1);
    }
    if (file2.exists() && file2.size() > 0) {
        emitFoundClosedCaptions(2);
    }
    file1.remove();
    file2.remove();

    // Notify the completion vie super-class.
    QtlMovieAction::emitCompleted(success, message);

    // Finally deallocate ourselves upon return to event loop.
    deleteLater();
}


//----------------------------------------------------------------------------
// Emit the foundClosedCaptions() signal.
//----------------------------------------------------------------------------

void QtlMovieClosedCaptionsSearch::emitFoundClosedCaptions(int field)
{
    // Compute Close Caption number.
    const int ccNumber = field + (_ccChannel == 1 ? 0 : 2);
    debug(tr("Found Closed Caption #%1").arg(ccNumber));

    // Build the stream information.
    const QtlMovieStreamInfoPtr info(new QtlMovieStreamInfo());
    info->setStreamType(QtlMovieStreamInfo::Subtitle);
    info->setSubtitleType(QtlMovieStreamInfo::SubCc);
    info->setCcNumber(ccNumber);

    // Send the new stream information to clients.
    emit foundClosedCaptions(info);
}


//----------------------------------------------------------------------------
// Generate a new unique temporary file name.
//----------------------------------------------------------------------------

QString QtlMovieClosedCaptionsSearch::newFileName()
{
    QString uuid(QUuid::createUuid().toString());
    uuid.remove(QRegExp("[^0-9a-zA-Z]+"));
    return QtlFile::absoluteNativeFilePath(QDir::tempPath() + QDir::separator() + "qtlcctemp-" + QString::number(QCoreApplication::applicationPid()) + "-" + uuid + ".srt");
}
