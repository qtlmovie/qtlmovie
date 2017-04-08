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
// Template methods for class QtlSmartPointer.
//
//----------------------------------------------------------------------------

#ifndef QTLSMARTPOINTERTEMPLATE_H
#define QTLSMARTPOINTERTEMPLATE_H


//----------------------------------------------------------------------------
// Destructor.
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
QtlSmartPointer<T,MUTEXLOCKER>::~QtlSmartPointer()
{
    if (_shared != 0) {
        _shared->detach();
        _shared = 0;
    }
}


//----------------------------------------------------------------------------
// Assignment between smart pointers.
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
const QtlSmartPointer<T,MUTEXLOCKER>& QtlSmartPointer<T,MUTEXLOCKER>::operator=(const QtlSmartPointer<T,MUTEXLOCKER>& sp)
{
    if (_shared != sp._shared) {
        _shared->detach();
        _shared = sp._shared->attach();
    }
    return *this;
}


//----------------------------------------------------------------------------
// Assignment from a standard pointer T*.
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
const QtlSmartPointer<T,MUTEXLOCKER>& QtlSmartPointer<T,MUTEXLOCKER>::operator=(T* p)
{
    _shared->detach();
    _shared = new SmartShared(p);
    return *this;
}


//----------------------------------------------------------------------------
// Destructor. Deallocate actual object (if any).
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
QtlSmartPointer<T,MUTEXLOCKER>::SmartShared::~SmartShared()
{
    if (_ptr != 0) {
        delete _ptr;
        _ptr = 0;
    }
}


//----------------------------------------------------------------------------
// Sets the pointer value to 0 and returns its old value.
// Do not deallocate the object.
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
T* QtlSmartPointer<T,MUTEXLOCKER>::SmartShared::release()
{
    MUTEXLOCKER lock(&_mutex);
    T* previous = _ptr;
    _ptr = 0;
    return previous;
}


//----------------------------------------------------------------------------
// Deallocate previous pointer and sets the pointer to specified value.
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
void QtlSmartPointer<T,MUTEXLOCKER>::SmartShared::reset(T* p)
{
    MUTEXLOCKER lock(&_mutex);
    if (_ptr != 0) {
        delete _ptr;
    }
    _ptr = p;
}


//----------------------------------------------------------------------------
// Get the pointer value.
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
T* QtlSmartPointer<T,MUTEXLOCKER>::SmartShared::pointer()
{
    MUTEXLOCKER lock(&_mutex);
    return _ptr;
}


//----------------------------------------------------------------------------
// Get the reference count value.
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
int QtlSmartPointer<T,MUTEXLOCKER>::SmartShared::count()
{
    MUTEXLOCKER lock(&_mutex);
    return _refCount;
}


//----------------------------------------------------------------------------
// Check for NULL on QtlSmartPointer object
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
bool QtlSmartPointer<T,MUTEXLOCKER>::SmartShared::isNull()
{
    MUTEXLOCKER lock(&_mutex);
    return _ptr == 0;
}


//----------------------------------------------------------------------------
// Perform a class upcast
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
template <typename ST>
QtlSmartPointer<ST,MUTEXLOCKER> QtlSmartPointer<T,MUTEXLOCKER>::SmartShared::upcast()
{
    MUTEXLOCKER lock(&_mutex);
    ST* sp = _ptr;
    _ptr = 0;
    return QtlSmartPointer<ST,MUTEXLOCKER>(sp);
}


//----------------------------------------------------------------------------
// Perform a class downcast
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
template <typename ST>
QtlSmartPointer<ST,MUTEXLOCKER> QtlSmartPointer<T,MUTEXLOCKER>::SmartShared::downcast()
{
    MUTEXLOCKER lock(&_mutex);
#if defined(QTL_NO_RTTI)
    ST* sp = static_cast<ST*>(_ptr);
#else
    ST* sp = dynamic_cast<ST*>(_ptr);
#endif
    if (sp != 0) {
        // Successful downcast, the original smart pointer is released.
        _ptr = 0;
    }
    return QtlSmartPointer<ST,MUTEXLOCKER>(sp);
}


//----------------------------------------------------------------------------
// Change mutex type.
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
template <typename NEWMUTEXLOCKER>
QtlSmartPointer<T,NEWMUTEXLOCKER> QtlSmartPointer<T,MUTEXLOCKER>::SmartShared::changeMutex()
{
    MUTEXLOCKER lock(&_mutex);
    T* sp = _ptr;
    _ptr = 0;
    return QtlSmartPointer<T,NEWMUTEXLOCKER>(sp);
}


//----------------------------------------------------------------------------
// Increment reference count and return this.
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
typename QtlSmartPointer<T,MUTEXLOCKER>::SmartShared* QtlSmartPointer<T,MUTEXLOCKER>::SmartShared::attach()
{
    MUTEXLOCKER lock(&_mutex);
    _refCount++;
    return this;
}


//----------------------------------------------------------------------------
// Decrement reference count and deallocate this if needed.
// Return true is deleted, false otherwise.
//----------------------------------------------------------------------------

template <typename T, class MUTEXLOCKER>
void QtlSmartPointer<T,MUTEXLOCKER>::SmartShared::detach()
{
    int refCount;
    {
        MUTEXLOCKER lock(&_mutex);
        refCount = --_refCount;
    }
    if (refCount == 0) {
        delete this;
    }
}

#endif // QTLSMARTPOINTERTEMPLATE_H
