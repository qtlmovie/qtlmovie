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
//! @file QtlIncrement.h
//!
//! Declare the template class QtlIncrement.
//!
//----------------------------------------------------------------------------

#ifndef QTLINCREMENT_H
#define QTLINCREMENT_H

#include "QtlCore.h"

//!
//! A class which safely increment and equivalently decrement a counter.
//! @tparam INT The integer type of the counter to increment.
//!
template<typename INT>
class QtlIncrement
{
public:
    //!
    //! Constructor.
    //! @param [in,out] counter The address of the counter to increment. Can be null.
    //! @param [in] increment The amount of unit to increment. Default to 1.
    //!
    explicit QtlIncrement(INT* counter = 0, INT increment = 1) :
        _counter(counter),
        _increment(increment)
    {
        if (_counter != 0) {
            *_counter += _increment;
        }
    }
    //!
    //! Destructor, decrement the counter.
    //! The counter is decremented by the same amount as it was incremented in the constructor.
    //!
    ~QtlIncrement()
    {
        if (_counter != 0) {
            *_counter -= _increment;
        }
    }

private:
    INT* _counter;   //!< The address of the counter to increment.
    INT  _increment; //!< The amount of unit to decrement in destructor.

    // Prevent object copy.
    Q_DISABLE_COPY(QtlIncrement)
};

#endif // QTLINCREMENT_H
