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
// Define the class QtlMovieEditTaskDialog.
//
//----------------------------------------------------------------------------

#include "QtlMovieEditTaskDialog.h"
#include "QtlMovie.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieEditTaskDialog::QtlMovieEditTaskDialog(QtlMovieTask* task, QtlMovieSettings* settings, QtlLogger* log, QWidget* parent) :
    QtlDialog(parent)
{
    // Title and icon for this dialog.
    setWindowTitle(tr("QtlMovie - Edit Trancoding Task"));
    setWindowIcon(QIcon(":/images/qtlmovie-64.png"));
    resize(QTL_EDIT_TASK_WIDTH, QTL_EDIT_TASK_HEIGHT);

    // Restore the window geometry from the saved settings.
    setObjectName(QStringLiteral("QtlMovieEditTaskDialog"));
    setGeometrySettings(settings, true);

    // Grid layout: 3 lines, 4 columns.
    QGridLayout* layout = new QGridLayout(this);

    // Line 1: edit task widget.
    QtlMovieEditTask* editTask = new QtlMovieEditTask(this);
    editTask->initialize(task, settings, log);
    layout->addWidget(editTask, 0, 0, 1, 4);

    // Line 2: vertical spacer.
    QSpacerItem* vSpacer = new QSpacerItem(10, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(vSpacer, 1, 0, 1, 1);

    // Line 3: buttons.
    QPushButton* propertiesButton = new QPushButton(tr("Properties"), this);
    QPushButton* audioButton = new QPushButton(tr("Test Audio"), this);
    QSpacerItem* hSpacer = new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QPushButton* okButton = new QPushButton(tr("OK"), this);

    layout->addWidget(propertiesButton, 2, 0, 1, 1);
    layout->addWidget(audioButton, 2, 1, 1, 1);
    layout->addItem(hSpacer, 2, 2, 1, 1);
    layout->addWidget(okButton, 2, 3, 1, 1);

    connect(propertiesButton, &QPushButton::clicked, editTask, &QtlMovieEditTask::showInputFileProperties);
    connect(audioButton, &QPushButton::clicked, editTask, &QtlMovieEditTask::showAudioTest);
    connect(okButton, &QPushButton::clicked, this, &QtlMovieEditTaskDialog::accept);
}
