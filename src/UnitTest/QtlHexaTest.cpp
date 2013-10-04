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
// Unit test for hexa utilities.
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlHexa.h"
#include <cstdarg>

class QtlHexaTest : public QObject
{
    Q_OBJECT
private slots:
    void testHexaDecode();
    void testHexaEncode();
};

#include "QtlHexaTest.moc"
QTL_TEST_CLASS(QtlHexaTest);

//----------------------------------------------------------------------------

// Reference byte array: 256 bytes, index == value
namespace {
    const quint8 refBytes[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
    };
}

// Build a QtlByteBlock from a variable number of int arguments.
// Stop at the first negative number.
namespace {
    QtlByteBlock byteBlock(int byte, ...)
    {
        QtlByteBlock res;
        if (byte >= 0) {
            res.appendUInt8(byte & 0xFF);
            va_list ap;
            va_start(ap, byte);
            for (;;) {
                const int b = va_arg(ap, int);
                if (b < 0) {
                    break;
                }
                else {
                    res.appendUInt8(b & 0xFF);
                }
            }
            va_end(ap);
        }
        return res;
    }
}

// Test case: hexa decode.
void QtlHexaTest::testHexaDecode()
{
    QtlByteBlock bytes;

    QVERIFY(qtlHexaDecode(bytes, "0123456789ABCDEF"));
    QVERIFY(bytes == byteBlock(0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, -1));

    QVERIFY(qtlHexaDecode(bytes, " 0 1234 56 789 ABC DEF "));
    QVERIFY(bytes == byteBlock(0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, -1));

    QVERIFY(!qtlHexaDecode(bytes, " 0 1234 56 - 789 ABC DEF "));
    QVERIFY(bytes == byteBlock(0x01, 0x23, 0x45, -1));

    QVERIFY(!qtlHexaDecode(bytes, "X 0 1234 56 - 789 ABC DEF "));
    QVERIFY(bytes.empty());
}

// Test case: hexa encode.
void QtlHexaTest::testHexaEncode()
{
    const QString hex1(qtlHexa(refBytes, 40));
    const char* ref1 =
        "00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19\n"
        "1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 \n";
    QVERIFY(hex1 == ref1);

    const QString hex2(qtlHexa(refBytes, 40, Qtl::HexHexa | Qtl::HexAscii));
    const char* ref2 =
        "00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11  ..................\n"
        "12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23  .............. !\"#\n"
        "24 25 26 27                                            $%&'\n";
    QVERIFY(hex2 == ref2);

    const QString hex3(qtlHexa(refBytes + 32,
                               40,
                               Qtl::HexHexa | Qtl::HexAscii | Qtl::HexOffset,
                               4,     // indent
                               50,    // lineWidth
                               32));  // initOffset
    const char* ref3 =
        "    0020:  20 21 22 23 24 25 26 27   !\"#$%&'\n"
        "    0028:  28 29 2A 2B 2C 2D 2E 2F  ()*+,-./\n"
        "    0030:  30 31 32 33 34 35 36 37  01234567\n"
        "    0038:  38 39 3A 3B 3C 3D 3E 3F  89:;<=>?\n"
        "    0040:  40 41 42 43 44 45 46 47  @ABCDEFG\n";
    QVERIFY(hex3 == ref3);

    const QString hex4(qtlHexa(refBytes + 32,
                               22,
                               Qtl::HexHexa | Qtl::HexAscii | Qtl::HexOffset | Qtl::HexBytesPerLine,
                               4,     // indent
                               10,    // lineWidth (in bytes)
                               32));  // initOffset
    const char* ref4 =
        "    0020:  20 21 22 23 24 25 26 27 28 29   !\"#$%&'()\n"
        "    002A:  2A 2B 2C 2D 2E 2F 30 31 32 33  *+,-./0123\n"
        "    0034:  34 35                          45\n";
    QVERIFY(hex4 == ref4);

    const QString hex5(qtlHexa(refBytes + 32, 12, Qtl::HexSingleLine));
    const char* ref5 = "20 21 22 23 24 25 26 27 28 29 2A 2B";
    QVERIFY(hex5 == ref5);

    const QString hex6(qtlHexa(refBytes + 32, 20, Qtl::HexHexa | Qtl::HexCStyle));
    const char* ref6 =
        "0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B,\n"
        "0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, \n";
    QVERIFY(hex6 == ref6);

    const QString hex7(qtlHexa(refBytes + 32, 10, Qtl::HexBinary | Qtl::HexAscii));
    const char* ref7 =
        "00100000 00100001 00100010 00100011 00100100 00100101   !\"#$%\n"
        "00100110 00100111 00101000 00101001                    &'()\n";
    QVERIFY(hex7 == ref7);

    const QString hex8(qtlHexa(refBytes + 32, 10, Qtl::HexBinNibble | Qtl::HexAscii));
    const char* ref8 =
        "0010.0000 0010.0001 0010.0010 0010.0011 0010.0100 0010.0101   !\"#$%\n"
        "0010.0110 0010.0111 0010.1000 0010.1001                      &'()\n";
    QVERIFY(hex8 == ref8);
}
