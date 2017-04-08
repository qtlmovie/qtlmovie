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
//! @file QtlIsoLanguages.h
//!
//! Declare the class QtlIsoLanguages.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLISOLANGUAGES_H
#define QTLISOLANGUAGES_H

#include "QtlCore.h"

//!
//! Using ISO language and country codes.
//!
//! The following standards are covered:
//! - ISO 639-1: 2-letter codes for language names.
//! - ISO 639-2: 3-letter codes for language names.
//! - ISO 3166-1: 2 letter, 3-letter and 3-digit codes for country names.
//!
//! @see http://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt
//! @see http://www.davros.org/misc/iso3166.txt
//!
class QtlIsoLanguages : public QObject
{
public:
    //!
    //! Get the single instance of QtlIsoLanguages.
    //! @return The single instance of QtlIsoLanguages.
    //!
    static const QtlIsoLanguages* instance();

    //!
    //! Get a localized language name from a language code.
    //! @param code The language code as a string, either an ISO 639-1 2-letter code or an ISO 638-2 3-letter code.
    //! @param defaultValue The string value to return is @a code is not found.
    //! By default, return the @a code string if not found.
    //! If @a defaultValue is an explicit empty string (""), then the empty
    //! string will be returned if @a code is not found.
    //! @return The corresponding language code or a default value if not found.
    //!
    QString languageName(const QString& code, const QString& defaultValue = QString()) const;

    //!
    //! Get a localized country name from a country code.
    //! @param code The ISO 6166-1 country code as a string, either a 2-letter, 3-letter or 3-digit code.
    //! @param defaultValue The string value to return is @a code is not found.
    //! By default, return the @a code string if not found.
    //! If @a defaultValue is an explicit empty string (""), then the empty
    //! string will be returned if @a code is not found.
    //! @return The corresponding country code or a default value if not found.
    //!
    QString countryName(const QString& code, const QString& defaultValue = QString()) const;

    //!
    //! Get the sorted list of all ISO 639-1 2-letter codes for language names.
    //! @return The sorted list of all ISO 639-1 2-letter codes for language names.
    //!
    QStringList language2LetterCodes() const
    {
        return codeList(_languageBy2Letter);
    }

    //!
    //! Get the sorted list of all ISO 639-2 3-letter codes for language names.
    //! @return The sorted list of all ISO 639-2 3-letter codes for language names.
    //!
    QStringList language3LetterCodes() const
    {
        return codeList(_languageBy3Letter);
    }

    //!
    //! Get the sorted list of all ISO 3166 2-letter codes for country names.
    //! @return The sorted list of all ISO 3166 2-letter codes for country names.
    //!
    QStringList country2LetterCodes() const
    {
        return codeList(_countryBy2Letter);
    }

    //!
    //! Get the sorted list of all ISO 3166 3-letter codes for country names.
    //! @return The sorted list of all ISO 3166 3-letter codes for country names.
    //!
    QStringList country3LetterCodes() const
    {
        return codeList(_countryBy3Letter);
    }

    //!
    //! Get the sorted list of all ISO 3166 3-digit codes for country names.
    //! @return The sorted list of all ISO 3166 3-digit codes for country names.
    //!
    QStringList country3DigitCodes() const
    {
        return codeList(_countryBy3Digit);
    }

private:
    // Private members.
    typedef QMap<QString, QString> StringMap; //!< Mapping a string to another string.
    StringMap _languageBy2Letter;             //!< Map key = 2-letter code to value = language name.
    StringMap _languageBy3Letter;             //!< Map key = 3-letter code to value = language name.
    StringMap _countryBy2Letter;              //!< Map key = 2-letter code to value = country name.
    StringMap _countryBy3Letter;              //!< Map key = 3-letter code to value = country name.
    StringMap _countryBy3Digit;               //!< Map key = 3-digit code to value = country name.
    static const QtlIsoLanguages* _instance;  //!< Single instance.
    static QMutex _instanceMutex;             //!< Protect the creation of the mutex.

    //!
    //! Private constructor.
    //!
    QtlIsoLanguages();

    //!
    //! Get the sorted list of all codes for a given map.
    //! @return The sorted list of all codes for a given map.
    //!
    QStringList codeList(const StringMap& map) const;

    //!
    //! Add an ISO 639 language code.
    //! @param [in] code3 ISO 639-1 3-letter code.
    //! @param [in] code3alt Optional alternate ISO 639-1 3-letter code.
    //! @param [in] code2 ISO 639-1 2-letter code.
    //! @param [in] name Language name.
    //!
    void addLanguage(const QString& code3, const QString& code3alt, const QString& code2, const QString& name);

    //!
    //! Add an ISO 3166-1 country code.
    //! @param [in] code2 2-letter code.
    //! @param [in] code3 3-letter code.
    //! @param [in] digit3 3-digit code.
    //! @param [in] name Country name.
    //!
    void addCountry(const QString& code2, const QString& code3, const QString& digit3, const QString& name);

    // Unaccessible operation.
    Q_DISABLE_COPY(QtlIsoLanguages)
};

#endif // QTLISOLANGUAGES_H
