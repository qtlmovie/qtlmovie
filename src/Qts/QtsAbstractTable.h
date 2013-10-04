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
//! @file QtsAbstractTable.h
//!
//! Declare the class QtsAbstractTable.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSABSTRACTTABLE_H
#define QTSABSTRACTTABLE_H

#include "QtsCore.h"
#include "QtsTable.h"

//!
//! Abstract base class for MPEG PSI/SI tables.
//!
class QtsAbstractTable
{
public:
    //!
    //! Check if the table is valid.
    //! @return True if the table is valid.
    //!
    bool isValid() const
    {
        return _isValid;
    }

    //!
    //! Invalidate the table.
    //! The table must be rebuilt.
    //!
    void invalidate()
    {
        _isValid = false;
    }

    //!
    //! Get the table id.
    //! @return The table id.
    //!
    QtsTableId tableId() const
    {
        return _tableId;
    }

    //!
    //! This abstract method serializes a table.
    //! @param [out] table Replaced with a binary representation (a list of sections) of this object.
    //! @return True on success, false on error (invalid table, data too large, etc.)
    //!
    virtual bool serialize(QtsTable& table) const = 0;

    //!
    //! This abstract method deserializes a binary table.
    //! @param [out] table Binary table to deserialize. If the binary
    //! table is not a valid representation of the QtsAbstractTable
    //! subclass, this object is invalid.
    //! @return True on success, false on error (invalid data, etc.)
    //!
    virtual bool deserialize(const QtsTable& table) = 0;

    //!
    //! Virtual destructor.
    //!
    virtual ~QtsAbstractTable()
    {
    }

protected:
    //!
    //! Table id, can be modified by subclasses.
    //!
    QtsTableId _tableId;

    //!
    //! Validity flag.
    //! It is the responsibility of the subclasses to set the valid flag.
    //!
    bool _isValid;

    //!
    //! Protected constructor for subclasses
    //! @param [in] tid Table id.
    //!
    QtsAbstractTable(QtsTableId tid) :
        _tableId(tid),
        _isValid(false)
    {
    }

    //!
    //! Serialization helper: Start a new binary table.
    //! @param [out] table The binary table into which this object
    //! will be serialized. We invalidate it initially.
    //! @return True on success, false on error (invalid table, etc.)
    //!
    bool serializeInit(QtsTable& table) const;

    //!
    //! Deserialization helper: Start the deserialization.
    //! This object is marked as invalid. The deserialize() method shall set
    //! it to true later if the serialization succeeds.
    //! @param [in] table Binary table to deserialize.
    //! @return True if the input is valid and deserialization may continue.
    //! False if the binary table is invalid or has the wrong tid.
    //!
    bool deserializeInit(const QtsTable& table);

private:
    // Unaccessible operations.
    QtsAbstractTable() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtsAbstractTable)
};

#endif // QTSABSTRACTTABLE_H
