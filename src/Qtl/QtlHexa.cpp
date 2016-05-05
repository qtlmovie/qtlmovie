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
//
// Qtl, Qt utility library.
// Define the utilities to manipulate hexadecimal strings.
//
//----------------------------------------------------------------------------

#include "QtlHexa.h"


//-----------------------------------------------------------------------------
// Interpret a string as a sequence of hexadecimal digits (ignore blanks).
// Append the result into a vector of bytes.
//-----------------------------------------------------------------------------

bool qtlHexaDecodeAndAppend(QtlByteBlock& result, const QString& hexaString)
{
    // Over-reserve (at most) extra space for new bytes
    result.reserve(result.size() + hexaString.size() / 2);

    bool gotFirstNibble = false;
    quint8 byte = 0;

    // Loop on all characters in the string.
    foreach (QChar c, hexaString) {
        quint8 nibble;
        if (c.isSpace()) {
            continue;
        }
        else if (c >= QChar('0') && c <= QChar('9')) {
            nibble = c.toLatin1() - '0';
        }
        else if (c >= QChar('A') && c <= QChar('F')) {
            nibble = c.toLatin1() - 'A' + 10;
        }
        else if (c >= QChar('a') && c <= QChar('f')) {
            nibble = c.toLatin1() - 'a' + 10;
        }
        else {
            return false;
        }
        if (gotFirstNibble) {
            result.appendUInt8(byte | nibble);
        }
        else {
            byte = nibble << 4;
        }
        gotFirstNibble = !gotFirstNibble;
    }

    return !gotFirstNibble;
}


//-----------------------------------------------------------------------------
// Build a multi-line string containing the hexadecimal dump of a memory area.
//-----------------------------------------------------------------------------

