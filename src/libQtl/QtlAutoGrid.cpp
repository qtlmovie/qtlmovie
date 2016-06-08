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
// Define the class QtlAutoGrid.
//
//----------------------------------------------------------------------------

#include "QtlAutoGrid.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlAutoGrid::QtlAutoGrid(QWidget* parent) :
    SuperClass(parent),
    _leftMargin(0),
    _rightMargin(0),
    _topMargin(0),
    _bottomMargin(0),
    _horizontalSpacing(24),
    _verticalSpacing(6),
    _children()
{
    // Compute the size policy of the grid.
    // The minimum width of the grid is the largest minimum width of all elements,
    // resulting in a one-column grid. But expanding the grid is useful (more columns).
    // Once the width is given, the number of columns is computed and there is no advantage
    // of being taller than the minimum. In all cases, the minimum height depends on the width.
    QSizePolicy pol(QSizePolicy::Expanding, QSizePolicy::Minimum);
    pol.setHeightForWidth(true);
    setSizePolicy(pol);
}


//----------------------------------------------------------------------------
// Compute the number of rows for a given number of columns.
//----------------------------------------------------------------------------

int QtlAutoGrid::rowsForColumns(int columns) const
{
    const int count = _children.size();
    columns = qBound(1, columns, count);
    return (count + columns - 1) / columns;
}


//----------------------------------------------------------------------------
// Compute the height of a given row, assuming a given row/column layout.
//----------------------------------------------------------------------------

int QtlAutoGrid::rowHeight(int row, int rows) const
{
    int rowHeight = 0;
    for (int i = row; i < _children.size(); i += rows) {
        rowHeight = qMax(rowHeight, _children[i].size.height());
    }
    return rowHeight;
}


//----------------------------------------------------------------------------
// Compute the width of a given column, assuming a given row/column layout.
//----------------------------------------------------------------------------

int QtlAutoGrid::columnWidth(int column, int rows) const
{
    int colWidth = 0;
    const int startIndex = column * rows;
    const int endIndex = qMin(startIndex + rows, _children.size());
    for (int i = startIndex; i < endIndex; ++i) {
        colWidth = qMax(colWidth, _children[i].size.width());
    }
    return colWidth;
}


//----------------------------------------------------------------------------
// Compute the number of columns for a given widget width.
//----------------------------------------------------------------------------

int QtlAutoGrid::columnsForWidth(int width) const
{
    // No child => no column.
    if (_children.isEmpty()) {
        return 0;
    }

    // Try to find a row / column layout which makes the maximum use of the width.
    // We cannot simply compute the grid layout based on a grid cell the size of
    // which is the maximum width and height of all children. By packing the column
    // and rows, we optimize the space. Currently, we use a brute force method:
    // we try all combinations from one row (as many columns as children) to one
    // column until one is found.

    for (int columns = _children.size(); columns > 1; --columns) {
        // When the computed grid width fits in the proposed widget width, keep this number of columns.
        const int gridWidth = widthForColumns(columns);
        if (gridWidth <= width) {
            return columns;
        }
    }

    // Nothing fits in this narrow width. Use one column anyway (likely to be truncated).
    return 1;
}


//----------------------------------------------------------------------------
// Compute the widget width for a given number of columns of children.
//----------------------------------------------------------------------------

int QtlAutoGrid::widthForColumns(int columns) const
{
    // Corresponding number of rows.
    const int rows = rowsForColumns(columns);

    // Compute the corresponding grid width.
    int width = _leftMargin + _rightMargin + _horizontalSpacing * qMax(0, columns - 1);
    for (int col = 0; col < columns; ++col) {
        width += columnWidth(col, rows);
    }
    return width;
}


//----------------------------------------------------------------------------
// Compute the widget height for a given width.
//----------------------------------------------------------------------------

