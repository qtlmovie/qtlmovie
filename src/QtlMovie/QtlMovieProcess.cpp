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
// Define the class QtlMovieProcess.
//
//----------------------------------------------------------------------------

#include "QtlMovieProcess.h"
#include "QtlMovieJob.h"
#include "QtlEnumUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieProcess::QtlMovieProcess(const QtlMovieExecFile* execFile,
                                 const QStringList& arguments,
                                 bool hasBinaryOutput,
                                 const QtlMovieSettings* settings,
                                 QtlLogger* log,
                                 QObject *parent,
                                 QtlDataPull* dataPull):
    QtlMovieAction(settings, log, parent),
    _process(new QtlProcess(this)),
    _execFile(execFile),
    _arguments(arguments),
    _hasBinaryOutput(hasBinaryOutput),
    _dataPull(dataPull),
    _stdOutput(),
    _stdError(),
    _dpProgress(false),
    _gotError(false),
    _processError(QProcess::UnknownError)
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

    // Replace variables in the argument list.
    for (QStringList::iterator it = _arguments.begin(); it != _arguments.end(); ) {
         if (it->startsWith("{") && it->endsWith("}")) {
             // Found a "{varname}" reference, get its value.
             const QString name(it->mid(1, it->length() - 2));
             const QStringList value(getJobVariable(name));

             // Remove the reference from the argument list.
             // Now it points to the next element, can be end().
             it = _arguments.erase(it);

             // Insert all string values.
             foreach (const QString& arg, value) {
                 // Insert arg before *it, return an iterator to the inserted element.
                 it = _arguments.insert(it, arg);
                 // Point to next element.
                 it++;
             }
         }
         else {
             // Not a variable reference, move to next element.
             it++;
         }
    }

    // Display the command in the log window.
    line(exec + " " + _arguments.join(" "), QColor(Qt::blue));
    line("");

    // Differenciate standard error and standard output channels.
    _process->setProcessChannelMode(QProcess::SeparateChannels);

    // Make sure that reading from the QProcess object means reading from standard output.
    _process->setReadChannel(QProcess::StandardOutput);

    // Get notifications from the QProcess object.
    connect(_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &QtlMovieProcess::processFinished);
    connect(_process, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error), this, &QtlMovieProcess::processError);
    connect(_process, &QProcess::readyReadStandardError, this, &QtlMovieProcess::readData);
    if (_hasBinaryOutput) {
        connect(_process, &QProcess::readyReadStandardOutput, this, &QtlMovieProcess::readyReadOutputData);
    }
    else {
        connect(_process, &QProcess::readyReadStandardOutput, this, &QtlMovieProcess::readData);
    }

    // Start the process.
    _process->start(exec, _arguments);

    if (_dataPull != 0) {
        // Start feed the standard input of the process using a QtlDataPull object.
        _dataPull->start(_process);
    }
    else {
        // Immediately close the write channel if we have nothing to write on the standard input of the process.
        _process->closeWriteChannel();
    }
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
    // An error was signaled by the QProcess object. This does not mean that the process has terminated.
    // If the error is fatal, then processFinished will be invoked later.
    _gotError = true;
    _processError = error;
    debug(tr("Process %1 reported error %2").arg(_execFile->name()).arg(qtlNameOf(error)));
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

    // When an error was reported, filter out errors we consider as "normal".
    QString realMessage(message);
    if (success && _gotError) {
        switch (_processError) {
            case QProcess::ReadError:
            case QProcess::WriteError:
                // Not a real error if we interrupted the process.
                success = true;
                break;
            case QProcess::FailedToStart:
            case QProcess::Crashed:
            case QProcess::Timedout:
            case QProcess::UnknownError:
            default:
                // Consider as a failure.
                success = false;
                realMessage = tr("Process %1 terminated with error %2").arg(_execFile->name()).arg(qtlNameOf(_processError));
                break;
        }
    }

    // Let superclass notify clients.
    emitCompleted(success, realMessage);
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


//----------------------------------------------------------------------------
// Store / retrieve a variable in the parent job.
//----------------------------------------------------------------------------

void QtlMovieProcess::setJobVariable(const QString& name, const QStringList& value)
{
    // Get the parent job, if any.
    QtlMovieJob* job = qobject_cast<QtlMovieJob*>(parent());
    if (job != 0) {
        job->setVariable(name, value);
    }
}

QStringList QtlMovieProcess::getJobVariable(const QString& name) const
{
    // Get the parent job, if any.
    QtlMovieJob* job = qobject_cast<QtlMovieJob*>(parent());
    return job == 0 ? QStringList() : job->getVariable(name);
}


//----------------------------------------------------------------------------
// Use the progress report from the QtlDataPull when possible.
//----------------------------------------------------------------------------

bool QtlMovieProcess::useDataPullProgressReport(bool on)
{
    const bool wasSet = _dpProgress;
    _dpProgress = on && _dataPull != 0 && _dataPull->progressAvailable();
    if (_dpProgress && !wasSet) {
        connect(_dataPull, &QtlDataPull::progress, this, &QtlMovieProcess::dataPullProgressed);
    }
    else if (wasSet && !_dpProgress && _dataPull != 0) {
        disconnect(_dataPull, &QtlDataPull::progress, this, &QtlMovieProcess::dataPullProgressed);
    }
    return _dpProgress;
}


//----------------------------------------------------------------------------
// Invoked when some progress in the QtlDataPull is available.
//----------------------------------------------------------------------------

void QtlMovieProcess::dataPullProgressed(qint64 current, qint64 maximum)
{
    // A data pull can extract gigabytes of data and sizes are qint64.
    // But QtlMovieAction uses int's. So, we may need to reduce the scale.
    const qint64 maxInt = std::numeric_limits<int>::max();
    while (maximum > maxInt) {
        maximum = maximum / 1000;
        current = current / 1000;
    }

    emitProgress(int(current), int(maximum));
}
