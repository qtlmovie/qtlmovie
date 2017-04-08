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
//! @file QtsDescriptorList.h
//!
//! Declare the class QtsDescriptorList.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDESCRIPTORLIST_H
#define QTSDESCRIPTORLIST_H

#include "QtsDescriptor.h"
class QtsAbstractDescriptor;

//!
//! List of MPEG PSI/SI descriptors.
//!
class QtsDescriptorList
{
public:
    //!
    //! Default constructor: empty list.
    //!
    QtsDescriptorList() :
        _list()
    {
    }

    //!
    //! Copy constructor: the descriptors objects are shared between the two lists.
    //! @param [in] other Other instance to copy.
    //!
    QtsDescriptorList(const QtsDescriptorList& other) :
        _list(other._list)
    {
    }

    //!
    //! Get the number of descriptors in the list.
    //! @return The number of descriptors in the list.
    //!
    int size() const
    {
        return _list.size();
    }

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to copy.
    //! @return True if the two lists are identical.
    //!
    bool operator==(const QtsDescriptorList& other) const;

    //!
    //! Difference operator.
    //! @param [in] other Other instance to copy.
    //! @return True if the two lists are different.
    //!
    bool operator!=(const QtsDescriptorList& other) const
    {
        return !(*this == other);
    }

    //!
    //! Return a reference to the descriptor at specified index.
    //! @param [in] index Index of descriptor in the list.
    //! Valid index are 0 to size()-1. Undefined behavior otherwise.
    //! @return A reference to the descriptor at specified index.
    //!
    const QtsDescriptorPtr& operator[](int index) const
    {
        Q_ASSERT(index >= 0);
        Q_ASSERT(index < _list.size());
        return _list[index].desc;
    }

    //!
    //! Return the "private data specifier" associated to the descriptor at the specified index.
    //! @param [in] index Index of descriptor in the list.
    //! Valid index are 0 to size()-1. Undefined behavior otherwise.
    //! @return The private data specified for the descriptor or zero if there is none.
    //!
    QtsPrivDataSpec privateDataSpecifier(int index) const
    {
        Q_ASSERT(index >= 0);
        Q_ASSERT(index < _list.size());
        return _list[index].pds;
    }

    //!
    //! Append one descriptor at end of list.
    //! @param [in] desc A smart pointer to a binary descriptor.
    //! The pointed binary descriptor is shared with the list.
    //! @return True on success, false on error (invalid descriptor, etc.)
    //!
    bool append(const QtsDescriptorPtr& desc);

    //!
    //! Append one descriptor at end of list.
    //! @param [in] desc A descriptor. A binary copy of the descriptor is stored in the list.
    //! @return True on success, false on error (invalid descriptor, etc.)
    //!
    bool append(const QtsAbstractDescriptor& desc);

    //!
    //! Append another list of descriptors at end of list.
    //! The descriptors objects are shared between the two lists.
    //! @param [in] list A list of descriptors to append.
    //!
    void append(const QtsDescriptorList& list)
    {
        _list.append(list._list);
    }

    //!
    //! Append all descriptors from a memory area at end of list.
    //! Stop when an invalid descriptor is encountered.
    //! @param [in] addr Address of memory area.
    //! @param [in] size Size in bytes of memory area.
    //! @return True on success, false on error (invalid descriptor, etc.)
    //!
    bool append(const void* addr, int size);

    //!
    //! Append all descriptors from a memory area at end of list.
    //! Same as append(const void*,int), but read a 2-byte length field before the descriptor list.
    //! The 2-byte length field has 4 reserved bits (all '1') and 12 bits for the length
    //! of the descriptor list.
    //! @param [in,out] addr Address where to start reading the list.
    //! @a addr is updated to point after the last deserialized descriptor.
    //! @param [in,out] size Size of the memory area from which to read descriptors.
    //! @a size is updated with the remaining space after the last deserialized descriptor.
    //! @return True if a complete descriptor list was read or true if the designated
    //! area was too small.
    //!
    bool appendFromLength(const quint8*& addr, int& size);

    //!
    //! Append one descriptor from a memory area at end of list.
    //! The size is extracted from the descriptor header.
    //! @param [in] addr Address of memory area.
    //! @return True on success, false on error (invalid descriptor, etc.)
    //!
    bool append(const void* addr)
    {
        const quint8* data = reinterpret_cast<const quint8*>(addr);
        return append(data, int(data[1]) + 2);
    }

    //!
    //! Append a private_data_specifier if necessary at end of list.
    //! If the current private data specified at end of list is
    //! already @a pds, do nothing.
    //! @param [in] pds Private data descriptor value.
    //!
    void appendPrivateDataSpecifier(QtsPrivDataSpec pds);

    //!
    //! Remove the descriptor at the specified index in the list.
    //! A private_data_specifier descriptor can be removed only if
    //! it is not necessary (no private descriptor ahead).
    //! @param [in] index Index of descriptor in the list.
    //! @return True on success, false on error (index out of range
    //! or required private_data_specifier descriptor).
    //!
    bool removeByIndex(int index);

    //!
    //! Remove all descriptors with the specified tag.
    //! If @a pds is non-zero and @a tag is >= 0x80, remove only
    //! descriptors with the corresponding "private data specifier".
    //! A private_data_specifier descriptor can be removed only if
    //! it is not necessary (no private descriptor ahead).
    //! @param [in] tag Tag of descriptors to remove.
    //! @param [in] pds Optional associated private data descriptor.
    //! @return The number of removed descriptors.
    //!
    int removeByTag(QtsDescTag tag, QtsPrivDataSpec pds = 0);

