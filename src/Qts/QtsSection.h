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
//! @file QtsSection.h
//!
//! Declare the class QtsSection.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSSECTION_H
#define QTSSECTION_H

#include "QtsCore.h"
#include "QtsCrc32.h"
#include "QtlByteBlock.h"
#include "QtlSmartPointer.h"

//!
//! Representation of a binary MPEG PSI/SI section.
//!
//! Data from sections are not deeply copied thanks to the implicit
//! sharing mechanism of the Qt containers. Whenever a section is
//! modified, then a copy-on-write occurs.
//!
//! General note: What to do with the CRC32 when building a section depends on the
//! parameter named @a validation of type Qts::Crc32 :
//!
//! - Ignore:  Neither check nor compute the CRC32.
//! - Check:   Validate the CRC32 from the section data. Mark the section as invalid if the CRC is incorrect.
//! - Compute: Compute the CRC32 and store it in the section.
//!
//! Typically, if the data come from the wire, use Check.
//! If the data are built by the application, use Compute,
//!
class QtsSection
{
public:
    //!
    //! Default constructor.
    //! Section is initially marked invalid.
    //!
    QtsSection()
    {
        initialize(QTS_PID_NULL);
    }

    //!
    //! Copy constructor.
    //! Data from sections are not deeply copied thanks to the implicit
    //! sharing mechanism of the Qt containers. Whenever a section is
    //! modified, then a copy-on-write occurs.
    //! @param [in] other Other instance to copy.
    //!
    QtsSection(const QtsSection& other);

    //!
    //! Constructor from full binary content.
    //! The content is copied into the section if valid.
    //! @param [in] addr Content address.
    //! @param [in] size Content size.
    //! @param [in] pid The PID from which the section was collected.
    //! @param [in] validation Type of validation to apply to the CRC32.
    //!
    QtsSection(const void* addr,
               int size,
               QtsPid pid = QTS_PID_NULL,
               QtsCrc32::Validation validation = QtsCrc32::Ignore)
    {
        initialize(QtlByteBlock(addr, size), pid, validation);
    }

    //!
    //! Constructor from full binary content.
    //! The content is copied into the section if valid.
    //! @param [in] content Binary content.
    //! @param [in] pid The PID from which the section was collected.
    //! @param [in] validation Type of validation to apply to the CRC32.
    //!
    QtsSection(const QtlByteBlock& content,
               QtsPid pid = QTS_PID_NULL,
               QtsCrc32::Validation validation = QtsCrc32::Ignore)
    {
        initialize(content, pid, validation);
    }

    //!
    //! Constructor from short section payload.
    //! @param [in] tid Table id.
    //! @param [in] isPrivateSection True if this is a "private" (ie. non-MPEG-defined) section.
    //! @param [in] payload Address of payload content.
    //! @param [in] payloadSize Size  of payload content.
    //! @param [in] pid The PID from which the section was collected.
    //!
    QtsSection(QtsTableId tid,
               bool isPrivateSection,
               const void* payload,
               int payloadSize,
               QtsPid pid = QTS_PID_NULL)
    {
        reload(tid, isPrivateSection, payload, payloadSize, pid);
    }

    //!
    //! Constructor from long section payload.
    //! The provided payload does not contain the CRC32.
    //! The CRC32 is automatically computed.
    //! @param [in] tid Table id.
    //! @param [in] isPrivateSection True if this is a "private" (ie. non-MPEG-defined) section.
    //! @param [in] tidExt Table id extension.
    //! @param [in] version Section version.
    //! @param [in] isCurrent True if "current" table (ie. not next).
    //! @param [in] sectionNumber This section number inside table.
    //! @param [in] lastSectionNumber Last section number inside table.
    //! @param [in] payload Address of payload content.
    //! @param [in] payloadSize Size  of payload content.
    //! @param [in] pid The PID from which the section was collected.
    //!
    QtsSection(QtsTableId tid,
               bool isPrivateSection,
               quint16 tidExt,
               quint8 version,
               bool isCurrent,
               quint8 sectionNumber,
               quint8 lastSectionNumber,
               const void* payload,
               int payloadSize,
               QtsPid pid = QTS_PID_NULL)
    {
        reload(tid, isPrivateSection, tidExt, version, isCurrent, sectionNumber, lastSectionNumber, payload, payloadSize, pid);
    }

