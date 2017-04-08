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
//! @file QtlUninitializedException.h
//!
//! Declare the QtlUnitializedException class.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLUNINITIALIZEDEXCEPTION_H
#define QTLUNINITIALIZEDEXCEPTION_H

#include <QException>

//!
//! Define the exceptions which are thrown when a QtlVariable instance is used without being initialized.
//!
class QtlUninitializedException: public QException
{
public:
    //!
    //! Virtual destructor.
    //!
    virtual ~QtlUninitializedException() throw()
    {
    }

    //!
    //! Raise this exception (required by QException).
    //!
    void raise() const
    {
        throw *this;
    }

    //!
    //! Clone this exception (required by QException).
    //! @return A cloned instance of this object.
    //!
    QtlUninitializedException* clone() const
    {
        return new QtlUninitializedException(*this);
    }
};

#endif // QTLUNINITIALIZEDEXCEPTION_H
