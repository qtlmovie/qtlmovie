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
//! @file QtlSmartPointer.h
//!
//! Declare the QtlSmartPointer template class.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLSMARTPOINTER_H
#define QTLSMARTPOINTER_H

#include "QtlCore.h"

//!
//! The QtlMutexLocker class is a direct subclass of QMutexLocker.
//! The only addition is the definition of the Mutex typedef.
//! QtlMutexLocker is designed to be API-compatible with QtlNullMutexLocker
//! so that they can be both used as template parameters to QtlSmartPointer.
//!
class QtlMutexLocker: public QMutexLocker
{
public:
    //!
    //! The Mutex type for this locker class.
    //!
    typedef QMutex Mutex;
    //!
    //! Constructor.
    //! @param [in,out] mutex The associated mutex.
    //!
    QtlMutexLocker(Mutex* mutex) :
        QMutexLocker(mutex)
    {
    }
private:
    // Unaccessible operations.
    QtlMutexLocker() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMutexLocker)
};

//!
//! The QtlNullMutexLocker class is API-compatible with QtlMutexLocker but does nothing.
//! QtlMutexLocker is designed so that it can be used as template parameters to QtlSmartPointer.
//!
class QtlNullMutexLocker
{
public:
    //!
    //! The Mutex type for this locker class.
    //! This is a placeholder which does nothing.
    //!
    class Mutex
    {
    };
    //!
    //! Constructor.
    //! @param [in,out] mutex The associated mutex.
    //!
    QtlNullMutexLocker(Mutex* mutex)
    {
    }
private:
    // Unaccessible operations.
    QtlNullMutexLocker() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlNullMutexLocker)
};

//!
//! Template smart pointer (reference-counted, auto-delete, optionally thread-safe).
//!
//! The QtlSmartPointer template class is an implementation of the
//! @e smart @e pointer design pattern. A smart pointer implements the
//! semantics of a standard pointer with automatic memory management.
//!
//! Smart pointer objects pointing to the same object can be assigned
//! like any standard pointer (elementary type). A reference counter
//! on the pointed object is maintained and the pointed object is
//! automatically deleted when no more smart pointer references the
//! object.
//!
//! @b Limitation: The automatic deletion of the pointed object occurs
//! @em only when the reference counter reaches zero. There are
//! cases where this never happens. Typically, when two objects reference
//! each other but are no longer referenced anywhere else, these two objects
//! are lost forever and will never be deleted. So, beware that smart
//! pointers do not prevent from memory leaks in some pathological cases.
//! As usual, be sure to design safely.
//!
//! @b Limitation: Because the automatic deletion is performed using the
//! operator @c delete, smart pointers cannot point on array types since
//! arrays must be deleted using the operator @c delete[].
//!
//! The standard operators for elementary type pointers also exist for
//! smart pointers (assignment, comparison, dereferencing, etc.)
//!
//! A smart pointer can be @e null, this is the default value. In this case,
//! the smart pointer does not reference any object. To test if a smart
//! pointer is a null pointer, use the method @c isNull(). Do not
//! use comparisons such as <code>p == 0</code>, the result will be incorrect.
//!
//! The QtlSmartPointer template class can be made thread-safe using a mutex.
//! The type of mutex to use is given by the template parameter @a MUTEXLOCKER.
//!
//! @tparam T The type of the pointed object. Cannot be an array type.
//! @tparam MUTEXLOCKER A class which is used to synchronize access to the
//! smart pointer internal state. Typically either QtlMutexLocker (for
//! thread-safe smart pointers) or QtlNullMutexLocker (for non-thread-safe
//! smart pointers).
//!
template <typename T, class MUTEXLOCKER>
class QtlSmartPointer
{
public:
    //!
    //! Default constructor using an optional unmanaged object.
    //!
    //! The optional argument @a p can be either @c 0 (null pointer)
    //! or the address of an @e unmanaged dynamically allocated object
    //! (i.e. which has been allocated using the operator @c new).
    //! In this case, @e unmanaged means that the object must not
    //! be already controlled by another set of smart pointers.
    //!
    //! This constructor is typically used in combination with the
    //! operator @c new to allocate an object which is immediatly
    //! managed by smart pointers.
    //!
    //! Example:
    //! @code
    //! QtlSmartPointer<Foo,QtlMutexLocker> ptr(new Foo(...));
    //! @endcode
    //!
    //! @param [in] p A pointer to an object of class @a T.
    //! The default value is @c 0, the null pointer. In this
    //! case, the smart pointer is a null pointer.
    //!
    QtlSmartPointer(T* p = 0) :
        _shared(new SmartShared(p))
    {
    }