    //!
    //! Reload full binary content.
    //! The content is copied into the section if valid.
    //! @param [in] addr Content address.
    //! @param [in] size Content size.
    //! @param [in] pid The PID from which the section was collected.
    //! @param [in] validation Type of validation to apply to the CRC32.
    //!
    void reload(const void* addr,
                int size,
                QtsPid pid = QTS_PID_NULL,
                QtsCrc32::Validation validation = QtsCrc32::Ignore)
    {
        initialize(QtlByteBlock(addr, size), pid, validation);
    }

    //!
    //! Reload full binary content.
    //! The content is copied into the section if valid.
    //! @param [in] content Binary content.
    //! @param [in] pid The PID from which the section was collected.
    //! @param [in] validation Type of validation to apply to the CRC32.
    //!
    void reload(const QtlByteBlock& content,
                QtsPid pid = QTS_PID_NULL,
                QtsCrc32::Validation validation = QtsCrc32::Ignore)
    {
        initialize(content, pid, validation);
    }

    //!
    //! Reload short section
    //! @param [in] tid Table id.
    //! @param [in] isPrivateSection True if this is a "private" (ie. non-MPEG-defined) section.
    //! @param [in] payload Address of payload content.
    //! @param [in] payloadSize Size  of payload content.
    //! @param [in] pid The PID from which the section was collected.
    //!
    void reload(QtsTableId tid,
                bool isPrivateSection,
                const void* payload,
                int payloadSize,
                QtsPid pid = QTS_PID_NULL);

    //!
    //! Reload long section.
    //! The provided payload does not contain the CRC32.
    //! The CRC32 is automatically computed.
    //! @param [in] tid Table id.
    //! @param [in] isPrivateSection True if this is a "private" (ie. non-MPEG-defined) section.
    //! @param [in] tidExt Table id extension.
    //! @param [in] version Section version.
    //! @param [in] isCurrent True if "current" table (ie. not next).
    //! @param [in] sectionNumber This section number inside table.
    //! @param [in] lastSectionNumber Last section number inside table.
    //! @param [in] payload Address of payload content.
    //! @param [in] payloadSize Size  of payload content.
    //! @param [in] pid The PID from which the section was collected.
    //!
    void reload(QtsTableId tid,
                bool isPrivateSection,
                quint16 tidExt,
                quint8 version,
                bool isCurrent,
                quint8 sectionNumber,
                quint8 lastSectionNumber,
                const void* payload,
                int payloadSize,
                QtsPid pid = QTS_PID_NULL);

    //!
    //! Clear section content.
    //! Becomes an invalid section.
    //!
    void clear();

    //!
    //! Assignment operator.
    //! Data from sections are not deeply copied thanks to the implicit
    //! sharing mechanism of the Qt containers. Whenever a section is
    //! modified, then a copy-on-write occurs.
    //! @param [in] other Other instance to copy.
    //!
    const QtsSection& operator=(const QtsSection& other);

    //!
    //! Check if a section has valid content
    //! @return True if the section is valid.
    //!
    bool isValid() const
    {
        return _isValid;
    }

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if the sections are identical, false otherwise.
    //! The source PID's are ignored, only the section contents are compared.
    //! Note: Invalid sections are never identical
    //!
    bool operator==(const QtsSection& other) const;

    //!
    //! Difference operator.
    //! @param [in] other Other instance to compare.
    //! @return True if the sections are different, false otherwise.
    //! The source PID's are ignored, only the section contents are compared.
    //! Note: Invalid sections are never identical
    //!
    bool operator!=(const QtsSection& other) const
    {
        return !(*this == other);
    }

    //!
    //! Get the table id.
    //! @return The table id.
    //!
    QtsTableId tableId() const
    {
        return _isValid ? _data[0] : 0xFF;
    }

    //!
    //! Check if the section has a long header.
    //! @return True if the section has a long header.
    //!
    bool isLongSection() const
    {
        return _isValid ? (_data[1] & 0x80) != 0 : false;
    }

