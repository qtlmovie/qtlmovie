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
//
// Qts, the Qt MPEG Transport Stream library.
// Define the class QtsTsFile.
//
//----------------------------------------------------------------------------

#include "QtsTsFile.h"

namespace {
    //!
    //! Number of TS packet to read to determine the file format.
    //!
    const int QTS_AUTODETECT_PACKETS = 16;
    //!
    //! Required number of matching TS packet to determine the file format.
    //!
    const int QTS_AUTODETECT_MIN_PACKETS = 12;
}


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtsTsFile::QtsTsFile(QObject* parent) :
    QFile(parent),
    _tsFileType(AutoDetect),
    _inBuffer()
{
}

QtsTsFile::QtsTsFile(const QString& name, TsFileType type, QObject* parent) :
    QFile(name, parent),
    _tsFileType(type),
    _inBuffer()
{
}


//----------------------------------------------------------------------------
// Set the TS packet format. Must be called before open().
//----------------------------------------------------------------------------

void QtsTsFile::setTsFileType(const TsFileType& tsFileType)
{
    if (!isOpen()) {
        _tsFileType = tsFileType;
    }
}


//----------------------------------------------------------------------------
// Open the file. Reimplemented from QIODevice.
//----------------------------------------------------------------------------

bool QtsTsFile::open(QIODevice::OpenMode mode)
{
    // Clear unsupported options.
    mode &= ~Text;

    // Open in superclass.
    const bool success = QFile::open(mode);

    // Reset internal state.
    if (success) {
        _inBuffer.clear();
    }

    return success;
}


//----------------------------------------------------------------------------
// Make sure that the internal input buffer contains at least a given number
// of bytes.
//----------------------------------------------------------------------------

bool QtsTsFile::fillBuffer(int size)
{
    // If the buffer needs more data from the file.
    if (_inBuffer.size() < size) {

        // Resize the buffer to accept all requested bytes.
        const int initialSize = _inBuffer.size();
        _inBuffer.resize(size);

        // Read data from the file.
        const qint64 received = QIODevice::read(reinterpret_cast<char*>(_inBuffer.data()) + initialSize, qint64(size - initialSize));
        if (received < 0) {
            // Read error, restore buffer size.
            _inBuffer.resize(initialSize);
            return false;
        }

        // New buffer size.
        _inBuffer.resize(initialSize + int(received));
    }
    return true;
}


//----------------------------------------------------------------------------
// Read enough packets in _inBuffer to determine the packet size.
//----------------------------------------------------------------------------

bool QtsTsFile::autoDetectFileFormat()
{
    // Fill the buffer for auto-detection.
    // Make sure we can read the requested number of largest packets (M2TS).
    fillBuffer(QTS_AUTODETECT_PACKETS * QTS_PKT_M2TS_SIZE);

    // Count matching synchronization bytes for TS and M2TS formats.
    int tsMatch = 0;
    int m2tsMatch = 0;
    for (int i = 0; i < _inBuffer.size(); i += QTS_PKT_SIZE) {
        if (_inBuffer[i] == QTS_SYNC_BYTE) {
            tsMatch++;
        }
    }
    for (int i = QTS_M2TS_HEADER_SIZE; i < _inBuffer.size(); i += QTS_PKT_M2TS_SIZE) {
        if (_inBuffer[i] == QTS_SYNC_BYTE) {
            m2tsMatch++;
        }
    }

    // Guess the format from accumulated values.
    // Get the format with the highest matches and at least the required number of packets.
    if (tsMatch > m2tsMatch && tsMatch >= QTS_AUTODETECT_MIN_PACKETS) {
        _tsFileType = TsFile;
        return true;
    }
    else if (m2tsMatch > tsMatch && m2tsMatch >= QTS_AUTODETECT_MIN_PACKETS) {
        _tsFileType = M2tsFile;
        return true;
    }
    else {
        // Cannot guess, not enough bytes or not a valid TS file.
        return false;
    }
}


//----------------------------------------------------------------------------
// Read as many TS packets as possible from the file.
//----------------------------------------------------------------------------

int QtsTsFile::read(QtsTsPacket* buffer, int maxPacketCount)
{
    // Filter out empty read.
    if (maxPacketCount <= 0) {
        return 0;
    }

    // Perform initial autodetection of packet format.
    if (_tsFileType == AutoDetect && !autoDetectFileFormat()) {
        // Read error or not a valid TS file.
        return -1;
    }

    // Format of packets to read.
    int headerSize = _tsFileType == M2tsFile ? QTS_M2TS_HEADER_SIZE : 0;
    int packetSize = headerSize + QTS_PKT_SIZE;
    int packetCount = 0;

    // Read packets one by one, using the internal buffer as intermediate.
    // Not optimal, especially in the case of plain TS files, but the code is simpler.
    while (packetCount < maxPacketCount) {

        // Make sure at least one packet is in the buffer.
        const bool readOk = fillBuffer(packetSize);

        // If not enough data in the buffer, stop there. Report an error only
        // if an I/O error was detected and no previous packet could be read.
        if (_inBuffer.size() < packetSize) {
            return readOk || packetCount > 0 ? packetCount : -1;
        }

        // Read the TS packet into the user buffer. Skip the optional header.
        ::memcpy(buffer++, _inBuffer.data() + headerSize, QTS_PKT_SIZE);
        packetCount++;

        // Remove the packet from the internal buffer.
        _inBuffer.remove(0, packetSize);
    }
    return packetCount;
}


//----------------------------------------------------------------------------
// Write TS packets to the file.
//----------------------------------------------------------------------------

bool QtsTsFile::write(const QtsTsPacket* buffer, int packetCount)
{
    while (packetCount-- > 0) {
        if (!writeWithTimeStamp(0, buffer++)) {
            return false;
        }
    }
    return true;
}


//----------------------------------------------------------------------------
// Write one TS packet with timestamp to the file.
//----------------------------------------------------------------------------

bool QtsTsFile::writeWithTimeStamp(quint32 timeStamp, const QtsTsPacket* packet)
{
    switch (_tsFileType) {
    case AutoDetect:
    case TsFile: {
        // Directly write the packet, ignore the time stamp.
        return writeRawData(packet, QTS_PKT_SIZE);
    }
    case M2tsFile: {
        // Convert the time stamp to big endian.
        quint8 timeStampBuffer[4];
        qToBigEndian<quint32>(timeStamp, timeStampBuffer);
        // Write the time stamp, then the packet.
        return writeRawData(timeStampBuffer, 4) && writeRawData(packet, QTS_PKT_SIZE);
    }
    default:
        // Invalid file format, error.
        return false;
    }

}


//----------------------------------------------------------------------------
// Write raw data to the file.
//----------------------------------------------------------------------------

bool QtsTsFile::writeRawData(const void* data, int size)
{
    const char* current = reinterpret_cast<const char*>(data);
    qint64 remain = qint64(size);
    while (remain > 0) {
        const qint64 written = QIODevice::write(current, remain);
        if (written <= 0) {
            // Negative means error. Zero means what? Return error to avoid infinite loop.
            return false;
        }
        current += written;
        remain -= written;
    }
    return true;
}
