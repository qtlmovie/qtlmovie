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
//! @file QtlMovieTaskList.h
//!
//! Declare the class QtlMovieTaskList, a list of a transcoding tasks.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIETASKLIST_H
#define QTLMOVIETASKLIST_H

#include "QtlMovieTask.h"
#include "QtlMovieSettings.h"

//!
//! A list of transcoding tasks.
//! This is both a widget for editing the tasks list and a reference model for the task list.
//!
class QtlMovieTaskList : public QTableWidget
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtlMovieTaskList(QWidget* parent = 0);

    //!
    //! Initialize the task list.
    //! Must be invoked before using the task list.
    //! May be used only once.
    //! @param [in] settings Application settings.
    //! @param [in] log Where to log errors.
    //!
    void initialize(QtlMovieSettings* settings, QtlLogger* log);

    //!
    //! Add a task at the end of the list.
    //! @param [in] task The task to add.
    //! @param [in] editNow If true, immediately opens an editor on it.
    //!
    void addTask(QtlMovieTask* task, bool editNow);

public slots:
    //!
    //! Clear the table content.
    //! Reimplemented from QTableWidget.
    //!
    void clear();
    //!
    //! Add a new task in the list and start editing it.
    //!
    void addAndEditTask();
    //!
    //! Start editing the first selected task.
    //!
    void editSelectedTask();
    //!
    //! Move the selected tasks up.
    //!
    void moveUpSelectedTasks();
    //!
    //! Move the selected tasks down.
    //!
    void moveDownSelectedTasks();
    //!
    //! Delete the selected tasks (need to confirm first).
    //!
    void deleteSelectedTasks();

private slots:
    //!
    //! Triggered when the input file name, output file name or output type of a task changes.
    //! @param [in] task The changed task (ie. signal emitter).
    //!
    void taskChanged(QtlMovieTask* task);

private:
    QtlMovieSettings* _settings;  //!< Application settings.
    QtlLogger*        _log;       //!< Where to log errors.

    //!
    //! Associate a table item with a task.
    //! @param [in,out] item The table item to set.
    //! @param [in] task The task.
    //!
    static void setTask(QTableWidgetItem* item, QtlMovieTask* task);

    //!
    //! Get the task associated to a row.
    //! @param [in] row The row in the table.
    //! @return The associated task or a null pointer if none found or the rwo does not exist.
    //!
    QtlMovieTask* taskOfRow(int row) const;

    //!
    //! Update the content of a row from the associated task content.
    //! @param [in] row The row to update.
    //!
    void updateRow(int row);

    //!
    //! Edit a task.
    //! @param [in,out] task The task to edit.
    //!
    void editTask(QtlMovieTask* task);

    // Unaccessible operations.
    QtlMovieTaskList() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieTaskList)
};

#endif // QTLMOVIETASKLIST_H
