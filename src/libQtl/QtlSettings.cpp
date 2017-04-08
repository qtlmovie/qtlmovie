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
// Define the QtlSettings class.
//
//----------------------------------------------------------------------------

#include "QtlSettings.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlSettings::QtlSettings(const QString &organization, const QString &application, QObject* parent) :
    QObject(parent),
    _settings(organization, application)
{
}


//----------------------------------------------------------------------------
// Get a value from the settings.
//----------------------------------------------------------------------------

int QtlSettings::intValue(const QString& key, int defaultValue) const
{
    const QVariant var(_settings.value(key));
    bool ok = false;
    const int value = var.toInt(&ok);
    return ok ? value : defaultValue;
}

bool QtlSettings::boolValue(const QString& key, bool defaultValue) const
{
    const QVariant var(_settings.value(key));
    return var.isValid() ? var.toBool() : defaultValue;
}


//----------------------------------------------------------------------------
// Save / restore the geometry of a widget into / from the settings.
//----------------------------------------------------------------------------

void QtlSettings::saveGeometry(const QWidget* widget)
{
    _settings.setValue(widget->objectName() + "/geometry", widget->saveGeometry());
}

void QtlSettings::restoreGeometry(QWidget* widget)
{
    widget->restoreGeometry(_settings.value(widget->objectName() + "/geometry").toByteArray());
}
