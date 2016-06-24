//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2016, Thierry Lelegard
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
// Define the class QtlCheckableHeaderView.
//
//----------------------------------------------------------------------------

#include "QtlCheckableHeaderView.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlCheckableHeaderView::QtlCheckableHeaderView(QWidget *parent) :
    QHeaderView(Qt::Horizontal, parent),
    _checkState(Qt::Unchecked),
    _forceCheck(false)
{
    // Make sure the header is clickable.
    setSectionsClickable(true);
}


//----------------------------------------------------------------------------
// Process a change of check state in the header.
//----------------------------------------------------------------------------

void QtlCheckableHeaderView::processCheckStateChanged()
{
    updateSection(0);
    emit checkStateChanged(_checkState);
}


//----------------------------------------------------------------------------
// Compute the position and size of the checkbox in the header.
//----------------------------------------------------------------------------

QRect QtlCheckableHeaderView::checkBoxRect(const QPoint& reference) const
{
    // Size of a standard checkbox in the style.
    QStyleOptionButton option;
    const QSize checkBoxSize = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option).size();

    // Amount of whitespace between buttons and the frame in the style.
    const int buttonMargin = style()->pixelMetric(QStyle::PM_ButtonMargin, 0, this);

    // Height of the header.
    QWidget* vp = viewport();
    const int headerHeight = vp != 0 ? vp->height() : this->height();

    // Make sure the checkbox is vertically centered.
    const int verticalMargin = qMax(0, (headerHeight - checkBoxSize.height()) / 2);

    // Top-left point of the checkbox.
    const QPoint checkBoxTopLeft(reference.x() + buttonMargin, reference.y() + verticalMargin);

    // Final position and size of the checkbox.
    return QRect(checkBoxTopLeft, checkBoxSize);
}


//----------------------------------------------------------------------------
// Paint the section. Reimplemented from QHeaderView
//----------------------------------------------------------------------------

void QtlCheckableHeaderView::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    // Invoke the superclass to do the main job.
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    // We need to paint the checkbox when we are in section 0 only.
    // We also need a model (sanity check).
    QAbstractItemModel* model = this->model();
    if (model == 0 || logicalIndex != 0) {
        return;
    }

    // Count the total number of checkable and checked items in the first column.
    int checkableCount = 0;
    int checkedCount = 0;
    for (int row = 0; row < model->rowCount(); row++) {
        const QModelIndex index = model->index(row, 0);
        if (index.isValid() && (model->flags(index) & Qt::ItemIsUserCheckable) != 0) {
            checkableCount++;
            const Qt::CheckState state= model->data(index, Qt::CheckStateRole).value<Qt::CheckState>();
            if (state != Qt::Unchecked) {
                checkedCount++;
            }
        }
    }

    // Final position and size of the checkbox.
    QStyleOptionButton option;
    option.rect = checkBoxRect(rect.topLeft());
    option.state |= QStyle::State_Enabled;

    // Compute the new check state.
    Qt::CheckState newCheckState = _checkState;
    if (_forceCheck) {
        // Force either checked or unchecked. Do it once only.
        _forceCheck = false;
        if (_checkState == Qt::Unchecked) {
            option.state |= QStyle::State_Off;
            newCheckState = Qt::Unchecked;
        }
        else {
            option.state |= QStyle::State_On;
            newCheckState = Qt::Checked;
        }
    }
    else if (checkedCount == 0) {
        option.state |= QStyle::State_Off;
        newCheckState = Qt::Unchecked;
    }
    else if (checkedCount < checkableCount) {
        option.state |= QStyle::State_NoChange;
        newCheckState = Qt::PartiallyChecked;
    }
    else if (checkedCount == checkableCount) {
        option.state |= QStyle::State_On;
        newCheckState = Qt::Checked;
    }
    else {
        // Should not get there.
        Q_ASSERT(false);
        option.state |= QStyle::State_None;
    }

    // Now draw the checkbox.
    style()->drawControl(QStyle::CE_CheckBox, &option, painter);

    // Finally report change in state.
    if (_checkState != newCheckState) {
        _checkState = newCheckState;
        const_cast<QtlCheckableHeaderView*>(this)->processCheckStateChanged();
    }
}


//----------------------------------------------------------------------------
// Process a mouse-press event.
//----------------------------------------------------------------------------

void QtlCheckableHeaderView::mousePressEvent(QMouseEvent* event)
{
    // Process this event when it occurs in the first section of the header, inside the checkbox.
    if (event != 0 && logicalIndexAt(event->pos()) == 0 && checkBoxRect().contains(event->pos())) {

        // We force either checked or unchecked in repaint.
        if (_checkState != Qt::Unchecked) {
            _checkState = Qt::Unchecked;
        }
        else {
            _checkState = Qt::Checked;
        }
        _forceCheck = true;

        // The state always changes here, process the change.
        processCheckStateChanged();

        // Update all check boxes in first column.
        updateAllRows();
    }

    // Update the widget.
    update();

    // Let the superclass process other effects for this event.
    QHeaderView::mousePressEvent(event);
}


//----------------------------------------------------------------------------
// Update the checkboxes in all rows, based on the checkbox in the header.
//----------------------------------------------------------------------------

void QtlCheckableHeaderView::updateAllRows()
{
    QAbstractItemModel* model = this->model();
    if (model != 0) {
        const Qt::CheckState state = (_checkState == Qt::Unchecked) ? Qt::Unchecked : Qt::Checked;
        for (int row = 0; row < model->rowCount(); row++) {
            const QModelIndex index = model->index(row, 0);
            if (index.isValid() && (model->flags(index) & Qt::ItemIsUserCheckable) != 0) {
                model->setData(index, state, Qt::CheckStateRole);
            }
        }
    }
}
