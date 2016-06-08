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
//! @file QtlProcessResult.h
//!
//! Declare the class QtlProcessResult.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLPROCESSRESULT_H
#define QTLPROCESSRESULT_H

#include "QtlCore.h"

//!
//! Result of a process execution.
//!
class QtlProcessResult
{
public:
    //!
    //! Constructor.
    //! @param [in] program Executable file path.
    //! @param [in] arguments Command line arguments.
    //!
    QtlProcessResult(const QString& program = QString(), const QStringList& arguments = QStringList());

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy.
    //!
    QtlProcessResult(const QtlProcessResult& other);

    //!
    //! Assignment operator.
    //! @param [in] other Other instance to copy.
    //!
    const QtlProcessResult& operator=(const QtlProcessResult& other);

    //!
    //! Get the executable file path.
    //! @return Executable file path.
    //!
    QString program() const
    {
        return _program;
    }

    //!
    //! Get the command line arguments.
    //! @return Command line arguments.
    //!
    QStringList arguments() const
    {
        return _arguments;
    }

    //!
    //! Get the content of the standard output after process termination.
    //! @return Content of standard output.
    //!
    QString standardOutput() const
    {
        return _stdOutput;
    }

    //!
    //! Get the size of the standard output after process termination.
    //! @return Content of standard output.
    //!
    int standardOutputSize() const
    {
        return _stdOutput.size();
    }

    //!
    //! Get the content of the standard error after process termination.
    //! @return Content of standard error.
    //!
    QString standardError() const
    {
        return _stdError;
    }

    //!
    //! Get the size of the standard error after process termination.
    //! @return Content of standard error.
    //!
    int standardErrorSize() const
    {
        return _stdError.size();
    }

    //!
    //! Check if the process terminated with an error.
    //! @return True on error, false otherwise.
    //!
    bool hasError() const
    {
        return !_errorMessage.isEmpty();
    }

    //!
    //! Get an error message after process termination if the process did not execute properly.
    //! @return Error message. If empty, the process executed correctly.
    //!
    QString errorMessage() const
    {
        return _errorMessage;
    }

    //!
    //! Get the process exit code after process termination.
    //! @return Process exit code.
    //!
    int exitCode() const
    {
        return _exitCode;
    }

    //!
    //! Append more data to standard output.
    //! @param [in] data Data to append.
    //!
    void appendStandardOutput(const QByteArray& data)
    {
        _stdOutput.append(data);
    }

    //!
    //! Append more data to standard error.
    //! @param [in] data Data to append.
    //!
    void appendStandardError(const QByteArray& data)
    {
        _stdError.append(data);
    }

    //!
    //! Set the error message.
    //! @param [in] errorMessage Error message.
    //!
    void setErrorMessage(const QString& errorMessage)
    {
        _errorMessage = errorMessage;
    }

    //!
    //! Set the exit code.
    //! @param exitCode Exit code.
    //!
    void setExitCode(int exitCode)
    {
        _exitCode = exitCode;
    }

private:
    QString     _program;       //!< Executable file path.
    QStringList _arguments;     //!< Command line arguments.
    QString     _stdOutput;     //!< Content of standard output.
    QString     _stdError;      //!< Content of standard output.
    QString     _errorMessage;  //!< Error message.
    int         _exitCode;      //!< Process exit code.
};

#endif // QTLPROCESSRESULT_H
