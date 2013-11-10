//----------------------------------------------------------------------------
//
// Copyright (c) 2013, Thierry Lelegard
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
//! @file QtlSysInfo.h
//!
//! Declare the class QtlSysInfo, an extension of QSysInfo.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLSYSINFO_H
#define QTLSYSINFO_H

#include <QtCore>

//!
//! An extension of QSysInfo.
//!
class QtlSysInfo : public QSysInfo
{
public:
    //!
    //! Check if the application is running on a 64-bit version of Windows.
    //! This cannot be detected by conditional compilation since a 32-bit
    //! Windows application may run on a 64-bit system.
    //! @return True if the application is running on a 64-bit version of Windows.
    //! Always return false on non-Windows systems.
    //!
    static bool runningOnWin64();

    //!
    //! Get the number of processors in the system.
    //! @param [in] defaultValue The value to return on error.
    //! Sometimes it can be useful to use 1 as default value.
    //! @return The number of processors in the system.
    //!
    static int numberOfProcessors(int defaultValue = -1);

private:
    // Unaccessible operation.
    QtlSysInfo() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlSysInfo)
};

#endif // QTLSYSINFO_H
