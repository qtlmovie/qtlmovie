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
// Command line tool to test the creation of directories.
//
//----------------------------------------------------------------------------

#include "QtlTestCommand.h"
#include "QtlFile.h"

class QtlTestMkdir : public QtlTestCommand
{
    Q_OBJECT
public:
    QtlTestMkdir() : QtlTestCommand("mkdir", "path-name [-createOnly]") {}
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;
};

//----------------------------------------------------------------------------

int QtlTestMkdir::run(const QStringList& args)
{
    if (args.size() != 1 && (args.size() != 2 || args[1] != "-createOnly")) {
        return syntaxError();
    }

    if (!QtlFile::createDirectory(args[0], args.size() >= 2 && args[1] == "-createOnly")) {
        err << "Error creating \"" << args[0] << "\"" << endl;
    }

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

#include "QtlTestMkdir.moc"
namespace {QtlTestMkdir thisTest;}
