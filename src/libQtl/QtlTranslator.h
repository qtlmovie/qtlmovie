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
//! @file QtlTranslator.h
//!
//! Declare the class QtlTranslator.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLTRANSLATOR_H
#define QTLTRANSLATOR_H

#include "QtlCore.h"

//!
//! A subclass of QTranslator with extended file search capabilities.
//!
class QtlTranslator : public QTranslator
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! The translator is automatically installed in the application.
    //! @param [in] fileNamePrefix Same as the @a filename parameter in QTranslator::load()
    //! without the locale name part.
    //! @param [in] localeName Locale name. Defaults to the system locale.
    //! @param [in] directories List of directories to search for the file. The Qt translation
    //! directory is automatically added at the end of the list.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtlTranslator(const QString& fileNamePrefix,
                           const QString& localeName = QString(),
                           const QStringList& directories = QStringList(),
                           QObject *parent = 0);

    //!
    //! Get the list of all loaded locales using this class.
    //! @return The list of all loaded locales using this class, most recent first.
    //! This list is never empty, its last element is the system locale.
    //!
    static QStringList loadedLocales();

    //!
    //! Search a locale variant of a file, based on all loaded locales.
    //! In the list of all loaded locales, seach the first existing file
    //! in the form {basename}_{locale}.{suffix}.
    //! @param [in] fileName A file name.
    //! @return The first existing locale variant or @a fileName if none is found.
    //!
    static QString searchLocaleFile(const QString& fileName);

private:
    static QStringList _loadedLocales;       //!< List of loaded locales.
    static QMutex      _loadedLocalesMutex;  //!< Synchronize access to _loadedLocales.

    // Unaccessible operation.
    QtlTranslator() Q_DECL_EQ_DELETE;
};

#endif // QTLTRANSLATOR_H
