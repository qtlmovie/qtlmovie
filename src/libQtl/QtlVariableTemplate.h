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
// Template methods for the QtlVariable class.
//
//----------------------------------------------------------------------------

#ifndef QTLVARIABLETEMPLATE_H
#define QTLVARIABLETEMPLATE_H


//-----------------------------------------------------------------------------
// Copy constructor.
//-----------------------------------------------------------------------------

template <typename T>
QtlVariable<T>::QtlVariable(const QtlVariable<T>& other) :
    _access(0)
{
    if (other._access != 0) {
        _access = new (_data) T (*other._access);
    }
}


//-----------------------------------------------------------------------------
// Assignment operator.
//-----------------------------------------------------------------------------

template <typename T>
QtlVariable<T>& QtlVariable<T>::operator=(const QtlVariable<T>& other)
{
    if (&other != this) {
        reset();
        if (other._access != 0) {
            _access = new (_data) T (*other._access);
        }
    }
    return *this;
}


//-----------------------------------------------------------------------------
// Assignment operator from a @a T object.
//-----------------------------------------------------------------------------

template <typename T>
QtlVariable<T>& QtlVariable<T>::operator=(const T& obj)
{
    reset();
    _access = new (_data) T (obj);
    return *this;
}


//-----------------------------------------------------------------------------
// Reset the value.
//-----------------------------------------------------------------------------

template <typename T>
void QtlVariable<T>::reset()
{
    if (_access != 0) {
        // Safe when the destructor throws an exception
        T* tmp = _access;
        _access = 0;
        tmp->~T();
    }
}


//-----------------------------------------------------------------------------
// Access the constant @a T value inside the variable.
//-----------------------------------------------------------------------------

template <typename T>
const T& QtlVariable<T>::value() const throw (QtlUninitializedException)
{
    if (_access != 0) {
        return *_access;
    }
    else {
        throw QtlUninitializedException();
    }
}


//-----------------------------------------------------------------------------
// Access the @a T value inside the variable.
//-----------------------------------------------------------------------------

template <typename T>
T& QtlVariable<T>::value() throw (QtlUninitializedException)
{
    if (_access != 0) {
        return *_access;
    }
    else {
        throw QtlUninitializedException();
    }
}

#endif // QTLVARIABLETEMPLATE_H
