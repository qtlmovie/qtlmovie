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
//
// Define the class QtlSelectionRectDelegate, a QStyledItemDelegate which
// draws a rectangle around selected items.
//
//----------------------------------------------------------------------------

#include "QtlSelectionRectDelegate.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlSelectionRectDelegate::QtlSelectionRectDelegate(QObject* parent,
                                                   const QColor& color,
                                                   int width,
                                                   Qt::PenStyle style,
                                                   Qt::PenJoinStyle join) :
    QStyledItemDelegate(parent),
    _color(color),
    _width(qMax(width, 0)),
    _style(style),
    _join(join)
{
}


//----------------------------------------------------------------------------
// Renders the delegate. Reimplemented from QStyledItemDelegate.
//----------------------------------------------------------------------------

void QtlSelectionRectDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Check if the item is selected (highlighted, whatever).
    const bool selected = (option.state & QStyle::State_Selected) != 0;

    // For some reason (?), the background shall be explicitly painted here if and
    // only if the item is selected. Experience shows that:
    // - If the background is not painted for selected items, the background color
    //   is ignored and a uniform color is applied for all selected items.
    // - If the background is explicitly painted on non-selected items, the correct
    //   color is used but the brush style is sometimes ignored (but not always!).
    const QVariant background(index.data(Qt::BackgroundRole));
    if (selected && background.canConvert<QBrush>()) {
        painter->fillRect(option.rect, background.value<QBrush>());
    }

    // Invoke superclass for the rest.
    QStyledItemDelegate::paint(painter, option, index);

    // Draw a rectangle around selected items.
    if (selected) {

        // Save the painter state since we will modify it.
        painter->save();

        // Prepare a pen to draw the restangle.
        // The cap style is useless since there is no end of line in a closed rectangle.
        const QPen pen(QBrush(_color), _width, _style, Qt::SquareCap, _join);

        // Draw the rectangle exactly inside the item edges.
        const int w = pen.width() / 2;
        painter->setPen(pen);
        painter->drawRect(option.rect.adjusted(w, w, qMin(-w, -1), qMin(-w, -1)));

        // Restore the input painter state.
        painter->restore();
    }
}