    //!
    //! Copy constructor.
    //!
    //! This object references the same @a T object as @a sp.
    //! If @a sp is a null pointer, this object is also a null pointer.
    //!
    //! @param [in] sp Another smart pointer instance.
    //!
    QtlSmartPointer(const QtlSmartPointer<T,MUTEXLOCKER>& sp) :
        _shared(sp._shared->attach())
    {
    }

    //!
    //! Destructor.
    //!
    //! If this object is not a null pointer, the reference counter
    //! is decremented. When the reference counter reaches zero, the
    //! pointed object is automatically deleted.
    //!
    ~QtlSmartPointer();

    //!
    //! Assignment between smart pointers.
    //!
    //! After the assignment, this object references the same @a T object
    //! as @a sp. If this object was previously not the null pointer, the
    //! reference counter of the previously referenced @a T object is
    //! decremented. If the reference counter reaches zero, the previously
    //! pointed object is automatically deleted.
    //!
    //! @param [in] sp The value to assign.
    //! @return A reference to this object.
    //!
    const QtlSmartPointer<T,MUTEXLOCKER>& operator=(const QtlSmartPointer<T,MUTEXLOCKER>& sp);

    //!
    //! Assignment from a standard pointer @c T*.
    //!
    //! The pointed @c T object becomes managed by this smart pointer.
    //!
    //! The standard pointer @a p can be either @c 0 (null pointer)
    //! or the address of an @e unmanaged dynamically allocated object
    //! (i.e. which has been allocated using the operator @c new).
    //! In this case, @e unmanaged means that the object must not
    //! be already controlled by another set of smart pointers.
    //!
    //! After the assignment, this object references the @a T object.
    //! If this smart pointer object was previously not the null pointer, the
    //! reference counter of the previously referenced @a T object is
    //! decremented. If the reference counter reaches zero, the previously
    //! pointed object is automatically deleted.
    //!
    //! @param [in] p A pointer to an object of class @a T.
    //! @return A reference to this object.
    //!
    const QtlSmartPointer<T,MUTEXLOCKER>& operator=(T* p);

    //!
    //! Equality operator.
    //!
    //! Check if this smart pointer and the @a sp smart pointer point to same object.
    //!
    //! @b Caveat: Null pointers are not reliably compared with this operator.
    //! It shall not be used to compare against null pointer. Do not
    //! check <code>== 0</code>, use the method @c isNull() instead.
    //! Also, if both smart pointers are null pointers, the result is
    //! unpredictable, it can be true or false.
    //!
    //! @param [in] sp A smart pointer to compare with.
    //! @return True if both smart pointers reference the same object
    //! and false otherwise.
    //!
    bool operator==(const QtlSmartPointer<T,MUTEXLOCKER>& sp) const
    {
        return sp._shared == this->_shared;
    }

    //!
    //! Unequality operator.
    //!
    //! Check if this smart pointer and the @a sp smart pointer point to different objects.
    //!
    //! @b Caveat: Null pointers are not reliably compared with this operator.
    //! It shall not be used to compare against null pointer. Do not
    //! check <code>!= 0</code>, use the method @c isNull() instead.
    //! Also, if both smart pointers are null pointers, the result is
    //! unpredictable, it can be true or false.
    //!
    //! @param [in] sp A smart pointer to compare with.
    //! @return True if both smart pointers reference distinct objects
    //! and false otherwise.
    //!
    bool operator!=(const QtlSmartPointer<T,MUTEXLOCKER>& sp) const
    {
        return sp._shared != this->_shared;
    }

    //!
    //! Redirection operator.
    //!
    //! With this operator, a smart pointer can be used in the same way as a
    //! standard pointer.
    //!
    //! Example:
    //! @code
    //! class Foo
    //! {
    //! public:
    //!    void open();
    //! };
    //!
    //! QtlSmartPointer<Foo,QtlMutexLocker> ptr(new Foo());
    //! ptr->open();
    //! @endcode
    //!
    //! If this object is the null pointer, the operator returns zero and
    //! the further dereferencing operation will likely throw an exception.
    //!
    //! @return A standard pointer @c T* to the pointed object or
    //! @c 0 if this object is the null pointer.
    //!
    T* operator->() const
    {
        return _shared->pointer();
    }

