//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
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
// Test tool main program.
//
//----------------------------------------------------------------------------

#include "QtlTestCommand.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args(app.arguments());
    QTextStream err(stderr, QIODevice::WriteOnly);

    // We need at least one subcommand.
    if (args.size() < 2) {
        err << "usage: " << app.applicationName() << " command [options]" << endl;
        return EXIT_FAILURE;
    }

    // Command help is hard-coded.
    if (args.at(1) == "help") {
        err << "Available commands:" << endl;
        const QtlStringList names(QtlTestCommand::allCommands());
        const int length = names.maxLength();
        foreach (const QString& name, names) {
            QtlTestCommand* command = QtlTestCommand::instance(name);
            if (command != 0) {
                err << "  " << command->command() << QString(length - command->command().length() + 1, QChar(' ')) << command->syntax() << endl;
            }
        }
        return EXIT_SUCCESS;
    }

    // Fetch the command description.
    QtlTestCommand* command = QtlTestCommand::instance(args.at(1));
    if (command == 0) {
        err << app.applicationName() << ": command \"" << args.at(1) << "\" is unknown, try \"help\"" << endl;
        return EXIT_FAILURE;
    }

    // Run the command.
    args.removeFirst();
    args.removeFirst();
    return command->run(args);
}
