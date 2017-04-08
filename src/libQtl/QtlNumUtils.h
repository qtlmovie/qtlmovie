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
//! @file QtlNumUtils.h
//!
//! Declare some utilities functions for numerics (integers and floats).
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLNUMUTILS_H
#define QTLNUMUTILS_H

#include "QtlCore.h"

//!
//! Compare two floating point values for equality.
//!
//! After several rounds of computations, floating point values lose their
//! precision. It is silly to use the strict comparison operator == on
//! floating point values. If the two values differ, for instance, only
//! by the least significant bit of the mantissa, the operator == return
//! false because the two values are more strictly identical. However,
//! it is likely that they are semantically identical and the differing
//! bit is only the result of a loss of precision in the previous computations.
//! In practice, semantically identical values may even differ by more than
//! one bit.
//!
//! To solve this issue, we consider floating point values as identical when
//! they differ only within a given precision.
//!
//! @tparam T A floating point type (eg. float or double).
//! @param [in] t1 First value to compare.
//! @param [in] t2 Second value to compare.
//! @param [in] precision Allowed precision in the comparison (defaults to 0.001).
//! @return True if @a t1 and @a t2 are identical regarding the given @a precision
//! and false otherwise.
//!
template <typename T>
inline bool qtlFloatEqual(const T& t1, const T& t2, const T& precision = 0.001)
{
    return qAbs(t2 - t1) < precision;
}

//!
//! Check if a floating point value is zero.
//!
//! @tparam T A floating point type (eg. float or double).
//! @param [in] t Value to check.
//! @param [in] precision Allowed precision in the comparison (defaults to 0.001).
//! @return True if @a t is zero regarding the given @a precision false otherwise.
//! @see qtlFloatEqual()
//!
template <typename T>
inline bool qtlFloatZero(const T& t, const T& precision = 0.001)
{
    return qAbs(t) < precision;
}

#endif // QTLNUMUTILS_H
