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
// Qtl, Qt utility library.
// Define the class QtlProcess, an enhanced version of QProcess.
//
//----------------------------------------------------------------------------

#include "QtlProcess.h"

#if defined(Q_OS_UNIX)
    #include <unistd.h>
#endif


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtlProcess::QtlProcess(QObject* parent) :
    QProcess(parent),
    _priority(Qtl::NormalPriority)
{
}


//----------------------------------------------------------------------------
// Set the process priority.
//----------------------------------------------------------------------------

void QtlProcess::setPriority(Qtl::ProcessPriority priority)
{
    // Must be called before start(), ignored otherwise.
    if (state() == NotRunning) {
        _priority = priority;

#if defined(Q_OS_WIN)
        // On Windows, set a function that will set the priority of the process.
        switch (_priority) {
            case Qtl::VeryLowPriority:
                setCreateProcessArgumentsModifier(setVeryLowPriority);
                break;
            case Qtl::LowPriority:
                setCreateProcessArgumentsModifier(setLowPriority);
                break;
            case Qtl::NormalPriority:
                setCreateProcessArgumentsModifier(setNormalPriority);
                break;
            case Qtl::HighPriority:
                setCreateProcessArgumentsModifier(setHighPriority);
                break;
            case Qtl::VeryHighPriority:
                setCreateProcessArgumentsModifier(setVeryHighPriority);
                break;
        }
#endif
    }
}


//----------------------------------------------------------------------------
// This function is called in the child process context just before the
// program is executed on Unix or OS X (i.e., after fork(), but before
// execve()). Reimplement this function to do last minute initialization
// of the child process.
//----------------------------------------------------------------------------

void QtlProcess::setupChildProcess()
{
#if defined(Q_OS_UNIX)
    // Warning: This code is written in a way to avoid warnings on recent
    // versions of Linux. Don't modify it without thinking twice.
    int niceValue = 0;
    switch (_priority) {
        case Qtl::VeryLowPriority:
            niceValue = 20;
            break;
        case Qtl::LowPriority:
            niceValue = 10;
            break;
        case Qtl::NormalPriority:
            niceValue = 0;
            break;
        case Qtl::HighPriority:
            niceValue = -10;
            break;
        case Qtl::VeryHighPriority:
            niceValue = -20;
            break;
    }
    niceValue = ::nice(niceValue);
#endif
}


//----------------------------------------------------------------------------
// Functions to set the priority of the Windows process.
//----------------------------------------------------------------------------

#if defined(Q_OS_WIN)

void QtlProcess::clearPriority(CreateProcessArguments* args)
{
    // The priority is embedded in the flags. Clear all priority flags.
    args->flags &= ~(IDLE_PRIORITY_CLASS |
                     BELOW_NORMAL_PRIORITY_CLASS |
                     NORMAL_PRIORITY_CLASS |
                     ABOVE_NORMAL_PRIORITY_CLASS |
                     HIGH_PRIORITY_CLASS |
                     REALTIME_PRIORITY_CLASS);
}

void QtlProcess::setVeryLowPriority(CreateProcessArguments* args)
{
    clearPriority(args);
    args->flags |= IDLE_PRIORITY_CLASS;
}

void QtlProcess::setLowPriority(CreateProcessArguments* args)
{
    clearPriority(args);
    args->flags |= BELOW_NORMAL_PRIORITY_CLASS;
}

void QtlProcess::setNormalPriority(CreateProcessArguments* args)
{
    clearPriority(args);
    args->flags |= NORMAL_PRIORITY_CLASS;
}

void QtlProcess::setHighPriority(CreateProcessArguments* args)
{
    clearPriority(args);
    args->flags |= ABOVE_NORMAL_PRIORITY_CLASS;
}

void QtlProcess::setVeryHighPriority(CreateProcessArguments* args)
{
    clearPriority(args);
    args->flags |= HIGH_PRIORITY_CLASS;
}

#endif
