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
// CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//!
//! @file QtlPointerList.h
//!
//! Declare the class QtlPointerList.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLPOINTERLIST_H
#define QTLPOINTERLIST_H

#include <QList>
#include <QtAlgorithms>

//!
//! Definition of a list of pointers which deletes the pointed items on destruction.
//!
template<typename T>
class QtlPointerList : public QList<T*>
{
private:
    typedef QList<T*> SuperClass; //!< A name for the superclass.

public:
    //!
    //! Default constructor.
    //!
    explicit QtlPointerList() :
        QList<T*>()
    {
    }

    //!
    //! Copy constructor
    //! @param [in] other Other instance to copy.
    //!
    QtlPointerList(const QList<T*>& other) :
        QList<T*>(other)
    {
    }

    //!
    //! Remove all items from the list.
    //! All pointed items are deleted.
    //!
    void clear()
    {
        qDeleteAll(*this);
        SuperClass::clear();
    }

    //!
    //! Destructor.
    //! All pointed items are deleted.
    //!
    virtual ~QtlPointerList()
    {
        this->clear();
    }
};

#endif // QTLPOINTERLIST_H
