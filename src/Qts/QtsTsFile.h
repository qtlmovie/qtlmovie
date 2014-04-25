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
    Q_OBJECT

public:
    //!
    //! Describe the packet format inside a TS file.
    //!
    enum TsFileType {
        AutoDetect,  //!< Automatically detect on input, same as TsFile on output.
        TsFile,      //!< Standard MPEG-2 Transport Stream file, 188 bytes per TS packet.
        M2tsFile     //!< M2TS file (BluRay and some IP-TV), 4-byte timestamp followed by 188-byte TS packet.
    };

    //!
    //! Constructor.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtsTsFile(QObject* parent = 0);

    //!
    //! Constructor.
    //! @param [in] name File name.
    //! @param [in] type TS packet format.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtsTsFile(const QString& name, TsFileType type = AutoDetect, QObject* parent = 0);

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
    //! If the file format is M2tsFile, a zero 4-byte timestamp is inserted before each packet.
    //! @param [in] packetCount Number of TS packets to write from @a buffer.
    //! @return True on success, false on error.
    //!
    bool write(const QtsTsPacket* buffer, int packetCount = 1);

    //!
    //! Write one TS packet with timestamp to the file.
    //! @param [in] packet TS packet to write.
    //! @param [in] timeStamp Time stamp to write (will be converted to big endian before writing).
    //! Ignored if the file format is not M2tsFile.
    //! @return True on success, false on error.
    //!
    bool writeWithTimeStamp(quint32 timeStamp, const QtsTsPacket* packet);

    //!
    //! Get the TS packet format.
    //! If initially set to AutoDetect on an input file, the returned value
    //! will be either TsFile or M2tsFile after reading the first packet.
    //! @return The TS packet format.
    //!
    TsFileType tsFileType() const
    {
        return _tsFileType;
    }

    //!
    //! Set the TS packet format.
    //! Must be called before open().
    //! @return The TS packet format.
    //!
    void setTsFileType(const TsFileType& tsFileType);

private:
    TsFileType   _tsFileType; //!< Packet format.
    QtlByteBlock _inBuffer;   //!< Buffer for partially read packets or initial auto-detection.

    //!
    //! Read enough packets in _inBuffer to determine the packet size.
    //! @return True on success, false on error. When true is returned,
    //! it is guaranteed that _tsFileType is no longer AutoDetect.
    //!
    bool autoDetectFileFormat();

    //!
    //! Make sure that the internal input buffer contains at least a given number of bytes.
    //! Read input file if necessary.
    //! @param size Requested byte count in buffer.
    //! @return False on read error. When true, the target number of bytes may not be
    //! reached if no data is currently available from the file.
    //!
    bool fillBuffer(int size);

    //!
    //! Write raw data to the file.
    //! @param [in] data Address of data to write.
    //! @param [in] size Number of bytes to write from @a data.
    //! @return True on success, false on error.
    //!
    bool writeRawData(const void* data, int size);

    // Unaccessible operations.
    QtsTsFile() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtsTsFile)
};

#endif // QTSTSFILE_H
