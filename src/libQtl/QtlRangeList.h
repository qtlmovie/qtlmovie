//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
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
//! @file QtlRangeList.h
//!
//! Declare the class QtlRangeList.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLRANGELIST_H
#define QTLRANGELIST_H

#include "QtlRange.h"

//!
//! Qtl namespace.
//!
namespace Qtl {
    //!
    //! Flags for QtlRangeList::merge().
    //!
    enum MergeRangeFlag {
        AdjacentOnly = 0x0000,  //!< Merge adjacent ranges only, keep total value count and order.
        Sorted       = 0x0001,  //!< Sort the list of ranges first.
        NoDuplicate  = 0x0002   //!< Remove duplicates, merge overlapped ranges.
    };
    Q_DECLARE_FLAGS(MergeRangeFlags, MergeRangeFlag)
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Qtl::MergeRangeFlags)

//!
//! A class which extends a list of QtlRange.
//!
class QtlRangeList : public QList<QtlRange>
{
public:
    //!
    //! Redefine the superclass type.
    //!
    typedef QList<QtlRange> SuperClass;

    //!
    //! Default constructor.
    //!
    QtlRangeList() : SuperClass() {}

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy.
    //!
    QtlRangeList(const SuperClass &other) : SuperClass(other) {}

    //!
    //! Constructor from one initial element.
    //! @param [in] e Initial element.
    //!
    QtlRangeList(const QtlRange& e) :
#if defined(Q_COMPILER_INITIALIZER_LISTS)
        SuperClass({e}) {}
#else
        SuperClass() {append(e);}
#endif

#if defined(Q_COMPILER_INITIALIZER_LISTS) || defined(DOXYGEN)
    //!
    //! Constructor from a variadic list of elements.
    //! This constructor is only enabled if the compiler supports C++11 initializer lists
    //! @param [in] args List of initial elements.
    //!
    QtlRangeList(std::initializer_list<QtlRange> args) : SuperClass(args) {}
#endif

    //!
    //! Get the total number of values in all ranges.
    //! @return The number of values in all ranges.
    //!
    quint64 totalValueCount() const;

    //!
    //! Add a given offset to first and last value of all elements in the list.
    //! There is no overflow, the ranges are bounded by @a qint64.
    //! @param [in] offset Value to add.
    //! @return A reference to this object.
    //!
    QtlRangeList& add(qint64 offset);

    //!
    //! Scale all ranges in the list by a given factor.
    //! @param [in] factor Scale factor.
    //! @return A reference to this object.
    //!
    QtlRangeList& scale(qint64 factor);

    //!
    //! Sort the list.
    //! @return A reference to this object.
    //!
    QtlRangeList& sort()
    {
        std::sort(this->begin(), this->end());
        return *this;
    }

    //!
    //! Merge all overlapping or adjacent segments.
    //! @param [in] flags Merge options.
    //! @return A reference to this object.
    //! @see Qtl::MergeFlags
    //!
    QtlRangeList& merge(Qtl::MergeRangeFlags flags = Qtl::AdjacentOnly);

    //!
    //! Remove values outside the given range.
    //! @param [in] range Limit the values within that range.
    //! @return A reference to this object.
    //!
    QtlRangeList& clip(const QtlRange& range);

    //!
    //! Convert the list of ranges to a string.
    //! @return A string in format "[first, last] [first, last] ...".
    //!
    QString toString() const;
};

#endif // QTLRANGELIST_H
