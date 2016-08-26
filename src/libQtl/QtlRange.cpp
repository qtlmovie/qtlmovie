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
// Define the class QtlRange.
//
//----------------------------------------------------------------------------

#include "QtlRange.h"


//
// Static definitions (required by the linker).
//
const qint64 QtlRange::RANGE_MIN;
const qint64 QtlRange::RANGE_MAX;


//----------------------------------------------------------------------------
// Change the first or last value in the range.
//----------------------------------------------------------------------------

void QtlRange::setFirst(qint64 first)
{
    if (first != _first) {
        if (first < _first) {
            _count += _first - first;
        }
        else { // first > _first
            const quint64 less = first - _first;
            _count = _count < less ? 0 : _count - less;
        }
        _first = first;
    }
}

void QtlRange::setLast(qint64 last)
{
    _count = _first > last ? 0 : last - _first + 1;
}


//----------------------------------------------------------------------------
// Get the last value in the range.
//----------------------------------------------------------------------------

qint64 QtlRange::last() const
{
    return _count == 0 && _first == RANGE_MIN ? RANGE_MIN : _first + _count - 1;
}


//----------------------------------------------------------------------------
// Add a given offset to first and last value.
//----------------------------------------------------------------------------

QtlRange& QtlRange::add(qint64 offset)
{
    if (offset > 0) {
        if (RANGE_MAX - offset < _first) {
            // Complete overflow at end.
            _first = RANGE_MAX;
            _count = 0;
        }
        else if ((_first != RANGE_MIN || _count != 0) && RANGE_MAX - offset < qint64(_first + _count - 1)) {
            // Partial overflow at end.
            _first += offset;
            _count = RANGE_MAX - _first + 1;
        }
        else {
            // No overflow.
            _first += offset;
        }
    }
    else { // offset <= 0
        if ((_first == RANGE_MIN && _count == 0) || ((_first != RANGE_MIN || _count != 0) && RANGE_MIN - offset > qint64(_first + _count - 1))) {
            // Complete overflow at beginning.
            _first = RANGE_MIN;
            _count = 0;
        }
        else if (RANGE_MIN - offset > _first) {
            // Partial overflow at beginning.
            _count -= RANGE_MIN - offset - _first;
            _first = RANGE_MIN;
        }
        else {
            // No overflow.
            _first += offset;
        }
    }
    return *this;
}


//----------------------------------------------------------------------------
// Scale the range by a given factor.
//----------------------------------------------------------------------------

QtlRange& QtlRange::scale(qint64 factor)
{
    if (_first < 0 || factor <= 0) {
        _first = 0;
        _count = 0;
    }
    else if (_first > (RANGE_MAX - factor + 1) / factor) {
        // Complete overflow at end.
        _first = RANGE_MAX;
        _count = 0;
    }
    else {
        _first = _first * factor;
        const quint64 maxCount = quint64(RANGE_MAX - _first) + 1;
        if (_count > (maxCount - factor + 1) / factor) {
            // Partial overflow at end.
            _count = RANGE_MAX - _first + 1;
        }
        else {
            _count = _count * factor;
        }
    }
    return *this;
}


//----------------------------------------------------------------------------
// Check if two objects have at least one element in common.
//----------------------------------------------------------------------------

bool QtlRange::overlap(const QtlRange& other) const
{
    if (_first <= other._first) {
        return quint64(other._first - _first) < _count;
    }
    else { // other._first > _first
        return quint64(_first - other._first) < other._count;
    }
}


//----------------------------------------------------------------------------
// Check if two objects are exactly adjacent.
//----------------------------------------------------------------------------

bool QtlRange::adjacent(const QtlRange& other, Qtl::AdjacentFlags flags) const
{
    if (_first == other._first) {
        return (_count == 0 && (flags & Qtl::AdjacentBefore)) || (other._count == 0 && (flags & Qtl::AdjacentAfter));
    }
    else if (_first < other._first) {
        return (flags & Qtl::AdjacentBefore) && quint64(other._first - _first) == _count;
    }
    else { // other._first > _first
        return (flags & Qtl::AdjacentAfter) && quint64(_first - other._first) == other._count;
    }
}


//----------------------------------------------------------------------------
// Merge the content of another object into this one.
//----------------------------------------------------------------------------

QtlRange& QtlRange::merge(const QtlRange& other)
{
    if (_first <= other._first) {
        _count = qMax(_count, quint64(other._first - _first) + other._count);
    }
    else { // other._first < _first
        _count = qMax(other._count, quint64(_first - other._first) + _count);
        _first = other._first;
    }
    return *this;
}


//----------------------------------------------------------------------------
// Reduce the range so that it fits in a given range.
//----------------------------------------------------------------------------

QtlRange& QtlRange::clip(const QtlRange& range)
{
    const qint64 rangeLast = range.last();
    if (_first > rangeLast) {
        _first = rangeLast + 1; // never overflow since rangeLast < _first, so rangeLast is not qint64'last.
        _count = 0;
    }
    else {
        const qint64 thisLast = this->last();
        if (_first < range._first) {
            _first = range._first;
        }
        setLast(qMin(thisLast, rangeLast));
    }
    return *this;
}



//----------------------------------------------------------------------------
// Convert the range to a string.
//----------------------------------------------------------------------------

QString QtlRange::toString() const
{
    return QStringLiteral("[%1, %2]").arg(_first).arg(last());
}
