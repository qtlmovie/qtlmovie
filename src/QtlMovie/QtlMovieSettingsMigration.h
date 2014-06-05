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
//! @file QtlMovieSettingsMigration.h
//!
//! Declare the class QtlMovieSettingsMigration.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIESETTINGSMIGRATION_H
#define QTLMOVIESETTINGSMIGRATION_H

#include "QtlMovieSettings.h"

//!
//! Migration of QtlMovie settings from old to new format.
//!
//! The old format used an XML-file to store the settings.
//! The new format is based QSettings and uses a platform-dependent backing store.
//!
//! This class manipulates legacy data and shall not be modified.
//!
class QtlMovieSettingsMigration
{
public:
    //!
    //! Constructor
    //! If an old-style XML configuration file exists, the configuration is loaded
    //! into the new format and the old file is renamed with an old prefix.
    //! @param [in,out] settings Where to store the converted old configuration.
    //! @param [in] log Where to log errors.
    //!
    QtlMovieSettingsMigration(QtlMovieSettings* settings, QtlLogger* log);

private:
    QtlMovieSettings* _settings;  //! The settings to fill from an old-style configuration file.
    QXmlStreamReader  _xml;       //! XML reader for the old-style configuration file.

    //!
    //! Decode an XML element with a "value" string attribute.
    //! @param [in] name Expected element name.
    //! @param [in] setter The setter method to invoke with the decoded value.
    //! @return True on success, false if not the expected element.
    //!
    bool getString(const QString& name, void (QtlMovieSettings::*setter)(const QString&));

    //!
    //! Decode an XML element with "type" and "value" string attributes.
    //! @param [in] name Expected element name.
    //! @param [in] setter The setter method to invoke with the decoded type and value.
    //! @return True on success, false if not the expected element.
    //!
    bool getString2(const QString& name, void (QtlMovieSettings::*setter)(const QString& type, const QString& value));

    //!
    //! Decode an XML element with a "value" comma-separated string list attribute.
    //! @param [in] name Expected element name.
    //! @param [in] setter The setter method to invoke with the decoded value.
    //! @return True on success, false if not the expected element.
    //!
    bool getList(const QString& name, void (QtlMovieSettings::*setter)(const QStringList&));

    //!
    //! Decode an XML element with a "value" boolean attribute.
    //! @param [in] name Expected element name.
    //! @param [in] setter The setter method to invoke with the decoded value.
    //! @return True on success, false on error: not the expected element,
    //! "value" attribute not found, not a valid boolean.
    //!
    bool getBool(const QString& name, void (QtlMovieSettings::*setter)(bool));

    //!
    //! Decode an XML element with a "value" integer or enum attribute.
    //! @tparam INT An integer or enum type.
    //! @param [in] name Expected element name.
    //! @param [in] setter The setter method to invoke with the decoded value.
    //! @return True on success, false on error: not the expected element,
    //! "value" attribute not found, not a valid integer.
    //!
    template<typename INT>
    bool getInt(const QString& name, void (QtlMovieSettings::*setter)(INT))
    {
        if (_xml.name() != name) {
            return false;
        }
        else {
            const QString stringValue(_xml.attributes().value("value").toString().trimmed());
            _xml.skipCurrentElement();
            bool ok = false;
            const int intValue = stringValue.toInt(&ok, 0);
            if (ok) {
                (_settings->*setter)(static_cast<INT>(intValue));
            }
            else {
                _xml.raiseError(QObject::tr("Invalid integer value %1 in element <%2>").arg(stringValue).arg(name));
            }
            return ok;
        }
    }

    //!
    //! Decode an XML element with geometry attributes.
    //! @param [in] name Expected element name.
    //! @param [in] setter The setter method to invoke with a widget of the decoded geometry.
    //! @return True on success, false on error: not the expected element,
    //! "value" attribute not found, not a valid boolean.
    //!
    bool getGeometry(const QString& name, void (QtlMovieSettings::*setter)(const QWidget*));
};

#endif // QTLMOVIESETTINGSMIGRATION_H
