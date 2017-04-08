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
//!
//! @file QtsExtTableId.h
//!
//! Declare the class QtsExtTableId.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSEXTTABLEID_H
#define QTSEXTTABLEID_H

#include "QtsCore.h"

//!
//! Extended table id (include table id extension).
//!
//! For convenience, it is sometimes useful to identify tables using an
//! "extended TID", a combination of TID and TIDext. On one PID, two tables
//! with the same TID but with different TIDext are considered as distinct
//! tables. By convention, the TIDext is always zero with short sections.
//!
class QtsExtTableId
{
private:
    quint32 _etid; //!< 7-bit: unused, 1-bit: long table, 8-bit: tid, 16-bit: tid-ext
public:
    //!
    //! Constructor from a short table id.
    //! @param [in] tid Table id.
    //!
    QtsExtTableId(QtsTableId tid = 0xFF) :
        _etid((quint32(tid) & 0xFF) << 16)
    {
    }
    //!
    //! Constructor from a long table id and tid-ext.
    //! @param [in] tid Table id.
    //! @param [in] tidExt Table id entension.
    //!
    QtsExtTableId(QtsTableId tid, QtsTableIdExt tidExt) :
        _etid(0x01000000 | ((quint32(tid) & 0xFF) << 16) | (quint32(tidExt) & 0xFFFF))
    {
    }
    //!
    //! Copy constructor
    //! @param [in] e Other instance to copy.
    //!
    QtsExtTableId(const QtsExtTableId& e) :
        _etid(e._etid)
    {
    }
    //!
    //! Assignment
    //! @param [in] e Other instance to copy.
    //! @return A reference to this object.
    //!
    const QtsExtTableId& operator=(const QtsExtTableId& e)
    {
        _etid = e._etid;
        return *this;
    }
    //!
    //! Check if the extended table id references a section with a long header.
    //! @return True if the extended table id references a section with a long header.
    //!
    bool isLongSection() const
    {
        return (_etid & 0x01000000) != 0;
    }
    //!
    //! Check if the extended table id references a section with a short header.
    //! @return True if the extended table id references a section with a short header.
    //!
    bool isShortSection() const
    {
        return (_etid & 0x01000000) == 0;
    }
    //!
    //! Get the Table Id part.
    //! @return The Table Id part.
    //!
    QtsTableId tid() const
    {
        return QtsTableId((_etid >> 16) & 0xFF);
    }
    //!
    //! Get the Table Id Extension part.
    //! @return The Table Id Extension part.
    //!
    QtsTableIdExt tidExt() const
    {
        return quint16(_etid & 0xFFFF);
    }
    //!
    //! Comparison operator.
    //! @param [in] e Other instance to compare.
    //! @return True is this object is equal to @a e.
    //!
    bool operator==(const QtsExtTableId& e) const
    {
        return _etid == e._etid;
    }
    //!
    //! Comparison operator.
    //! @param [in] e Other instance to compare.
    //! @return True is this object is different from @a e.
    //!
    bool operator!=(const QtsExtTableId& e) const
    {
        return _etid != e._etid;
    }
    //!
    //! Comparison operator.
    //! @param [in] e Other instance to compare.
    //! @return True is this object is logically less than @a e.
    //!
    bool operator<(const QtsExtTableId& e) const
    {
        return _etid <  e._etid;
    }
    //!
    //! Comparison operator.
    //! @param [in] e Other instance to compare.
    //! @return True is this object is logically less than or equal to @a e.
    //!
    bool operator<=(const QtsExtTableId& e) const
    {
        return _etid <= e._etid;
    }
    //!
    //! Comparison operator.
    //! @param [in] e Other instance to compare.
    //! @return True is this object is logically greater than @a e.
    //!
    bool operator>(const QtsExtTableId& e) const
    {
        return _etid >  e._etid;
    }
    //!
    //! Comparison operator.
    //! @param [in] e Other instance to compare.
    //! @return True is this object is logically greater than or equal to @a e.
    //!
    bool operator>=(const QtsExtTableId& e) const
    {
        return _etid >= e._etid;
    }
};

#endif // QTSEXTTABLEID_H