    //!
    //! Accessor operator.
    //!
    //! With this operator, a smart pointer can be used in the same way as a
    //! standard pointer.
    //!
    //! Example:
    //! @code
    //! void f(Foo&);
    //!
    //! QtlSmartPointer<Foo,QtlMutexLocker> ptr(new Foo());
    //! f(*ptr);
    //! @endcode
    //!
    //! If this object is the null pointer, the operator will likely throw an exception.
    //!
    //! @return A reference @c T& to the pointed object.
    //!
    T& operator*() const
    {
        return *_shared->pointer();
    }

    //!
    //! Release the pointed object from the smart pointer management.
    //!
    //! The previously pointed object is not deallocated,
    //! its address is returned. All smart pointers which pointed
    //! to the object now point to @c 0 (null pointers).
    //!
    //! @b Caveat: The previously pointed object will no longer
    //! be automatically deleted. The caller must explicitly delete
    //! it later using the returned pointer value.
    //!
    //! @return A standard pointer @c T* to the previously pointed object.
    //! Return @c 0 if this object was the null pointer.
    //!
    T* release()
    {
        return _shared->release();
    }

    //!
    //! Deallocate the previous pointed object and set the pointer to the new object.
    //!
    //! All smart pointers which pointed to the same object now point to the new one.
    //! The previously pointed object is deleted using the operator @c delete.
    //!
    //! The standard pointer @a p can be either @c 0 (null pointer)
    //! or the address of an @e unmanaged dynamically allocated object
    //! (i.e. which has been allocated using the operator @c new).
    //! In this case, @e unmanaged means that the object must not
    //! be already controlled by another set of smart pointers.
    //!
    //! @param [in] p A pointer to an object of class @a T.
    //!
    void reset(T* p = 0)
    {
        _shared->reset(p);
    }

    //!
    //! Clear this instance of the smart pointer.
    //!
    //! The referenced object is deallocated if no more reference exists.
    //! Then, this smart pointer becomes the null pointer.
    //! @c sp.clear() is equivalent to <code>sp = (T*)(0)</code>.
    //!
    void clear()
    {
        _shared->detach();
        _shared = new SmartShared(0);
    }

    //!
    //! Check if this smart pointer is a null pointer.
    //!
    //! @return True if this smart pointer is a null pointer,
    //! false otherwise.
    //!
    bool isNull() const
    {
        return _shared->isNull();
    }

    //!
    //! Upcast operation.
    //!
    //! This method converts a smart pointer to an object of class @a T into a smart
    //! pointer to an object of a super-class @a ST of @a T.
    //!
    //! If this object is not the null pointer, the ownership of the pointed object
    //! is @e transfered to a new smart pointer to @a ST. This new smart pointer is
    //! returned. This object (the smart pointer to @a T) and all other smart pointers
    //! to @a T which pointed to the same object become null pointers.
    //!
    //! @tparam ST A super-class of @a T (immediate or indirect).
    //! @return If this object is not the null pointer, return a smart pointer to the
    //! same object, interpreted as @a ST. Otherwise, return the null pointer. The
    //! @a MUTEXLOCKER type of the returned smart pointer is the same as used in this object.
    //!
    template <typename ST>
    QtlSmartPointer<ST,MUTEXLOCKER> upcast()
    {
        return _shared->template upcast<ST>();
    }

