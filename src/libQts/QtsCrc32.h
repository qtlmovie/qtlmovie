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
//! @file QtsCrc32.h
//!
//! Declare the class QtsCrc32.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSCRC32_H
#define QTSCRC32_H

#include "QtsCore.h"

//!
//! Cyclic Redundancy Check as used in MPEG sections.
//! Original code, authors & copyright are unclear.
//!
class QtsCrc32
{
public:
    //!
    //! Default constructor.
    //!
    QtsCrc32() :
        _fcs(0xFFFFFFFF)
    {
    }

    //!
    //! Copy constructor.
    //! @param [in] c Other instance to copy.
    //!
    QtsCrc32(const QtsCrc32& c) :
        _fcs(c._fcs)
    {
    }

    //!
    //! Constructor, compute the QtsCrc32 of a data area.
    //! @param [in] data Address of data area.
    //! @param [in] size Size of data area.
    //!
    QtsCrc32(const void* data, size_t size) :
        _fcs(0xFFFFFFFF)
    {
        add(data, size);
    }

    //!
    //! Continue the computation of a data area, following a previous CRC32.
    //! @param [in] data Address of data area.
    //! @param [in] size Size of data area.
    //!
    void add(const void* data, int size);

    //!
    //! Get the value of a CRC32.
    //! @return The 32-bit value of the CRC32.
    //!
    quint32 value() const
    {
        return _fcs;
    }

    //!
    //! Convert to a 32-bit integer.
    //! @return The 32-bit value of the CRC32.
    //!
    operator quint32() const
    {
        return _fcs;
    }

    //!
    //! Assigment operator.
    //! @param [in] c Other instance to copy.
    //! @return A reference to this object.
    //!
    const QtsCrc32& operator=(const QtsCrc32& c)
    {
        _fcs = c._fcs;
        return *this;
    }

    //!
    //! Comparison operator.
    //! @param [in] c Other instance to compare.
    //! @return True if both CRC32 are identical.
    //!
    bool operator==(const QtsCrc32& c) const
    {
        return _fcs == c._fcs;
    }

    //!
    //! Difference operator.
    //! @param [in] c Other instance to compare.
    //! @return True if CRC32 are different.
    //!
    bool operator!=(const QtsCrc32& c) const
    {
        return _fcs != c._fcs;
    }

    //!
    //! Comparison operator.
    //! @param [in] c An integer CRC32 to compare.
    //! @return True if both CRC32 are identical.
    //!
    bool operator==(quint32 c) const
    {
        return _fcs == c;
    }

    //!
    //! Difference operator.
    //! @param [in] c An integer CRC32 to compare.
    //! @return True if CRC32 are different.
    //!
    bool operator!=(quint32 c) const
    {
        return _fcs != c;
    }

    //!
    //! What to do with a CRC32 in a section.
    //!
    enum Validation {
        Ignore,   //!< Ignore CRC32 value.
        Check,    //!< Check CRC32 value to detect error.
        Compute   //!< Compute the new CRC32 value.
    };

private:
    quint32 _fcs; //!< Current computed value.
};

//!
//! Comparison operator.
//! Reversed form of operator which is not a member function.
//! @param [in] c1 An integer CRC32 to compare.
//! @param [in] c2 An instance of QtsCrc32 to compare.
//! @return True if both CRC32 are identical.
//!
inline bool operator==(quint32 c1, const QtsCrc32& c2)
{
    return c2 == c1;  // this one is a member function
}

//!
//! Difference operator.
//! Reversed form of operator which is not a member function.
//! @param [in] c1 An integer CRC32 to compare.
//! @param [in] c2 An instance of QtsCrc32 to compare.
//! @return True if CRC32 are different.
//!
inline bool operator!=(quint32 c1, const QtsCrc32& c2)
{
    return c2 != c1;  // this one is a member function
}

Q_DECLARE_METATYPE(QtsCrc32::Validation)

#endif // QTSCRC32_H
