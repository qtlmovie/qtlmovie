//----------------------------------------------------------------------------
//
// Copyright(c) 2013, Thierry Lelegard
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

#include <QtCore>

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
    //! Replace the content with a data block.
    //! @param [in] addr Address of the data block.
    //! @param [in] size Size of the data block.
    //!
    void copy(const void* addr, int size);

    //!
    //! Increase size by @a n and return pointer to new n-byte area at end of block.
    //! @param [in] n Number of bytes to add.
    //!
    void* enlarge(int n);

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
    //! Get an UTF-8 string at a given position.
    //! @param [in,out] index Index in the byte block where to read the data.
    //! If the data is correctly read, @a index is updated to point after the read data.
    //! @param [in] stringSize Requested string size in bytes.
    //! @param [out] s Returned string.
    //! @return True on success, false on error.
    //!
    bool getUtf8(int& index, int stringSize, QString& s);

    //!
    //! Get an UTF-8 string at a given position.
    //! @param [in] index Index in the byte block where to read the data.
    //! @param [in] stringSize Requested string size in bytes.
    //! @return The string, possibly truncated to stay within the block.
    //!
    QString getUtf8(int index, int stringSize);

    //!
    //! Get a Latin-1 string at a given position.
    //! @param [in,out] index Index in the byte block where to read the data.
    //! If the data is correctly read, @a index is updated to point after the read data.
    //! @param [in] stringSize Requested string size in bytes.
    //! @param [out] s Returned string.
    //! @return True on success, false on error.
    //!
    bool getLatin1(int& index, int stringSize, QString& s);

    //!
    //! Get a Latin1 string at a given position.
    //! @param [in] index Index in the byte block where to read the data.
    //! @param [in] stringSize Requested string size in bytes.
    //! @return The string, possibly truncated to stay within the block.
    //!
    QString getLatin1(int index, int stringSize);

    //!
    //! Get an integer in big endian representation at a given position.
    //! @param [in,out] index Index in the byte block where to read the data.
    //! If the data is correctly read, @a index is updated to point after the read data.
    //! @param [out] i Returned integer value.
    //! @return True on success, false on error.
    //!
    template<typename INT>
    bool fromBigEndian(int& index, INT& i)
    {
        if (index < 0 || index + sizeof(i) > size()) {
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
    INT fromBigEndian(int index)
    {
        return index < 0 || index + sizeof(INT) > size() ? 0 : qFromBigEndian<INT>(data() + index);
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

#endif // QTLBYTEBLOCK_H