QString qtlHexa(const void* data, int size, Qtl::HexaFlags flags, int indent, int lineWidth, int initOffset)
{
    QString str; // the formatted string to return.
    const quint8* raw = static_cast<const quint8*>(data);

    // Make sure we have something to display (default is hexa)
    if ((flags & (Qtl::HexHexa | Qtl::HexCStyle | Qtl::HexBinary | Qtl::HexBinNibble | Qtl::HexAscii)) == 0) {
        flags |= Qtl::HexHexa;
    }

    // Width of an hexa byte: "XX" (2) or "0xXX," (5)
    int hexaWidth;
    const char* bytePrefix;
    const char* byteSuffix;

    if (flags.testFlag(Qtl::HexCStyle)) {
        hexaWidth  = 5;
        bytePrefix = "0x";
        byteSuffix = ",";
        flags |= Qtl::HexHexa; // Enforce hexa flag
    }
    else if (flags.testFlag(Qtl::HexHexa) || flags.testFlag(Qtl::HexSingleLine) || flags.testFlag(Qtl::HexCompact)) {
        hexaWidth  = 2;
        bytePrefix = "";
        byteSuffix = "";
    }
    else {
        hexaWidth  = 0;
        bytePrefix = "";
        byteSuffix = "";
    }

    // Specific case: simple dump, everything on one line.
    if (flags.testFlag(Qtl::HexSingleLine) || flags.testFlag(Qtl::HexCompact)) {
        str.reserve((hexaWidth + 1) * size);
        for (int i = 0; i < size; ++i) {
            if (i > 0 && !flags.testFlag(Qtl::HexCompact)) {
                str.append(QChar(' '));
            }
            str.append(bytePrefix);
            str.append(qtlHexaInt(raw[i]));
            str.append(byteSuffix);
        }
        return str;
    }

    // Width of offset field
    int offsetWidth;
    if (!flags.testFlag(Qtl::HexOffset)) {
        offsetWidth = 0;
    }
    else if (flags.testFlag(Qtl::HexWideOffset)) {
        offsetWidth = 8;
    }
    else if (initOffset + size <= 0x10000) {
        offsetWidth = 4;
    }
    else {
        offsetWidth = 8;
    }

    // Width of a binary byte
    int binWidth;
    if (flags.testFlag(Qtl::HexBinNibble)) {
        binWidth = 9;
        flags |= Qtl::HexBinary;  // Enforce binary flag
    }
    else if (flags.testFlag(Qtl::HexBinary)) {
        binWidth = 8;
    }
    else {
        binWidth = 0;
    }

    // Pre-allocation to avoid too frequent reallocations.
    str.reserve((hexaWidth + binWidth + 5) * size);

    // Number of non-byte characters
    int addWidth = indent;
    if (offsetWidth != 0) {
        addWidth += offsetWidth + 3;
    }
    if (flags.testFlag(Qtl::HexHexa) && (flags.testFlag(Qtl::HexBinary) || flags.testFlag(Qtl::HexAscii))) {
        addWidth += 2;
    }
    if (flags.testFlag(Qtl::HexBinary) && flags.testFlag(Qtl::HexAscii)) {
        addWidth += 2;
    }

    // Computes max number of dumped bytes per line
    int bytesPerLine;
    if (flags.testFlag(Qtl::HexBytesPerLine)) {
        bytesPerLine = lineWidth;
    }
    else if (addWidth >= lineWidth) {
        bytesPerLine = 8;  // arbitrary, if indent is too long
    }
    else {
        bytesPerLine = (lineWidth - addWidth) /
            ((flags.testFlag(Qtl::HexHexa) ? (hexaWidth + 1) : 0) +
             (flags.testFlag(Qtl::HexBinary) ? (binWidth + 1) : 0) +
             (flags.testFlag(Qtl::HexAscii) ? 1 : 0));
        if (bytesPerLine > 1) {
            bytesPerLine = bytesPerLine & ~1; // force even value
        }
    }
    if (bytesPerLine == 0) {
        bytesPerLine = 8;  // arbitrary, if ended up with none
    }

    // Display data
    for (int line = 0; line < size; line += bytesPerLine) {

        // Number of bytes on this line (last line may be shorter)
        int lineSize = line + bytesPerLine <= size ? bytesPerLine : size - line;

        // Beginning of line
        str.append(QString(indent, QChar(' ')));
        if (flags.testFlag(Qtl::HexOffset)) {
            str.append(qtlHexaInt(initOffset + line, offsetWidth));
            str.append(QStringLiteral(":  "));
        }

        // Hexa dump
        if (flags.testFlag(Qtl::HexHexa)) {
            for (int byte = 0; byte < lineSize; byte++) {
                str.append(bytePrefix);
                str.append(qtlHexaInt(raw[line + byte]));
                str.append(byteSuffix);
                if (byte < bytesPerLine - 1) {
                    str.append(QChar(' '));
                }
            }
            if (flags.testFlag(Qtl::HexBinary) || flags.testFlag(Qtl::HexAscii)) { // more to come
                if (lineSize < bytesPerLine) {
                    str.append(QString((hexaWidth + 1) * (bytesPerLine - lineSize) - 1, QChar(' ')));
                }
                str.append(QString(2, QChar(' ')));
            }
        }

        // Binary dump
        if (flags.testFlag(Qtl::HexBinary)) {
            for (int byte = 0; byte < lineSize; byte++) {
                int b = int(raw[line + byte]);
                for (int i = 7; i >= 0; i--) {
                    str.append(QChar('0' + ((b >> i) & 0x01)));
                    if (i == 4 && flags.testFlag(Qtl::HexBinNibble)) {
                        str.append(QChar('.'));
                    }
                }
                if (byte < bytesPerLine - 1) {
                    str.append(QChar(' '));
                }
            }
            if (flags.testFlag(Qtl::HexAscii)) { // more to come
                if (lineSize < bytesPerLine) {
                    str.append(QString((binWidth + 1) * (bytesPerLine - lineSize) - 1, ' '));
                }
                str.append(QString(2, QChar(' ')));
            }
        }

        // ASCII dump
        if (flags.testFlag(Qtl::HexAscii)) {
            for (int byte = 0; byte < lineSize; byte++) {
                const QChar c = char(raw[line + byte]);
                str.append(c.isPrint() ? c : QChar('.'));
            }
        }
        str.append(QChar('\n'));
    }

    return str;
}
