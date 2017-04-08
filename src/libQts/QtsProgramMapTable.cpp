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
// Define the class QtsProgramMapTable.
//
//----------------------------------------------------------------------------

#include "QtsProgramMapTable.h"
#include "QtsTeletextDescriptor.h"


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtsProgramMapTable::QtsProgramMapTable(quint8 version, bool isCurrent, quint16 sid, QtsPid pcr) :
    QtsAbstractLongTable(QTS_TID_PMT, version, isCurrent),
    serviceId(sid),
    pcrPid(pcr),
    descs(),
    streams()
{
    // The PMT is initially valid.
    _isValid = true;
}

QtsProgramMapTable::QtsProgramMapTable(const QtsTable& table) :
    QtsAbstractLongTable(QTS_TID_PMT)
{
    // Deserialize the table. Let this method set the validity.
    deserialize(table);
}


//----------------------------------------------------------------------------
// This abstract method serializes a table.
//----------------------------------------------------------------------------

bool QtsProgramMapTable::serialize(QtsTable& table) const
{
    // Serialization initialization.
    if (!serializeInit(table)) {
        return false;
    }

    // Build the section in this payload.
    // Note that a PMT is not allowed to use more than one section,
    // see ISO/IEC 13818-1:2000 2.4.4.8 & 2.4.4.9.
    QtlByteBlock payload;

    // Add PCR PID
    payload.appendUInt16(pcrPid | 0xE000);

    // Insert program_info descriptor list (with leading length field).
    int next = descs.lengthSerialize(payload, QTS_MAX_PSI_LONG_SECTION_PAYLOAD_SIZE);
    if (next < descs.size()) {
        // Could not serialize all descriptors, table is too big for one section.
        return false;
    }

    // Add description of all elementary streams.
    foreach (const StreamEntry& stream, streams) {

        // Insert stream type and pid (will check max payload size later).
        payload.appendUInt8(stream.type);
        payload.appendUInt16(stream.pid | 0xE000);

        // Insert descriptor list for elem. stream (with leading length field).
        next = stream.descs.lengthSerialize(payload, QTS_MAX_PSI_LONG_SECTION_PAYLOAD_SIZE);

        // Check that the payload fits in a section.
        if (payload.size() > QTS_MAX_PSI_LONG_SECTION_PAYLOAD_SIZE || next < stream.descs.size()) {
            return false;
        }
    }

    // Add one single section in the table
    addSection(table, false, serviceId, payload);
    return table.isValid();
}


//----------------------------------------------------------------------------
// This abstract method deserializes a binary table.
//----------------------------------------------------------------------------

bool QtsProgramMapTable::deserialize(const QtsTable& table)
{
    // Clear previous content.
    serviceId = 0;
    pcrPid = QTS_PID_NULL;
    descs.clear();
    streams.clear();

    // Start deserialization.
    // Note that a PMT is not allowed to use more than one section,
    // see ISO/IEC 13818-1:2000 2.4.4.8 & 2.4.4.9.
    if (!deserializeInit(table) || table.sectionCount() != 1) {
        return false;
    }

    // Reference to unique section.
    const QtsSectionPtr& sect(table.sectionAt(0));

    // Get common properties.
    setVersion(sect->version());
    setIsCurrent(sect->isCurrent());
    serviceId = sect->tableIdExtension();

    // Analyze the section payload.
    const quint8* data = sect->payload();
    int remain = sect->payloadSize();

    // Get PCR PID
    if (remain < 2) {
        return false;
    }
    pcrPid = qFromBigEndian<quint16>(data) & 0x1FFF;
    data += 2;
    remain -= 2;

    // Get program information descriptor list.
    if (!descs.appendFromLength(data, remain)) {
        return false;
    }

    // Get elementary streams descriptions.
    while (remain >= 3) {

        // Read stream type and pid.
        const QtsStreamType type = data[0];
        const QtsPid pid = qFromBigEndian<quint16>(data + 1) & 0x1FFF;
        data += 3;
        remain -= 3;

        // Build a stream entry.
        StreamEntry stream(pid, type);

        // Read the stream-level descriptors.
        if (!stream.descs.appendFromLength(data, remain)) {
            return false;
        }

        // Add the stream description.
        streams << stream;
    }

    // Valid if the complete section was read.
    _isValid = remain == 0;
    return _isValid;
}


//----------------------------------------------------------------------------
// Check if an elementary stream carries audio.
//----------------------------------------------------------------------------

bool QtsProgramMapTable::StreamEntry::isAudio() const
{
    // AC-3 or HE-AAC components may have "PES private data" stream type
    // but are identifier by specific descriptors.

    return qtsIsAudioStreamType(type) ||
        descs.search(QTS_DID_DTS) < descs.size() ||
        descs.search(QTS_DID_AC3) < descs.size() ||
        descs.search(QTS_DID_ENHANCED_AC3) < descs.size() ||
        descs.search(QTS_DID_AAC) < descs.size();
}


//----------------------------------------------------------------------------
// Check if an elementary stream carries video.
//----------------------------------------------------------------------------

bool QtsProgramMapTable::StreamEntry::isVideo() const
{
    return qtsIsVideoStreamType(type);
}


//----------------------------------------------------------------------------
// Check if an elementary stream carries subtitles.
//----------------------------------------------------------------------------

bool QtsProgramMapTable::StreamEntry::isSubtitles() const
{
    // A subtitling descriptor always indicates subtitles.
    if (descs.search(QTS_DID_SUBTITLING) < descs.size()) {
        return true;
    }

    // A teletext descriptor may indicate subtitles.
    for (int index = 0; (index = descs.search(QTS_DID_TELETEXT, index)) < descs.size(); ++index) {
        // Deserialize teletext descriptor.
        const QtsTeletextDescriptor td(*descs[index]);
        if (td.isValid()) {
            // Loop on all language entries, check if teletext type is a subtitle.
            foreach (const QtsTeletextDescriptor::Entry& entry, td.entries) {
                if (entry.type == QTS_TELETEXT_SUBTITLES || entry.type == QTS_TELETEXT_SUBTITLES_HI) {
                    return true; // teletext subtitles types
                }
            }
        }
    }

    // After all, no subtitle here...
    return false;
}
