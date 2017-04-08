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
//
// Qts, the Qt MPEG Transport Stream library.
// Define the class QtsDescriptorList.
//
//----------------------------------------------------------------------------

#include "QtsDescriptorList.h"
#include "QtsAbstractDescriptor.h"
#include "QtsPrivateDataSpecifierDescriptor.h"


//-----------------------------------------------------------------------------
// Comparison operator
//-----------------------------------------------------------------------------

bool QtsDescriptorList::operator==(const QtsDescriptorList& other) const
{
    if (_list.size() != other._list.size()) {
        return false;
    }
    for (int i = 0; i < _list.size(); ++i) {
        const QtsDescriptorPtr& desc1(_list[i].desc);
        const QtsDescriptorPtr& desc2(other._list[i].desc);
        if (desc1.isNull() || desc2.isNull() || *desc1 != *desc2) {
            return false;
        }
    }
    return true;
}


//-----------------------------------------------------------------------------
// Append one descriptor at end of list
//-----------------------------------------------------------------------------

bool QtsDescriptorList::append(const QtsDescriptorPtr& desc)
{
    // Filter invalid descriptors.
    if (desc.isNull() || !desc->isValid()) {
        return false;
    }

    // Determine which PDS to associate with the descriptor
    QtsPrivDataSpec pds;
    if (desc->tag() == QTS_DID_PRIV_DATA_SPECIF) {
        // This descriptor defines a new "private data specifier".
        // The PDS is the only thing in the descriptor payload.
        pds = desc->payloadSize() < 4 ? 0 : qFromBigEndian<quint32>(desc->payload());
    }
    else if (_list.empty()) {
        // First descriptor in the list
        pds = 0;
    }
    else {
        // Use same PDS as previous descriptor
        pds = _list.last().pds;
    }

    // Add the descriptor in the list
    _list.append(Element(desc, pds));
    return true;
}


//-----------------------------------------------------------------------------
// Append one descriptor at end of list
//-----------------------------------------------------------------------------

bool QtsDescriptorList::append(const QtsAbstractDescriptor& desc)
{
    if (!desc.isValid()) {
        return false;
    }
    QtsDescriptorPtr pd(new QtsDescriptor);
    if (pd.isNull()) {
        return false;
    }
    desc.serialize(*pd);
    return append(pd);
}


//-----------------------------------------------------------------------------
// Append descriptors from a memory area
//-----------------------------------------------------------------------------

bool QtsDescriptorList::append(const void* addr, int size)
{
    const quint8* desc = reinterpret_cast<const quint8*>(addr);
    int length;
    while (size >= 2 && (length = int(desc[1]) + 2) <= size) {
        if (!append(QtsDescriptorPtr(new QtsDescriptor(desc, length)))) {
            return false;
        }
        desc += length;
        size -= length;
    }
    // Success if all memory was consumed.
    return size == 0;
}


//-----------------------------------------------------------------------------
// Same as append(const void*,int), but read a 2-byte length field.
//-----------------------------------------------------------------------------

bool QtsDescriptorList::appendFromLength(const quint8*& addr, int& size)
{
    // Check that the 2-byte length fields can be read.
    if (size < 2) {
        return false;
    }

    // Get the length field (12 significant bits).
    const int listSize = qFromBigEndian<quint16>(addr) & 0x0FFF;
    if (2 + listSize > size) {
        // Cannot read the complete list.
        return false;
    }

    // Read the descriptor list.
    const quint8* listAddr = addr + 2;
    addr += 2 + listSize;
    size -= 2 + listSize;
    return append(listAddr, listSize);
}


//-----------------------------------------------------------------------------
// Add a private_data_specifier if necessary at end of list
//-----------------------------------------------------------------------------

void QtsDescriptorList::appendPrivateDataSpecifier(QtsPrivDataSpec pds)
{
    if (pds != 0 && (_list.isEmpty() || _list.last().pds != pds)) {
        append(QtsPrivateDataSpecifierDescriptor(pds));
    }
}


//-----------------------------------------------------------------------------
// Remove all private descriptors without preceding
// private_data_specifier_descriptor.
// Return the number of removed descriptors.
//-----------------------------------------------------------------------------

int QtsDescriptorList::removeInvalidPrivateDescriptors()
{
    int count = 0;
    for (int n = 0; n < _list.size(); n++) {
        if (_list[n].pds == 0 && !_list[n].desc.isNull() && _list[n].desc->isValid() && _list[n].desc->tag() >= 0x80) {
            _list.removeAt(n);
            count++;
        }
    }
    return count;
}


//-----------------------------------------------------------------------------
// Prepare the removal of a private_data_specifier descriptor.
//-----------------------------------------------------------------------------

bool QtsDescriptorList::prepareRemovePds(int index)
{
    // Eliminate invalid cases
    if (index < 0 || index >= _list.size() || _list[index].desc->tag() != QTS_DID_PRIV_DATA_SPECIF) {
        return false;
    }

    // Search for private descriptors ahead.
    int end;
    for (end = index + 1; end < _list.size(); ++end) {
        QtsDescTag tag = _list[end].desc->tag();
        if (tag >= 0x80) {
            // This is a private descriptor, the private_data_specifier descriptor
            // is necessary and cannot be removed.
            return false;
        }
        if (tag == QTS_DID_PRIV_DATA_SPECIF) {
            // Found another private_data_specifier descriptor with no private
            // descriptor between the two => the first one can be removed.
            break;
        }
    }

    // Update the current PDS after removed private_data_specifier descriptor
    QtsPrivDataSpec previousPds = index == 0 ? 0 : _list[index-1].pds;
    while (--end > index) {
        _list[end].pds = previousPds;
    }

    return true;
}


