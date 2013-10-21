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
// Define the class QtlMovieAction.
//
//----------------------------------------------------------------------------

#include "QtlMovieAction.h"



//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieAction::QtlMovieAction(const QtlMovieSettings* settings, QtlLogger* log, QObject* parent):
    QObject(parent),
    _settings(settings),
    _log(log),
    _description(),
    _startTime(),
    _started(false),
    _completed(false),
    _silent(false)
{
    Q_ASSERT(log != 0);
    Q_ASSERT(settings != 0);
}


//----------------------------------------------------------------------------
// Implementation of QtlLogger.
//----------------------------------------------------------------------------

void QtlMovieAction::text(const QString& text)
{
    if (_log != 0 && !_silent) {
        _log->text(text);
    }
}

void QtlMovieAction::line(const QString& line, const QColor& color)
{
    if (_log != 0) {
        if (_silent) {
            _log->debug(line);
        }
        else {
            _log->line(line, color);
        }
    }
}

void QtlMovieAction::debug(const QString& line, const QColor& color)
{
    if (_log != 0) {
        _log->debug(line, color);
    }
}


//----------------------------------------------------------------------------
// Start the action.
//----------------------------------------------------------------------------

bool QtlMovieAction::start()
{
    if (_started) {
        // Do not start twice.
        return false;
    }
    else {
        _started = true;
        _startTime = QDateTime::currentDateTimeUtc();

        // Display the action description in the log window.
        if (!_description.isEmpty()) {
            line("");
            line(tr("Start %1").arg(_description));
        }

        // Immediately emit started() to avoid emitting completed() without a previous started().
        emit started();

        // Signal initial "progress".
        emitProgress(0, 0);
        return true;
    }
}


//----------------------------------------------------------------------------
// Abort Action execution.
//----------------------------------------------------------------------------

void QtlMovieAction::abort()
{
    // The default implementation does not do anything.
    // Subclasses should overwrite it.
}


//----------------------------------------------------------------------------
// Emit the progress() signal.
//----------------------------------------------------------------------------

void QtlMovieAction::emitProgress(int current, int maximum, int remainingSeconds)
{
    // Seconds since started
    const int elapsed = int(_startTime.secsTo(QDateTime::currentDateTimeUtc()));

    // Compute remaining time if necessary and possible (assume strictly linear).
    if (remainingSeconds < 0 && elapsed > 0 && current > 0 && maximum >= current) {
        remainingSeconds = ((elapsed * maximum) / current) - elapsed;
    }

    // Emit the signal.
    emit progress(_description, current, maximum, elapsed, remainingSeconds);

    // Simply echo a dot in the log.
    text(".");
}


//----------------------------------------------------------------------------
// Emit the completed() signal.
//----------------------------------------------------------------------------

void QtlMovieAction::emitCompleted(bool success, const QString& message)
{
    // Log optional error message.
    if (!message.isEmpty()) {
        line(message, success ? QColor() : QColor("red"));
    }

    // Emit completed() only once.
    if (_started && !_completed) {
        _completed = true;
        emit completed(success);
    }
}
