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
//! @file QtlTlv.h
//!
//! Declare the template class QtlTlv.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLTLV_H
#define QTLTLV_H

#include "QtlByteBlock.h"
#include <limits>

//!
//! A template class for TLV items (Tag / Length / Value).
//! @tparam TAG An integer type representing tag fields.
//! @tparam LENGTH An integer type representing length fields.
//!
template<typename TAG, typename LENGTH>
class QtlTlv
{
public:

    //!
    //! Default constructor.
    //! @param [in] order Byte order to use for serialization.
    //! @param [in] tag Tag value.
    //! @param [in] value Binary data.
    //!
    explicit QtlTlv(QtlByteBlock::ByteOrder order = QtlByteBlock::BigEndian, TAG tag = 0, const QtlByteBlock& value = QtlByteBlock()) :
        _order(order),
        _tag(tag),
        _value()
    {
        setValue(value);
    }

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy.
    //!
    QtlTlv(const QtlTlv& other) :
        _order(other._order),
        _tag(other._tag),
        _value(other._value)
    {
    }

    //!
    //! Assignment operator.
    //! @param [in] other Other instance to copy.
    //! @return A reference to this object.
    //!
    const QtlTlv& operator=(const QtlTlv& other)
    {
        if (&other != this) {
            _order = other._order;
            _tag = other._tag;
            _value = other._value;
        }
        return *this;
    }

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True is the two objects are identical, false otherwise.
    //!
    bool operator==(const QtlTlv& other) const
    {
        return _order == other._order && _tag == other._tag && _value == other._value;
    }

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True is the two objects are different, false otherwise.
    //!
    bool operator!=(const QtlTlv& other) const
    {
        return !operator==(other);
    }

    //!
    //! Get the byte order for serialization.
    //! @return The byte order for serialization.
    //!
    QtlByteBlock::ByteOrder byteOrder() const
    {
        return _order;
    }

    //!
    //! Set the byte order for serialization.
    //! @param [in] order The byte order for serialization.
    //!
    void setByteOrder(const QtlByteBlock::ByteOrder& order)
    {
        _order = order;
    }

    //!
    //! Get the tag.
    //! @return The tag.
    //!
    TAG tag() const
    {
        return _tag;
    }

    //!
    //! Set the tag.
    //! @param [in] tag The tag.
    //!
    void setTag(const TAG& tag)
    {
        _tag = tag;
    }

    //!
    //! Get the binary value.
    //! @return The binary value.
    //!
    const QtlByteBlock& value() const
    {
        return _value;
    }

    //!
    //! Get the size of binary value.
    //! @return The size of binary value.
    //!
    int valueSize() const
    {
        return _value.size();
    }

    //!
    //! Set the binary value.
    //! @param [in] value The binary value.
    //!
    void setValue(const QtlByteBlock& value);

    //!
    //! Set the value as an integer.
    //! The binary content is set according to the size of @a INT and the byte order of this instance.
    //! @tparam INT The integer type of the value.
    //! @param [in] value The integer value.
    //!
    template<typename INT>
    void setIntValue(const INT& value)
    {
        _value.clear();
        _value.appendToByteOrder<INT>(value, _order);
    }

    //!
    //! Get the value as an integer.
    //! The binary content is deserialized according to the size of @a INT and the byte order of this instance.
    //! @tparam INT The integer type of the value.
    //! @param [out] value The integer value.
    //! @return True if the binary content has the right size, false otherwise.
    //!
    template<typename INT>
    bool getIntValue(INT& value) const
    {
        int index = 0;
        return _value.size() == sizeof(INT) && _value.fromByteOrder<INT>(index, value, _order);
    }

    //!
    //! Get the value as an integer.
    //! The binary content is deserialized according to the size of @a INT and the byte order of this instance.
    //! @tparam INT The integer type of the value.
    //! @return The integer value or zero if the binary content has the wrong size.
    //!
    template<typename INT>
    INT getIntValue() const
    {
        return _value.size() == sizeof(INT) ? _value.fromByteOrder<INT>(0, _order) : 0;
    }

    //!
    //! Serialize the TLV at end of a given byte block.
    //! @param [in,out] data The byte block to use. Its size is increased to include the TLV.
    //!
    void appendTo(QtlByteBlock& data) const;

    //!
    //! Deserialize the TLV from a given byte block.
    //! @param [in] data The byte block to use.
    //! @param [in,out] index Index in the byte block where to read the TLV.
    //! If the TLV is correctly read, @a index is updated to point after the read TLV.
    //! @param [in] end Next index after last byte to read. If negative, use the end of byte block.
    //! @return True on success, false on error (incorrect data, block too short).
    //!
    bool readAt(const QtlByteBlock& data, int& index, int end = -1);

private:
    QtlByteBlock::ByteOrder _order;  //!< Byte order for serialization.
    TAG                     _tag;    //!< Tag.
    QtlByteBlock            _value;  //!< Binary value.

    //!
    //! Get the maximal value length as an @c int type.
    //! @return The maximal value length as an @c int type.
    //!
    static int intLengthMax()
    {
        return sizeof(LENGTH) < sizeof(int) ?
            static_cast<int>(std::numeric_limits<LENGTH>::max()) :
            std::numeric_limits<int>::max();
    }

    //!
    //! Get the maximal value length as a @c LENGTH type.
    //! @return The maximal value length as a @c LENGTH type.
    //!
    static LENGTH lengthMax()
    {
        return sizeof(LENGTH) >= sizeof(int) ?
            static_cast<LENGTH>(std::numeric_limits<int>::max()) :
            std::numeric_limits<LENGTH>::max();
    }
};

#include "QtlTlvTemplate.h"
#endif // QTLTLV_H
