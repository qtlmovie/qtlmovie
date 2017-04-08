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
// Test tool main program.
//
//----------------------------------------------------------------------------

#include "TestToolCommand.h"

int main(int argc, char* argv[])
{
    // Do not remove the QApplication instance. On Windows, it ensures that
    // CoInitialize is invoked and this is required for some classes in Qtl.
    QApplication app(argc, argv);

    QStringList args(app.arguments());
    QTextStream err(stderr, QIODevice::WriteOnly);
    int status = EXIT_SUCCESS;

    if (args.size() < 2) {
        // We need at least one subcommand.
        err << "Usage: " << app.applicationName() << " command [options]" << endl
            << "Try \"" << app.applicationName() << " help\"" << endl;
        status = EXIT_FAILURE;
    }
    else {
        // Execute one command.
        TestToolCommand* command = TestToolCommand::instance(args.at(1));
        if (command == 0) {
            err << app.applicationName() << ": command \"" << args.at(1) << "\" is unknown, try \"help\"" << endl;
            status = EXIT_FAILURE;
        }
        else {
            // Run the command.
            args.removeFirst();
            args.removeFirst();
            status = command->run(args);
        }
    }

    return status;
}
