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
//! @file QtlMovieEditTask.h
//!
//! Declare the class QtlMovieEditTask, an instance of the UI for editing a task.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEEDITTASK_H
#define QTLMOVIEEDITTASK_H

#include "ui_QtlMovieEditTask.h"
#include "QtlMovieTask.h"
#include "QtlMovieSettings.h"
#include "QtlMovieInputFile.h"
#include "QtlMovieOutputFile.h"

//!
//! A subclass of QWidget which implements the UI for the editing a task.
//! This form is typically inserted either in the QtlMovieMainWindow
//! (single file mode) or in a QtlMovieEditTaskDialog (batch mode).
//! The design of the UI is done using Qt Designer.
//!
class QtlMovieEditTask : public QWidget
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlMovieEditTask(QWidget* parent = 0);

    //!
    //! Initialize the editor.
    //! Must be invoked before using the editor.
    //! May be used only once.
    //! @param [in] task The task to edit.
    //! @param [in] settings Application settings.
    //! @param [in] log Where to log errors.
    //!
    void initialize(QtlMovieTask* task, QtlMovieSettings* settings, QtlLogger* log);

    //!
    //! Get the task to edit.
    //! @return A pointer to the task to edit.
    //!
    QtlMovieTask* task() const
    {
        return _task;
    }

public slots:
    //!
    //! Update the UI when transcoding starts or stops.
    //! @param [in] started True if transcoding starts, false if transcoding stops.
    //!
    void transcodingUpdateUi(bool started);
    //!
    //! Invoked by the "Show Input File Properties..." button.
    //!
    void showInputFileProperties();
    //!
    //! Invoked by the "Audio Test..." button.
    //!
    void showAudioTest();
    //!
    //! Invoked to select input file.
    //!
    void selectInputFile();
    //!
    //! Invoked to select output file.
    //!
    void selectOutputFile();
    //!
    //! Invoked to select subtitle file.
    //!
    void selectSubtitleFile();

private slots:
    //!
    //! Set the output file type based on selected radio button.
    //!
    void setOutputFileType();
    //!
    //! Invoked when the edition of the input file name changed in the edit box.
    //!
    void inputFileNameEdited();
    //!
    //! Invoked when the input file name has changed.
    //! @param [in] fileName Absolute file path.
    //!
    void inputFileNameChanged(const QString& fileName);
    //!
    //! Invoked when new media information is available on the input file.
    //!
    void inputFileFormatChanged();
    //!
    //! Invoked when the edition of the output file name changed in the edit box.
    //!
    void outputFileNameEdited();
    //!
    //! Invoked when the output file name has changed.
    //! @param [in] fileName Absolute file path.
    //!
    void outputFileNameChanged(const QString& fileName);
    //!
    //! Enable or disable the various output types based on the input file.
    //!
    void enableOutputTypes();
    //!
    //! Clear input file stream information.
    //!
    void clearInputStreamInfo();
    //!
    //! Invoked when the selections of input streams have changed.
    //! Update the selected streams in the input file object.
    //! Enable/disable output types according to what is possible or not.
    //!
    void inputStreamSelectionUpdated();
    //!
    //! Reset the content of the "forced display aspect ratio" with the
    //! actual DAR of the selected video stream.
    //!
    void resetForcedDisplayAspectRatio();
    //!
    //! Invoked when the "forced display aspect ratio" has been changed.
    //!
    void forcedDarChanged();

private:
    Ui::QtlMovieEditTask _ui;                 //!< UI from Qt Designer.
    QtlMovieSettings*    _settings;           //!< Application settings.
    QtlLogger*           _log;                //!< Where to log errors.
    QtlMovieTask*        _task;               //!< The task to edit.
    int                  _updatingSelections; //!< A counter to protect inputStreamSelectionUpdated().

    // Unaccessible operations.
    QtlMovieEditTask() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieEditTask)
};

#endif // QTLMOVIEEDITTASK_H
