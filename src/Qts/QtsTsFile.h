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
//! @file QtsTsFile.h
//!
//! Declare the class QtsTsFile.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSTSFILE_H
#define QTSTSFILE_H

#include <QtCore>
#include "QtsTsPacket.h"
#include "QtlByteBlock.h"

//!
//! A subclass of QFile which reads and writes MPEG transport stream packets instead of raw data.
//!
class QtsTsFile : public QFile
{
#if !defined (DOXYGEN)
    Q_OBJECT
#endif

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtsTsFile(QObject* parent = 0);

    //!
    //! Constructor.
    //! @param [in] name File name.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtsTsFile(const QString& name, QObject* parent = 0);

    //!
    //! Open the file.
    //! Reimplemented from QIODevice.
    //! @param [in] mode The mode into which the file shall been open.
    //! @return True on success, false on error.
    //!
    virtual bool open(OpenMode mode = ReadOnly);

    //!
    //! Read as many TS packets as possible from the file.
    //! @param [out] buffer Buffer receiving the TS packets.
    //! @param [in] maxPacketCount Size in @a buffer in number of TS packets.
    //! @return Number of TS packets actually read or negative on error.
    //!
    int read(QtsTsPacket* buffer, int maxPacketCount = 1);

    //!
    //! Write TS packets to the file.
    //! @param [in] buffer Buffer containing the TS packets to write.
    //! @param [in] packetCount Number of TS packets to write from @a buffer.
    //! @return True on success, false on error.
    //!
    bool write(const QtsTsPacket* buffer, int packetCount = 1);

private:
    QtlByteBlock _inBuffer; //!< Buffer for partially read packets.

    // Unaccessible operations.
    QtsTsFile() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtsTsFile)
};

#endif // QTSTSFILE_H
