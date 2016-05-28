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
// Declare the class QtlMovieTsDemux.
//
//----------------------------------------------------------------------------

#include "QtlMovieTsDemux.h"
#include "QtlMovie.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieTsDemux::QtlMovieTsDemux(const QString& fileName,
                                 const QtlMovieSettings* settings,
                                 QtlLogger* log,
                                 QObject* parent) :
    QtlMovieAction(settings, log, parent),
    _file(fileName),
    _isM2ts(false),
    _timerId(-1),
    _totalPackets(0),
    _packetInterval(0),
    _nextReport(0)
{
}


//----------------------------------------------------------------------------
// Start the analysis.
//----------------------------------------------------------------------------

bool QtlMovieTsDemux::start()
{
    // Do not start twice.
    if (!QtlMovieAction::start()) {
        return false;
    }

    // Open the file.
    if (!_file.open()) {
        emitCompleted(false, tr("Error opening %1").arg(_file.fileName()));
        return true; // true = started (and completed as well in that case.
    }

    // Start immediate timer, a way to read continuously packets but return periodically to the event loop.
    _timerId = startTimer(0);

    // Do not report progress more often that every 1% of the file size.
    _totalPackets = int(_file.size() / QTS_PKT_SIZE);
    _packetInterval = qMax(1, _totalPackets / 100);
    _nextReport = _packetInterval;

    // Will read packets later.
    return true;
}


//----------------------------------------------------------------------------
// Abort analysis.
//----------------------------------------------------------------------------

void QtlMovieTsDemux::abort()
{
    // Declare the completion with error status but no message.
    emitCompleted(false);
}


//----------------------------------------------------------------------------
// Emit the completed() signal.
//----------------------------------------------------------------------------

void QtlMovieTsDemux::emitCompleted(bool success, const QString& message)
{
    // Stop our polling for the file.
    killTimer(_timerId);
    _timerId = -1;

    // Close the file.
    _file.close();
    disconnect(&_file, 0, this, 0);

    // Cleanup the demux.
    demux()->reset();

    // Notify the completion via super-class.
    QtlMovieAction::emitCompleted(success, message);
}


//-----------------------------------------------------------------------------
// Event handler to handle timer.
//-----------------------------------------------------------------------------

void QtlMovieTsDemux::timerEvent(QTimerEvent* event)
{
    // This handler is invoked for all timer, make sure it is ours.
    if (event == 0 || _timerId < 0 || event->timerId() != _timerId) {
        // Not our poll timer, invoke superclass.
        QtlMovieAction::timerEvent(event);
        return;
    }

    // Read TS packets.
    QtsTsPacket buffer[QTL_TS_PACKETS_CHUNK];
    const int count = _file.read(buffer, QTL_TS_PACKETS_CHUNK);
    _isM2ts = _file.tsFileType() == QtsTsFile::M2tsFile;

    if (count < 0) {
        // File error.
        emitCompleted(false, tr("Error reading %1").arg(_file.fileName()));
    }
    else if (count == 0) {
        // End of file.
        emitCompleted(true);
    }
    else {
        // Process packets.
        for (int i = 0; i < count; i++) {
            demux()->feedPacket(buffer[i]);
        }
        // Report progress in the file.
        const int current  = int(demux()->packetCount());
        if (current >= _nextReport) {
            emitProgress(current, _totalPackets);
            _nextReport += _packetInterval;
        }
    }
}
