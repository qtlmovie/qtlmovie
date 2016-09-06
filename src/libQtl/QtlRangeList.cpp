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
//
// Qtl, Qt utility library.
// Define the class QtlRangeList.
//
//----------------------------------------------------------------------------

#include "QtlRangeList.h"


//----------------------------------------------------------------------------
// Get the total number of values in all ranges.
//----------------------------------------------------------------------------

quint64 QtlRangeList::totalValueCount() const
{
    quint64 result = 0;
    for (ConstIterator it(begin()); it != end(); ++it) {
        result += it->count();
    }
    return result;
}


//----------------------------------------------------------------------------
// Get the lowest or highest value in all ranges.
//----------------------------------------------------------------------------

qint64 QtlRangeList::first() const
{
    if (isEmpty()) {
        return 0;
    }
    else {
        qint64 result = QtlRange::RANGE_MAX;
        foreach (const QtlRange& range, *this) {
            const qint64 f = range.first();
            if (f < result) {
                result = f;
            }
        }
        return result;
    }
}

qint64 QtlRangeList::last() const
{
    if (isEmpty()) {
        return -1;
    }
    else {
        qint64 result = QtlRange::RANGE_MIN;
        foreach (const QtlRange& range, *this) {
            const qint64 l = range.last();
            if (l > result) {
                result = l;
            }
        }
        return result;
    }
}

QtlRange QtlRangeList::enclosing() const
{
    return QtlRange(first(), last());
}


//----------------------------------------------------------------------------
// Add or scale all elements.
//----------------------------------------------------------------------------

QtlRangeList& QtlRangeList::add(qint64 offset)
{
    for (Iterator it(begin()); it != end(); ++it) {
        it->add(offset);
    }
    return *this;
}

QtlRangeList& QtlRangeList::scale(qint64 factor)
{
    for (Iterator it(begin()); it != end(); ++it) {
        it->scale(factor);
    }
    return *this;
}


//----------------------------------------------------------------------------
// Merge all overlapping or adjacent segments.
//----------------------------------------------------------------------------

QtlRangeList& QtlRangeList::merge(Qtl::MergeRangeFlags flags)
{
    // Sort the list first if required.
    if (flags & Qtl::Sorted) {
        this->sort();
    }

    Iterator it(begin());
    if (flags & Qtl::NoDuplicate) {
        // Remove duplicates, merge overlapped ranges.
        while (it != this->end()) {
            Iterator next(it + 1);
            if (next != end() && (it->overlap(*next) || it->adjacent(*next))) {
                it->merge(*next);
                it = erase(next) - 1;
                // If not sorted, the merge may have set the first value at iterator backward.
                // We may need to merge backward.
                if (!(flags & Qtl::Sorted) && it != begin()) {
                    --it;
                }
            }
            else {
                it = next;
            }
        }
    }
    else {
        // Only merge exactly adjacent ranges.
        while (it != this->end()) {
            Iterator next(it + 1);
            if (next != end() && it->adjacent(*next, Qtl::AdjacentBefore)) {
                it->merge(*next);
                it = erase(next) - 1;
            }
            else {
                it = next;
            }
        }
    }

    return *this;
}


//----------------------------------------------------------------------------
// Remove values outside the given range.
//----------------------------------------------------------------------------

QtlRangeList& QtlRangeList::clip(const QtlRange& range)
{
    Iterator it(begin());
    while (it != end()) {
        it->clip(range);
        if (it->isEmpty()) {
            it = erase(it);
        }
        else {
            ++it;
        }
    }
    return *this;
}


//----------------------------------------------------------------------------
// Convert the range to a string.
//----------------------------------------------------------------------------

QString QtlRangeList::toString(const QString& separator) const
{
    QString result;
    foreach (const QtlRange& range, *this) {
        if (!result.isEmpty()) {
            result.append(separator);
        }
        result.append(range.toString());
    }
    return result;
}
