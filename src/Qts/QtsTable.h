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
//! @file QtsTable.h
//!
//! Declare the class QtsTable.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSTABLE_H
#define QTSTABLE_H

#include "QtsCore.h"
#include "QtsSection.h"
#include "QtlSmartPointer.h"

//!
//! Representation of a binary MPEG PSI/SI table.
//!
//! A table is built by adding sections using addSection().
//! When all sections are present, the table becomes valid.
//!
//! Data from sections are not deeply copied thanks to the implicit
//! sharing mechanism of the Qt containers. Whenever a section is
//! modified, then a copy-on-write occurs.
//!
//! The table_id, version and number of sections is determined when
//! the first section is added. Subsequent sections must have the
//! same properties.
//!
class QtsTable
{
public:
    //!
    //! Default constructor.
    //!
    QtsTable();

    //!
    //! Copy constructor.
    //! Data from sections are not deeply copied thanks to the implicit
    //! sharing mechanism of the Qt containers. Whenever a section is
    //! modified, then a copy-on-write occurs.
    //! @param [in] other Other instance to copy.
    //!
    QtsTable(const QtsTable& other);

    //!
    //! Assignment operator.
    //! Data from sections are not deeply copied thanks to the implicit
    //! sharing mechanism of the Qt containers. Whenever a section is
    //! modified, then a copy-on-write occurs.
    //! @param [in] other Other instance to copy.
    //!
    const QtsTable& operator=(const QtsTable& other);

    //!
    //! Comparison operator.
    //! @param [in] other Other instance to compare.
    //! @return True if the tables are identical, false otherwise.
    //! The source PID's are ignored, only the sections contents are compared.
    //! Note: Invalid tables are never identical.
    //!
    bool operator==(const QtsTable& other) const;

    //!
    //! Difference operator.
    //! @param [in] other Other instance to compare.
    //! @return True if the tables are different, false otherwise.
    //! The source PID's are ignored, only the sections contents are compared.
    //! Note: Invalid tables are never identical.
    //!
    bool operator!=(const QtsTable& other) const
    {
        return !(*this == other);
    }

    //!
    //! Add a section to a table.
    //! @param [in] section The section to add.
    //! @param [in] replace If true, an existing section may be replaced.
    //! Otherwise, replacing an existing section is an error.
    //! @param [in] grow If true, the "last_section_number" of the @a section
    //! may be greater than the "last_section_number" in the table.
    //! In this case, all sections which were previously added in the table are modified.
    //! @return True on success, false if the section could not be added (inconsistent property).
    //!
    bool addSection(const QtsSection& section, bool replace = true, bool grow = true)
    {
        return addSection(QtsSectionPtr(new QtsSection(section)), replace, grow);
    }

    //!
    //! Add a section to a table.
    //! @param [in] section A smart pointer to the section to add. The section is not
    //! copied, only the smart pointer is used. Do not modify the section object from
    //! outside after invoking this method.
    //! @param [in] replace If true, an existing section may be replaced.
    //! Otherwise, replacing an existing section is an error.
    //! @param [in] grow If true, the "last_section_number" of the @a section
    //! may be greater than the "last_section_number" in the table.
    //! In this case, all sections which were previously added in the table are modified.
    //! @return True on success, false if the section could not be added (inconsistent property).
    //!
    bool addSection(const QtsSectionPtr& section, bool replace = true, bool grow = true);

    //!
    //! Check if the table is valid.
    //! @return True if the table is valid, false otherwise;
    //!
    bool isValid() const
    {
        return _isValid;
    }

    //!
    //! Clear the content of the table.
    //! The table must be rebuilt using calls to addSection().
    //!
    void clear();

    //!
    //! Get the table id.
    //! @return The table id.
    //!
    QtsTableId tableId() const
    {
        return _tid;
    }

    //!
    //! Get the table id extension (long sections only).
    //! @return The table id extension (zero for short sections).
    //!
    quint16 tableIdExtension() const
    {
        return _tidExt;
    }

    //!
    //! Get the table version (long sections only).
    //! @return The table version (zero for short sections).
    //!
    quint8 version() const
    {
        return _version;
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
    //! Set the PID from which the table was collected (informational only).
    //! @param [in] pid The PID from which the table was collected.
    //!
    void setSourcePid(QtsPid pid);

    //!
    //! Get the index of first TS packet of the table in the demultiplexed stream.
    //! @return The index of first TS packet of the table.
    //!
    QtsPacketCounter getFirstTsPacketIndex() const;

    //!
    //! Get the index of last TS packet of the table in the demultiplexed stream.
    //! @return The index of last TS packet of the table.
    //!
    QtsPacketCounter getLastTsPacketIndex() const;

    //!
    //! Get the number of sections in the table.
    //! @return The number of sections in the table.
    //!
    int sectionCount() const
    {
        return _sections.size ();
    }

    //!
    //! Get the total size in bytes of all sections in the table.
    //! @return The total size in bytes of all sections in the table.
    //!
    int totalSize() const;

    //!
    //! Get a smart pointer to a section in the table.
    //! @param [in] index Section number.
    //! @return A pointer to the section in the table or zero if not present.
    //! The pointed object shall not be modified.
    //!
    QtsSectionPtr sectionAt(int index) const;

private:
    //!< Vector of smart pointers to sections.
    typedef QVector<QtsSectionPtr> SectionVector;

    bool          _isValid;      //!< Content of _data is a valid section.
    QtsTableId    _tid;          //!< Table id.
    quint16       _tidExt;       //!< Table id extension.
    quint8        _version;      //!< Table version.
    QtsPid        _sourcePid;    //!< Source PID (informational).
    int           _missingCount; //!< Number of missing sections.
    SectionVector _sections;     //!< Vector of sections.
};

//!
//! Smart pointer to a QtsTable (non thread-safe).
//!
typedef QtlSmartPointer<QtsTable,QtlNullMutexLocker> QtsTablePtr;

//!
//! Vector of smart pointers to QtsTable (non thread-safe).
//!
typedef QVector<QtsTablePtr> QtsTablePtrVector;

//!
//! Smart pointer to a QtsTable (thread-safe).
//!
typedef QtlSmartPointer<QtsTable,QtlMutexLocker> QtsTableSafePtr;

//!
//! Vector of smart pointers to QtsTable (thread-safe).
//!
typedef QVector<QtsTableSafePtr> QtsTableSafePtrVector;

#endif // QTSTABLE_H