    //!
    //! Check if the section has a short header.
    //! @return True if the section has a short header.
    //!
    bool isShortSection() const
    {
        return _isValid ? (_data[1] & 0x80) == 0 : false;
    }

    //!
    //! Check if the section is private (ie not-MPEG-defined).
    //! @return True if the section is private.
    //!
    bool isPrivateSection() const
    {
        return _isValid ? (_data[1] & 0x40) != 0 : false;
    }

    //!
    //! Get the table id extension (long sections only).
    //! @return The table id extension (zero for short sections).
    //!
    quint16 tableIdExtension() const
    {
        return isLongSection() ? qFromBigEndian<quint16>(&_data[3]) : 0;
    }

    //!
    //! Get the table version (long sections only).
    //! @return The table version (zero for short sections).
    //!
    quint8 version() const
    {
        return isLongSection() ? ((_data[5] >> 1) & 0x1F) : 0;
    }

    //!
    //! Check if the section is "current" (long sections only).
    //! @return True if the section is "current" (false for short sections).
    //!
    bool isCurrent() const
    {
        return isLongSection() ? (_data[5] & 0x01) != 0 : false;
    }

    //!
    //! Check if the section is "next" (long sections only).
    //! @return True if the section is "next" (false for short sections).
    //!
    bool isNext() const
    {
        return isLongSection() ? (_data[5] & 0x01) == 0 : false;
    }

    //!
    //! Get the section number (long sections only).
    //! @return The section number (zero for short sections).
    //!
    quint8 sectionNumber() const
    {
        return isLongSection() ? _data[6] : 0;
    }

    //!
    //! Get the last section number in the table (long sections only).
    //! @return The section number (zero for short sections).
    //!
    quint8 lastSectionNumber() const
    {
        return isLongSection() ? _data[7] : 0;
    }

    //!
    //! Get the extended table id.
    //! @return The extended table id.
    //!
    QtsExtTableId etid() const
    {
        return isLongSection() ? QtsExtTableId(tableId(), tableIdExtension()) : QtsExtTableId(tableId());
    }

    //!
    //! Get the PID from which the section was collected.
    //! @return The PID from which the section was collected.
    //!
    QtsPid sourcePid() const
    {
        return _sourcePid;
    }

    //!
    //! Get the address of the full binary content of the section.
    //! @return The address of the full binary content of the section.
    //! Do not modify the content. May be invalidated after modification in section.
    //!
    const quint8* content() const
    {
        return _data.data();
    }

    //!
    //! Get the size of the full binary content of the section.
    //! @return The size of the full binary content of the section.
    //!
    int size() const
    {
        return _data.size();
    }

    //!
    //! Get the size of the section header.
    //! @return The size of the section header.
    //!
    int headerSize() const
    {
        return _isValid ? (isLongSection() ? QTS_LONG_SECTION_HEADER_SIZE : QTS_SHORT_SECTION_HEADER_SIZE) : 0;
    }

    //!
    //! Get the address of the section payload.
    //! For short sections, the payload starts after the
    //! private_section_length field. For long sections, the payload
    //! starts after the last_section_number field and ends before the CRC32 field.
    //! @return The address of the section payload.
    //! Do not modify the content. May be invalidated after modification in section.
    //!
    const quint8* payload() const
    {
        return _isValid ? _data.data() + (isLongSection() ? QTS_LONG_SECTION_HEADER_SIZE : QTS_SHORT_SECTION_HEADER_SIZE) : 0;
    }

    //!
    //! Get the size of the section payload.
    //! For short sections, the payload starts after the
    //! private_section_length field. For long sections, the payload
    //! starts after the last_section_number field and ends before the CRC32 field.
    //! @return The size of the section payload.
    //!
    int payloadSize() const
    {
        return _isValid ? _data.size() - (isLongSection() ? QTS_LONG_SECTION_HEADER_SIZE + QTS_SECTION_CRC32_SIZE : QTS_SHORT_SECTION_HEADER_SIZE) : 0;
    }

    //!
    //! Get the minimum number of TS packets required to transport the section.
    //! @return The minimum number of TS packets required to transport the section.
    //!
    QtsPacketCounter packetCount() const
    {
        return qtsSectionPacketCount(size());
    }

