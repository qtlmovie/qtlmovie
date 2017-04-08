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
//! @file QtsAbstractLongTable.h
//!
//! Declare the class QtsAbstractLongTable.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSABSTRACTLONGTABLE_H
#define QTSABSTRACTLONGTABLE_H

#include "QtsAbstractTable.h"

//!
//! Abstract base class for MPEG PSI/SI tables with long sections.
//!
class QtsAbstractLongTable : public QtsAbstractTable
{
public:
    //!
    //! Get the table version.
    //! @return The table version.
    //!
    quint8 version() const
    {
        return _version;
    }

    //!
    //! Modify the table version.
    //! @param [in] version New table version.
    //!
    void setVersion(quint8 version)
    {
        _version = version;
    }

    //!
    //! Check if the section is "current" (vs. "next").
    //! @return True if the section is "current" (vs. "next").
    //!
    bool isCurrent() const
    {
        return _isCurrent;
    }

    //!
    //! Modify the "current" flag.
    //! @param [in] isCurrent New "current" flag.
    //!
    void setIsCurrent(bool isCurrent)
    {
        _isCurrent = isCurrent;
    }

protected:
    //!
    //! Protected constructor for subclasses
    //! @param [in] tid Table id.
    //! @param [in] version Table version.
    //! @param [in] isCurrent "current" flag.
    //!
    QtsAbstractLongTable(QtsTableId tid, quint8 version = 0, bool isCurrent = true) :
        QtsAbstractTable(tid),
        _version(version),
        _isCurrent(isCurrent)
    {
    }

    //!
    //! Serialization helper: add a section in the binary table.
    //! @param [in] table The binary table to build.
    //! @param [in] isPrivateSection False for MPEG-defined sections, true for all others.
    //! @param [in] tidExt Table id extension.
    //! @param [in] payload Address of section payload to copy.
    //! @param [in] payloadSize Size of section payload.
    //! @return True on success, false if the section could not be added (inconsistent property).
    //!
    bool addSection(QtsTable& table, bool isPrivateSection, quint16 tidExt, const void* payload, int payloadSize) const;

    //!
    //! Serialization helper: add a section in the binary table.
    //! @param [in] table The binary table to build.
    //! @param [in] isPrivateSection False for MPEG-defined sections, true for all others.
    //! @param [in] tidExt Table id extension.
    //! @param [in] payload Section payload to copy.
    //! @return True on success, false if the section could not be added (inconsistent property).
    //!
    bool addSection(QtsTable& table, bool isPrivateSection, quint16 tidExt, const QtlByteBlock& payload) const
    {
        return addSection(table, isPrivateSection, tidExt, payload.data(), payload.size());
    }

private:
    quint8 _version;    //!< Table version.
    bool   _isCurrent;  //!< Table "current" flag (vs. next).
};

#endif // QTSABSTRACTLONGTABLE_H
