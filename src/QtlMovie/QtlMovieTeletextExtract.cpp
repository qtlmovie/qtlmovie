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
// Declare the class QtlMovieTeletextExtract.
//
//----------------------------------------------------------------------------

#include "QtlMovieTeletextExtract.h"
#include "QtlMovie.h"
#include "QtsTeletextFrame.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------


QtlMovieTeletextExtract::QtlMovieTeletextExtract(const QString& inputFileName,
                                                 QtsPid pid,
                                                 int teletextPage,
                                                 const QString& outputFileName,
                                                 const QtlMovieSettings* settings,
                                                 QtlLogger* log,
                                                 QObject* parent) :
    QtlMovieTsDemux(inputFileName, settings, log, parent),
    _demux(this),
    _output(outputFileName),
    _stream(&_output),
    _pid(pid),
    _page(teletextPage)
{
    // Set the demux to collect the Teletext PID.
    _demux.addPid(pid);
}


//----------------------------------------------------------------------------
// Start the extraction.
//----------------------------------------------------------------------------

bool QtlMovieTeletextExtract::start()
{
    // Open input file in superclass.
    if (!QtlMovieTsDemux::start()) {
        return false;
    }

    // Create the output file.
    _stream.setCodec("UTF-8");
    _stream.setGenerateByteOrderMark(true);
    if (!_output.open(QFile::WriteOnly)) {
        emitCompleted(false, tr("Error creating %1").arg(_output.fileName()));
        return true; // true = started (and completed as well in that case).
    }

    return true;
}


//----------------------------------------------------------------------------
// Emit the completed() signal.
//----------------------------------------------------------------------------

void QtlMovieTeletextExtract::emitCompleted(bool success, const QString& message)
{
    if (_output.isOpen()) {

        // Flush pending Teletext messages.
        _demux.flushTeletext();

        // Close the output file.
        _stream.flush();
        _output.close();
    }

    // Cleanup the demux.
    demux()->reset();

    // Notify the completion via super-class.
    QtlMovieTsDemux::emitCompleted(success, message);
}


//----------------------------------------------------------------------------
// Invoked when a complete Teletext message is available.
//----------------------------------------------------------------------------

void QtlMovieTeletextExtract::handleTeletextMessage(QtsTeletextDemux& demux, const QtsTeletextFrame& frame)
{
    if (frame.pid() == _pid && frame.page() == _page && _output.isOpen()) {
        _stream << frame.srtFrame();
    }
}
