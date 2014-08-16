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
//
// Define the class QtlMovieTaskList, a list of a transcoding tasks.
//
//----------------------------------------------------------------------------

#include "QtlMovieTaskList.h"
#include "QtlMovieEditTaskDialog.h"
#include "QtlTableWidgetUtils.h"
#include "QtlMessageBoxUtils.h"
#include "QtlStringList.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieTaskList::QtlMovieTaskList(QWidget *parent) :
    QTableWidget(parent),
    _settings(0),
    _log(0)
{
    // The table is read-only.
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    // There are 3 columns: output type, input file name, input file name directory.
    setColumnCount(3);

    // Setup headers.
    verticalHeader()->setVisible(false);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader()->setVisible(true);
    horizontalHeader()->setCascadingSectionResizes(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader()->setStretchLastSection(true);

    qtlSetTableHorizontalHeader(this, 0, tr("Output"));
    qtlSetTableHorizontalHeader(this, 1, tr("Input file"));
    qtlSetTableHorizontalHeader(this, 2, tr("Input directory"));

    // Select by row (one row = one task).
    setSelectionBehavior(QAbstractItemView::SelectRows);

    // Don't sort, this is a sequential list.
    setSortingEnabled(false);

    // Get activated items (double click, enter, return).
    connect(this, &QtlMovieTaskList::activated, this, &QtlMovieTaskList::editSelectedTask);

    // Setup the context menu.
    QAction* action = new QAction(tr("Edit ..."), this);
    action->setShortcut(QKeySequence(Qt::Key_Return));
    connect(action, &QAction::triggered, this, &QtlMovieTaskList::editSelectedTask);
    addAction(action);

    action = new QAction(tr("New ..."), this);
    connect(action, &QAction::triggered, this, &QtlMovieTaskList::addAndEditTask);
    addAction(action);

    action = new QAction(tr("Move up"), this);
    action->setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_Up));
    connect(action, &QAction::triggered, this, &QtlMovieTaskList::moveUpSelectedTasks);
    addAction(action);

    action = new QAction(tr("Move down"), this);
    action->setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_Down));
    connect(action, &QAction::triggered, this, &QtlMovieTaskList::moveDownSelectedTasks);
    addAction(action);

    action = new QAction(tr("Delete"), this);
    action->setShortcut(QKeySequence(QKeySequence::Delete));
    connect(action, &QAction::triggered, this, &QtlMovieTaskList::deleteSelectedTasks);
    addAction(action);

    setContextMenuPolicy(Qt::ActionsContextMenu);
}


//----------------------------------------------------------------------------
// Initialize the task list.
//----------------------------------------------------------------------------

void QtlMovieTaskList::initialize(QtlMovieSettings* settings, QtlLogger* log)
{
    // Can be called only once.
    Q_ASSERT(_settings == 0);
    Q_ASSERT(_log == 0);

    Q_ASSERT(settings != 0);
    Q_ASSERT(log != 0);

    _settings = settings;
    _log = log;
}


//-----------------------------------------------------------------------------
// Add a task at the end of the list.
//-----------------------------------------------------------------------------

void QtlMovieTaskList::addTask(QtlMovieTask* task, bool editNow)
{
    if (task != 0 && task->inputFile() != 0 && task->outputFile() != 0) {

        // Take ownership of the object.
        task->setParent(this);

        // Create the row with empty items on each column.
        setRowCount(rowCount() + 1);
        for (int column = 0; column < columnCount(); ++column) {
            // Create the item.
            QTableWidgetItem* item = new QTableWidgetItem();
            setItem(rowCount() - 1, column, item);
            // Associate the task to the item.
            setTask(item, task);
        }

        // Update the items text.
        updateRow(rowCount() - 1);

        // Get notified when the task changes.
        connect(task, &QtlMovieTask::taskChanged, this, &QtlMovieTaskList::taskChanged);

        // Edit the task if required.
        if (editNow) {
            editTask(task);
        }
    }
}


//-----------------------------------------------------------------------------
// Associate a table item with a task.
// We store a qulonglong value of the task pointer value as associated data
// with "user role" in the item. This is valid only since we use this value
// in the current process only.
//-----------------------------------------------------------------------------

void QtlMovieTaskList::setTask(QTableWidgetItem* item, QtlMovieTask* task)
{
    item->setData(Qt::UserRole, qulonglong(reinterpret_cast<uintptr_t>(task)));
}

QtlMovieTask* QtlMovieTaskList::taskOfRow(int row) const
{
    QTableWidgetItem* item = row < 0 || row >= rowCount() ? 0 : this->item(row, 0);
    return item == 0 ? 0 : reinterpret_cast<QtlMovieTask*>(uintptr_t(item->data(Qt::UserRole).toULongLong()));
}


//-----------------------------------------------------------------------------
// Update the content of a row from the associated task content.
//-----------------------------------------------------------------------------

void QtlMovieTaskList::updateRow(int row)
{
    QtlMovieTask* task = taskOfRow(row);
    if (task != 0 && task->inputFile() != 0 && task->outputFile() != 0) {
        // The columns contain the output type, input file name, input file directory.
        const QString inFile(task->inputFile()->fileName());
        qtlSetTableRow(this, row, QtlStringList(QtlMovieOutputFile::outputTypeName(task->outputFile()->outputType()),
                                                inFile.isEmpty() ? "" : QFileInfo(inFile).fileName(),
                                                inFile.isEmpty() ? "" : QtlFile::parentPath(inFile)));
    }
}


//-----------------------------------------------------------------------------
// Edit a task.
//-----------------------------------------------------------------------------

