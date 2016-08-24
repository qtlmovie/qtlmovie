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
// Template methods for class QtlRange.
//
//----------------------------------------------------------------------------

#ifndef QTLRANGETEMPLATE_H
#define QTLRANGETEMPLATE_H


//----------------------------------------------------------------------------
// Change the first value in the range.
//----------------------------------------------------------------------------

template<typename INT>
void QtlRange<INT>::setFirst(INT first)
{
    if (first != _first) {
        if (first < _first) {
            _count += _first - first;
        }
        else { // first > _first
            const quint64 less = static_cast<quint64>(first - _first);
            _count = _count < less ? 0 : _count - less;
        }
        _first = first;
    }
}


//----------------------------------------------------------------------------
// Get the last value in the range.
//----------------------------------------------------------------------------

template<typename INT>
INT QtlRange<INT>::last() const
{
    if (_count == 0) {
        return _first == std::numeric_limits<INT>::min() ? _first : _first - 1;
    }
    else if (std::numeric_limits<INT>::is_signed) {
        return static_cast<INT>(static_cast<qint64>(_first) + (_count - 1));
    }
    else {
        return static_cast<INT>(static_cast<quint64>(_first) + (_count - 1));
    }
}


//----------------------------------------------------------------------------
// Check if two objects have at least one element in common.
//----------------------------------------------------------------------------

template<typename INT>
bool QtlRange<INT>::overlap(const QtlRange<INT>& other) const
{
    if (_first <= other._first) {
        return static_cast<quint64>(other._first - _first) < _count;
    }
    else { // other._first > _first
        return static_cast<quint64>(_first - other._first) < other._count;
    }
}


//----------------------------------------------------------------------------
// Check if two objects are exactly adjacent.
//----------------------------------------------------------------------------

template<typename INT>
bool QtlRange<INT>::adjacent(const QtlRange<INT>& other, Qtl::AdjacentFlags flags) const
{
    if (_first == other._first) {
        return (_count == 0 && (flags & Qtl::AdjacentBefore)) || (other._count == 0 && (flags & Qtl::AdjacentAfter));
    }
    else if (_first < other._first) {
        return (flags & Qtl::AdjacentBefore) && static_cast<quint64>(other._first - _first) == _count;
    }
    else { // other._first > _first
        return (flags & Qtl::AdjacentAfter) && static_cast<quint64>(_first - other._first) == other._count;
    }
}


//----------------------------------------------------------------------------
// Merge the content of another object into this one.
//----------------------------------------------------------------------------

template<typename INT>
void QtlRange<INT>::merge(const QtlRange<INT>& other)
{
    if (_first <= other._first) {
        _count = qMax(_count, static_cast<quint64>(other._first - _first) + other._count);
    }
    else { // other._first < _first
        _count = qMax(other._count, static_cast<quint64>(_first - other._first) + _count);
        _first = other._first;
    }
}

#endif // QTLRANGETEMPLATE_H