int QtlAutoGrid::heightForWidth(int width) const
{
    // Compute row/column layout.
    const int columns = columnsForWidth(width);
    const int rows = rowsForColumns(columns);

    // Compute the corresponding grid height.
    int height = _topMargin + _bottomMargin + _verticalSpacing * qMax(0, rows - 1);
    for (int row = 0; row < rows; ++row) {
        height += rowHeight(row, rows);
    }
    return height;
}


//----------------------------------------------------------------------------
// Return the recommended minimum size for the widget.
//----------------------------------------------------------------------------

QSize QtlAutoGrid::minimumSizeHint() const
{
    // We return the size of the grid containing only the largest element.

    // Compute maximum width and height of elements.
    int width = 0;
    int height = 0;
    foreach (const ChildBox& child, _children) {
        width = qMax(width, child.size.width());
        height = qMax(height, child.size.height());
    }

    // Add margins.
    width += _leftMargin + _rightMargin;
    height += _topMargin + _bottomMargin;

    return QSize(width, height);
}


//----------------------------------------------------------------------------
// Return the widget size hint.
//----------------------------------------------------------------------------

QSize QtlAutoGrid::sizeHint() const
{
    // We return the size of the grid with all elements in one row.

    const int width = widthForColumns(_children.size());
    const int height = heightForWidth(width);
    return QSize(width, height);
}


//----------------------------------------------------------------------------
// General event handler.
//----------------------------------------------------------------------------

bool QtlAutoGrid::event(QEvent* e)
{
    // We treat here only the events which cannot be processed in a more specific method.
    if (e != 0 && e->type() == QEvent::LayoutRequest) {
        // One of our children notifies that its geometry has changed.
        e->accept();
        bool changed = false;
        for (int i = 0; i < _children.size(); ++i) {
            changed = _children[i].updateSize() || changed;
        }
        if (changed) {
            reorganizeLayout(true);
        }
    }

    // Propagate the event to the superclass.
    return SuperClass::event(e);
}


//----------------------------------------------------------------------------
// Invoked when the widget is resized.
//----------------------------------------------------------------------------

void QtlAutoGrid::resizeEvent(QResizeEvent* event)
{
    // Reorganize the children according to the new size.
    reorganizeLayout(false);

    // Propagate the event to the superclass.
    SuperClass::resizeEvent(event);
}


//----------------------------------------------------------------------------
// Invoked when a child is added, polished or removed.
//----------------------------------------------------------------------------

void QtlAutoGrid::childEvent(QChildEvent* event)
{
    // Let the superclass handle the event first.
    SuperClass::childEvent(event);

    // Caveat: When a child is added or removed, the event is triggered by
    // the constructor or destructor of the QObject superclass of the child.
    // The child cannot be converted to a QWidget*.

    if (event->removed()) {
        // A child is removed, check if it is part of our layout.
        for (int i = 0; i < _children.size(); ++i) {
            if (_children[i].child == event->child()) {
                // This child was part of our layout, remove it.
                _children.removeAt(i);
                reorganizeLayout(true);
                break;
            }
        }
    }
    else if (event->polished()) {
        // A new child is "polished" (ie. completely initialized).
        // Add the new child (if not already done).
        addNewChild(qobject_cast<QWidget*>(event->child()));
    }
}


//----------------------------------------------------------------------------
// Add a new child child in the group, if not yet known.
//----------------------------------------------------------------------------

bool QtlAutoGrid::addNewChild(QWidget* child)
{
    // If not a direct child, nothing to do.
    if (child == 0 || child->parent() != this) {
        return false;
    }

    // Check if the child is alreay part of our layout.
    for (int i = 0; i < _children.size(); ++i) {
        if (_children[i].child == child) {
            // Already known, nothing to do.
            return false;
        }
    }

    // This is a new direct child, add it.
    _children.append(ChildBox(child));
    reorganizeLayout(true);

    // Make the child visible.
    child->setVisible(true);

    return true;
}


