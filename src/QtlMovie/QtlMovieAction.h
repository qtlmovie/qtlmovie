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
//! @file QtlMovieAction.h
//!
//! Declare the class QtlMovieAction.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEACTION_H
#define QTLMOVIEACTION_H

#include <QtCore>
#include "QtlLogger.h"
#include "QtlMovieSettings.h"

//!
//! A class which represents an action which starts, progresses and completes.
//!
class QtlMovieAction : public QObject, public QtlLogger
{
#if !defined (DOXYGEN)
    Q_OBJECT
#endif

public:
    //!
    //! Constructor.
    //! @param [in] settings Application settings.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieAction(const QtlMovieSettings* settings, QtlLogger* log, QObject *parent = 0);

    //!
    //! Virtual destructor.
    //!
    virtual ~QtlMovieAction()
    {
    }

    //!
    //! Get the application settings.
    //! @return The application settings.
    //!
    const QtlMovieSettings* settings() const
    {
        return _settings;
    }

    //!
    //! Get the description of the action.
    //! @return The description.
    //!
    QString description() const
    {
        return _description;
    }

    //!
    //! Set the description of the action.
    //! @param [in] description The description.
    //!
    void setDescription(const QString& description)
    {
        _description = description;
    }

    //!
    //! Start the action.
    //! If the action was not yet started, emit started().
    //! @return False if already started. True otherwise.
    //!
    virtual bool start();

    //!
    //! Abort action execution.
    //! If the action was started, the signal completed() will be emitted when the action actually terminates.
    //!
    virtual void abort();

    //!
    //! Check if the action was started (and possible completed in the meantime).
    //! @return True if the action was started.
    //!
    bool isStarted() const
    {
        return _started;
    }

    //!
    //! Get the number of seconds since the action started.
    //! @return The number of seconds since the action started.
    //!
    int elapsedSeconds() const
    {
        return _started ? int(_startTime.secsTo(QDateTime::currentDateTimeUtc())) : 0;
    }

    //!
    //! Check if the action is completed.
    //! @return True if the action is completed.
    //!
    bool isCompleted() const
    {
        return _completed;
    }

    //!
    //! Check if unimportant messages are skipped.
    //! @return True if unimportant messages are skipped.
    //!
    bool isSilent() const
    {
        return _silent;
    }

    //!
    //! Set if unimportant messages are skipped.
    //! @param [in] silent If true, unimportant messages are skipped.
    //!
    void setSilent(bool silent)
    {
        _silent = silent;
    }

    //!
    //! Log text.
    //! Implementation of QtlLogger.
    //! @param [in] text Text to log.
    //!
    virtual void text(const QString& text);

    //!
    //! Log a line of text.
    //! Implementation of QtlLogger.
    //! @param [in] line Line to log. No need to contain a trailing new-line character.
    //! @param [in] color When a valid color is passed, try to display the text in this color.
    //!
    virtual void line(const QString& line, const QColor& color = QColor());

    //!
    //! Log a line of debug text.
    //! Implementation of QtlLogger.
    //! @param [in] line Line to log. No need to contain a trailing new-line character.
    //! @param [in] color When a valid color is passed, try to display the text in this color.
    //!
    virtual void debug(const QString& line, const QColor& color = QColor());

signals:
    //!
    //! Emitted when the action starts.
    //!
    void started();

    //!
    //! Emitted when some progress in the reporting is available.
    //! @param [in] description The description of the current process in the action.
    //! @param [in] current Current value (whatever unit it means).
    //! @param [in] maximum Value indicating full completion.
    //! If zero, we cannot evaluate the progress.
    //! @param [in] elapsedSeconds Elapsed seconds since the action started.
    //! @param [in] remainingSeconds Estimated remaining seconds to process.
    //! Negative if the remaining time cannot be estimated.
    //!
    void progress(const QString& description, int current, int maximum, int elapsedSeconds, int remainingSeconds);

    //!
    //! Emitted when the action completes.
    //! @param [in] success True when the action completed successfully, false otherwise.
    //!
    void completed(bool success);

protected:
    //!
    //! Emit the progress() signal.
    //! @param [in] current Current value (whatever unit it means).
    //! @param [in] maximum Value indicating full completion.
    //! If zero, we cannot evaluate the progress.
    //! @param [in] remainingSeconds Estimated remaining seconds to process.
    //! If negative, the processing time is considered linear and the remaining
    //! time is computed from @a current and @a maximum.
    //!
    virtual void emitProgress(int current, int maximum, int remainingSeconds = -1);

    //!
    //! Emit the completed() signal.
    //! @param [in] success True when the action completed successfully, false otherwise.
    //! @param [in] message Optional error message to log.
    //!
    virtual void emitCompleted(bool success, const QString& message = QString());

private:
    const QtlMovieSettings* _settings;    //!< Application settings.
    QtlLogger*              _log;         //!< Message logger.
    QString                 _description; //!< Description of the operation.
    QDateTime               _startTime;   //!< Start time.
    bool                    _started;     //!< start() was called.
    bool                    _completed;   //!< completed() has been signaled.
    bool                    _silent;      //!< Do not report unimportant messages.

    // Unaccessible operations.
    QtlMovieAction() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieAction)
};

#endif // QTLMOVIEACTION_H
