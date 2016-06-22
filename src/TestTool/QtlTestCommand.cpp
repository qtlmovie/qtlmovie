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
// Define the class QtlTestCommand.
//
//----------------------------------------------------------------------------

#include "QtlTestCommand.h"

QtlTestCommand::CommandMap* QtlTestCommand::_commandMap = 0;


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlTestCommand::QtlTestCommand(const QString& command, const QString& syntax, QObject* parent) :
    QObject(parent),
    out(stdout, QIODevice::WriteOnly),
    err(stderr, QIODevice::WriteOnly),
    log(0, true),
    _command(command),
    _syntax(syntax)
{
    if (_commandMap == 0) {
        _commandMap = new CommandMap;
    }
    _commandMap->insert(command.toLower(), this);
}

int QtlTestCommand::syntaxError()
{
    err << "syntax: " << QCoreApplication::applicationName() << " " << _command << " " << _syntax << endl;
    return EXIT_FAILURE;
}


//----------------------------------------------------------------------------
// Command repository.
//----------------------------------------------------------------------------

QtlStringList QtlTestCommand::allCommands()
{
    if (_commandMap == 0) {
        return QtlStringList();
    }
    else {
        QtlStringList names(_commandMap->keys());
        names.sort(Qt::CaseInsensitive);
        return names;
    }
}

QtlTestCommand* QtlTestCommand::instance(const QString& command)
{
    CommandMap::ConstIterator it;
    return _commandMap != 0 && (it = _commandMap->find(command.toLower())) != _commandMap->end() ? it.value() : 0;
}
