//----------------------------------------------------------------------------
//
// Copyright (c) 2014, Thierry Lelegard
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
// Define the class QtlMovieTask.
//
//----------------------------------------------------------------------------

#include "QtlMovieTask.h"
#include "QtlMessageBoxUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieTask::QtlMovieTask(const QtlMovieSettings* settings, QtlLogger* log, QObject* parent) :
    QObject(parent),
    _settings(settings),
    _inFile(new QtlMovieInputFile("", settings, log, this)),
    _outFile(new QtlMovieOutputFile("", settings, log, this)),
    _state(Queued)
{
    // Emit taskChanged when required.
    connect(_inFile,  &QtlMovieInputFile::fileNameChanged,    this, &QtlMovieTask::emitTaskChanged);
    connect(_outFile, &QtlMovieOutputFile::fileNameChanged,   this, &QtlMovieTask::emitTaskChanged);
    connect(_outFile, &QtlMovieOutputFile::outputTypeChanged, this, &QtlMovieTask::emitTaskChanged);
}


//----------------------------------------------------------------------------
// Slots invoked when the state changes.
//----------------------------------------------------------------------------

void QtlMovieTask::setStarted()
{
    setState(Running);
}

void QtlMovieTask::setCompleted(bool success)
{
    setState(success ? Success : Failed);
}

void QtlMovieTask::setState(State state)
{
    if (_state != state) {
        _state = state;
        emit stateChanged(this);
    }
}


//----------------------------------------------------------------------------
// This slot is used as relay to emit the signal taskChanged().
//----------------------------------------------------------------------------

void QtlMovieTask::emitTaskChanged()
{
    emit taskChanged(this);
}


//----------------------------------------------------------------------------
// Ask the user if the output file may be overwritten.
//----------------------------------------------------------------------------

bool QtlMovieTask::askOverwriteOutput()
{
    // If the output file already exists...
    QFile out(_outFile->fileName());
    if (out.exists()) {

        // Ask for confirmation to overwrite it.
        if (!qtlConfirm(this, tr("File %1 already exists.\nOverwrite it?").arg(_outFile->fileName()))) {
            // Don't overwrite, give up.
            return false;
        }

        // Delete the previous output file to avoid using it by mistake if the conversion fails.
        if (!out.remove()) {
            // Failed to delete it. Continue anyway, will be overwritten by converter.
            _outFile->log()->line(tr("Failed to delete %1").arg(_outFile->fileName()));
        }
    }

    // Yes, we can overwrite the output file (non existent or just deleted in fact).
    return true;
}
