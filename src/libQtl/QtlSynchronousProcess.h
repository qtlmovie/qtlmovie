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
//!
//! @file QtlSynchronousProcess.h
//!
//! Declare the class QtlSynchronousProcess, a synchronous encapsulation of QProcess.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLSYNCHRONOUSPROCESS_H
#define QTLSYNCHRONOUSPROCESS_H

#include <QObject>
#include "QtlBoundProcess.h"

//!
//! The QtlSynchronousProcess class implements the execution of a synchronous process.
//!
//! The process is entirely executed during the constructor. Then, the standard
//! output and error can be read.
//!
class QtlSynchronousProcess : public QObject
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! The process executes synchronously. The constructor returns only upon
    //! process completion.
    //! @param [in] program Executable file path.
    //! @param [in] arguments Command line arguments.
    //! @param [in] msRunTestTimeout Timeout of process execution in milliseconds. Ignored if negative or zero.
    //! @param [in] maxProcessOutputSize Maximum size in bytes of process output. Ignored if negative or zero.
    //! @param [in] parent Optional parent object.
    //! @param [in] env Environment for the process. Current environment by default.
    //!
    QtlSynchronousProcess(const QString& program,
                          const QStringList& arguments = QStringList(),
                          int msRunTestTimeout = 0,
                          int maxProcessOutputSize = 0,
                          QObject* parent = 0,
                          const QProcessEnvironment& env = QProcessEnvironment());

    //!
    //! Get the content of the standard output after process termination.
    //! @return Content of standard output.
    //!
    QString standardOutput() const
    {
        return _result.standardOutput();
    }

    //!
    //! Get the content of the standard output after process termination as list of lines.
    //! @return Text of lines of standard output.
    //!
    QStringList standardOutputLines() const
    {
        return _result.standardOutput().split(QRegExp("\\r*\\n"));
    }

    //!
    //! Get the size of the standard output after process termination.
    //! @return Content of standard output.
    //!
    int standardOutputSize() const
    {
        return _result.standardOutputSize();
    }

    //!
    //! Get the content of the standard error after process termination.
    //! @return Content of standard error.
    //!
    QString standardError() const
    {
        return _result.standardError();
    }

    //!
    //! Get the content of the standard error after process termination as list of lines.
    //! @return Text of lines of standard error.
    //!
    QStringList standardErrorLines() const
    {
        return _result.standardError().split(QRegExp("\\r*\\n"));
    }

    //!
    //! Get the size of the standard error after process termination.
    //! @return Content of standard error.
    //!
    int standardErrorSize() const
    {
        return _result.standardErrorSize();
    }

    //!
    //! Check if the process terminated with an error.
    //! @return True on error, false otherwise.
    //!
    bool hasError() const
    {
        return _result.hasError();
    }

    //!
    //! Get an error message after process termination if the process did not execute properly.
    //! @return Error message. If empty, the process executed correctly.
    //!
    QString errorMessage() const
    {
        return _result.errorMessage();
    }

    //!
    //! Get the process exit code after process termination.
    //! @return Process exit code.
    //!
    int exitCode() const
    {
        return _result.exitCode();
    }

private slots:
    //!
    //! Invoked when the process is terminated or failed to start.
    //! @param [in] result Process execution results.
    //!
    void processTerminated(const QtlBoundProcessResult& result);

private:
    QEventLoop            _eventLoop;  //!< Event loop which executes in the constructor.
    QtlBoundProcessResult _result;     //!< Process execution result.
    bool                  _terminated; //!< Process execution is completed.

    // Unaccessible operations.
    QtlSynchronousProcess() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlSynchronousProcess)
};

#endif // QTLSYNCHRONOUSPROCESS_H
