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
#include "QtlTableWidgetUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieTaskList::QtlMovieTaskList(QWidget *parent) :
    QTableWidget(parent)
{
    // The table is read-only.
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    // There are 3 columns: output type, input file name, input file name directory.
    setColumnCount(3);

    // Setup headers.
    verticalHeader()->setVisible(false);
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

    action = new QAction(tr("Delete"), this);
    action->setShortcut(QKeySequence(QKeySequence::Delete));
    connect(action, &QAction::triggered, this, &QtlMovieTaskList::deletedSelectedTasks);
    addAction(action);

    setContextMenuPolicy(Qt::ActionsContextMenu);
}


//----------------------------------------------------------------------------
// Add a new task in the list and start editing it.
//----------------------------------------------------------------------------

void QtlMovieTaskList::addAndEditTask()
{

}


//----------------------------------------------------------------------------
// Start editing the first selected task.
//----------------------------------------------------------------------------

void QtlMovieTaskList::editSelectedTask()
{

}


//----------------------------------------------------------------------------
// Move the selected tasks up.
//----------------------------------------------------------------------------

void QtlMovieTaskList::moveUpSelectedTasks()
{

}


//----------------------------------------------------------------------------
// Move the selected tasks down.
//----------------------------------------------------------------------------

void QtlMovieTaskList::moveDownSelectedTasks()
{

}


//----------------------------------------------------------------------------
// Delete the selected tasks (need to confirm first).
//----------------------------------------------------------------------------

void QtlMovieTaskList::deletedSelectedTasks()
{

}
