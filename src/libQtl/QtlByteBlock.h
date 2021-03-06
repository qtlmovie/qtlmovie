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
// CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//!
//! @file QtlByteBlock.h
//!
//! Declare the class QtlByteBlock.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLBYTEBLOCK_H
#define QTLBYTEBLOCK_H

#include "QtlCore.h"

//!
//! Definitions of a generic block of bytes.
//!
class QtlByteBlock : public QVector<quint8>
{
public:
    //!
    //! Type definition for the superclass, a Qt vector of bytes.
    //!
    typedef QVector<quint8> ByteVector;

    //!
    //! Byte order for integer serializing and deserializing.
    //!
    enum ByteOrder {
        BigEndian    = QSysInfo::BigEndian,    //!< Most significant byte first.
        LittleEndian = QSysInfo::LittleEndian  //!< Least significant byte first.
    };

    //!
    //! Default constructor
    //! @param [in] size Size of the byte block.
    //! @param [in] value Value to assign to each byte.
    //!
    explicit QtlByteBlock(int size = 0, quint8 value = 0) :
        ByteVector(size, value)
    {
    }

    //!
    //! Copy constructor
    //! @param [in] other Other instance to copy.
    //!
    QtlByteBlock(const ByteVector& other) :
        ByteVector(other)
    {
    }

    //!
    //! Constructor from a data block.
    //! @param [in] addr Address of the data block.
    //! @param [in] size Size of the data block.
    //!
    QtlByteBlock(const void* addr, int size) :
        ByteVector(size)
    {
        if (size > 0) {
            ::memcpy(data(), addr, size);
        }
    }

    //!
    //! Constructor from a QByteArray.
    //! @param [in] ba Byte array to copy.
    //!
    QtlByteBlock(const QByteArray& ba) :
        ByteVector(ba.size())
    {
        if (size() > 0) {
            ::memcpy(data(), ba.data(), size());
        }
    }

    //!
    //! Constructor from a C string.
    //! @param [in] str A nul-terminated string.
    //!
    QtlByteBlock(const char* str) :
        ByteVector(::strlen(str))
    {
        if (size() > 0) {
            ::memcpy(data(), str, size());
        }
    }

    //!
    //! Convert a QtlByteBlock into a QByteArray.
    //! @return A byte array with the same content as this object.
    //!
    QByteArray toByteArray() const
    {
        return QByteArray(reinterpret_cast<const char*>(data()), size());
    }

    //!
    //! Replace the content with a data block.
    //! @param [in] addr Address of the data block.
    //! @param [in] size Size of the data block.
    //!
    void copy(const void* addr, int size);

    //!
    //! Increase size by @a n and return pointer to new n-byte area at end of block.
    //! @param [in] n Number of bytes to add.
    //! @return Starting address of enlarged area.
    //!
    void* enlarge(int n);

    //!
    //! Make sure there are at @n bytes available at the specified @a index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index of area. If negative, append at end of data block.
    //! @param [in] n Number of bytes to make available.
    //! @return Starting address at @a index or a null pointer if @a index is negative.
    //!
    void* enlargeFrom(int index, int n);

    //!
    //! Append raw data to a byte block.
    //! @param [in] addr Address of the data block.
    //! @param [in] size Size of the data block.
    //!
    void append(const void* addr, int size);

    //!
    //! Append a byte block to a byte block.
    //! @param [in] bb Byte block to append.
    //!
    void append(const QtlByteBlock& bb)
    {
        append(bb.data(), bb.size());
    }

    //!
    //! Append a C-string to a byte block.
    //! @param [in] str A nul-terminated string.
    //!
    void appendString(const char* str)
    {
        if (str != 0) {
            append(str, ::strlen(str));
        }
    }

    //!
    //! Append a QByteArray to a byte block.
    //! @param [in] ba Byte array to append.
    //!
    void append(const QByteArray& ba)
    {
        append(ba.data(), ba.size());
    }

    //!
    //! Append n bytes with value i.
    //! @param [in] count Number of bytes to add.
    //! @param [in] value Value to assign to each byte.
    //!
    void append(int count, quint8 value)
    {
        if (count > 0) {
            ::memset(enlarge(count), value, count);
        }
    }

    //!
    //! Add an integer in big endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendUInt8(quint8 i)
    {
        ByteVector::append(i);
    }

