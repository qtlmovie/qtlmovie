//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
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
// Define the class QtlMovieCleanupSubtitles.
//
//----------------------------------------------------------------------------

#include "QtlMovieCleanupSubtitles.h"
#include "QtlMovie.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieCleanupSubtitles::QtlMovieCleanupSubtitles(const QString& inputFileName,
                                                   const QString& outputFileName,
                                                   const QtlMovieSettings* settings,
                                                   QtlLogger* log,
                                                   QObject* parent) :
    QtlMovieAction(settings, log, parent),
    _inputFile(inputFileName),
    _outputFile(outputFileName),
    _timerId(-1),
    _totalSize(0),
    _readSize(0),
    _reportInterval(0),
    _nextReport(0)
{
}


//----------------------------------------------------------------------------
// Start the analysis.
//----------------------------------------------------------------------------

bool QtlMovieCleanupSubtitles::start()
{
    // Do not start twice.
    if (!QtlMovieAction::start()) {
        return false;
    }

    // Open the input file.
    if (!_inputFile.open(QFile::ReadOnly)) {
        emitCompleted(false, tr("Error opening %1").arg(_inputFile.fileName()));
        return true; // true = started (and completed as well in that case).
    }

    // Create the output file.
    if (!_outputFile.open(QFile::WriteOnly)) {
        _inputFile.close();
        emitCompleted(false, tr("Error creating %1").arg(_outputFile.fileName()));
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

void QtlMovieCleanupSubtitles::abort()
{
    // Declare the completion with error status but no message.
    emitCompleted(false);
}


//----------------------------------------------------------------------------
// Emit the completed() signal.
//----------------------------------------------------------------------------

void QtlMovieCleanupSubtitles::emitCompleted(bool success, const QString& message)
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

void QtlMovieCleanupSubtitles::timerEvent(QTimerEvent* event)
{
    // This handler is invoked for all timer, make sure it is ours.
    if (event == 0 || _timerId < 0 || event->timerId() != _timerId) {
        // Not our poll timer, invoke superclass.
        QtlMovieAction::timerEvent(event);
        return;
    }

    // Read a chunk of the input file.
    char buffer[QTL_FILE_CHUNK];
    const qint64 inCount = _inputFile.read(buffer, sizeof(buffer));

    if (inCount < 0) {
        // File error.
        emitCompleted(false, tr("Error reading %1").arg(_inputFile.fileName()));
    }
    else if (inCount == 0) {
        // End of file.
        emitCompleted(true);
    }
    else {
        // Count total input bytes.
        _readSize += inCount;

        // Cleanup the buffer, remove nul bytes.
        char* wr = buffer;
        for (const char* rd = buffer; rd < buffer + inCount; ++rd) {
            if (*rd != 0) {
                *wr++ = *rd;
            }
        }
        const qint64 outCount = wr - buffer;

        // Write the clean buffer to output file.
        if (_outputFile.write(buffer, outCount) < outCount) {
            emitCompleted(false, tr("Error writing %1").arg(_outputFile.fileName()));
        }
        else if (_readSize >= _nextReport) {
            // Report progress in the file.
            emitProgress(int(_readSize), int(_totalSize));
            _nextReport += _reportInterval;
        }
    }
}
