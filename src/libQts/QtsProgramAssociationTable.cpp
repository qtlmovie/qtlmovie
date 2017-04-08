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
//
// Qts, the Qt MPEG Transport Stream library.
// Define the class QtsProgramAssociationTable.
//
//----------------------------------------------------------------------------

#include "QtsProgramAssociationTable.h"


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtsProgramAssociationTable::QtsProgramAssociationTable(quint8 version, bool isCurrent, quint16 tsId, QtsPid nit) :
    QtsAbstractLongTable(QTS_TID_PAT, version, isCurrent),
    transportStreamId(tsId),
    nitPid(nit),
    serviceList()
{
    // The PAT is initially valid.
    _isValid = true;
}

QtsProgramAssociationTable::QtsProgramAssociationTable(const QtsTable& table) :
    QtsAbstractLongTable(QTS_TID_PAT)
{
    // Deserialize the table. Let this method set the validity.
    deserialize(table);
}


//----------------------------------------------------------------------------
// This abstract method serializes a table.
//----------------------------------------------------------------------------

bool QtsProgramAssociationTable::serialize(QtsTable& table) const
{
    // Serialization initialization.
    if (!serializeInit(table)) {
        return false;
    }

    // Build the sections in this payload.
    QtlByteBlock payload;

    // Add the NIT PID in the first section
    if (nitPid != QTS_PID_NULL) {
        payload.appendUInt16(0); // pseudo service_id
        payload.appendUInt16(nitPid | 0xE000);
    }

    // Add all services
    foreach (const ServiceEntry& service, serviceList) {
        // If current section payload is full, close the current section.
        if (payload.size() > QTS_MAX_PSI_LONG_SECTION_PAYLOAD_SIZE - 4) {
            addSection(table, false, transportStreamId, payload);
            payload.clear();
        }

        // Add current service entry into the PAT section
        payload.appendUInt16(service.serviceId);
        payload.appendUInt16(service.pmtPid | 0xE000);
    }

    // Add final partial section.
    addSection(table, false, transportStreamId, payload);
    return table.isValid();
}


//----------------------------------------------------------------------------
// This abstract method deserializes a binary table.
//----------------------------------------------------------------------------

bool QtsProgramAssociationTable::deserialize(const QtsTable& table)
{
    // Clear previous content.
    transportStreamId = 0;
    nitPid = QTS_PID_NULL;
    serviceList.clear();

    // Start deserialization.
    if (!deserializeInit(table)) {
        return false;
    }

    // Loop on all sections
    for (int si = 0; si < table.sectionCount(); ++si) {

        // Reference to current section
        const QtsSectionPtr& sect(table.sectionAt(si));

        // Get common properties (should be identical in all sections)
        setVersion(sect->version());
        setIsCurrent(sect->isCurrent());
        transportStreamId = sect->tableIdExtension();

        // Analyze the section payload:
        // This is a list of service_id/pmt_pid pairs
        const quint8* data = sect->payload();
        int remain = sect->payloadSize();

        while (remain >= 4) {
            // Extract one id/pid entry
            quint16 sid = qFromBigEndian<quint16>(data);
            quint16 pid = qFromBigEndian<quint16>(data + 2) & 0x1FFF;
            data += 4;
            remain -= 4;

            // Register the PID
            if (sid == 0) {
                nitPid = pid;
            }
            else {
                serviceList << ServiceEntry(sid, pid);
            }
        }
    }

    _isValid = true;
    return true;
}
