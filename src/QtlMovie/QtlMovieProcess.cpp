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
// Define the class QtlMovieProcess.
//
//----------------------------------------------------------------------------

#include "QtlMovieProcess.h"
#include "QtlUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieProcess::QtlMovieProcess(const QtlMovieExecFile* execFile,
                                 const QStringList& arguments,
                                 bool hasBinaryOutput,
                                 const QtlMovieSettings* settings,
                                 QtlLogger* log,
                                 QObject *parent):
    QtlMovieAction(settings, log, parent),
    _process(new QProcess(this)),
    _execFile(execFile),
    _arguments(arguments),
    _hasBinaryOutput(hasBinaryOutput),
    _stdOutput(),
    _stdError()
{
    Q_ASSERT(execFile != 0);
}


//----------------------------------------------------------------------------
// Set the command line arguments.
//----------------------------------------------------------------------------

void QtlMovieProcess::setArguments(const QStringList& arguments)
{
    if (!isStarted()) {
        _arguments = arguments;
    }
}


//----------------------------------------------------------------------------
// Start the process.
//----------------------------------------------------------------------------

bool QtlMovieProcess::start()
{
    // Do not start twice.
    if (!QtlMovieAction::start()) {
        return false;
    }

    // Get executable.
    const QString exec(_execFile->fileName());
    if (exec.isEmpty()) {
        processCompleted(false, tr("%1 not found, install it or set explicit path in settings.").arg(_execFile->name()));
        return true;
    }

    // Set the environment variables for the process.
    // Use current process environment as a starting point.
    QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
    updateEnvironment(env);
    _process->setProcessEnvironment(env);

    // Display the command in the log window.
    line(exec + " " + _arguments.join(" "), QColor("blue"));
    line("");

    // Differenciate standard error and standard output channels.
    _process->setProcessChannelMode(QProcess::SeparateChannels);

    // Make sure that reading from the QProcess object means reading from standard output.
    _process->setReadChannel(QProcess::StandardOutput);

    // Get notifications from the QProcess object.
    connect(_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
    connect(_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
    connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(readData()));
    if (_hasBinaryOutput) {
        connect(_process, SIGNAL(readyReadStandardOutput()), this, SIGNAL(readyReadOutputData()));
    }
    else {
        connect(_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readData()));
    }

    // Start the process.
    _process->start(exec, _arguments);

    // Immediately close the write channel.
    // We have nothing to write on the standard input of the process.
    _process->closeWriteChannel();
    return true;
}


//----------------------------------------------------------------------------
// Abort process execution.
//----------------------------------------------------------------------------

void QtlMovieProcess::abort()
{
    // Silently kill the process. If the process was executing, the process completion will be notified.
    _process->kill();
}


//----------------------------------------------------------------------------
// Invoked when the process is finished.
//----------------------------------------------------------------------------

void QtlMovieProcess::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit) {
        processCompleted(true, tr("%1 completed with status %2").arg(_execFile->name()).arg(exitCode));
    }
    else {
        processCompleted(false, tr("%1 process crashed").arg(_execFile->name()));
    }
}


//----------------------------------------------------------------------------
// Invoked when an error occurs on the process.
//----------------------------------------------------------------------------

void QtlMovieProcess::processError(QProcess::ProcessError error)
{
    switch (error) {
    case QProcess::FailedToStart:
        processCompleted(false, tr("Failed to start %1 process").arg(_execFile->name()));
        break;
    case QProcess::Crashed:
        processCompleted(false, tr("%1 process crashed").arg(_execFile->name()));
        break;
    default:
        processCompleted(false, tr("%1 failed, unknown process error").arg(_execFile->name()));
        break;
    }
}


//----------------------------------------------------------------------------
// Process the termination of the process.
//----------------------------------------------------------------------------

void QtlMovieProcess::processCompleted(bool success, const QString& message)
{
    // Disconnect notifications from our internal objects.
    disconnect(_process, 0, this, 0);

    // Kill the process in case it is not yet terminated.
    _process->kill();

    // Flush standard error.
    readData();
    if (!_stdOutput.isEmpty()) {
        line(_stdOutput);
    }
    if (!_stdError.isEmpty()) {
        line(_stdError);
    }

    // Let superclass notify clients.
    emitCompleted(success, message);
}


//----------------------------------------------------------------------------
// Update a QProcessEnvironment for the target process.
//----------------------------------------------------------------------------

void QtlMovieProcess::updateEnvironment(QProcessEnvironment& env)
{
    // Does nothing by default. Subclasses may overwrite it.
    Q_UNUSED(env);
}


//----------------------------------------------------------------------------
// Process one text line from standard error or standard output.
//----------------------------------------------------------------------------

void QtlMovieProcess::processOutputLine(QProcess::ProcessChannel channel, const QString& textLine)
{
    // Simply log the line. Subclasses may overwrite it.
    Q_UNUSED(channel);
    line(textLine);
}


//----------------------------------------------------------------------------
// Read as much data as possible from the process standard error or output.
//----------------------------------------------------------------------------

void QtlMovieProcess::readData()
{
    _stdError.append(_process->readAllStandardError());
    processOutputBuffer(QProcess::StandardError, _stdError);

    if (!_hasBinaryOutput) {
        _stdOutput.append(_process->readAllStandardOutput());
        processOutputBuffer(QProcess::StandardOutput, _stdOutput);
    }
}


//----------------------------------------------------------------------------
// Process a standard error or standard output buffer.
//----------------------------------------------------------------------------

void QtlMovieProcess::processOutputBuffer(QProcess::ProcessChannel channel, QString& buffer)
{
    // Normalize line terminators in buffer, remove empty lines.
    buffer.replace(QRegExp("[\\r\\n]+"), "\n");

    // Loop on all text lines in the buffer.
    int eol;
    while ((eol = buffer.indexOf(QChar('\n'))) >= 0) {

        // Extract the line from the buffer.
        const QString line(buffer.left(eol));
        buffer.remove(0, eol + 1);

        // Process the line.
        processOutputLine(channel, line);
    }
}
