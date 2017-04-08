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
//  Define the QtlSynchronousProcess class.
//
//----------------------------------------------------------------------------

#include "QtlSynchronousProcess.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------


QtlSynchronousProcess::QtlSynchronousProcess(const QString &program,
                                             const QStringList &arguments,
                                             int msRunTestTimeout,
                                             int maxProcessOutputSize,
                                             QObject *parent,
                                             const QProcessEnvironment &env) :
    QObject(parent),
    _eventLoop(),
    _result(),
    _terminated(false)
{
    // Create the process. The object will delete itself upon termination.
    QtlBoundProcess* process = QtlBoundProcess::newInstance(program, arguments, msRunTestTimeout, maxProcessOutputSize, this, env);
    connect(process, &QtlBoundProcess::terminated, this, &QtlSynchronousProcess::processTerminated);
    process->start();

    // Run a private event loop while the process executes.
    while (!_terminated) {
        _eventLoop.exec();
    }
}


//----------------------------------------------------------------------------
// Invoked when the process is terminated or failed to start.
//----------------------------------------------------------------------------

void QtlSynchronousProcess::processTerminated(const QtlBoundProcessResult &result)
{
    _terminated = true;
    _result = result;
    _eventLoop.exit();
}