void QtlMovieTaskList::editTask(QtlMovieTask* task)
{
    if (task != 0 && _settings != 0 && _log != 0) {
        QtlMovieEditTaskDialog dialog(task, _settings, _log, this);
        dialog.exec();
    }
}


//-----------------------------------------------------------------------------
// Triggered when a task changes.
//-----------------------------------------------------------------------------

void QtlMovieTaskList::taskChanged(QtlMovieTask* task)
{
    // Look for a row describing this task.
    for (int row = 0; row < rowCount(); ++ row) {
        if (task != 0 && task == taskOfRow(row)) {
            updateRow(row);
        }
    }
}


//-----------------------------------------------------------------------------
// Clear the table content. Reimplemented from QTableWidget.
//-----------------------------------------------------------------------------

void QtlMovieTaskList::clear()
{
    // Delete all rows but keep the headers.
    removeRows(0, rowCount() - 1);
}


//----------------------------------------------------------------------------
// Remove a row from the table. Reimplemented from QTableWidget.
//----------------------------------------------------------------------------

void QtlMovieTaskList::removeRow(int row)
{
    // Get the associated transcoding task.
    QtlMovieTask* task = taskOfRow(row);

    //@@@@ check if transcoding task in progress.

    // Delete the row and the task.
    QTableWidget::removeRow(row);
    delete task;
}


//----------------------------------------------------------------------------
// Remove rows from the table.
//----------------------------------------------------------------------------

void QtlMovieTaskList::removeRows(int firstRow, int lastRow)
{
    // Remove rows in sequence, from last to first.
    for (int row = lastRow; row >= firstRow; --row) {
        removeRow(row);
    }
}


//----------------------------------------------------------------------------
// Add a new task in the list and start editing it.
//----------------------------------------------------------------------------

void QtlMovieTaskList::addAndEditTask()
{
    addTask(new QtlMovieTask(_settings, _log, this), true);
}


//----------------------------------------------------------------------------
// Start editing the first selected task.
//----------------------------------------------------------------------------

void QtlMovieTaskList::editSelectedTask()
{
    const QList<QTableWidgetItem*> selected(selectedItems());
    editTask(taskOfRow(selected.isEmpty() ? -1 : selected.first()->row()));
}


//----------------------------------------------------------------------------
// Move the selected tasks up.
//----------------------------------------------------------------------------

void QtlMovieTaskList::moveUpSelectedTasks()
{
    // Get all contiguous ranges of selections.
    const QList<QTableWidgetSelectionRange> selected(selectedRanges());
    if (!selected.empty()) {

        // Get first contiguous range of selections.
        const QTableWidgetSelectionRange range(selected.first());

        // If the top row is included in the selection, we can't move the whole things up.
        if (range.rowCount() > 0 && range.topRow() > 0) {

            // Move each row up.
            for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
                // Get content of line up.
                const QList<QTableWidgetItem*> up(qtlTakeTableRow(this, row - 1));
                // Get content of current line.
                const QList<QTableWidgetItem*> current(qtlTakeTableRow(this, row));
                // Swap the content of the line.
                qtlSetTableRow(this, row - 1, current);
                qtlSetTableRow(this, row, up);
            }

            // Keep selection on the moved rows.
            clearSelection();
            setRangeSelected(QTableWidgetSelectionRange(range.topRow() - 1, 0, range.bottomRow() - 1, columnCount() - 1), true);
        }
    }
}


//----------------------------------------------------------------------------
// Move the selected tasks down.
//----------------------------------------------------------------------------

void QtlMovieTaskList::moveDownSelectedTasks()
{
    // Get all contiguous ranges of selections.
    const QList<QTableWidgetSelectionRange> selected(selectedRanges());
    if (!selected.empty()) {

        // Get first contiguous range of selections.
        const QTableWidgetSelectionRange range(selected.first());

        // If the bottom row is included in the selection, we can't move the whole things down.
        if (range.rowCount() > 0 && range.topRow() >= 0 && range.bottomRow() < rowCount() - 1) {

            // Move each row down.
            for (int row = range.bottomRow(); row >= range.topRow(); --row) {
                // Get content of line down.
                const QList<QTableWidgetItem*> down(qtlTakeTableRow(this, row + 1));
                // Get content of current line.
                const QList<QTableWidgetItem*> current(qtlTakeTableRow(this, row));
                // Swap the content of the line.
                qtlSetTableRow(this, row + 1, current);
                qtlSetTableRow(this, row, down);
            }

            // Keep selection on the moved rows.
            clearSelection();
            setRangeSelected(QTableWidgetSelectionRange(range.topRow() + 1, 0, range.bottomRow() + 1, columnCount() - 1), true);
        }
    }
}


//----------------------------------------------------------------------------
// Delete the selected tasks (need to confirm first).
//----------------------------------------------------------------------------

void QtlMovieTaskList::deleteSelectedTasks()
{
    // Get all contiguous ranges of selections.
    const QList<QTableWidgetSelectionRange> selected(selectedRanges());
    if (!selected.empty()) {
        const int start = selected.first().topRow();
        const int count = selected.first().rowCount();
        if (start >= 0 && count > 0) {
            // Require confirmation.
            QString text(tr("Delete these %1 tasks?").arg(count));
            if (count == 1) {
                QtlMovieTask* task = taskOfRow(start);
                if (task != 0 && task->inputFile() != 0) {
                    text = tr("Delete task %1?").arg(QFileInfo(task->inputFile()->fileName()).fileName());
                }
            }
            if (qtlConfirm(this, text)) {
                removeRows(start, start + count - 1);
            }
        }
    }
}