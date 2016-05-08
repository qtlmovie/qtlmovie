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
// Qtl, Qt utility library.
// Define the class QtlProcess, an encapsulation of QProcess.
//
//----------------------------------------------------------------------------

#include "QtlProcess.h"


//----------------------------------------------------------------------------
// Create a new instance. Enforce object creation on the heap.
//----------------------------------------------------------------------------

QtlProcess* QtlProcess::newInstance(const QString& program,
                                    const QStringList& arguments,
                                    int msRunTestTimeout,
                                    int maxProcessOutputSize,
                                    QObject* parent,
                                    const QProcessEnvironment& env)
{
    return new QtlProcess(program, arguments, msRunTestTimeout, maxProcessOutputSize, parent, env);
}


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlProcess::QtlProcess(const QString& program,
                       const QStringList& arguments,
                       int msRunTestTimeout,
                       int maxProcessOutputSize,
                       QObject* parent,
                       const QProcessEnvironment& env) :
    QObject(parent),
    _msRunTestTimeout(msRunTestTimeout),
    _maxProcessOutputSize(maxProcessOutputSize),
    _result(program, arguments),
    _started(false),
    _terminated(false),
    _process(new QProcess(this)),
    _timer(new QTimer(this))
{
    if (!env.isEmpty()) {
        _process->setProcessEnvironment(env);
    }
}


//----------------------------------------------------------------------------
// Start the process.
//----------------------------------------------------------------------------

void QtlProcess::start()
{
    // Do not start twice.
    if (_started) {
        return;
    }
    _started = true;

    // Get notifications from the QProcess object.
    connect(_process, &QProcess::readyReadStandardOutput, this, &QtlProcess::readOutputData);
    connect(_process, &QProcess::readyReadStandardError, this, &QtlProcess::readOutputData);
    connect(_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &QtlProcess::processFinished);
    connect(_process, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error), this, &QtlProcess::processError);

    // Start the process.
    _process->start(_result.program(), _result.arguments());

    // Immediately close the write channel.
    // We have nothing to write on the standard input of the process.
    _process->closeWriteChannel();

    // Set a sigle-shot timer to kill process on timeout.
    if (_msRunTestTimeout > 0) {
        connect(_timer, &QTimer::timeout, this, &QtlProcess::processTimeout);
        _timer->setSingleShot(true);
        _timer->start(_msRunTestTimeout);
    }
}


//----------------------------------------------------------------------------
// Cancel the start of the process.
//----------------------------------------------------------------------------

void QtlProcess::cancel()
{
    // Simulate a start and an immediate abort.
    if (!_started) {
        _started = true;
        sendTerminated(tr("Process execution canceled"));
    }
}


//----------------------------------------------------------------------------
// Read as much data as possible from the process standard error and output.
//----------------------------------------------------------------------------

void QtlProcess::readOutputData()
{
    // Read all available data.
    _result.appendStandardOutput(_process->readAllStandardOutput());
    _result.appendStandardError(_process->readAllStandardError());

    // Check if the output limit has been reached.
    if (_maxProcessOutputSize > 0 && (_result.standardOutputSize() > _maxProcessOutputSize || _result.standardErrorSize() > _maxProcessOutputSize)) {
        sendTerminated(tr("Process output/error size exceeds limit"));
    }
}


//----------------------------------------------------------------------------
// Invoked when the process is finished.
//----------------------------------------------------------------------------

void QtlProcess::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    _result.setExitCode(exitCode);
    sendTerminated(exitStatus == QProcess::NormalExit ? "" : tr("Process crashed"));
}


//----------------------------------------------------------------------------
// Invoked when an error occurs on the process.
//----------------------------------------------------------------------------

void QtlProcess::processError(QProcess::ProcessError error)
{
    // Send an appropriate message.
    switch (error) {
    case QProcess::FailedToStart:
        sendTerminated(tr("Failed to start process"));
        break;
    case QProcess::Crashed:
        sendTerminated(tr("Process crashed"));
        break;
    default:
        sendTerminated(tr("Unknown process error"));
        break;
    }
}


//----------------------------------------------------------------------------
// Invoked when the process execution is too long.
//----------------------------------------------------------------------------

void QtlProcess::processTimeout()
{
    sendTerminated(tr("Process exceeds execution time limit"));
}


//----------------------------------------------------------------------------
// Send the terminated() signal.
//----------------------------------------------------------------------------

void QtlProcess::sendTerminated(const QString& message)
{
    // Do not send twice.
    if (!_started || _terminated) {
        return;
    }
    _terminated = true;
    _result.setErrorMessage(message);

    // Disconnect notifications from our internal objects.
    disconnect(_process, 0, this, 0);
    disconnect(_timer, 0, this, 0);

    // Cancel the timer, if still running.
    _timer->stop();

    // Read all remaining available data.
    _result.appendStandardOutput(_process->readAllStandardOutput());
    _result.appendStandardError(_process->readAllStandardError());

    // Notify the clients. The exit code is available only when there is no other error.
    emit terminated(_result);

    // Be sure to kill the process, in case of premature error.
    _process->kill();

    // Finally deallocate ourselves upon return to event loop.
    deleteLater();
}
