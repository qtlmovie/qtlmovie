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
//! @file QtsDescriptor.h
//!
//! Declare the class QtsDescriptor.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDESCRIPTOR_H
#define QTSDESCRIPTOR_H

#include "QtsCore.h"
#include "QtlByteBlock.h"
#include "QtlSmartPointer.h"

//!
//! Representation of a binary MPEG PSI/SI descriptor.
//!
//! Data from descriptors are not deeply copied thanks to the implicit
//! sharing mechanism of the Qt containers. Whenever a descriptor is
//! modified, then a copy-on-write occurs.
//!
class QtsDescriptor
{
public:
    //!
    //! Default constructor.
    //!
    QtsDescriptor() :
        _data()
    {
    }

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy.
    //!
    QtsDescriptor(const QtsDescriptor& other) :
        _data(other._data)
    {
    }

    //!
    //! Constructor from full binary content.
    //! The content is copied into the descriptor if valid.
    //! @param [in] addr Content address.
    //! @param [in] size Content size.
    //!
    QtsDescriptor(const void* addr, int size);

    //!
    //! Constructor from full binary content.
    //! The content is copied into the descriptor if valid.
    //! @param [in] content Binary content.
    //!
    QtsDescriptor(const QtlByteBlock& content);

    //!
    //! Assignment operator.
    //! @param [in] other Other instance to copy.
    //!
    const QtsDescriptor& operator=(const QtsDescriptor& other);

    //!
    //! Check if a descriptor has valid content.
    //! @return True if a descriptor has valid content.
    //!
    bool isValid() const;

    //!
    //! Invalidate the descriptor content.
    //!
    void invalidate()
    {
        _data.clear();
    }

    //!
    //! Get the descriptor tag.
    //! @return The descriptor tag or zero (reserved value) if invalid.
    //!
    QtsDescTag tag() const
    {
        return _data.isEmpty() ? 0 : _data[0];
    }

    //!
    //! Get the address of the full binary content of the descriptor.
    //! @return The address of the full binary content of the descriptor or zero if the descriptor is invalid.
    //! Do not modify the content. May be invalidated after modification in the descriptor.
    //!
    const quint8* content() const
    {
        return _data.isEmpty() ? 0 : _data.data();
    }

    //!
    //! Get the size of the full binary content of the descriptor.
    //! @return The size of the full binary content of the descriptor.
    //!
    int size() const
    {
        return _data.size();
    }

    //!
    //! Get the address of the descriptor payload.
    //! @return The address of the descriptor payload or zero if the descriptor is invalid.
    //! Do not modify the content. May be invalidated after modification in the descriptor.
    //!
    const quint8* payload() const
    {
        return _data.isEmpty() ? 0 : _data.data() + 2;
    }

    //!
    //! Get the address of the descriptor payload for payload update.
    //! @return The address of the descriptor payload or zero if the descriptor is invalid.
    //! May be invalidated after modification in the descriptor.
    //!
    quint8* payload()
    {
        return _data.isEmpty() ? 0 : _data.data() + 2;
    }

    //!
    //! Get the size of the descriptor payload.
    //! @return The size of the descriptor payload.
    //!
    int payloadSize() const
    {
        return _data.isEmpty() ? 0 : _data.size() - 2;
    }

    //!
    //! Replace the payload of the descriptor.
    //! The tag is unchanged, the size is adjusted.
    //! @param [in] addr New payload content address.
    //! @param [in] size New payload content size.
    //!
    void replacePayload(const void* addr, int size);

    //!
    //! Replace the payload of the descriptor.
    //! The tag is unchanged, the size is adjusted.
    //! @param [in] payload New payload content.
    //!
    void replacePayload(const QtlByteBlock& payload)
    {
        replacePayload (payload.data(), payload.size());
    }

    //!
    //! Resize (truncate or extend) the payload of the descriptor.
    //! The tag is unchanged, the size is adjusted.
    //! If the payload is extended, new bytes are zeroes.
    //! @param [in] size New payload size.
    //!
    void resizePayload(int size);

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if the descriptos are identical, false otherwise.
    //!
    bool operator==(const QtsDescriptor& other) const
    {
        return _data == other._data;
    }

    //!
    //! Difference operator.
    //! @param [in] other Other instance to compare.
    //! @return True if the descriptos are different, false otherwise.
    //!
    bool operator!=(const QtsDescriptor& other) const
    {
        return _data != other._data;
    }

private:
    QtlByteBlock _data; //!< Full binary content of the descriptor.
};

//!
//! Smart pointer to a QtsDescriptor (non thread-safe).
//!
typedef QtlSmartPointer<QtsDescriptor,QtlNullMutexLocker> QtsDescriptorPtr;

//!
//! Smart pointer to a QtsDescriptor (thread-safe).
//!
typedef QtlSmartPointer<QtsDescriptor,QtlMutexLocker> QtsDescriptorSafePtr;

#endif // QTSDESCRIPTOR_H