//----------------------------------------------------------------------------
// Reorganize the layout of the children.
//----------------------------------------------------------------------------

void QtlAutoGrid::reorganizeLayout(bool needUpdateGeometry)
{
    // Compute row/column layout, assuming current width is mandatory.
    const int columns = columnsForWidth(width());
    const int rows = rowsForColumns(columns);

    // Compute the width of all columns.
    QVector<int> cWidth(columns);
    for (int col = 0; col < columns; ++col) {
        // Note that Coverity detects a false positive here (COPY_PASTE_ERROR)
        cWidth[col] = columnWidth(col, rows);
    }

    // Compute the height of all rows.
    QVector<int> rHeight(rows);
    for (int row = 0; row < rows; ++row) {
        rHeight[row] = rowHeight(row, rows);
    }

    // Place all children.
    for (int i = 0; i < _children.size(); ++i) {

        // Compute position.
        const int row = i % rows;
        const int col = i / rows;
        int x = _leftMargin + col * _horizontalSpacing;
        for (int c = 0; c < col; ++c) {
            x += cWidth[c];
        }
        int y = _topMargin + row * _verticalSpacing;
        for (int r = 0; r < row; ++r) {
            y += rHeight[r];
        }
        const QRect geometry(x, y, cWidth[col], rHeight[row]);

        // Move/resize child if necessary.
        if (geometry != _children[i].child->geometry()) {
            _children[i].child->setGeometry(geometry);
            needUpdateGeometry = true;
        }
    }

    // Update our geometry if required.
    if (needUpdateGeometry) {
        updateGeometry();
    }
}


//----------------------------------------------------------------------------
// Update margins and spacing.
// The layout needs to be reorganized after applying a new value.
// The request to updateGeometry() is optional with horizontal/vertical
// spacing. It shall be invoked only if a child is moved.
// With margins, the request to updateGeometry() is mandatory since
// this will change the size of the grid, even if there is no child.
//----------------------------------------------------------------------------

void QtlAutoGrid::setVerticalSpacing(int verticalSpacing)
{
    if (_verticalSpacing != verticalSpacing) {
        _verticalSpacing = verticalSpacing;
        reorganizeLayout(false); // updateGeometry() is optional
    }
}

void QtlAutoGrid::setHorizontalSpacing(int horizontalSpacing)
{
    if (_horizontalSpacing != horizontalSpacing) {
        _horizontalSpacing = horizontalSpacing;
        reorganizeLayout(false); // updateGeometry() is optional
    }
}

void QtlAutoGrid::setBottomMargin(int bottomMargin)
{
    if (_bottomMargin != bottomMargin) {
        _bottomMargin = bottomMargin;
        reorganizeLayout(true); // updateGeometry() is mandatory
    }
}

void QtlAutoGrid::setTopMargin(int topMargin)
{
    if (_topMargin != topMargin) {
        _topMargin = topMargin;
        reorganizeLayout(true); // updateGeometry() is mandatory
    }
}

void QtlAutoGrid::setLeftMargin(int leftMargin)
{
    if (_leftMargin != leftMargin) {
        _leftMargin = leftMargin;
        reorganizeLayout(true); // updateGeometry() is mandatory
    }
}

void QtlAutoGrid::setRightMargin(int rightMargin)
{
    if (_rightMargin != rightMargin) {
        _rightMargin = rightMargin;
        reorganizeLayout(true); // updateGeometry() is mandatory
    }
}


//----------------------------------------------------------------------------
// ChildBox implementation.
//----------------------------------------------------------------------------

QtlAutoGrid::ChildBox::ChildBox(QWidget* child_):
    child(child_),
    size()
{
    updateSize();
}

bool QtlAutoGrid::ChildBox::updateSize()
{
    if (child == 0) {
        return false;
    }
    else {
        const QSize previous(size);
        size = child->sizeHint();
        return size != previous;
    }
}