    //!
    //! Add an integer in big endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendUInt16(quint16 i)
    {
        qToBigEndian(i, (uchar*)(enlarge(2)));
    }

    //!
    //! Add an integer in big endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendUInt32(quint32 i)
    {
        qToBigEndian(i, (uchar*)(enlarge(4)));
    }

    //!
    //! Add an integer in big endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendUInt64(quint64 i)
    {
        qToBigEndian(i, (uchar*)(enlarge(8)));
    }

    //!
    //! Add an integer in big endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendInt8(qint8 i)
    {
        ByteVector::append(quint8(i));
    }

    //!
    //! Add an integer in big endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendInt16(qint16 i)
    {
        qToBigEndian(i, (uchar*)(enlarge(2)));
    }

    //!
    //! Add an integer in big endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendInt32(qint32 i)
    {
        qToBigEndian(i, (uchar*)(enlarge(4)));
    }

    //!
    //! Add an integer in big endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendInt64(qint64 i)
    {
        qToBigEndian(i, (uchar*)(enlarge(8)));
    }

    //!
    //! Add an integer in big endian representation at the end.
    //! Template variant
    //! @tparam INT An integer type.
    //! @param [in] i Integer value to append.
    //!
    template<typename INT>
    void appendToBigEndian(INT i)
    {
        qToBigEndian<INT>(i, (uchar*)(enlarge(sizeof(INT))));
    }

    //!
    //! Store an integer in big endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeUInt8(int index, quint8 i)
    {
        *(quint8*)(enlargeFrom(index, 1)) = i;
    }

    //!
    //! Store an integer in big endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeUInt16(int index, quint16 i)
    {
        qToBigEndian(i, (uchar*)(enlargeFrom(index, 2)));
    }

    //!
    //! Store an integer in big endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeUInt32(int index, quint32 i)
    {
        qToBigEndian(i, (uchar*)(enlargeFrom(index, 4)));
    }

    //!
    //! Store an integer in big endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeUInt64(int index, quint64 i)
    {
        qToBigEndian(i, (uchar*)(enlargeFrom(index, 8)));
    }

    //!
    //! Store an integer in big endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeInt8(int index, qint8 i)
    {
        *(qint8*)(enlargeFrom(index, 1)) = i;
    }

    //!
    //! Store an integer in big endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeInt16(int index, qint16 i)
    {
        qToBigEndian(i, (uchar*)(enlargeFrom(index, 2)));
    }

    //!
    //! Store an integer in big endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeInt32(int index, qint32 i)
    {
        qToBigEndian(i, (uchar*)(enlargeFrom(index, 4)));
    }

    //!
    //! Store an integer in big endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeInt64(int index, qint64 i)
    {
        qToBigEndian(i, (uchar*)(enlargeFrom(index, 8)));
    }

    //!
    //! Store an integer in big endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! Template variant
    //! @tparam INT An integer type.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    template<typename INT>
    void storeToBigEndian(int index, INT i)
    {
        qToBigEndian<INT>(i, (uchar*)(enlargeFrom(index, sizeof(INT))));
    }

    //!
    //! Add an integer in little endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendUInt16LittleEndian(quint16 i)
    {
        qToLittleEndian(i, (uchar*)(enlarge(2)));
    }

    //!
    //! Add an integer in little endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendUInt32LittleEndian(quint32 i)
    {
        qToLittleEndian(i, (uchar*)(enlarge(4)));
    }

    //!
    //! Add an integer in little endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendUInt64LittleEndian(quint64 i)
    {
        qToLittleEndian(i, (uchar*)(enlarge(8)));
    }

    //!
    //! Add an integer in little endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendInt16LittleEndian(qint16 i)
    {
        qToLittleEndian(i, (uchar*)(enlarge(2)));
    }

    //!
    //! Add an integer in little endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendInt32LittleEndian(qint32 i)
    {
        qToLittleEndian(i, (uchar*)(enlarge(4)));
    }

    //!
    //! Add an integer in little endian representation at the end.
    //! @param [in] i Integer value to append.
    //!
    void appendInt64LittleEndian(qint64 i)
    {
        qToLittleEndian(i, (uchar*)(enlarge(8)));
    }

