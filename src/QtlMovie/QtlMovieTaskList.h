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
    //! The QtlMovieTaskList intance takes ownership of the @a task object.
    //! @param [in] task The task to add.
    //! @param [in] editNow If true, immediately opens an editor on it.
    //!
    void addTask(QtlMovieTask* task, bool editNow);

    //!
    //! Add a task at the end of the list.
    //! @param [in] inputFileName Input file name. All defaults apply.
    //! @param [in] editNow If true, immediately opens an editor on it.
    //! @return The created task.
    //!
    QtlMovieTask* addTask(const QString& inputFileName, bool editNow);

    //!
    //! Get the first task in "queued" state.
    //! The returned task becomes the "currently executing task".
    //! @return The next task to execute or zero if there is none.
    //!
    QtlMovieTask* nextTask();

    //!
    //! Check if some queued tasks are present.
    //! @return True if some queued tasks are present, false otherwise.
    //!
    bool hasQueuedTasks() const;

    //!
    //! Get the currently executing task.
    //! @return The currently executing task or zero if none is defined.
    //!
    QtlMovieTask* currentTask() const
    {
        return _currentTask;
    }

public slots:
    //!
    //! Clear the table content.
    //! Reimplemented from QTableWidget.
    //!
    void clear();
    //!
    //! Remove a row from the table.
    //! Reimplemented from QTableWidget.
    //! @param [in] row The row to remove.
    //!
    void removeRow(int row);
    //!
    //! Remove rows from the table.
    //! @param [in] firstRow The first row to remove.
    //! @param [in] lastRow The last row to remove.
    //!
    void removeRows(int firstRow, int lastRow);
    //!
    //! Remove all completed tasks.
    //!
    void removeCompletedTasks();
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
    //!
    //! Requeue the selected tasks (if completed).
    //!
    void requeueSelectedTasks();

private slots:
    //!
    //! Triggered when the input file name, output file name or output type of a task changes.
    //! @param [in] task The changed task (ie. signal emitter).
    //!
    void taskChanged(QtlMovieTask* task);
    //!
    //! Triggered when the state of the task changes.
    //! @param [in] task The changed task (ie. signal emitter).
    //!
    void taskStateChanged(QtlMovieTask* task);

protected:
    //!
    //! Invoked when a drag operation enters the widget.
    //! @param [in,out] event The notified event.
    //!
    virtual void dragEnterEvent(QDragEnterEvent* event) Q_DECL_OVERRIDE;
    //!
    //! Invoked when a drag operation is in progress and the cursor moves.
    //! @param [in,out] event The notified event.
    //!
    virtual void dragMoveEvent(QDragMoveEvent* event) Q_DECL_OVERRIDE;
    //!
    //! Invoked when a drop operation is performed.
    //! @param [in,out] event The notified event.
    //!
    virtual void dropEvent(QDropEvent* event) Q_DECL_OVERRIDE;

private:
    QtlMovieSettings* _settings;    //!< Application settings.
    QtlLogger*        _log;         //!< Where to log errors.
    QtlMovieTask*     _currentTask; //!< Currently executing task.

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
    //! Get the row of a task.
    //! @param [in] task The task to seach.
    //! @return The associated row or -1 if the task is unknown.
    //!
    int rowOfTask(QtlMovieTask* task) const;

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
