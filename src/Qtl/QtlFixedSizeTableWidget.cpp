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
//
// Qtl, Qt utility library.
// Define the QtlFixedSizeTableWidget class.
//
//----------------------------------------------------------------------------

#include "QtlFixedSizeTableWidget.h"
#include <QHeaderView>
#include <QShowEvent>


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

QtlFixedSizeTableWidget::QtlFixedSizeTableWidget(QWidget* parent) :
    QTableWidget(parent)
{
    init();
}

QtlFixedSizeTableWidget::QtlFixedSizeTableWidget(int rows, int columns, QWidget* parent) :
    QTableWidget(rows, columns, parent)
{
    init();
}

void QtlFixedSizeTableWidget::init()
{
    // Establish a fixed size policy.
    // Thus, the actual size of the widget will be the complete size of the table when possible.
    QSizePolicy pol(sizePolicy());
    pol.setHorizontalPolicy(QSizePolicy::Fixed);
    pol.setVerticalPolicy(QSizePolicy::Fixed);
    setSizePolicy(pol);

    // Get notified when the size of a header changes.
    connect(horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(sectionResized()));
    connect(verticalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(sectionResized()));
}


//----------------------------------------------------------------------------
// Enforce the table widget size.
//----------------------------------------------------------------------------

void QtlFixedSizeTableWidget::enforceSize()
{
    // Enforce the size hint of the widget.
    const QSize size(sizeHint());
    setMinimumSize(size);
    setMaximumSize(size);
}


//----------------------------------------------------------------------------
// Invoked when a header section is resized.
//----------------------------------------------------------------------------

void QtlFixedSizeTableWidget::sectionResized()
{
    // Enforce the table size each time a row or column has been resized.
    enforceSize();
}


//----------------------------------------------------------------------------
// Show event handler.
//----------------------------------------------------------------------------

void QtlFixedSizeTableWidget::showEvent(QShowEvent* event)
{
    // Enforce the table size each time the widget is shown.
    // Not necessary in most cases but has proven to be required in some cases.
    enforceSize();

    // Tranfer event to superclass.
    QTableWidget::showEvent(event);
}


//----------------------------------------------------------------------------
// Compute the widget size hint.
//----------------------------------------------------------------------------

QSize QtlFixedSizeTableWidget::sizeHint() const
{
    QSize size(QTableWidget::sizeHint());
    int width = verticalHeader()->width() + 2 * frameWidth();
    for (int a = 0; a < columnCount(); ++a) {
        width += columnWidth(a);
    }
    int height = horizontalHeader()->height() + 2 * frameWidth();
    for (int a = 0; a < rowCount(); ++a) {
        height += rowHeight(a);
    }
    size.setWidth(width);
    size.setHeight(height);
    return size;
}