    //!
    //! Downcast operation.
    //!
    //! This method converts a smart pointer to an object of class @a T into a smart
    //! pointer to an object of a subclass @a ST of @a T.
    //! This operation is equivalent to a @a dynamic_cast operator on regular pointers.
    //!
    //! If this object is not the null pointer and points to an instance of @a ST,
    //! the ownership of the pointed object is @e transfered to a new smart pointer
    //! to @a ST. This new smart pointer is returned. This object (the smart pointer
    //! to @a T) and all other smart pointers to @a T which pointed to the same
    //! object become null pointers.
    //!
    //! If this object is the null pointer or points to a @a T object which is not
    //! an instance of @a ST, the returned value is the null pointer and this object
    //! is unmodified.
    //!
    //! If RTTI is not supported, define the symbol @c QTL_NO_RTTI before including
    //! this header file. In that case, this method is equivalent to a static_cast.
    //! @b Warning: In this case, the transfer of ownership to the returned value is
    //! always performed. It is the responsibility of the application to ensure that
    //! the conversion is valid.
    //!
    //! @tparam ST A subclass of @a T (immediate or indirect).
    //! @return If this object is not the null pointer and points to an instance of @a ST,
    //! return a smart pointer to the same object, interpreted as @a ST. Otherwise, return
    //! the null pointer. The @a MUTEXLOCKER type of the returned smart pointer is the same as
    //! used in this object.
    //!
    template <typename ST>
    QtlSmartPointer<ST,MUTEXLOCKER> downcast()
    {
        return _shared->template downcast<ST>();
    }

    //!
    //! Change mutex locker type.
    //!
    //! This method converts a smart pointer to an object of class @a T into a smart
    //! pointer to the same class @a T with a different class of mutex locker.
    //!
    //! If this object is not the null pointer, the ownership of the pointed object
    //! is @e transfered to a new smart pointer. This new smart pointer is returned.
    //! This object and all other smart pointers of the same class which pointed to
    //! the same object become null pointers.
    //!
    //! @tparam NEWMUTEXLOCKER Another class which is used to synchronize access to the
    //! smart pointer internal state. Typically used to switch back and forth between
    //! QtlMutexLocker  and QtlNullMutexLocker.
    //! @return A smart pointer to the same object.
    //!
    template <typename NEWMUTEXLOCKER>
    QtlSmartPointer<T,NEWMUTEXLOCKER> changeMutex()
    {
        return _shared->template changeMutex<NEWMUTEXLOCKER>();
    }

    //!
    //! Get a standard pointer @c T* on the pointed object.
    //!
    //! @b Warning: This is an unchecked operation. Do not store
    //! this pointer to dereference it later since the pointed
    //! object may have been deleted in the meantime if no more
    //! smart pointer reference the object.
    //!
    //! @return A standard pointer @c T* to the pointed object
    //! or @c 0 if this object is the null pointer.
    //!
    T* pointer() const
    {
        return _shared->pointer();
    }

    //!
    //! Get the reference count value.
    //!
    //! This is informational only. In multi-threaded environments,
    //! the actual reference count may change before the result is
    //! actually used.
    //!
    //! @return The number of smart pointer objects which reference
    //! the same pointed object.
    //!
    int count() const
    {
        return _shared->count();
    }

private:
    //!
    //! Shared implementation of the smart pointer.
    //! All smart pointer objects which reference the same @c T object share
    //! one single @c SmartShared object.
    //!
    class SmartShared
    {
    private:
        T*  _ptr;       //!< Pointer to actual object.
        int _refCount;  //!< Reference counter.
        typename MUTEXLOCKER::Mutex _mutex; //!< Protect the SmartShared.

    public:
        //!
        //! Constructor.
        //! Initial reference count is 1.
        //! @param [in] p A pointer to an unmanaged object of class @a T.
        //!
        SmartShared(T* p = 0) :
            _ptr (p),
            _refCount (1),
            _mutex()
        {
        }

        //!
        //! Destructor.
        //! Deallocate actual object (if any).
        //!
        ~SmartShared ();

        //!
        //! Increment reference count and return this.
        //! @return This object.
        //!
        SmartShared* attach();

        //!
        //! Decrement reference count and deallocate this if needed.
        //!
        void detach();

        // Same semantics as QtlSmartPointer counterparts:
        T* release();
        void reset (T* p = 0);
        T* pointer();
        int count();
        bool isNull();
        template <typename ST> QtlSmartPointer<ST,MUTEXLOCKER> downcast();
        template <typename ST> QtlSmartPointer<ST,MUTEXLOCKER> upcast();
        template <typename NEWMUTEXLOCKER> QtlSmartPointer<T,NEWMUTEXLOCKER> changeMutex();

    private:
        // Unaccessible operations.
        SmartShared() Q_DECL_EQ_DELETE;
        Q_DISABLE_COPY(SmartShared)
    };

    //! QtlSmartPointer private field simply points to a SmartShared instance.
    SmartShared* _shared;
};

#include "QtlSmartPointerTemplate.h"
#endif // QTLSMARTPOINTER_H
