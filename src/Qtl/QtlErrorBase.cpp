//----------------------------------------------------------------------------
//
// Copyright(c) 2013, Thierry Lelegard
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
// CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//
// Define the class QtlErrorBase.
// Qtl, Qt utility library.
//
//----------------------------------------------------------------------------

#include "QtlErrorBase.h"
#include <QtWidgets>
#include <cstdlib>

QtlNullLogger QtlErrorBase::_nullLog;


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------

QtlErrorBase::QtlErrorBase(QObject* parent, QtlLogger* log, bool autoReport) :
    QObject(parent),
    _log(log == 0 ? &_nullLog : log),
    _valid(false),
    _success(false),
    _autoReport(autoReport),
    _errorMessage()
{
}


//-----------------------------------------------------------------------------
// Copy constructor.
//-----------------------------------------------------------------------------

QtlErrorBase::QtlErrorBase(const QtlErrorBase& other) :
    QObject(0),
    _log(other._log),
    _valid(other._valid),
    _success(other._success),
    _autoReport(other._autoReport),
    _errorMessage(other._errorMessage)
{
}


//-----------------------------------------------------------------------------
// Assignment operator.
//-----------------------------------------------------------------------------

const QtlErrorBase& QtlErrorBase::operator=(const QtlErrorBase& other)
{
    if (this != &other) {
        _log = other._log;
        _valid = other._valid;
        _success = other._success;
        _autoReport = other._autoReport;
        _errorMessage = other._errorMessage;
    }
    return *this;
}


//-----------------------------------------------------------------------------
// Report success & error.
//-----------------------------------------------------------------------------

void QtlErrorBase::setSuccess() const
{
    _valid = true;
    _success = true;
    _errorMessage.clear();
}

void QtlErrorBase::invalidate()
{
    _valid = false;
    _success = false;
    _errorMessage.clear();
}

void QtlErrorBase::setError(const QString& message, ErrorType type) const
{
    _success = false;
    _errorMessage = message;
    if (type == QTL_INVALIDATE) {
        _valid = false;
    }
    if ((_autoReport && !message.isEmpty()) || type == QTL_FATAL) {
        reportError(_errorMessage, type == QTL_FATAL);
    }
}


//-----------------------------------------------------------------------------
// Report an error message.
//-----------------------------------------------------------------------------

void QtlErrorBase::reportError(const QString& message, bool fatal) const
{
    // Actual error message.
    const QString& msg(message.isEmpty() ? _errorMessage : message);
    if (msg.isEmpty()) {
        return; // no error
    }

    // Get the application main window, if there is one.
    QWidget* mainWindow = 0;
    if (qApp != 0) {
        const QWidgetList topWidgets(QApplication::topLevelWidgets());
        foreach (QWidget* w, topWidgets) {
            if (w != 0 && w->isVisible()) {
                mainWindow = w;
                break;
            }
        }
    }

    // Report the error.
    if (fatal) {
        _log->line(QObject::tr("Fatal error: %1").arg(message));
        if (mainWindow != 0) {
            QMessageBox::critical(mainWindow, QObject::tr("Fatal Error"), QObject::tr("%1\nApplication will abort").arg(message));
        }
        qWarning("Fatal error: %s", qPrintable(message));
        // Abort application
        ::exit(EXIT_FAILURE);
    }
    else {
        _log->line(QObject::tr("Error: %1").arg(message));
        if (mainWindow != 0) {
            QMessageBox::critical(mainWindow, QObject::tr("Error"), message);
        }
        else {
            qWarning("Error: %s", qPrintable(message));
        }
    }
}
