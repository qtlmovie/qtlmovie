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
//! Declare the class QtlRange.
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
//! A class which describes a contiguous range of qint64 integer values.
//!
//! Note that this class could be transformed into a template one for any integer type.
//! However, the handling of type boundaries, overflow, signed vs. unsigned make the
//! code bloated (believe me, I started from that), so I wanted to keep it simple.
//!
class QtlRange
{
public:
    //!
    //! Minimum value in a range.
    //!
    static const qint64 RANGE_MIN = Q_INT64_C(0x8000000000000000);

    //!
    //! Maximum value in a range.
    //!
    static const qint64 RANGE_MAX = Q_INT64_C(0x7FFFFFFFFFFFFFFF);

    //!
    //! Default constructor.
    //!
    explicit QtlRange() :
        _first(0),
        _count(0)
    {
    }

    //!
    //! Constructor.
    //! @param [in] first First value in the range.
    //! @param [in] last Last value in the range. Cannot be less than @a first - 1.
    //!
    explicit QtlRange(qint64 first, qint64 last)
    {
        setRange(first, last);
    }

    //!
    //! Change the range.
    //! @param [in] first First value in the range.
    //! @param [in] last Last value in the range. Cannot be less than @a first - 1.
    //!
    void setRange(qint64 first, qint64 last)
    {
        _first = first;
        setLast(last);
    }

    //!
    //! Change the first value in the range, without changing the last one.
    //! @param [in] first First value in the range.
    //!
    void setFirst(qint64 first);

    //!
    //! Change the last value in the range, without changing the first one.
    //! @param [in] last Last value in the range. Cannot be less than @a first - 1.
    //!
    void setLast(qint64 last);

    //!
    //! Get the first value in the range.
    //! @return The first value in the range.
    //!
    qint64 first() const
    {
        return _first;
    }

    //!
    //! Get the last value in the range.
    //! @return The last value in the range. Note that if the range is empty
    //! and @a first is the minimum value in the type @c qint64, then the
    //! returned last is also this minimum value. This can be misleading.
    //!
    qint64 last() const;

    //!
    //! Get the number of values in the range.
    //! @return The number of values in the range. The returned type is
    //! @a quint64 and not @a qint64 since this is the only type which is
    //! guaranteed to always hold the right count for any combination of
    //! @a first and @a last.
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
    //! Check if a given value is in the range.
    //! @param [in] value A value to check.
    //! @return True if @a value is in the range.
    //!
    bool contains(qint64 value);

    //!
    //! Add a given offset to first and last value.
    //! There is no overflow, the range is bounded by @a qint64.
    //! @param [in] offset Value to add.
    //! @return A reference to this object.
    //!
    QtlRange& add(qint64 offset);

    //!
    //! Scale the range by a given factor.
    //! If first() or last() are negative, the scale operation is meaningless and sets the range to zero.
    //! Otherwise, the new first() is the previous first() times @a factor and
    //! the new last() is the previous last() plus one, times @a factor, minus one.
    //! There is no overflow, the range is bounded by @a qint64.
    //! @param [in] factor Scale factor.
    //! @return A reference to this object.
    //!
    QtlRange& scale(qint64 factor);

    //!
    //! Check if this object and @a other overlap (if they have at least one element in common).
    //! @param [in] other An other instance to compare.
    //! @return True if the two objects have at least one element in common.
    //!
    bool overlap(const QtlRange& other) const;

    //!
    //! Check if this object and @a other are exactly adjacent.
    //! @param [in] other An other instance to compare.
    //! @param [in] flags Check if ranges are adjacent in a specific order.
    //! @return True if the two objects are exactly adjacent.
    //!
    bool adjacent(const QtlRange& other, Qtl::AdjacentFlags flags = Qtl::AdjacentAny) const;

    //!
    //! Merge the content of another object into this one.
    //! The new first value is the smallest of the two first values.
    //! The new last value is the largest of the two last values.
    //! @param [in] other Other instance to merge.
    //! @return A reference to this object.
    //!
    QtlRange& merge(const QtlRange& other);

    //!
    //! Reduce the range so that it fits in a given range.
    //! @param [in] range Limit the values within that range.
    //! @return A reference to this object.
    //!
    QtlRange& clip(const QtlRange& range);

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is equal to @a other.
    //! False otherwise.
    //!
    bool operator==(const QtlRange& other) const
    {
        return _first == other._first && _count == other._count;
    }

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if this instance is different from @a other.
    //! False otherwise.
    //!
    bool operator!=(const QtlRange& other) const
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
    bool operator<(const QtlRange& other) const
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
    bool operator<=(const QtlRange& other) const
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
    bool operator>(const QtlRange& other) const
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
    bool operator>=(const QtlRange& other) const
    {
        return !(*this < other);
    }

    //!
    //! Convert the range to a string.
    //! @return A string in format "[first, last]".
    //!
    QString toString() const;

private:
    qint64  _first;  //!< First value.
    quint64 _count;  //!< Number of values in the range.
};

//!
//! Format a QtlRange object on a text stream.
//! @param [in] stream The text stream to write to.
//! @param [in] object The range to format.
//! @return A reference to @a stream.
//!
inline QTextStream& operator<<(QTextStream& stream, const QtlRange& object)
{
    return stream << object.toString();
}

#endif // QTLRANGE_H
