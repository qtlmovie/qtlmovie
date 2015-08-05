//----------------------------------------------------------------------------
//
// Copyright (c) 2014, Thierry Lelegard
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
//! @file QtlMovieTask.h
//!
//! Declare the class QtlMovieTask, an instance of a transcoding task.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIETASK_H
#define QTLMOVIETASK_H

#include "QtlMovieSettings.h"
#include "QtlMovieInputFile.h"
#include "QtlMovieOutputFile.h"

//!
//! A class implementing one transcoding task.
//!
class QtlMovieTask : public QObject
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] settings Application settings.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieTask(const QtlMovieSettings* settings, QtlLogger* log, QObject* parent = 0);

    //!
    //! Get the input file for the task.
    //! @return A pointer to the input file for the task.
    //!
    QtlMovieInputFile* inputFile() const
    {
        return _inFile;
    }

    //!
    //! Get the output file for the task.
    //! @return A pointer to the output file for the task.
    //!
    QtlMovieOutputFile* outputFile() const
    {
        return _outFile;
    }

    //!
    //! Ask the user if the output file may be overwritten.
    //! If the output file does not already exist, ask nothing.
    //! If the output file already exists and the user is OK to overwrite it,
    //! the previous output file is deleted.
    //! @return True if the output file does not exist or can be overwritten, false otherwise.
    //!
    bool askOverwriteOutput();

    //!
    //! Define the state of the task.
    //!
    enum State {
        Queued,   //!< Waiting to start.
        Running,  //!< Transcoding in progress.
        Success,  //!< Completed successfully.
        Failed    //!< Completed with failure.
    };

    //!
    //! Get the current state of the task (queued, running, etc).
    //! @return The current state of the task.
    //!
    State state() const
    {
        return _state;
    }

signals:
    //!
    //! Emitted when the input file name, output file name or output type changes.
    //! @param [in] task The changed task (ie. signal emitter).
    //!
    void taskChanged(QtlMovieTask* task);
    //!
    //! Emitted when the state of the task changes.
    //! @param [in] task The changed task (ie. signal emitter).
    //!
    void stateChanged(QtlMovieTask* task);

public slots:
    //!
    //! Invoked when the task starts.
    //!
   void setStarted();
    //!
    //! Invoked when the task completes.
    //! @param [in] success True when the task completed successfully, false otherwise.
    //!
    void setCompleted(bool success);

private slots:
    //!
    //! Used as relay to emit the signal taskChanged().
    //!
    void emitTaskChanged();

private:
    const QtlMovieSettings* _settings;  //!< Global settings.
    QtlMovieInputFile*      _inFile;    //!< Input file description.
    QtlMovieOutputFile*     _outFile;   //!< Output file description.
    State                   _state;     //!< State of the task (queued, running, etc.)

    //!
    //! Set the current state of the task (queued, running, etc).
    //! @param [in] state The current state of the task.
    //!
    void setState(State state);

    // Unaccessible operations.
    QtlMovieTask() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieTask)
};

#endif // QTLMOVIETASK_H