    //!
    //! Add an integer in little endian representation at the end.
    //! Template variant
    //! @tparam INT An integer type.
    //! @param [in] i Integer value to append.
    //!
    template<typename INT>
    void appendToLittleEndian(INT i)
    {
        qToLittleEndian<INT>(i, (uchar*)(enlarge(sizeof(INT))));
    }

    //!
    //! Store an integer in little endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeUInt16LittleEndian(int index, quint16 i)
    {
        qToLittleEndian(i, (uchar*)(enlargeFrom(index, 2)));
    }

    //!
    //! Store an integer in little endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeUInt32LittleEndian(int index, quint32 i)
    {
        qToLittleEndian(i, (uchar*)(enlargeFrom(index, 4)));
    }

    //!
    //! Store an integer in little endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeUInt64LittleEndian(int index, quint64 i)
    {
        qToLittleEndian(i, (uchar*)(enlargeFrom(index, 8)));
    }

    //!
    //! Store an integer in little endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeInt16LittleEndian(int index, qint16 i)
    {
        qToLittleEndian(i, (uchar*)(enlargeFrom(index, 2)));
    }

    //!
    //! Store an integer in little endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeInt32LittleEndian(int index, qint32 i)
    {
        qToLittleEndian(i, (uchar*)(enlargeFrom(index, 4)));
    }

    //!
    //! Store an integer in little endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    void storeInt64LittleEndian(int index, qint64 i)
    {
        qToLittleEndian(i, (uchar*)(enlargeFrom(index, 8)));
    }

    //!
    //! Store an integer in little endian representation at a given index.
    //! The byte block is enlarged if necessary.
    //! Template variant
    //! @tparam INT An integer type.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //!
    template<typename INT>
    void storeToLittleEndian(int index, INT i)
    {
        qToLittleEndian<INT>(i, (uchar*)(enlargeFrom(index, sizeof(INT))));
    }

    //!
    //! Add an integer in a given byte order at the end.
    //! @tparam INT An integer type.
    //! @param [in] i Integer value to append.
    //! @param [in] order The byte order to serialize @a i.
    //!
    template<typename INT>
    void appendToByteOrder(INT i, ByteOrder order)
    {
        switch (order) {
        case LittleEndian:
            appendToLittleEndian(i);
            break;
        case BigEndian:
            appendToBigEndian(i);
            break;
        }
    }

    //!
    //! Store an integer in a given byte order at a given index.
    //! The byte block is enlarged if necessary.
    //! @tparam INT An integer type.
    //! @param [in] index Index where to store the integer. If negative, append at end of data block.
    //! @param [in] i Integer value to append.
    //! @param [in] order The byte order to serialize @a i.
    //!
    template<typename INT>
    void storeToByteOrder(int index, INT i, ByteOrder order)
    {
        switch (order) {
        case LittleEndian:
            storeToLittleEndian(index, i);
            break;
        case BigEndian:
            storeToBigEndian(index, i);
            break;
        }
    }

    //!
    //! Get an UTF-8 string at a given position.
    //! @param [in,out] index Index in the byte block where to read the data.
    //! If the data is correctly read, @a index is updated to point after the read data.
    //! @param [in] stringSize Requested string size in bytes.
    //! @param [out] s Returned string.
    //! @return True on success, false on error.
    //!
    bool getUtf8(int& index, int stringSize, QString& s) const;

    //!
    //! Get an UTF-8 string at a given position.
    //! @param [in] index Index in the byte block where to read the data.
    //! @param [in] stringSize Requested string size in bytes.
    //! @return The string, possibly truncated to stay within the block.
    //!
    QString getUtf8(int index, int stringSize) const;

    //!
    //! Get a Latin-1 string at a given position.
    //! @param [in,out] index Index in the byte block where to read the data.
    //! If the data is correctly read, @a index is updated to point after the read data.
    //! @param [in] stringSize Requested string size in bytes.
    //! @param [out] s Returned string.
    //! @return True on success, false on error.
    //!
    bool getLatin1(int& index, int stringSize, QString& s) const;

    //!
    //! Get a Latin1 string at a given position.
    //! @param [in] index Index in the byte block where to read the data.
    //! @param [in] stringSize Requested string size in bytes.
    //! @return The string, possibly truncated to stay within the block.
    //!
    QString getLatin1(int index, int stringSize) const;

