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
//! @file QtlRange.h
//!
//! Declare the template class QtlRange.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLRANGE_H
#define QTLRANGE_H

#include "QtlCore.h"

//!
//! Qtl namespace.
//!
namespace Qtl {
    //!
    //! Flags for QtlRange::adjacent().
    //!
    enum AdjacentFlag {
        AdjacentBefore = 0x0001,  //!< The first (or "this") object is adjacent before the second (or "other") object.
        AdjacentAfter  = 0x0002,  //!< The first (or "this") object is adjacent after the second (or "other") object.
        AdjacentAny    = 0x0003   //!< The two objects are adjacent in any order.
    };
    Q_DECLARE_FLAGS(AdjacentFlags, AdjacentFlag)
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Qtl::AdjacentFlags)

//!
//! A class which describes a contiguous range of integer values.
//! @tparam INT Any integer type, signed or unsigned.
//!
template<typename INT>
class QtlRange
{
public:
    //!
    //! Default constructor.
    //!
    explicit QtlRange() :
        _first(1),
        _count(0)
    {
    }

    //!
    //! Constructor.
    //! @param [in] first First value in the range.
    //! @param [in] last Last value in the range. Cannot be less than @first - 1.
    //!
    explicit QtlRange(INT first, INT last)
    {
        setRange(first, last);
    }

    //!
    //! Change the range.
    //! @param [in] first First value in the range.
    //! @param [in] last Last value in the range. Cannot be less than @first - 1.
    //!
    void setRange(INT first, INT last)
    {
        _first = first;
        setLast(last);
    }

    //!
    //! Change the first value in the range.
    //! @param [in] first First value in the range.
    //!
    void setFirst(INT first);

    //!
    //! Change the last value in the range.
    //! @param [in] last Last value in the range. Cannot be less than @first - 1.
    //!
    void setLast(INT last)
    {
        _count = _first > last ? 0 : static_cast<quint64>(last - _first) + 1;
    }

    //!
    //! Get the first value in the range.
    //! @return The first value in the range.
    //!
    INT first() const
    {
        return _first;
    }

    //!
    //! Get the last value in the range.
    //! @return The last value in the range. Note that if the range is empty
    //! and @a first is the minimum value in the type @a INT, then the
    //! returned last is also this minimum value. This can be misleading.
    //!
    INT last() const;

    //!
    //! Get the number of values in the range.
    //! @return The number of values in the range. The returned type is
    //! @a quint64 and not @a INT since this is the only type which is
    //! guaranteed to always hold the right count for any combination of
    //! @a first, @a last and @a INT.
    //!
    quint64 count() const
    {
        return _count;
    }

    //!
    //! Check if the range contains no value.
    //! @return True if the range contains no value.
    //!
    bool isEmpty() const
    {
        return _count == 0;
    }

    //!
    //! Clear the range.
    //! The first value remains unchanged.
    //!
    void clear()
    {
        _count = 0;
    }

    //!
    //! Check if this object and @a other overlap (if they have at least one element in common).
    //! @param [in] other An other instance to compare.
    //! @return True if the two objects have at least one element in common.
    //!
    bool overlap(const QtlRange<INT>& other) const;

    //!
    //! Check if this object and @a other are exactly adjacent.
    //! @param [in] other An other instance to compare.
    //! @param [in] flags Check if ranges are adjacent in a specific order.
    //! @return True if the two objects are exactly adjacent.
    //!
    bool adjacent(const QtlRange<INT>& other, Qtl::AdjacentFlags flags = Qtl::AdjacentAny) const;

    //!
    //! Merge the content of another object into this one.
    //! The new first value is the smallest of the two first values.
    //! The new last value is the largest of the two last values.
    //! @param [in] other Other instance to merge.
    //!
    void merge(const QtlRange<INT>& other);

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is equal to @a other.
    //! False otherwise.
    //!
    bool operator==(const QtlRange<INT>& other) const
    {
        return _first == other._first && _count == other._count;
    }

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is different from @a other.
    //! False otherwise.
    //!
    bool operator!=(const QtlRange<INT>& other) const
    {
        return !(*this == other);
    }

    //!
    //! Comparison operator.
    //! The ranges are first ordered by their first value, then by last value.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is less than @a other.
    //! False otherwise.
    //!
    bool operator<(const QtlRange<INT>& other) const
    {
        return _first != other._first ? _first < other._first : _count < other._count;
    }

    //!
    //! Comparison operator.
    //! The ranges are first ordered by their first value, then by last value.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is less than or equal to @a other.
    //! False otherwise.
    //!
    bool operator<=(const QtlRange<INT>& other) const
    {
        return *this < other || *this == other;
    }

    //!
    //! Comparison operator.
    //! The ranges are first ordered by their first value, then by last value.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is greater than @a other.
    //! False otherwise.
    //!
    bool operator>(const QtlRange<INT>& other) const
    {
        return !(*this <= other);
    }

    //!
    //! Comparison operator.
    //! The ranges are first ordered by their first value, then by last value.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is greater than or equal to @a other.
    //! False otherwise.
    //!
    bool operator>=(const QtlRange<INT>& other) const
    {
        return !(*this < other);
    }

private:
    INT     _first;  //!< First value.
    quint64 _count;  //!< Number of values in the range.
};

#include "QtlRangeTemplate.h"
#endif // QTLRANGE_H
