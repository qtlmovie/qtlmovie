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
//! @file QtsProgramAssociationTable.h
//!
//! Declare the class QtsProgramAssociationTable.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSPROGRAMASSOCIATIONTABLE_H
#define QTSPROGRAMASSOCIATIONTABLE_H

#include "QtsAbstractLongTable.h"

//!
//! The MPEG-defined Program Association Table (PAT).
//!
class QtsProgramAssociationTable : public QtsAbstractLongTable
{
public:
    //!
    //! Describes a service entry in the PAT.
    //!
    struct ServiceEntry
    {
        quint16 serviceId;  //!< Service identifier.
        QtsPid  pmtPid;     //!< PID of the PMT of the service.
        //!
        //! Constructor.
        //! @param [in] id Service identifier.
        //! @param [in] pid PID of the PMT of the service.
        //!
        ServiceEntry(quint16 id, QtsPid pid) :
            serviceId(id),
            pmtPid(pid)
        {
        }
    };

    //!
    //! Describe the list of services in the PAT.
    //!
    typedef QList<ServiceEntry> ServiceList;

    //
    // The PAT is a simple table.
    // Its content is self-consistent and has public fields.
    //
    quint16     transportStreamId; //!< Transport stream id.
    QtsPid      nitPid;            //!< PID of the Network Information Table.
    ServiceList serviceList;       //!< List of services in the transport stream.

    //!
    //! Default constructor.
    //! @param [in] version Table version.
    //! @param [in] isCurrent True if the table is "current", false if it is "next".
    //! @param [in] tsId Transport stream id.
    //! @param [in] nit PID of the Network Information Table.
    //!
    QtsProgramAssociationTable(quint8 version = 0,
                               bool isCurrent = true,
                               quint16 tsId = 0,
                               QtsPid nit = QTS_PID_NIT);

    //!
    //! Constructor from a binary table.
    //! @param [in] table The table to deserialize.
    //!
    QtsProgramAssociationTable(const QtsTable& table);

    //!
    //! This abstract method serializes a table.
    //! Implementation required by QtsAbstractTable.
    //! @param [out] table Replaced with a binary representation of this object.
    //! @return True on success, false on error (invalid table, data too large, etc.)
    //!
    virtual bool serialize(QtsTable& table) const;

    //!
    //! This abstract method deserializes a binary table.
    //! Implementation required by QtsAbstractTable.
    //! @param [out] table Binary table to deserialize.
    //! @return True on success, false on error (invalid data, etc.)
    //!
    virtual bool deserialize(const QtsTable& table);
};

#endif // QTSPROGRAMASSOCIATIONTABLE_H