    //!
    //! Modify the table id extension.
    //! @param [in] tidExt New table id extension.
    //! @param [in] recomputeCrc If true, recompute the CRC32 of the section.
    //!
    void setTableIdExtension(quint16 tidExt, bool recomputeCrc = true);

    //!
    //! Modify the table version.
    //! @param [in] version New table version.
    //! @param [in] recomputeCrc If true, recompute the CRC32 of the section.
    //!
    void setVersion(quint8 version, bool recomputeCrc = true);

    //!
    //! Modify the "current" flag.
    //! @param [in] isCurrent New "current" flag.
    //! @param [in] recomputeCrc If true, recompute the CRC32 of the section.
    //!
    void setIsCurrent(bool isCurrent, bool recomputeCrc = true);

    //!
    //! Modify the section number.
    //! @param [in] num New section number.
    //! @param [in] recomputeCrc If true, recompute the CRC32 of the section.
    //!
    void setSectionNumber(quint8 num, bool recomputeCrc = true);

    //!
    //! Modify the last section number in the table.
    //! @param [in] num New last section number in the table.
    //! @param [in] recomputeCrc If true, recompute the CRC32 of the section.
    //!
    void setLastSectionNumber(quint8 num, bool recomputeCrc = true);

    //!
    //! Set the PID from which the section was collected (informational only).
    //! @param [in] pid The PID from which the section was collected.
    //!
    void setSourcePid(QtsPid pid)
    {
        _sourcePid = pid;
    }

    //!
    //! Get the index of first TS packet of the section in the demultiplexed stream.
    //! @return The index of first TS packet of the section.
    //!
    QtsPacketCounter getFirstTsPacketIndex() const
    {
        return _firstPkt;
    }

    //!
    //! Get the index of last TS packet of the section in the demultiplexed stream.
    //! @return The index of last TS packet of the section.
    //!
    QtsPacketCounter getLastTsPacketIndex() const
    {
        return _lastPkt;
    }

    //!
    //! Set the index of first TS packet of the section in the demultiplexed stream.
    //! @param [in] i The index of first TS packet of the section.
    //!
    void setFirstTsPacketIndex(QtsPacketCounter i)
    {
        _firstPkt = i;
    }

    //!
    //! Set the index of last TS packet of the section in the demultiplexed stream.
    //! @param [in] i The index of last TS packet of the section.
    //!
    void setLastTsPacketIndex(QtsPacketCounter i)
    {
        _lastPkt = i;
    }

    //!
    //! Recomputes and replaces the CRC32 of the section.
    //!
    void recomputeCrc32();

private:
    bool             _isValid;   //!< Content of _data is a valid section.
    QtsPid           _sourcePid; //!< Source PID (informational).
    QtsPacketCounter _firstPkt;  //!< Index of first packet in stream (informational).
    QtsPacketCounter _lastPkt;   //!< Index of last packet in stream (informational).
    QtlByteBlock     _data;      //!< Full binary content of the section.

    //!
    //! Initialize the object instance, helper for constructors.
    //! @param [in] pid The PID from which the section was collected.
    //!
    void initialize(QtsPid pid);

    //!
    //! Initialize the object instance, helper for constructors.
    //! @param [in] content Binary content.
    //! @param [in] pid The PID from which the section was collected.
    //! @param [in] validation Type of validation to apply to the CRC32.
    //!
    void initialize(const QtlByteBlock& content, QtsPid pid, QtsCrc32::Validation validation);
};

//!
//! Smart pointer to a QtsSection (non thread-safe).
//!
typedef QtlSmartPointer<QtsSection,QtlNullMutexLocker> QtsSectionPtr;

//!
//! Vector of smart pointers to QtsSection (non thread-safe).
//!
typedef QVector<QtsSectionPtr> QtsSectionPtrVector;

//!
//! Smart pointer to a QtsSection (thread-safe).
//!
typedef QtlSmartPointer<QtsSection,QtlMutexLocker> QtsSectionSafePtr;

//!
//! Vector of smart pointers to QtsSection (thread-safe).
//!
typedef QVector<QtsSectionSafePtr> QtsSectionSafePtrVector;

#endif // QTSSECTION_H
