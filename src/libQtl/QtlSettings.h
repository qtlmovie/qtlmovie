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
//! @file QtlSettings.h
//!
//! Declare the class QtlSettings, a base class for application settings.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLSETTINGS_H
#define QTLSETTINGS_H

#include "QtlCore.h"
#include "QtlGeometrySettingsInterface.h"

//!
//! A base class for application settings.
//! Actual settings are based on QSettings.
//! Services are available for saving / restoring windows' geometry.
//!
class QtlSettings : public QObject, public QtlGeometrySettingsInterface
{
    Q_OBJECT

public:
    //!
    //! Constructor
    //! @param [in] organization Organization name. Used to locate the actual settings (see QSettings).
    //! @param [in] application Application name within the organization.
    //! @param [in] parent Optional parent widget.
    //!
    QtlSettings(const QString &organization, const QString &application, QObject* parent = 0);

    //!
    //! Make sure that all settings are properly saved on disk immediately.
    //! This is automatically done by Qt from time to time anyway.
    //!
    void sync()
    {
        _settings.sync();
    }

    //!
    //! Save the geometry of a widget.
    //! Implementation of QtlGeometrySettingsInterface.
    //! @param [in] widget The widget to save the geometry of.
    //!
    virtual void saveGeometry(const QWidget* widget) Q_DECL_OVERRIDE;

    //!
    //! Restore the geometry of a widget.
    //! Implementation of QtlGeometrySettingsInterface.
    //! @param [in,out] widget The widget to restore the geometry of.
    //!
    virtual void restoreGeometry(QWidget* widget) Q_DECL_OVERRIDE;

    //!
    //! Save the "state" of a widget, for widget classes implementing save/restore state.
    //! @tparam WIDGET A widget class implementing save/restore state, typically one of
    //! QMainWindow, QSplitter, QHeaderView, QFileDialog.
    //! @param [in] widget The widget to save the state of.
    //!
    template<class WIDGET>
    void saveState(const WIDGET* widget)
    {
        _settings.setValue(widget->objectName() + "/state", widget->saveState());
    }

    //!
    //! Restore the "state" of a widget, for widget classes implementing save/restore state.
    //! @tparam WIDGET A widget class implementing save/restore state, typically one of
    //! QMainWindow, QSplitter, QHeaderView, QFileDialog.
    //! @param [in,out] widget The widget to restore the state of.
    //!
    template<class WIDGET>
    void restoreState(WIDGET* widget)
    {
        widget->restoreState(_settings.value(widget->objectName() + "/state").toByteArray());
    }

protected:
    //!
    //! Settings in Qt format, accessible to subclasses.
    //!
    QSettings _settings;

    //!
    //! Get an integer value from the settings.
    //! @param [in] key Key in the QSettings
    //! @param [in] defaultValue Default value if undefined or incorrectly defined.
    //! @return The final value.
    //!
    int intValue(const QString& key, int defaultValue) const;

    //!
    //! Get a boolean value from the settings.
    //! @param [in] key Key in the QSettings
    //! @param [in] defaultValue Default value if undefined or incorrectly defined.
    //! @return The final value.
    //!
    bool boolValue(const QString& key, bool defaultValue) const;

    //!
    //! @hideinitializer
    //! Create getter and setter for a string setting.
    //!
    //! The getter and setter definitions are inlined.
    //! These accessors simply load and store the option without further processing.
    //!
    //! @param getter Name of the getter method (also used as setting name).
    //! @param setter Name of the setter method.
    //! @param defaultValue Default value when the setting is unset.
    //!
#define QTL_SETTINGS_STRING(getter, setter, defaultValue)      \
    QString getter() const {return _settings.value(#getter, defaultValue).toString();} \
    void setter(const QString& _x) {_settings.setValue(#getter, _x);}

    //!
    //! @hideinitializer
    //! Create getter and setter for an integer setting.
    //!
    //! The getter and setter definitions are inlined.
    //! These accessors simply load and store the option without further processing.
    //!
    //! @param getter Name of the getter method (also used as setting name).
    //! @param setter Name of the setter method.
    //! @param defaultValue Default value when the setting is unset.
    //!
#define QTL_SETTINGS_INT(getter, setter, defaultValue) \
    int getter() const {return intValue(#getter, defaultValue);} \
    void setter(int _x) {_settings.setValue(#getter, _x);}

    //!
    //! @hideinitializer
    //! Create getter and setter for an enumeration setting.
    //!
    //! The getter and setter definitions are inlined.
    //! These accessors simply load and store the option without further processing.
    //! The enumeration values are stored as integer value. Be sure to preserve
    //! the order of enumeration values in the various versions of the application
    //! so that the settings from version N-1 can be safely loaded in version N.
    //!
    //! @param getter Name of the getter method (also used as setting name).
    //! @param setter Name of the setter method.
    //! @param enumType Enumeration type name.
    //! @param defaultValue Default value when the setting is unset.
    //!
#define QTL_SETTINGS_ENUM(getter, setter, enumType, defaultValue) \
    enumType getter() const {return static_cast<enumType>(intValue(#getter, static_cast<int>(defaultValue)));} \
    void setter(enumType _x) {_settings.setValue(#getter, static_cast<int>(_x));}

    //!
    //! @hideinitializer
    //! Create getter and setter for a boolean setting.
    //!
    //! The getter and setter definitions are inlined.
    //! These accessors simply load and store the option without further processing.
    //!
    //! @param getter Name of the getter method (also used as setting name).
    //! @param setter Name of the setter method.
    //! @param defaultValue Default value when the setting is unset.
    //!
#define QTL_SETTINGS_BOOL(getter, setter, defaultValue) \
    bool getter() const {return boolValue(#getter, defaultValue);} \
    void setter(bool _x) {_settings.setValue(#getter, _x);}
};

#endif // QTLSETTINGS_H
