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
// Static objects.
//----------------------------------------------------------------------------

QStringList QtlTranslator::_loadedLocales;
QMutex QtlTranslator::_loadedLocalesMutex;


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlTranslator::QtlTranslator(const QString& fileNamePrefix, const QString& localeName, const QStringList& directories, QObject* parent) :
    QTranslator(parent)
{
    // Make sure the system locale is always last in the list.
    loadedLocales();

    // Insert locale in the list. Make sure it at in first position in the list (ie. "most recent").
    if (!localeName.isEmpty()) {
        QMutexLocker lock(&_loadedLocalesMutex);
        Q_ASSERT(!_loadedLocales.isEmpty());
        if (_loadedLocales.first() != localeName) {
            _loadedLocales.removeAll(localeName);
            _loadedLocales.prepend(localeName);
        }
    }

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


//----------------------------------------------------------------------------
// Get the list of all loaded locales using this class.
//----------------------------------------------------------------------------

QStringList QtlTranslator::loadedLocales()
{
    QMutexLocker lock(&_loadedLocalesMutex);

    // Make sure the system locale is always last in the list.
    if (_loadedLocales.isEmpty()) {
        _loadedLocales << QLocale::system().name();
    }

    return _loadedLocales;
}


//----------------------------------------------------------------------------
// Search a locale variant of a file, based on all loaded locales.
//----------------------------------------------------------------------------

QString QtlTranslator::searchLocaleFile(const QString& fileName)
{
    // Get base name and suffix.
    const int index = fileName.lastIndexOf(QRegExp("[\\.\\\\/:]"));
    const bool hasSuffix = index >= 0 && fileName[index] == '.';
    const QString baseName(hasSuffix ? fileName.left(index) : fileName);
    const QString suffix(hasSuffix ? fileName.mid(index) : QString());

    // Loop on all locales starting at most recent.
    foreach (const QString& locale, loadedLocales()) {
        QString loc(locale);
        // Seach all reduced forms of locale.
        for (int len = loc.length(); len > 0; len = loc.lastIndexOf('_')) {
            // Search using next reduced form of locale.
            loc.truncate(len);
            const QString name(baseName + "_" + loc + suffix);
            if (QFile(name).exists()) {
                return name;
            }
            // Special case, "en" (English) is the default language of files.
            // The <prefix>_en<suffix> usually does not exist. The English file is <prefix><suffix>.
            // We must handle this case here since other locale may exist in the list but
            // "en" is explicitely present here, so we want English at that point.
            if (loc.toLower() == "en" && QFile(fileName).exists()) {
                return fileName;
            }
        }
    }

    // Not found, return original file name.
    return fileName;
}
