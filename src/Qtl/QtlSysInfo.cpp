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
//
// Qtl, Qt utility library.
// Define the class QtlSysInfo, an extension of QSysInfo.
//
//----------------------------------------------------------------------------

#include "QtlSysInfo.h"

#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX)
#include <unistd.h>
#endif


//-----------------------------------------------------------------------------
// Check if the application is running on a 64-bit version of Windows.
//-----------------------------------------------------------------------------

bool QtlSysInfo::runningOnWin64()
{
#if defined(_WIN64) || defined(Q_OS_WIN64)
    // Compiled on Win64, can run only on Win64.
    return true;
#elif defined(_WIN32) || defined(Q_OS_WIN)
    // Running on another flavor of Windows, we must check.
    BOOL win64 = FALSE;
    return ::IsWow64Process(::GetCurrentProcess(), &win64) && win64;
#else
    // Not running on Windows at all.
    return false;
#endif
}


//-----------------------------------------------------------------------------
// Get the number of processors in the system.
//-----------------------------------------------------------------------------

int QtlSysInfo::numberOfProcessors(int defaultValue)
{
#if defined(Q_OS_WIN)
    ::SYSTEM_INFO info;
    GetSystemInfo(&info);
    return int(info.dwNumberOfProcessors);
#elif defined(_SC_NPROCESSORS_ONLN)
    const long count = ::sysconf(_SC_NPROCESSORS_ONLN);
    return count <= 0 ? defaultValue : int(count);
#else
    // Don't know how to get the number of CPU on this beast.
    return defaultValue;
#endif
}
