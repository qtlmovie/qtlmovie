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
//! @file QtlProcess.h
//!
//! Declare the class QtlProcess, an enhanced version of QProcess.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLPROCESS_H
#define QTLPROCESS_H

#include "QtlCore.h"

//!
//! Qtl namespace.
//!
namespace Qtl {
    //!
    //! Priority of a process, as used by QtlProcess.
    //! Exact mapping depends on the operating system.
    //!
    enum ProcessPriority {
        VeryLowPriority,  //!< Unix: nice +20, Windows: IDLE_PRIORITY_CLASS
        LowPriority,      //!< Unix: nice +10, Windows: BELOW_NORMAL_PRIORITY_CLASS
        NormalPriority,   //!< Unix: nice 0,   Windows: NORMAL_PRIORITY_CLASS
        HighPriority,     //!< Unix: nice -10, Windows: ABOVE_NORMAL_PRIORITY_CLASS
        VeryHighPriority  //!< Unix: nice -20, Windows: HIGH_PRIORITY_CLASS
    };
}

//!
//! A subclass of QProcess with additional features.
//!
class QtlProcess : public QProcess
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent object.
    //!
    QtlProcess(QObject* parent = 0);

    //!
    //! Get the process priority.
    //! @return The process priority.
    //!
    Qtl::ProcessPriority priority() const
    {
        return _priority;
    }

    //!
    //! Set the process priority.
    //! Must be called before start(), ignored otherwise.
    //! Special privileges may be required to set Qtl::HighPriority and Qtl::VeryHighPriority.
    //! @param [in] priority Requested priority.
    //!
    void setPriority(Qtl::ProcessPriority priority);

protected:
    //!
    //! Reimplemented from QProcess.
    //!
    //! This function is called in the child process context just before the
    //! program is executed on Unix or OS X (i.e., after fork(), but before
    //! execve()). Reimplement this function to do last minute initialization
    //! of the child process.
    //!
    virtual void setupChildProcess();

private:
    Qtl::ProcessPriority _priority;  //!< Priority of the created process.

#if defined(Q_OS_WIN)
    //
    // Functions to set the priority of the Windows process.
    // Since QProcess::setCreateProcessArgumentsModifier() only accepts
    // static functions or lambda functions, we must have one function
    // per priority.
    //
    static void clearPriority(CreateProcessArguments* args);
    static void setVeryLowPriority(CreateProcessArguments* args);
    static void setLowPriority(CreateProcessArguments* args);
    static void setNormalPriority(CreateProcessArguments* args);
    static void setHighPriority(CreateProcessArguments* args);
    static void setVeryHighPriority(CreateProcessArguments* args);
#endif
};

#endif // QTLPROCESS_H