    //!
    //! Get an integer in big endian representation at a given position.
    //! @param [in,out] index Index in the byte block where to read the data.
    //! If the data is correctly read, @a index is updated to point after the read data.
    //! @param [out] i Returned integer value.
    //! @return True on success, false on error.
    //!
    template<typename INT>
    bool fromBigEndian(int& index, INT& i) const
    {
        if (index < 0 || index + int(sizeof(i)) > size()) {
            return false;
        }
        else {
            i = qFromBigEndian<INT>(data() + index);
            index += sizeof(i);
            return true;
        }
    }

    //!
    //! Get an integer in big endian representation at a given position.
    //! @param [in] index Index in the byte block where to read the data.
    //! @return The decoded integer value or zero if no space in byte block.
    //!
    template<typename INT>
    INT fromBigEndian(int index) const
    {
        return index < 0 || index + int(sizeof(INT)) > size() ? 0 : qFromBigEndian<INT>(data() + index);
    }

    //!
    //! Get an integer in little endian representation at a given position.
    //! @param [in,out] index Index in the byte block where to read the data.
    //! If the data is correctly read, @a index is updated to point after the read data.
    //! @param [out] i Returned integer value.
    //! @return True on success, false on error.
    //!
    template<typename INT>
    bool fromLittleEndian(int& index, INT& i) const
    {
        if (index < 0 || index + int(sizeof(i)) > size()) {
            return false;
        }
        else {
            i = qFromLittleEndian<INT>(data() + index);
            index += sizeof(i);
            return true;
        }
    }

    //!
    //! Get an integer in little endian representation at a given position.
    //! @param [in] index Index in the byte block where to read the data.
    //! @return The decoded integer value or zero if no space in byte block.
    //!
    template<typename INT>
    INT fromLittleEndian(int index) const
    {
        return index < 0 || index + int(sizeof(INT)) > size() ? 0 : qFromLittleEndian<INT>(data() + index);
    }

    //!
    //! Get an integer in a given byte order at a given position.
    //! @param [in,out] index Index in the byte block where to read the data.
    //! If the data is correctly read, @a index is updated to point after the read data.
    //! @param [out] i Returned integer value.
    //! @param [in] order The byte order to deserialize @a i.
    //! @return True on success, false on error.
    //!
    template<typename INT>
    bool fromByteOrder(int& index, INT& i, ByteOrder order) const
    {
        switch (order) {
        case BigEndian:
            return fromBigEndian(index, i);
        case LittleEndian:
            return fromLittleEndian(index, i);
        default:
            return false;
        }
    }

    //!
    //! Get an integer in a given byte order at a given position.
    //! @param [in] index Index in the byte block where to read the data.
    //! @param [in] order The byte order to deserialize the integer.
    //! @return The decoded integer value or zero if no space in byte block.
    //!
    template<typename INT>
    INT fromByteOrder(int index, ByteOrder order) const
    {
        if (index >= 0 && index + int(sizeof(INT)) <= size()) {
            switch (order) {
            case BigEndian:
                return qFromBigEndian<INT>(data() + index);
            case LittleEndian:
                return qFromLittleEndian<INT>(data() + index);
            }
        }
        return 0;
    }
};

//!
//! Add an integer in big endian representation at the end.
//! Template specializations for performance
//! @param [in] i Integer value to append.
//!
template<> inline void QtlByteBlock::appendToBigEndian(quint8 i)
{
    ByteVector::append(i);
}

//!
//! Add an integer in big endian representation at the end.
//! Template specializations for performance
//! @param [in] i Integer value to append.
//!
template<> inline void QtlByteBlock::appendToBigEndian(qint8 i)
{
    ByteVector::append(quint8(i));
}

//!
//! Add an integer in little endian representation at the end.
//! Template specializations for performance
//! @param [in] i Integer value to append.
//!
template<> inline void QtlByteBlock::appendToLittleEndian(quint8 i)
{
    ByteVector::append(i);
}

//!
//! Add an integer in little endian representation at the end.
//! Template specializations for performance
//! @param [in] i Integer value to append.
//!
template<> inline void QtlByteBlock::appendToLittleEndian(qint8 i)
{
    ByteVector::append(quint8(i));
}

#endif // QTLBYTEBLOCK_H
