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
// Qtl, Qt utility library.
// Define the class QtlTranslator.
//
//----------------------------------------------------------------------------

#include "QtlTranslator.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlTranslator::QtlTranslator(const QString& fileNamePrefix, const QString& localeName, const QStringList& directories, QObject* parent) :
    QTranslator(parent)
{
    // Actual file name.
    const QString fileName(fileNamePrefix + "_" + (localeName.isEmpty() ? QLocale::system().name() : localeName));

    // Add Qt translation directory and application directory.
    QStringList dirs(directories);
    dirs << QLibraryInfo::location(QLibraryInfo::TranslationsPath)
         << QCoreApplication::applicationDirPath()
         << (QCoreApplication::applicationDirPath() + QDir::separator() + "translations");

    // Try to load the file from the first possible directory.
    bool success = false;
    for (QStringList::ConstIterator it = dirs.begin(); !success && it != dirs.end(); ++it) {
        success = load(fileName, *it);
    }

    // If found, install translator.
    if (success) {
        QCoreApplication::installTranslator(this);
    }
}
