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
//! @file QtlVariable.h
//!
//! Declare the QtlVariable template class.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLVARIABLE_H
#define QTLVARIABLE_H

#include <QtGlobal>
#include "QtlUninitializedException.h"

//!
//! A template class which defines a @e variable which can be either initialized or uninitialized.
//!
//! @tparam T A type or class which describes the content of the variable.
//! The basic requirement on @a T is the availability of a copy constructor and
//! operators for assignment and equality.
//!
template <typename T>
class QtlVariable
{
private:
    T* _access;
    quint8 _data[sizeof(T)];

public:
    //!
    //! Default constructor, the variable is uninitialized.
    //!
    QtlVariable() throw() :
        _access(0)
    {
    }

    //!
    //! Copy constructor.
    //!
    //! This object is in the same state as @a other. If @a other is
    //! initialized, this object becomes initialized with the same @a T value.
    //!
    //! @param [in] other Another instance from which to build this object.
    //!
    QtlVariable(const QtlVariable<T>& other);

    //!
    //! Constructor from a @a T instance, the variable is initialized.
    //!
    //! @param [in] obj The initial value for the variable.
    //!
    QtlVariable(const T& obj) :
        _access(new (_data) T (obj))
    {
    }

    //!
    //! Virtual destructor
    //!
    virtual ~QtlVariable()
    {
        reset();
    }

    //!
    //! Assignment operator.
    //!
    //! This object is in the same state as @a other. If @a other is
    //! initialized, this object becomes initialized with the same @a T value.
    //!
    //! @param [in] other Another instance from which to assign this object.
    //! @return A reference to this object.
    //!
    QtlVariable<T>& operator=(const QtlVariable<T>& other);

    //!
    //! Assignment operator from a @a T object.
    //!
    //! This object becomes initialized if it was not already.
    //!
    //! @param [in] obj Value from which to assign this object.
    //! @return A reference to this object.
    //!
    QtlVariable<T>& operator=(const T& obj);

    //!
    //! Check the presence of a value.
    //!
    //! @return True if the variable is initialized, false otherwise.
    //!
    bool set() const
    {
        return _access != 0;
    }

    //!
    //! Reset the value.
    //!
    //! This object becomes uninitialized if it was not already.
    //!
    void reset();

    //!
    //! Access the constant @a T value inside the variable.
    //!
    //! @return A constant reference to the @a T value inside the variable.
    //! @throw QtlUnitializedException If the variable is uninitialized.
    //!
    const T& value() const throw(QtlUninitializedException);

    //!
    //! Access the @a T value inside the variable.
    //!
    //! @return A reference to the @a T value inside the variable.
    //! @throw QtlUnitializedException If the variable is uninitialized.
    //!
    T& value() throw(QtlUninitializedException);

    //!
    //! Get a copy of the @a T value inside the variable or a default value.
    //!
    //! @param [in] def A default @a T value if the variable is uninitialized.
    //! @return A copy the @a T value inside the variable if the variable is
    //! initialized, @a def otherwise.
    //!
    T value(const T& def) const
    {
        return _access != 0 ? *_access : def;
    }

    //!
    //! Equality operator.
    //!
    //! @param [in] other An other instance to compare with.
    //! @return True if both instances are initialized and contain equal values.
    //!
    bool operator==(const QtlVariable<T>& other) const
    {
        return _access != 0 && other._access != 0 && *_access == *other._access;
    }

    //!
    //! Unequality operator.
    //!
    //! @param [in] other An other instance to compare with.
    //! @return True if any instance is uninitialized or both
    //! are initialized with unequal values.
    //!
    bool operator!=(const QtlVariable<T>& other) const
    {
        // Note than we do not require that T provides operator!=.
        // We just require operator==. So we use !(.. == ..).
        return _access == 0 || other._access == 0 || !(*_access == *other._access);
    }

    //!
    //! Equality operator with a @a T instance.
    //!
    //! @param [in] obj An object to compare with.
    //! @return True if this object is initialized and its value
    //! is equal to @a obj.
    //!
    bool operator==(const T& obj) const
    {
        return _access != 0 && *_access == obj;
    }

    //!
    //! Unequality operator with a @a T instance.
    //!
    //! @param [in] obj An object to compare with.
    //! @return True if this object is uninitialized or its value
    //! is not equal to @a obj.
    //!
    bool operator!=(const T& obj) const
    {
        // Note than we do not require that T provides operator!=.
        // We just require operator==. So we use !(.. == ..).
        return _access == 0 || !(*_access == obj);
    }
};

#include "QtlVariableTemplate.h"

#endif // QTLVARIABLE_H
