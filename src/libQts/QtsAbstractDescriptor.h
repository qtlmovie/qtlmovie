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
//! @file QtsAbstractDescriptor.h
//!
//! Declare the class QtsAbstractDescriptor.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSABSTRACTDESCRIPTOR_H
#define QTSABSTRACTDESCRIPTOR_H

#include "QtsCore.h"
#include "QtsDescriptor.h"
#include "QtlSmartPointer.h"

//!
//! Abstract base class for MPEG PSI/SI descriptors.
//!
class QtsAbstractDescriptor
{
public:
    //!
    //! Check if the descriptor is valid.
    //! @return True if the descriptor is valid.
    //!
    bool isValid() const
    {
        return _isValid;
    }

    //!
    //! Invalidate the descriptor.
    //! The descriptor must be rebuilt.
    //!
    void invalidate()
    {
        _isValid = false;
    }

    //!
    //! Get the descriptor tag.
    //! @return The descriptor tag.
    //!
    QtsDescTag tag() const
    {
        return _descTag;
    }

    //!
    //! This abstract method serializes a descriptor.
    //! @param [out] descriptor Replaced with a binary representation of this object.
    //! @return True on success, false on error (invalid descriptor, data too large, etc.)
    //!
    virtual bool serialize(QtsDescriptor& descriptor) const = 0;

    //!
    //! This abstract method deserializes a binary descriptor.
    //! @param [in] descriptor Binary descriptor to deserialize. If the binary
    //! descriptor is not a valid representation of the QtsAbstractDescriptor
    //! subclass, this object is invalid.
    //! @return True on success, false on error (invalid data, etc.)
    //!
    virtual bool deserialize(const QtsDescriptor& descriptor) = 0;

    //!
    //! Virtual destructor.
    //!
    virtual ~QtsAbstractDescriptor()
    {
    }

protected:
    //!
    //! Descriptor tag, can be modified by subclasses.
    //!
    QtsDescTag _descTag;

    //!
    //! Validity flag.
    //! It is the responsibility of the subclasses to set the valid flag.
    //!
    bool _isValid;

    //!
    //! Protected constructor for subclasses
    //! @param [in] tag Descriptor tag.
    //!
    QtsAbstractDescriptor(QtsDescTag tag) :
        _descTag(tag),
        _isValid(false)
    {
    }

    //!
    //! Serialization helper: Start a new binary descriptor.
    //! @param [out] descriptor The binary descriptor into which this object
    //! will be serialized. We invalidate it initially.
    //! @param [out] data A byte block which receives the descriptor header.
    //! The descriptor_length field is set to zero.
    //! @return True on success, false on error (invalid descriptor, etc.)
    //!
    bool serializeInit(QtsDescriptor& descriptor, QtlByteBlock& data) const;

    //!
    //! Serialization helper: Terminate a binary descriptor.
    //! @param [out] descriptor The serialized descriptor. Set only if the
    //! binary descriptor is valid.
    //! @param [in,out] data A byte block which received the descriptor data.
    //! The descriptor_length field is updated.
    //! @return True if the descriptor is valid, false otherwise.
    //! When false is returned, @a descriptor is invalidated.
    //!
    bool serializeEnd(QtsDescriptor& descriptor, QtlByteBlock& data) const;

    //!
    //! Deserialization helper: Start the deserialization.
    //! This object is marked as invalid. The deserialize() method shall set
    //! it to true later if the serialization succeeds.
    //! @param [in] descriptor Binary descriptor to deserialize.
    //! @param [out] payload Receive the address of the descriptor payload.
    //! @param [out] payloadSize Receive the descriptor payload size.
    //! @return True if the input is valid and deserialization may continue.
    //! False if the binary descriptor is invalid or has the wrong tag.
    //!
    bool deserializeInit(const QtsDescriptor& descriptor, const quint8*& payload, int& payloadSize);

private:
    // Unaccessible operations.
    QtsAbstractDescriptor() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtsAbstractDescriptor)
};

#endif // QTSABSTRACTDESCRIPTOR_H
