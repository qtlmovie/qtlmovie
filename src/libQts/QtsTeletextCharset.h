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
//! @file QtsTeletextCharset.h
//!
//! Declare the class QtsTeletextCharset.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSTELETEXTCHARSET_H
#define QTSTELETEXTCHARSET_H

#include "QtsCore.h"

//!
//! Implementation of a Teletext character set.
//!
class QtsTeletextCharset
{
public:
    //!
    //! Constructor.
    //!
    QtsTeletextCharset();

    //!
    //! Check parity and translate any reasonable Teletext character into UCS-2.
    //! @param [in] c Teletext character.
    //! @return UCS-2 equivalent.
    //!
    quint16 teletextToUcs2(quint8 c) const;

    //!
    //! Translate a G2 character into UCS-2.
    //! @param [in] c Teletext character.
    //! @param [in] accent Accent mode (0 to 14) if @a c is a letter.
    //! @return UCS-2 equivalent.
    //!
    quint16 g2AccentToUcs2(quint8 c, quint8 accent) const;

    //!
    //! Translate a G2 character into UCS-2.
    //! @param [in] c Teletext character.
    //! @return UCS-2 equivalent.
    //!
    quint16 g2ToUcs2(quint8 c) const;

    //!
    //! Set the X/28 character set and use it as current character set.
    //! @param [in] charset Charactet set index.
    //!
    void setX28(quint8 charset);

    //!
    //! Set the M/29 character set and use it as current character set if X/28 is not defined.
    //! @param [in] charset Charactet set index.
    //!
    void setM29(quint8 charset);

    //!
    //! Reset the X/28 character set to undefined.
    //! Use M/29 as current character or use @a fallback if M/29 is undefined.
    //! @param [in] fallback Fallback charactet set index if M/29 is undefined.
    //!
    void resetX28(quint8 fallback);

    //!
    //! Number of characters per charset.
    //!
    static const size_t CHAR_COUNT = 96;

private:
    //!
    //! Index of G0 character sets in G0 table.
    //!
    enum G0CharsetIndex {
        LATIN     = 0,
        CYRILLIC1 = 1,
        CYRILLIC2 = 2,
        CYRILLIC3 = 3,
        GREEK     = 4,
        ARABIC    = 5,
        HEBREW    = 6,
        G0_COUNT  = 7
    };

    //!
    //! G0 charsets data.
    //!
    typedef quint16 G0CharsetData[G0_COUNT][CHAR_COUNT];

    //!
    //! Initial base content of a charset.
    //! Charset can be -- and always is -- changed during transmission.
    //!
    static const G0CharsetData G0Base;

    //!
    //! Undefined charset index.
    //!
    static const quint8 UNDEFINED = 0xFF;

    //!
    //! Remap the GO character set.
    //! @param [in] charset Charactet set index.
    //!
    void remapG0(quint8 charset);

    // Private members.
    quint8        _current;  //!< Current charset index.
    quint8        _g0m29;    //!< M/29 charset.
    quint8        _g0x28;    //!< X/28 charset.
    G0CharsetData _G0;       //!< Current character set data.
};

#endif // QTSTELETEXTCHARSET_H
