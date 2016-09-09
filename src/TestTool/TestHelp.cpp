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
// Help command for TestTool.
//
//----------------------------------------------------------------------------

#include "TestToolCommand.h"

class TestHelp : public TestToolCommand
{
    Q_OBJECT
public:
    TestHelp() : TestToolCommand("help", "[-v | command]", "Display help on one or all commands.") {}
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;
private:
    void display(TestToolCommand* cmd)
    {
        if (cmd != 0) {
            err << QCoreApplication::applicationName() << " " << cmd->command() << " " << cmd->syntax() << endl
                << endl
                << cmd->description() << endl;
        }
    }
};

//----------------------------------------------------------------------------

int TestHelp::run(const QStringList& args)
{
    if (args.isEmpty()) {
        // List all available commands with short syntax.
        const QtlStringList names(allCommands());
        const int length = names.maxLength();
        foreach (const QString& name, names) {
            TestToolCommand* cmd = TestToolCommand::instance(name);
            if (cmd != 0) {
                err << "  " << cmd->command() << QString(length - cmd->command().length() + 1, QChar(' ')) << cmd->syntax() << endl;
            }
        }
    }
    else if (args.first() == "-v") {
        // Display all available commands with verbose help.
        err << endl;
        foreach (const QString& name, allCommands()) {
            err << "==== ";
            display(instance(name));
            err << endl;
        }
    }
    else {
        // Display help on a specific command.
        TestToolCommand* cmd = TestToolCommand::instance(args.first());
        if (cmd == 0) {
            err << QCoreApplication::applicationName() << ": command \"" << args.first() << "\" is unknown, try \"help\"" << endl;
            return EXIT_FAILURE;
        }
        err << endl;
        display(cmd);
        err << endl;
    }
    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

#include "TestHelp.moc"
namespace {TestHelp thisTest;}
