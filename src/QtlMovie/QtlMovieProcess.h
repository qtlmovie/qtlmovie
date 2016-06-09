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
//!
//! @file QtlMovieProcess.h
//!
//! Declare the class QtlMovieProcess.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEPROCESS_H
#define QTLMOVIEPROCESS_H

#include <QtCore>
#include "QtlMovieAction.h"
#include "QtlMovieExecFile.h"

//!
//! An execution of a media tool process.
//!
//! Text lines from standard error are logged but can be intercepted by
//! subclasses overwriting processOutputLine().
//!
//! The standard output can be either text (in which case it is processed
//! in the same way as standard error) or binary data. In the later case,
//! availability of standard output data is signaled by readyReadOutputData().
//! Standard output data can be read from outputDevice().
//!
//! The progress report can be signaled by progress(). Emitting this signal
//! is entirely up to the subclass.
//!
class QtlMovieProcess : public QtlMovieAction
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] execFile Process executable file.
    //! @param [in] arguments Command line arguments.
    //! @param [in] hasBinaryOutput When true, the standard output must be read from
    //! another object. Availability of data is signaled by readyReadOutputData().
    //! When false, the standard output is logged, just like the standard error.
    //! @param [in] settings Application settings.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //! @param [in] pipeInput If true, the standard input of the process can be fed
    //! using inputDevice().
    //!
    QtlMovieProcess(const QtlMovieExecFile* execFile,
                    const QStringList& arguments,
                    bool hasBinaryOutput,
                    const QtlMovieSettings* settings,
                    QtlLogger* log,
                    QObject* parent = 0,
                    bool pipeInput = false);

    //!
    //! Start the process.
    //! @return False if already started. True otherwise.
    //!
    virtual bool start();

    //!
    //! Abort process execution.
    //! If the process was started, the signal completed() will be emitted when the process actually terminates.
    //!
    virtual void abort();

    //!
    //! Get the command executable file.
    //! @return The command executable file.
    //!
    const QtlMovieExecFile* execFile() const
    {
        return _execFile;
    }

    //!
    //! Get the command line arguments.
    //! @return The command line arguments.
    //!
    QStringList arguments() const
    {
        return _arguments;
    }

    //!
    //! Set the command line arguments.
    //! The process must not be already started.
    //! @return The command line arguments.
    //!
    void setArguments(const QStringList& arguments);

    //!
    //! Return the device from which  standard output can be read.
    //! @return A device from which to read or zero if @a hasBinaryOutput is false.
    //!
    QIODevice* outputDevice() const
    {
        return _hasBinaryOutput ? _process : 0;
    }

    //!
    //! Get the device representing the input pipe of the process.
    //! The returned QIODevice is writeable only.
    //! @return The input pipe as a QIODevice or zero if @a pipeInput
    //! was set to false in the constructor.
    //!
    QIODevice* inputDevice() const
    {
        return _pipeInput ? _process : 0;
    }

signals:
    //!
    //! Emitted when some binary data are available from standard output.
    //!
    void readyReadOutputData();

protected:
    //!
    //! Process one text line from standard error or standard output.
    //! Simply log the line. Subclasses may overwrite it.
    //! @param [in] channel Origin of the line (QProcess::StandardOutput or QProcess::StandardError).
    //! @param [in] textLine Text line.
    //!
    virtual void processOutputLine(QProcess::ProcessChannel channel, const QString& textLine);

    //!
    //! Update the QProcessEnvironment for the target process before it starts.
    //! Does nothing by default. Subclasses may overwrite it.
    //! @param [in] env A set of environment variables to update.
    //!
    virtual void updateEnvironment(QProcessEnvironment& env);

    //!
    //! Store a variable in the parent job.
    //! @param [in] name Variable name.
    //! @param [in] value Variable value as a list of strings.
    //!
    void setJobVariable(const QString& name, const QStringList& value);

    //!
    //! Get the value of a variable in the job.
    //! @param [in] name Variable name.
    //! @return Variable value as a list of strings.
    //!
    QStringList getJobVariable(const QString& name) const;

private slots:
    //!
    //! Read as much data as possible from the process standard error
    //! (and standard output if hasBinaryOutput is false).
    //!
    void readData();

    //!
    //! Invoked when the process is finished.
    //! @param [in] exitCode Process exit code.
    //! @param [in] exitStatus QProcess exit status.
    //!
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

    //!
    //! Invoked when an error occurs on the process.
    //! @param [in] error QProcess error code.
    //!
    void processError(QProcess::ProcessError error);

private:
    QProcess*               _process;         //!< Process instance.
    const QtlMovieExecFile* _execFile;        //!< Process executable file.
    QStringList             _arguments;       //!< Command line arguments.
    bool                    _hasBinaryOutput; //!< Treat standard output as binary data.
    bool                    _pipeInput;       //!< Process input is an accessible pipe.
    QString                 _stdOutput;       //!< Standard error buffer.
    QString                 _stdError;        //!< Standard error buffer.

    //!
    //! Process the termination of the process.
    //! @param [in] success True when the process completed successfully, false otherwise.
    //! @param [in] message Optional error message to log.
    //!
    void processCompleted(bool success, const QString& message = QString());

    //!
    //! Process a standard error or standard output buffer.
    //! @param [in] channel Origin of the line (QProcess::StandardOutput or QProcess::StandardError).
    //! @param [in,out] buffer The buffer to process. Complete lines are removed from the buffer.
    //!
    void processOutputBuffer(QProcess::ProcessChannel channel, QString& buffer);

    // Unaccessible operations.
    QtlMovieProcess() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieProcess)
};

#endif // QTLMOVIEPROCESS_H
