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
// Qtl, Qt utility library.
// Define the class QtlBoundProcessResult.
//
//----------------------------------------------------------------------------

#include "QtlBoundProcessResult.h"


//----------------------------------------------------------------------------
// Constructors and assignment.
//----------------------------------------------------------------------------

QtlBoundProcessResult::QtlBoundProcessResult(const QString& program, const QStringList& arguments) :
    _program(program),
    _arguments(arguments),
    _stdOutput(),
    _stdError(),
    _errorMessage(),
    _exitCode(-1)
{
}

QtlBoundProcessResult::QtlBoundProcessResult(const QtlBoundProcessResult &other) :
    _program(other._program),
    _arguments(other._arguments),
    _stdOutput(other._stdOutput),
    _stdError(other._stdError),
    _errorMessage(other._errorMessage),
    _exitCode(other._exitCode)
{

}

const QtlBoundProcessResult& QtlBoundProcessResult::operator=(const QtlBoundProcessResult &other)
{
    if (&other != this) {
        _program = other._program;
        _arguments = other._arguments;
        _stdOutput = other._stdOutput;
        _stdError = other._stdError;
        _errorMessage = other._errorMessage;
        _exitCode = other._exitCode;
    }
    return *this;
}
