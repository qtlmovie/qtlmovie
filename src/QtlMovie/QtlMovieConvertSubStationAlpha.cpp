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
// Define the class QtlMovieConvertSubStationAlpha.
//
//----------------------------------------------------------------------------

#include "QtlMovieConvertSubStationAlpha.h"
#include "QtlMovie.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieConvertSubStationAlpha::QtlMovieConvertSubStationAlpha(const QString& inputFileName,
                                                               const QString& outputFileName,
                                                               const QtlMovieSettings* settings,
                                                               QtlLogger* log,
                                                               QObject* parent) :
    QtlMovieAction(settings, log, parent),
    _inputFile(inputFileName),
    _inputStream(&_inputFile),
    _inputParser(log),
    _outputFile(),
    _outputFileName(outputFileName),
    _useHtml(settings->useSrtHtmlTags()),
    _timerId(-1),
    _totalSize(0),
    _reportInterval(0),
    _nextReport(0)
{
    // Always assume that SSA/ASS files are UTF-8. Is this true?
    // Most SSA/ASS files seen so far are UTF-8 without BOM. Because of the
    // lack of BOM, not specifying UTF-8 explicitely fails to correctly
    // interpret the file content.
    _inputStream.setCodec("UTF-8");

    // Each time the SSA/ASS parser finds a frame, get notified in this object.
    connect(&_inputParser, &QtlSubStationAlphaParser::subtitleFrame, this, &QtlMovieConvertSubStationAlpha::processSubtitleFrame);
}


//----------------------------------------------------------------------------
// Start the analysis.
//----------------------------------------------------------------------------

bool QtlMovieConvertSubStationAlpha::start()
{
    // Do not start twice.
    if (!QtlMovieAction::start()) {
        return false;
    }

    debug(tr("Starting SSA/ASS to SRT conversion"));
    debug(tr("Input file: %1").arg(_inputFile.fileName()));
    debug(tr("Output file: %1").arg(_outputFileName));

    // Open the input file.
    if (!_inputFile.open(QFile::ReadOnly)) {
        emitCompleted(false, tr("Error opening %1").arg(_inputFile.fileName()));
        return true; // true = started (and completed as well in that case).
    }

    // Create the output file.
    if (!_outputFile.open(_outputFileName)) {
        _inputFile.close();
        emitCompleted(false, tr("Error creating %1").arg(_outputFileName));
        return true; // true = started (and completed as well in that case).
    }

    // Start immediate timer, a way to read continuously packets but return periodically to the event loop.
    _timerId = startTimer(0);

    // Do not report progress more often that every 1% of the file size or 1 kB.
    _totalSize = _inputFile.size();
    _reportInterval = qMax(Q_INT64_C(1024), _totalSize / 100);
    _nextReport = _reportInterval;

    // Will read packets later.
    return true;
}


//----------------------------------------------------------------------------
// Abort analysis.
//----------------------------------------------------------------------------

void QtlMovieConvertSubStationAlpha::abort()
{
    // Declare the completion with error status but no message.
    emitCompleted(false);
}


//----------------------------------------------------------------------------
// Emit the completed() signal.
//----------------------------------------------------------------------------

void QtlMovieConvertSubStationAlpha::emitCompleted(bool success, const QString& message)
{
    // Stop our polling for the file.
    killTimer(_timerId);
    _timerId = -1;

    // Close the files.
    _inputFile.close();
    _outputFile.close();

    // Notify the completion via super-class.
    QtlMovieAction::emitCompleted(success, message);
}


//-----------------------------------------------------------------------------
// Event handler to handle timer.
//-----------------------------------------------------------------------------

void QtlMovieConvertSubStationAlpha::timerEvent(QTimerEvent* event)
{
    // This handler is invoked for all timer, make sure it is ours.
    if (event == 0 || _timerId < 0 || event->timerId() != _timerId) {
        // Not our poll timer, invoke superclass.
        QtlMovieAction::timerEvent(event);
        return;
    }

    // Read a chunk of the input file.
    qint64 current = _inputFile.pos();
    const qint64 endChunk = current + QTL_FILE_CHUNK;
    QString line;
    while (current < endChunk) {

        // Read one line of input text.
        if (!_inputStream.readLineInto(&line)) {
            // End of file (or maybe read error).
            emitCompleted(true);
            break;
        }

        // Submit it to the SSA/ASS parser.
        _inputParser.addLine(line);

        // Report progress in the file.
        current = _inputFile.pos();
        if (current >= _nextReport) {
            emitProgress(int(current), int(_totalSize));
            _nextReport += _reportInterval;
        }
    }
}


//-----------------------------------------------------------------------------
// Invoked when a subtitle frame is available.
//-----------------------------------------------------------------------------

void QtlMovieConvertSubStationAlpha::processSubtitleFrame(const QtlSubStationAlphaFramePtr& frame)
{
    if (!frame.isNull()) {
        _outputFile.addFrame(frame->showTimestamp(), frame->hideTimestamp(), frame->toSubRip(_useHtml));
    }
}
