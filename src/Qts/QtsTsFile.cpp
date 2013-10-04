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


//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

QtsTsFile::QtsTsFile(QObject* parent) :
    QFile(parent),
    _inBuffer()
{
}

QtsTsFile::QtsTsFile(const QString& name, QObject* parent) :
    QFile(name, parent),
    _inBuffer()
{
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
// Read as many TS packets as possible from the file.
//----------------------------------------------------------------------------

int QtsTsFile::read(QtsTsPacket* buffer, int maxPacketCount)
{
    // Filter out empty read.
    if (maxPacketCount <= 0) {
        return 0;
    }

    // Copy buffered partial packet.
    const int initialSize = _inBuffer.size();
    Q_ASSERT(initialSize >= 0);
    Q_ASSERT(initialSize < QTS_PKT_SIZE);
    ::memcpy(buffer, _inBuffer.data(), initialSize);

    // Maximum size to read.
    const qint64 maxRead = qint64(maxPacketCount) * QTS_PKT_SIZE - initialSize;
    const qint64 received = QIODevice::read(reinterpret_cast<char*>(buffer) + initialSize, maxRead);
    if (received <= 0) {
        // Read error.
        return received;
    }

    // Copy back the partial buffer (if any partial TS packet is present).
    const int packetCount = int((initialSize + received) / QTS_PKT_SIZE);
    const int residue = int((initialSize + received) % QTS_PKT_SIZE);
    _inBuffer.resize(residue);
    if (residue > 0) {
        ::memcpy(_inBuffer.data(), buffer + packetCount, residue);
    }
    return packetCount;
}


//----------------------------------------------------------------------------
// Write TS packets to the file.
//----------------------------------------------------------------------------

bool QtsTsFile::write(const QtsTsPacket* buffer, int packetCount)
{
    const char* current = reinterpret_cast<const char*>(buffer);
    qint64 remain = qint64(packetCount) * QTS_PKT_SIZE;
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
