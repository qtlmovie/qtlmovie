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
//! @file QtlHexa.h
//!
//! Declare some utilities to manipulate hexadecimal strings.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLHEXA_H
#define QTLHEXA_H

#include <QtCore>
#include "QtlByteBlock.h"

//!
//! Interpret a string as a sequence of hexadecimal digits.
//!
//! Interpret all hexadecimal digits in @a hexaString.
//! Each pair of hexadecimal digits is interpreted as one byte value
//! which is appended into a byte array.
//! Space characters are ignored.
//! Other characters generate an error.
//!
//! @param [in,out] result A byte array. All decoded bytes are appended to this container.
//! @param [in] hexaString The string to decode.
//! @return True on success, false on error (invalid hexadecimal format).
//! When returning false, @a result contains all bytes that could
//! be decoded before getting the error.
//!
bool qtlHexaDecodeAndAppend(QtlByteBlock& result, const QString& hexaString);

//!
//! Interpret a string as a sequence of hexadecimal digits.
//!
//! Interpret all hexadecimal digits in @a hexaString.
//! Each pair of hexadecimal digits is interpreted as one byte value.
//! A byte array is assigned with the result.
//! Space characters are ignored.
//! Other characters generate an error.
//!
//! @param [out] result A byte array. All decoded bytes are
//! stored into this vector.
//! @param [in] hexaString The string to decode.
//! @return True on success, false on error (invalid hexadecimal format).
//! When returning false, @a result contains all bytes that could
//! be decoded before getting the error.
//!
inline bool qtlHexaDecode(QtlByteBlock& result, const QString& hexaString)
{
    result.clear();
    return qtlHexaDecodeAndAppend(result, hexaString);
}

//!
//! Interpret a string as a sequence of hexadecimal digits and return a QtlByteBlock.
//!
//! Interpret all hexadecimal digits in @a hexaString.
//! Each pair of hexadecimal digits is interpreted as one byte value.
//! A byte array is assigned with the result.
//! Space characters are ignored.
//! Other characters generate an error.
//!
//! @param [in] hexaString The string to decode.
//! @return A byte block. All decoded bytes are stored into this vector.
//!
inline QtlByteBlock qtlHexaDecode(const QString& hexaString)
{
    QtlByteBlock result;
    qtlHexaDecodeAndAppend(result, hexaString);
    return result;
}

//!
//! Build an hexadecimal string from an integer value.
//! @tparam INT An integer type.
//! @param [in] i The integer value to format.
//! @param [in] width The number of hexadecimal digits.
//! If negative or zero, use the "natural" width for @a i.
//!
template<typename INT>
inline QString qtlHexaInt(INT i, int width = 0)
{
    return QStringLiteral("%1").arg(quint32(i), width <= 0 ? 2 * sizeof(i) : width, 16, QChar('0')).toUpper();
}

//!
//! Build an hexadecimal string from an integer value.
//! Template specialization.
//! @param [in] i The integer value to format.
//! @param [in] width The number of hexadecimal digits.
//! If negative or zero, use the "natural" width for @a i.
//!
template <>
inline QString qtlHexaInt(quint64 i, int width)
{
    return QStringLiteral("%1").arg(i, width <= 0 ? 2 * sizeof(i) : width, 16, QChar('0')).toUpper();
}

//!
//! Build an hexadecimal string from an integer value.
//! Template specialization.
//! @param [in] i The integer value to format.
//! @param [in] width The number of hexadecimal digits.
//! If negative or zero, use the "natural" width for @a i.
//!
template <>
inline QString qtlHexaInt(qint64 i, int width)
{
    return QStringLiteral("%1").arg(quint64(i), width <= 0 ? 2 * sizeof(i) : width, 16, QChar('0')).toUpper();
}

//!
//! Qtl namespace.
//!
namespace Qtl {
    //!
    //! Default line width for output formatting.
    //!
    const int DefautLineWidth = 78;

    //!
    //! Flags for the @c qtlHexa family of functions.
    //!
    enum HexaFlag {
        HexHexa         = 0x0001,  //!< Dump hexa values.
        HexAscii        = 0x0002,  //!< Dump ascii values.
        HexOffset       = 0x0004,  //!< Display address offsets.
        HexWideOffset   = 0x0008,  //!< Always wide offset.
        HexSingleLine   = 0x0010,  //!< Hexa on one single line, no line feed, ignore other flags.
        HexBytesPerLine = 0x0020,  //!< Interpret @a lineWidth as number of displayed Bytes Per Line.
        HexCStyle       = 0x0040,  //!< C-style hexa value ("0xXX," instead of "XX").
        HexBinary       = 0x0080,  //!< Dump binary values ("XXXXXXXX" binary digits).
        HexBinNibble    = 0x0100   //!< Binary values are grouped by nibble ("XXXX XXXX").
    };
    Q_DECLARE_FLAGS(HexaFlags, HexaFlag)
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Qtl::HexaFlags)

//!
//! Build a multi-line string containing the hexadecimal dump of a memory area.
//!
//! @param [in] data Starting address of the memory area to dump.
//! @param [in] size Size in bytes of the memory area to dump.
//! @param [in] flags A combination of option flags indicating how to format the data.
//! @param [in] indent Each line is indented by this number of characters.
//! @param [in] lineWidth Maximum number of characters per line.
//! If the flag @c HexBytesPerLine is specified, @a maxLineWidth is interpreted as
//! the number of displayed byte values per line.
//! @param [in] initOffset If the flag @c HexOffset is specified, an offset
//! in the memory area is displayed at the beginning of each line. In this
//! case, @a initOffset specified the offset value for the first byte.
//! @return A string containing the formatted hexadecimal dump.
//! Lines are separated with embedded new-line characters ('\\n').
//! @see Qtl::HexaFlag
//!
QString qtlHexa(const void* data,
                int size,
                Qtl::HexaFlags flags = Qtl::HexaFlags(Qtl::HexHexa),
                int indent = 0,
                int lineWidth = Qtl::DefautLineWidth,
                int initOffset = 0);

//!
//! Build a multi-line string containing the hexadecimal dump of the content of a byte array.
//!
//! @param [in] data The vector of bytes to dump.
//! @param [in] flags A combination of option flags indicating how to format the data.
//! @param [in] indent Each line is indented by this number of characters.
//! @param [in] lineWidth Maximum number of characters per line.
//! If the flag @c HexBytesPerLine is specified, @a maxLineWidth is interpreted as
//! the number of displayed byte values per line.
//! @param [in] initOffset If the flag @c HexOffset is specified, an offset
//! in the memory area is displayed at the beginning of each line. In this
//! case, @a initOffset specified the offset value for the first byte.
//! @return A string containing the formatted hexadecimal dump.
//! Lines are separated with embedded new-line characters ('\\n').
//! @see Qtl::HexaFlag
//!
inline QString qtlHexa(const QtlByteBlock& data,
                       Qtl::HexaFlags flags = Qtl::HexaFlags(Qtl::HexHexa),
                       int indent = 0,
                       int lineWidth = Qtl::DefautLineWidth,
                       int initOffset = 0)
{
    return qtlHexa(data.data(), data.size(), flags, indent, lineWidth, initOffset);
}

#endif // QTLHEXA_H
