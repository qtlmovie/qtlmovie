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
//!
//! @file QtlErrorBase.h
//!
//! Declare the class QtlErrorBase.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLERRORBASE_H
#define QTLERRORBASE_H

#include "QtlNullLogger.h"
#include <QtCore>

//!
//! Base class for objet reporting errors.
//!
class QtlErrorBase: public QObject
{
    Q_OBJECT

public:
    //!
    //! Virtual destructor.
    //!
    virtual ~QtlErrorBase()
    {
    }

    //!
    //! Get the error logger.
    //! @return The error logger. Never null, can always be used.
    //!
    QtlLogger* log() const
    {
        return _log;
    }

    //!
    //! Check if the object is valid for use.
    //! @return True if the object is valid for use.
    //!
    bool valid() const
    {
        return _valid;
    }

    //!
    //! Check if the last operation was successful.
    //! @return True if the last operation was successful, false otherwise.
    //!
    bool success() const
    {
        return _success;
    }

    //!
    //! Get the last error message.
    //! If valid() is false, return the reason why the object is invalid.
    //! @return An error message or the empty string if there was no error.
    //!
    QString errorMessage() const
    {
        return _errorMessage;
    }

    //!
    //! Check if errors are automatically reported and displayed to the user.
    //! @return True if errors are automatically reported, false otherwise.
    //!
    bool autoReport() const
    {
        return _autoReport;
    }

    //!
    //! Set if errors are automatically reported and displayed to the user.
    //! @param [in] autoReport True if errors are automatically reported, false otherwise.
    //!
    void setAutoReport(bool autoReport)
    {
        _autoReport = autoReport;
    }

protected:
    //!
    //! Protected constructor for subclasses.
    //! The object is initially invalid, the subclass constructor shall validate it if necessary.
    //! @param [in] parent Parent object.
    //! @param [in] log Where to log errors.
    //! @param [in] autoReport Automatically report errors.
    //!
    QtlErrorBase(QObject* parent, QtlLogger* log, bool autoReport);

    //!
    //! Copy constructor.
    //! The new object has no parent, it does not copy @a other's parent.
    //! @param [in] other Other instance to copy.
    //!
    QtlErrorBase(const QtlErrorBase& other);

    //!
    //! Assignment operator.
    //! The object's parent is unchanged.
    //! @param [in] other Other instance to copy.
    //!
    const QtlErrorBase& operator=(const QtlErrorBase& other);

    //!
    //! Type of error to report.
    //!
    enum ErrorType {
        QTL_ERROR,        //!< Error of last operation.
        QTL_INVALIDATE,   //!< Invalidate this instance.
        QTL_FATAL         //!< Fatal error, abort application.
    };

    //!
    //! Report success of the last operation.
    //!
    void setSuccess();

    //!
    //! Invalidate this object without reporting a message.
    //!
    void invalidate();

    //!
    //! Set an error message.
    //! @param [in] message The error message.
    //! @param [in] type Error type.
    //!
    void setError(const QString& message, ErrorType type = QTL_ERROR);

    //!
    //! Report an error message.
    //! @param [in] message The error message. If omitted, use the current error message
    //! of this instance. If still empty, do not report anything.
    //! @param [in] fatal If true, the application is aborted and this function never returns.
    //!
    void reportError(const QString& message = QString(), bool fatal = false);

private:
    static QtlNullLogger _nullLog;       //!< A void error logger.
    QtlLogger*           _log;           //!< Where to log errors, never null.
    bool                 _valid;         //!< True if the object is valid for use.
    bool                 _success;       //!< True if last operation was successful.
    bool                 _autoReport;    //!< Automatically report errors.
    QString              _errorMessage;  //!< Error message.
};

#endif // QTLERRORBASE_H
