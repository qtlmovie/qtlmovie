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
// Define the class QtsTable.
//
//----------------------------------------------------------------------------

#include "QtsTable.h"


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtsTable::QtsTable() :
    _isValid(false),
    _tid(0xFF),
    _tidExt(0),
    _version(0),
    _sourcePid(QTS_PID_NULL),
    _missingCount(0),
    _sections()
{
}


//----------------------------------------------------------------------------
// Copy constructor.
//----------------------------------------------------------------------------

QtsTable::QtsTable(const QtsTable& other) :
    _isValid(other._isValid),
    _tid(other._tid),
    _tidExt(other._tidExt),
    _version(other._version),
    _sourcePid(other._sourcePid),
    _missingCount(other._missingCount),
    _sections()
{
    // Copy the sections.
    _sections.resize(other._sections.size());
    for (int i = 0; i < _sections.size(); ++i) {
        if (other._sections[i].isNull()) {
            _sections[i].clear();
        }
        else {
            _sections[i] = new QtsSection(*(other._sections[i]));
        }
    }
}


//----------------------------------------------------------------------------
// Assignment.
//----------------------------------------------------------------------------

const QtsTable& QtsTable::operator=(const QtsTable& other)
{
    if (&other != this) {
        _isValid = other._isValid;
        _tid = other._tid;
        _tidExt = other._tidExt;
        _version = other._version;
        _sourcePid = other._sourcePid;
        _missingCount = other._missingCount;
        _sections.resize(other._sections.size());
        for (int i = 0; i < _sections.size(); ++i) {
            if (other._sections[i].isNull()) {
                _sections[i].clear();
            }
            else {
                _sections[i] = new QtsSection(*(other._sections[i]));
            }
        }
    }
    return *this;
}


//----------------------------------------------------------------------------
// Comparison. Note: Invalid tables are never identical
//----------------------------------------------------------------------------

bool QtsTable::operator==(const QtsTable& other) const
{
    bool equal =
        _isValid &&
        other._isValid &&
        _tid == other._tid &&
        _tidExt == other._tidExt &&
        _version == other._version &&
        _sections.size() == other._sections.size();

    for (int i = 0; equal && i < _sections.size(); ++i) {
        equal = !_sections[i].isNull() && !other._sections[i].isNull() && *_sections[i] == *other._sections[i];
    }

    return equal;
}


//----------------------------------------------------------------------------
// Modifiable properties.
//----------------------------------------------------------------------------

void QtsTable::setTableIdExtension(quint16 tidExt, bool recomputeCrc)
{
    _tidExt = tidExt;
    foreach (const QtsSectionPtr section, _sections) {
        if (!section.isNull()) {
            section->setTableIdExtension(tidExt, recomputeCrc);
        }
    }
}

void QtsTable::setVersion(quint8 version, bool recomputeCrc)
{
    _version = version;
    foreach (const QtsSectionPtr section, _sections) {
        if (!section.isNull()) {
            section->setVersion(version, recomputeCrc);
        }
    }
}

void QtsTable::setSourcePid(QtsPid pid)
{
    _sourcePid = pid;
    foreach (const QtsSectionPtr section, _sections) {
        if (!section.isNull()) {
            section->setSourcePid(pid);
        }
    }
}


//----------------------------------------------------------------------------
// Index of first and last TS packet of the table in the demultiplexed stream.
//----------------------------------------------------------------------------

QtsPacketCounter QtsTable::getFirstTsPacketIndex() const
{
    bool found = false;
    QtsPacketCounter first = 0;
    foreach (const QtsSectionPtr section, _sections) {
        if (!section.isNull()) {
            const QtsPacketCounter index = section->getFirstTsPacketIndex();
            first = found ? qMin(first, index) : index;
            found = true;
        }
    }
    return found ? first : 0;
}

QtsPacketCounter QtsTable::getLastTsPacketIndex() const
{
    QtsPacketCounter last = 0;
    foreach (const QtsSectionPtr section, _sections) {
        if (!section.isNull()) {
            last = qMax(last, section->getLastTsPacketIndex());
        }
    }
    return last;
}


//----------------------------------------------------------------------------
// Clear the content of the table. The table must be rebuilt
// using calls to addSection.
//----------------------------------------------------------------------------

void QtsTable::clear ()
{
    _isValid = false;
    _tid = 0xFF;
    _tidExt = 0;
    _version = 0;
    _sourcePid = QTS_PID_NULL;
    _missingCount = 0;
    _sections.clear();
}


//----------------------------------------------------------------------------
// Return the total size in bytes of all sections in the table.
//----------------------------------------------------------------------------

int QtsTable::totalSize() const
{
    int size = 0;
    foreach (const QtsSectionPtr section, _sections) {
        if (!section.isNull() && section->isValid()) {
            size += section->size();
        }
    }
    return size;
}


//----------------------------------------------------------------------------
// Get a pointer to a section in the table.
//----------------------------------------------------------------------------

QtsSectionPtr QtsTable::sectionAt(int index) const
{
    return index >= 0 && index < _sections.size() ? _sections[index] : 0;
}


//----------------------------------------------------------------------------
// A table is built by adding sections using addSection.
// When all sections are present, the table becomes valid.
//----------------------------------------------------------------------------

bool QtsTable::addSection (const QtsSectionPtr& section, bool replace, bool grow)
{
    // Reject invalid sections
    if (!section->isValid()) {
        return false;
    }

    // Check the compatibility of the section with the table
    const int index = section->sectionNumber();
    if (_sections.size() == 0) {
        // This is the first section, set the various global table parameters.
        _sections.resize(int(section->lastSectionNumber()) + 1);
        Q_ASSERT(index < _sections.size());
        _tid = section->tableId();
        _tidExt = section->tableIdExtension();
        _version = section->version();
        _sourcePid = section->sourcePid();
        _missingCount = int(_sections.size());
    }
    else if (section->tableId() != _tid || section->tableIdExtension() != _tidExt || section->version() != _version) {
        // Not the same table
        return false;
    }
    else if (!grow && (index >= _sections.size() || int(section->lastSectionNumber()) != _sections.size() - 1)) {
        // Incompatible number of sections and do not grow the table.
        return false;
    }
    else if (int(section->lastSectionNumber()) != _sections.size() - 1) {
        // Incompatible number of sections but the table is allowed to grow
        if (int(section->lastSectionNumber()) < _sections.size() - 1) {
            // The new section must be updated (its last section number is too low).
            section->setLastSectionNumber(quint8(_sections.size() - 1));
        }
        else {
            // The table must be updated (more sections).
            _missingCount += int(section->lastSectionNumber()) + 1 - _sections.size();
            _sections.resize (int(section->lastSectionNumber()) + 1);
            Q_ASSERT(index < _sections.size());
            // Modify all previously entered sections.
            foreach (const QtsSectionPtr s, _sections) {
                if (!s.isNull()) {
                    s->setLastSectionNumber(section->lastSectionNumber());
                }
            }
        }
    }

    // Now add the section
    if (_sections[index].isNull ()) {
        // The section was not present, add it
        _sections[index] = section;
        _missingCount--;
    }
    else if (!replace) {
        // Section already present, don't replace
        return false;
    }
    else {
        // Section already present but replace
        _sections[index] = section;
    }

    // The table becomes valid if there is no more missing section
    _isValid = _missingCount == 0;
    Q_ASSERT(_missingCount >= 0);
    return true;
}
