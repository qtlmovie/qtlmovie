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
//!
//! @file TestToolCommand.h
//!
//! Declare the class TestToolCommand.
//!
//----------------------------------------------------------------------------

#ifndef TESTTOOLCOMMAND_H
#define TESTTOOLCOMMAND_H

#include "QtlStdoutLogger.h"
#include "QtlStringList.h"

//!
//! Base class for all subcommands in TestTool.
//!
class TestToolCommand : public QObject
{
    Q_OBJECT

public:
    //!
    //! Get the list of available commands.
    //! @return The list of available commands.
    //!
    static QtlStringList allCommands();

    //!
    //! Get the instance for one specific command.
    //! @param [in] command Name of the TestTool subcommand.
    //! @return The instance for @a command or zero if not found.
    //!
    static TestToolCommand* instance(const QString& command);

    //!
    //! Get the command name.
    //! @return The command name.
    //!
    QString command() const
    {
        return _command;
    }

    //!
    //! Get the brief parameter syntax overview.
    //! @return The brief parameter syntax overview.
    //!
    QString syntax() const
    {
        return _syntax;
    }

    //!
    //! Get the multi-line command description.
    //! @return The multi-line command description.
    //!
    QString description() const
    {
        return _description;
    }

    //!
    //! Run the test which is implemented by this class.
    //! @param [in] args Command line arguments (after subcommand name).
    //! @return Either EXIT_SUCCESS or EXIT_FAILURE.
    //!
    virtual int run(const QStringList& args) = 0;

protected:
    //!
    //! Protected constructor.
    //! @param [in] command Name of the TestTool subcommand.
    //! @param [in] syntax Brief parameter syntax overview.
    //! @param [in] description Multi-line command description.
    //!
    TestToolCommand(const QString& command, const QString& syntax, const QString& description);

    //!
    //! Display an syntax error message and return EXIT_FAILURE.
    //! @return EXIT_FAILURE.
    //!
    int syntaxError();

    QTextStream     out;  //!< QTextStream on stdout.
    QTextStream     err;  //!< QTextStream on stderr.
    QtlStdoutLogger log;  //!< QtlLogger on stdout.

private:
    QString _command;
    QString _syntax;
    QString _description;

    typedef QMap<QString,TestToolCommand*> CommandMap;
    static CommandMap* _commandMap;

    // Unaccessible operations.
    TestToolCommand() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(TestToolCommand)
};

#endif // TESTTOOLCOMMAND_H