    //!
    //! Remove all private descriptors without preceding private_data_specifier_descriptor.
    //! @return The number of removed descriptors.
    //!
    int removeInvalidPrivateDescriptors();

    //!
    //! Clear the content of the descriptor list.
    //!
    void clear()
    {
        _list.clear();
    }

    //!
    //! Search a descriptor with the specified tag.
    //! If @a pds is non-zero and @a tag is >= 0x80, search only
    //! descriptors with the corresponding "private data specifier".
    //! @param [in] tag Tag of descriptor to search.
    //! @param [in] startIndex Start searching at that index in the list.
    //! @param [in] pds Optional associated private data descriptor.
    //! @return The the index of the descriptor in the list
    //! or size() if no such descriptor is found.
    //!
    int search (QtsDescTag tag, int startIndex = 0, QtsPrivDataSpec pds = 0) const;

    //!
    //! Evaluate the total number of bytes that is required to serialize the list of descriptors.
    //!
    int binarySize() const;

    //!
    //! Serialize the content of the descriptor list.
    //! Descriptors are written one by one until either the end
    //! of the list or until one descriptor does not fit in the
    //! memory area.
    //! @param [in,out] addr Address where to start writing descriptors.
    //! @a addr is updated to point after the last written descriptor.
    //! @param [in,out] size Size of the memory area to write descriptors.
    //! @a size is updated with the remaining free space after the list.
    //! @param [in] startIndex Start serializing at that index in the list.
    //! @return The index of the first descriptor that could not be serialized
    //! or size() if all descriptors were serialized. In the first case, the
    //! returned index can be used as @a startIndex to serialize the rest of
    //! the list, in another section for instance.
    //!
    int serialize(quint8*& addr, int& size, int startIndex = 0) const;

    //!
    //! Serialize the content of the descriptor list.
    //! Descriptors are written one by one until either the end
    //! of the list or until one descriptor does not fit in the
    //! byte block.
    //! @param [in,out] data Byte block into which the descriptor list is appended.
    //! @param [in] maxDataSize Maximum size of the byte block to write descriptors.
    //! @param [in] startIndex Start serializing at that index in the list.
    //! @return The index of the first descriptor that could not be serialized
    //! or size() if all descriptors were serialized. In the first case, the
    //! returned index can be used as @a startIndex to serialize the rest of
    //! the list, in another section for instance.
    //!
    int serialize(QtlByteBlock& data, int maxDataSize, int startIndex = 0) const;

    //!
    //! Serialize the content of the descriptor list.
    //! Same as serialize(), but prepend a 2-byte length field
    //! before the descriptor list. The 2-byte length field
    //! has 4 reserved bits (all '1') and 12 bits for the length
    //! of the descriptor list.
    //! @param [in,out] addr Address where to start writing descriptors.
    //! @a addr is updated to point after the last written descriptor.
    //! @param [in,out] size Size of the memory area to write descriptors.
    //! @a size is updated with the remaining free space after the list.
    //! @param [in] startIndex Start serializing at that index in the list.
    //! @return The index of the first descriptor that could not be serialized
    //! or size() if all descriptors were serialized. In the first case, the
    //! returned index can be used as @a startIndex to serialize the rest of
    //! the list, in another section for instance.
    //!
    int lengthSerialize(quint8*& addr, int& size, int startIndex = 0) const;

    //!
    //! Serialize the content of the descriptor list.
    //! Same as serialize(), but prepend a 2-byte length field
    //! before the descriptor list. The 2-byte length field
    //! has 4 reserved bits (all '1') and 12 bits for the length
    //! of the descriptor list.
    //! @param [in,out] data Byte block into which the descriptor list is appended.
    //! @param [in] maxDataSize Maximum size of the byte block to write descriptors.
    //! @param [in] startIndex Start serializing at that index in the list.
    //! @return The index of the first descriptor that could not be serialized
    //! or size() if all descriptors were serialized. In the first case, the
    //! returned index can be used as @a startIndex to serialize the rest of
    //! the list, in another section for instance.
    //!
    int lengthSerialize(QtlByteBlock& data, int maxDataSize, int startIndex = 0) const;

private:
    //!
    //! Each entry contains a descriptor and its corresponding private data specifier.
    //!
    struct Element
    {
        QtsDescriptorPtr desc; //!< Smart pointer to the descriptor.
        QtsPrivDataSpec  pds;  //!< Current private data specifier in the list.
        //!
        //! Default constructor.
        //! @param [in] d Optional descriptor pointer.
        //! @param [in] p Optional private data specifier.
        //!
        Element(const QtsDescriptorPtr& d = 0, QtsPrivDataSpec p = 0) :
            desc(d),
            pds(p)
        {
        }
    };

    //!
    //! List of elements.
    //!
    typedef QList<Element> ElementList;

    //!
    //! The only data field is a list of elements.
    //!
    ElementList _list;

    //!
    //! Prepare the removal of a private_data_specifier descriptor.
    //! Return true if can be removed, false if it cannot (private descriptors ahead).
    //! When it can be removed, the current PDS of all subsequent descriptors is updated.
    //! @param [in] index Index in the list of the private_data_specifier descriptor
    //! is about to be removed.
    //!
    bool prepareRemovePds(int index);
};

#endif // QTSDESCRIPTORLIST_H
