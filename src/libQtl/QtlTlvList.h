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
//! @file QtlTlvList.h
//!
//! Declare the template class QtlTlvList.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLTLVLIST_H
#define QTLTLVLIST_H

#include "QtlTlv.h"

//!
//! A template class for lists of TLV items (Tag / Length / Value).
//!
//! @tparam TAG An integer type representing tag fields.
//! @tparam LENGTH An integer type representing length fields.
//! @tparam ORDER Byte order to use for serialization.
//!
template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
class QtlTlvList: public QList< QtlTlv<TAG,LENGTH,ORDER> >
{
public:
    //!
    //! Type name for a TLV item in the list.
    //!
    typedef QtlTlv<TAG,LENGTH,ORDER> Tlv;

    //!
    //! Type name for the superclass.
    //!
    typedef QList<Tlv> SuperClass;

    //!
    //! Default constructor.
    //!
    QtlTlvList() :
        SuperClass(),
        _tagMap()
    {
    }

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy.
    //!
    QtlTlvList(const QtlTlvList& other) :
        SuperClass(other),
        _tagMap(other._tagMap)
    {
    }

    //!
    //! Serialize the TLV list at end of a given byte block.
    //! @param [in,out] data The byte block to use. Its size is increased to include the TLV list.
    //!
    void appendTo(QtlByteBlock& data) const;

    //!
    //! Deserialize the TLV list from a given byte block.
    //! The deserialized TLV items are @em appended to this object.
    //! @param [in] data The byte block to use.
    //! @param [in,out] index Index in the byte block where to read the TLV list.
    //! @a index is updated to point after the read TLV list.
    //! @param [in] end Next index after last byte to read. If negative, use the end of byte block.
    //! @return True on success, false on error (incorrect data, block too short). In case of
    //! error, some TLV items may have been read into this object and @a index point after the last
    //! successfully read TLV item.
    //!
    bool readAt(const QtlByteBlock& data, int& index, int end = -1);

    //!
    //! Add a tag definition for the list.
    //! @see validate()
    //! @param [in] tag The tag value.
    //! @param [in] minCount Minimum number of items with this tag.
    //! @param [in] maxCount Maximum number of items with this tag.
    //! @param [in] minSize Minimum size in bytes of the value of items with this tag.
    //! @param [in] maxSize Maximum size in bytes of the value of items with this tag.
    //!
    void addTagDefinition(TAG tag, int minCount, int maxCount, int minSize, int maxSize)
    {
        _tagMap.insert(tag, TagDefinition(minCount, maxCount, minSize, maxSize));
    }

    //!
    //! Clear the list of tag definitions.
    //!
    void clearTagDefinitions()
    {
        _tagMap.clear();
    }

    //!
    //! Get the number of tag definitions.
    //! @return The number of tag definitions.
    //!
    int tagDefinitionsCount() const
    {
        return _tagMap.size();
    }

    //!
    //! Validate the TLV list according to its tags definitions.
    //! @return True if the list matches the tag definitions, false otherwise.
    //!
    bool validate() const;

private:
    //!
    //! Internal structure to hold a tag definition.
    //!
    class TagDefinition
    {
    public:
        int minCount; //!< Minimum number of items with this tag.
        int maxCount; //!< Maximum number of items with this tag.
        int minSize;  //!< Minimum size in bytes of the value of items with this tag.
        int maxSize;  //!< Maximum size in bytes of the value of items with this tag.

        //!
        //! Constructor.
        //! @param [in] minCount_ Minimum number of items with this tag.
        //! @param [in] maxCount_ Maximum number of items with this tag.
        //! @param [in] minSize_ Minimum size in bytes of the value of items with this tag.
        //! @param [in] maxSize_ Maximum size in bytes of the value of items with this tag.
        //!
        TagDefinition(int minCount_ = 0, int maxCount_ = 0, int minSize_ = 0, int maxSize_ = 0) :
            minCount(minCount_),
            maxCount(maxCount_),
            minSize(minSize_),
            maxSize(maxSize_)
        {
        }
    };

    //!
    //! A map of tag definitions, index by tag value.
    //!
    typedef QMap<TAG,TagDefinition> TagDefinitionMap;

    TagDefinitionMap _tagMap; //!< The map of tag definitions.
};

#include "QtlTlvListTemplate.h"
#endif // QTLTLVLIST_H
