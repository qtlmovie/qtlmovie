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
//! @file QtsProgramMapTable.h
//!
//! Declare the class QtsProgramMapTable.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSPROGRAMMAPTABLE_H
#define QTSPROGRAMMAPTABLE_H

#include "QtsAbstractLongTable.h"
#include "QtsDescriptorList.h"

//!
//! The MPEG-defined Program Map Table (PMT).
//!
class QtsProgramMapTable : public QtsAbstractLongTable
{
public:
    //!
    //! Describes a stream entry in the PMT.
    //!
    struct StreamEntry
    {
        QtsPid             pid;    //!< PID of the stream.
        QtsStreamType      type;   //!< Type of the stream.
        QtsDescriptorList  descs;  //!< Stream-level descriptor list
        //!
        //! Constructor.
        //! @param [in] p PID of the stream.
        //! @param [in] t Stream type.
        //!
        StreamEntry(QtsPid p = QTS_PID_NULL, QtsStreamType t = 0) :
            pid(p),
            type(t),
            descs()
        {
        }
        //!
        //! Check if an elementary stream carries audio.
        //! Does not just look at the stream type, also analyzes the descriptor list for additional information.
        //! @return True if the stream carries audio.
        //!
        bool isAudio() const;
        //!
        //! Check if an elementary stream carries video.
        //! Does not just look at the stream type, also analyzes the descriptor list for additional information.
        //! @return True if the stream carries video.
        //!
        bool isVideo() const;
        //!
        //! Check if an elementary stream carries subtitles.
        //! Does not just look at the stream type, also analyzes the descriptor list for additional information.
        //! @return True if the stream carries subtitles.
        //!
        bool isSubtitles() const;
    };

    //!
    //! Describes the list of stream in the PMT.
    //!
    typedef QList<StreamEntry> StreamList;

    //
    // The PMT is a simple table.
    // Its content is self-consistent and has public fields.
    //
    quint16           serviceId;  //!< Service id aka "program_number".
    QtsPid            pcrPid;     //!< PID for PCR data.
    QtsDescriptorList descs;      //!< Program-level descriptor list.
    StreamList        streams;    //!< List of elementary streams in the service.

    //!
    //! Default constructor.
    //! @param [in] version Table version.
    //! @param [in] isCurrent True if the table is "current", false if it is "next".
    //! @param [in] sid Service id.
    //! @param [in] pcr PID for PCR data.
    //!
    QtsProgramMapTable(quint8 version = 0,
                       bool isCurrent = true,
                       quint16 sid = 0,
                       QtsPid pcr = QTS_PID_NULL);

    //!
    //! Constructor from a binary table.
    //! @param [in] table The table to deserialize.
    //!
    QtsProgramMapTable(const QtsTable& table);

    //!
    //! This abstract method serializes a table.
    //! Implementation required by QtsAbstractTable.
    //! @param [out] table Replaced with a binary representation of this object.
    //! @return True on success, false on error (invalid table, data too large, etc.)
    //!
    virtual bool serialize(QtsTable& table) const Q_DECL_OVERRIDE;

    //!
    //! This abstract method deserializes a binary table.
    //! Implementation required by QtsAbstractTable.
    //! @param [out] table Binary table to deserialize.
    //! @return True on success, false on error (invalid data, etc.)
    //!
    virtual bool deserialize(const QtsTable& table) Q_DECL_OVERRIDE;
};

#endif // QTSPROGRAMMAPTABLE_H
