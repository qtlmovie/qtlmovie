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
// Define the class QtsDvdFile.
//
//----------------------------------------------------------------------------

#include "QtsDvdFile.h"
#include "QtsDvdMedia.h"


//----------------------------------------------------------------------------
// Constructor and destructor.
//----------------------------------------------------------------------------

QtsDvdFile::QtsDvdFile(const QString& path, int startSector, int sizeInBytes) :
    _path(path),
    _sector(startSector),
    _size(sizeInBytes)
{
}

QtsDvdFile::~QtsDvdFile()
{
}


//----------------------------------------------------------------------------
// Get the file characteristics.
//----------------------------------------------------------------------------

QString QtsDvdFile::name() const
{
    return _path.isEmpty() ? QString() : QFileInfo(_path).fileName();
}

QString QtsDvdFile::description() const
{
    return _path.isEmpty() ? "metadata" : _path;
}

int QtsDvdFile::sectorCount() const
{
    return _size / QTS_DVD_SECTOR_SIZE + int(_size % QTS_DVD_SECTOR_SIZE > 0);
}


bool QtsDvdFile::isVob() const
{
    return _path.endsWith(".VOB", Qt::CaseInsensitive);
}


//----------------------------------------------------------------------------
// Clear the content of this object.
//----------------------------------------------------------------------------

void QtsDvdFile::clear()
{
    _path.clear();
    _sector = -1;
    _size = 0;
}


//----------------------------------------------------------------------------
// Operator "less than" on QtsDvdFilePtr to sort files.
//----------------------------------------------------------------------------

bool operator<(const QtsDvdFilePtr& f1, const QtsDvdFilePtr& f2)
{
    // Null pointers preceeds everything.
    if (!f1.isNull() && !f2.isNull()) {
        return *f1 < *f2;
    }
    else {
        return !f2.isNull();
    }
}