//-----------------------------------------------------------------------------
// Remove the descriptor at the specified index in the list.
//-----------------------------------------------------------------------------

bool QtsDescriptorList::removeByIndex(int index)
{
    // Check index validity
    if (index < 0 || index >= _list.size()) {
        return false;
    }

    // Private_data_specifier descriptor can be removed under certain conditions only
    if (_list[index].desc->tag() == QTS_DID_PRIV_DATA_SPECIF && !prepareRemovePds(index)) {
        return false;
    }

    // Remove the specified descriptor
    _list.removeAt(index);
    return true;
}


//-----------------------------------------------------------------------------
// Remove all descriptors with the specified tag.
//-----------------------------------------------------------------------------

int QtsDescriptorList::removeByTag(QtsDescTag tag, QtsPrivDataSpec pds)
{
    const bool checkPds = pds != 0 && tag >= 0x80;
    int removedCount = 0;

    for (int i = 0; i < _list.size(); ) {
        const QtsDescTag itag = _list[i].desc->tag();
        if (itag == tag && (!checkPds || _list[i].pds == pds) && (itag != QTS_DID_PRIV_DATA_SPECIF || prepareRemovePds(i))) {
            _list.removeAt(i);
            ++removedCount;
        }
        else {
            ++i;
        }
    }

    return removedCount;
}


//-----------------------------------------------------------------------------
// Total number of bytes that is required to serialize the list of descriptors.
//-----------------------------------------------------------------------------

int QtsDescriptorList::binarySize() const
{
    int size = 0;
    for (int i = 0; i < _list.size(); ++i) {
        size += _list[i].desc->size();
    }
    return size;
}


//-----------------------------------------------------------------------------
// Serialize the content of the descriptor list.
//-----------------------------------------------------------------------------

int QtsDescriptorList::serialize(quint8*& addr, int& size, int startIndex) const
{
    int i;
    for (i = startIndex; i < _list.size() && _list[i].desc->size() <= size; ++i) {
        ::memcpy(addr, _list[i].desc->content(), _list[i].desc->size());
        addr += _list[i].desc->size();
        size -= _list[i].desc->size();
    }
    return i;
}


//-----------------------------------------------------------------------------
// Serialize the content of the descriptor list.
//-----------------------------------------------------------------------------

int QtsDescriptorList::serialize(QtlByteBlock& data, int maxDataSize, int startIndex) const
{
    // If the data block is already too big, cannot serialize anything.
    const int oldSize = data.size();
    if (oldSize >= maxDataSize) {
        return startIndex;
    }

    // Resize the buffer to maximum allowed.
    data.resize(maxDataSize);

    // Serialize the descriptor list in the enlarged area.
    quint8* addr = data.data() + oldSize;
    int size = maxDataSize - oldSize;
    const int nextIndex = serialize(addr, size, startIndex);

    // Shrink the data block after the last written byte.
    Q_ASSERT(size >= 0);
    data.resize(oldSize + size);
    return nextIndex;
}


//-----------------------------------------------------------------------------
// Same as serialize, but prepend a 2-byte length field.
//-----------------------------------------------------------------------------

int QtsDescriptorList::lengthSerialize(quint8*& addr, int& size, int startIndex) const
{
    // Check that we can at least serialize the length field.
    if (size < 2) {
        return startIndex;
    }

    // Reserve space for descriptor list length
    quint8* lengthAddr = addr;
    addr += 2;
    size -= 2;

    // Insert descriptor list
    const int result = serialize(addr, size, startIndex);

    // Update length
    int length = addr - lengthAddr - 2;
    qToBigEndian<quint16>(quint16(length | 0xF000), lengthAddr);

    return result;
}


//-----------------------------------------------------------------------------
// Same as serialize, but prepend a 2-byte length field.
//-----------------------------------------------------------------------------

int QtsDescriptorList::lengthSerialize(QtlByteBlock& data, int maxDataSize, int startIndex) const
{
    // If the data block is already too big, cannot serialize anything.
    const int oldSize = data.size();
    if (oldSize + 2 >= maxDataSize) {
        return startIndex;
    }

    // Resize the buffer to maximum allowed.
    data.resize(maxDataSize);

    // Serialize the descriptor list in the enlarged area.
    quint8* addr = data.data() + oldSize;
    int size = maxDataSize - oldSize;
    const int nextIndex = lengthSerialize(addr, size, startIndex);

    // Shrink the data block after the last written byte.
    Q_ASSERT(size >= 0);
    data.resize(oldSize + size);
    return nextIndex;
}


//-----------------------------------------------------------------------------
// Search a descriptor with the specified tag, starting at the
// specified index.
//-----------------------------------------------------------------------------

int QtsDescriptorList::search(QtsDescTag tag, int startIndex, QtsPrivDataSpec pds) const
{
    bool checkPds = pds != 0 && tag >= 0x80;
    int index = startIndex;
    while (index < _list.size() && (_list[index].desc->tag() != tag || (checkPds && _list[index].pds != pds))) {
        index++;
    }
    return index;
}
