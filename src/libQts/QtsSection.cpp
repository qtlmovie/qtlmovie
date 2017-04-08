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
// Define the class QtsSection.
//
//----------------------------------------------------------------------------

#include "QtsSection.h"


//-----------------------------------------------------------------------------
// Copy constructor.
//-----------------------------------------------------------------------------

QtsSection::QtsSection(const QtsSection& other) :
    _isValid(other._isValid),
    _sourcePid(other._sourcePid),
    _firstPkt(other._firstPkt),
    _lastPkt(other._lastPkt),
    _data(other._data)
{
}


//----------------------------------------------------------------------------
// Clear section content. Becomes invalid sections.
//----------------------------------------------------------------------------

void QtsSection::clear()
{
    _isValid = false;
    _sourcePid = QTS_PID_NULL;
    _firstPkt = 0;
    _lastPkt = 0;
    _data.clear();
}


//-----------------------------------------------------------------------------
// Private method: Helper for constructors.
//-----------------------------------------------------------------------------

void QtsSection::initialize(QtsPid pid)
{
    _isValid = false;
    _sourcePid = pid;
    _firstPkt = 0;
    _lastPkt = 0;
    _data.clear();
}


//-----------------------------------------------------------------------------
// Private method: Helper for constructors.
//-----------------------------------------------------------------------------

void QtsSection::initialize(const QtlByteBlock& content, QtsPid pid, QtsCrc32::Validation validation)
{
    // Reset content.
    initialize(pid);
    _data = content;

    // Basic check, for min and max section size
    _isValid = _data.size() >= QTS_MIN_SHORT_SECTION_SIZE && _data.size() <= QTS_MAX_PRIVATE_SECTION_SIZE;

    // Extract short section header info
    if (_isValid) {
        const quint16 length = qFromBigEndian<quint16>(&_data[1]) & 0x0FFF;
        _isValid = length == _data.size() - 3;
    }

    // Extract long section header info
    const bool isLongSection = _isValid && (_data[1] & 0x80) != 0 && _data.size() >= QTS_MIN_LONG_SECTION_SIZE;
    if (isLongSection) {
        const quint8 sectionNumber = _data[6];
        const quint8 lastSectionNumber = _data[7];
        _isValid = sectionNumber <= lastSectionNumber;
    }

    // Check CRC32 if required
    if (isLongSection && _isValid) {
        const int size = _data.size() - QTS_SECTION_CRC32_SIZE;
        switch (validation) {
        case QtsCrc32::Check:
            _isValid = QtsCrc32(_data.data(), size) == qFromBigEndian<quint32>(&_data[size]);
            break;
        case QtsCrc32::Compute:
            recomputeCrc32();
            break;
        case QtsCrc32::Ignore:
            break;
        }
    }

    // Reset content if invalid.
    if (!_isValid) {
        _data.clear();
    }
}


//-----------------------------------------------------------------------------
// Reload short section
//-----------------------------------------------------------------------------

void QtsSection::reload(QtsTableId tid,
                        bool isPrivateSection,
                        const void* payload,
                        int payloadSize,
                        QtsPid pid)
{
    initialize(pid);
    _isValid = QTS_SHORT_SECTION_HEADER_SIZE + payloadSize <= QTS_MAX_PRIVATE_SECTION_SIZE;
    if (_isValid) {
        _data.appendUInt8(tid);
        _data.appendUInt16((isPrivateSection ? 0x4000 : 0x0000) | 0x3000 | quint16 (payloadSize & 0x0FFF));
        _data.append(payload, payloadSize);
    }
}


//-----------------------------------------------------------------------------
// Reload long section.
//-----------------------------------------------------------------------------

void QtsSection::reload(QtsTableId tid,
                        bool isPrivateSection,
                        quint16 tidExt,
                        quint8 version,
                        bool isCurrent,
                        quint8 sectionNumber,
                        quint8 lastSectionNumber,
                        const void* payload,
                        int payloadSize,
                        QtsPid pid)
{
    initialize(pid);
    _isValid = sectionNumber <= lastSectionNumber &&
            version <= 31 &&
            QTS_LONG_SECTION_HEADER_SIZE + payloadSize + QTS_SECTION_CRC32_SIZE <= QTS_MAX_PRIVATE_SECTION_SIZE;
    if (_isValid) {
        _data.appendUInt8(tid);
        _data.appendUInt16(0x8000 |
                           (isPrivateSection ? 0x4000 : 0x0000) |
                           0x3000 |
                           quint16((QTS_LONG_SECTION_HEADER_SIZE - 3 + payloadSize + QTS_SECTION_CRC32_SIZE) & 0x0FFF));
        _data.appendUInt16(tidExt);
        _data.appendUInt8(0xC0 | ((version & 0x1F) << 1) | (isCurrent ? 0x01 : 0x00));
        _data.appendUInt8(sectionNumber);
        _data.appendUInt8(lastSectionNumber);
        _data.append(payload, payloadSize);
        _data.appendUInt32(0); // CRC32 placeholder
        recomputeCrc32();
    }
}


//-----------------------------------------------------------------------------
// Assignment.
//-----------------------------------------------------------------------------

const QtsSection& QtsSection::operator=(const QtsSection& other)
{
    if (&other != this) {
        _isValid = other._isValid;
        _sourcePid = other._sourcePid;
        _firstPkt = other._firstPkt;
        _lastPkt = other._lastPkt;
        _data = other._data;
    }
    return *this;
}


//-----------------------------------------------------------------------------
// Comparison. Note: Invalid sections are never identical
//-----------------------------------------------------------------------------

bool QtsSection::operator==(const QtsSection& other) const
{
    return _isValid && other._isValid && _data == other._data;
}


//-----------------------------------------------------------------------------
// This method recomputes and replaces the CRC32 of the section.
//-----------------------------------------------------------------------------

void QtsSection::recomputeCrc32()
{
    if (isLongSection()) {
        const int size = _data.size() - QTS_SECTION_CRC32_SIZE;
        qToBigEndian<quint32>(QtsCrc32(_data.data(), size).value(), _data.data() + size);
    }
}


//-----------------------------------------------------------------------------
// Modifiable properties.
//-----------------------------------------------------------------------------

void QtsSection::setTableIdExtension(quint16 tidExt, bool recomputeCrc)
{
    if (isLongSection()) {
        qToBigEndian<quint16>(tidExt, _data.data() + 3);
        if (recomputeCrc) {
            recomputeCrc32();
        }
    }
}

void QtsSection::setVersion(quint8 version, bool recomputeCrc)
{
    if (isLongSection()) {
        _data[5] = (_data[5] & 0xC1) | ((version & 0x1F) << 1);
        if (recomputeCrc) {
            recomputeCrc32();
        }
    }
}

void QtsSection::setIsCurrent(bool isCurrent, bool recomputeCrc)
{
    if (isLongSection()) {
        _data[5] = (_data[5] & 0xFE) | (isCurrent ? 0x01 : 0x00);
        if (recomputeCrc) {
            recomputeCrc32();
        }
    }
}

void QtsSection::setSectionNumber(quint8 num, bool recomputeCrc)
{
    if (isLongSection()) {
        _data[6] = num;
        if (recomputeCrc) {
            recomputeCrc32();
        }
    }
}

void QtsSection::setLastSectionNumber(quint8 num, bool recomputeCrc)
{
    if (isLongSection()) {
        _data[7] = num;
        if (recomputeCrc) {
            recomputeCrc32();
        }
    }
}
