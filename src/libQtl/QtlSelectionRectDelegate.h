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
//! @file QtlSelectionRectDelegate.h
//!
//! Declare the class QtlSelectionRectDelegate, a QStyledItemDelegate which
//! draws a rectangle around selected items.
//!
//----------------------------------------------------------------------------

#ifndef QTLSELECTIONRECTDELEGATE_H
#define QTLSELECTIONRECTDELEGATE_H

#include "QtlCore.h"

//!
//! A subclass of QStyledItemDelegate which draws a rectangle around selected items.
//!
class QtlSelectionRectDelegate : public QStyledItemDelegate
{
public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent object.
    //! @param [in] color Color of the rectangle. Black by default.
    //! @param [in] width Line width. When zero (the default), use a generic one-pixel width.
    //! @param [in] style Line style.
    //! @param [in] join Join style (for angles).
    //!
    explicit QtlSelectionRectDelegate(QObject* parent = 0,
                                      const QColor& color = Qt::black,
                                      int width = 0,
                                      Qt::PenStyle style = Qt::SolidLine,
                                      Qt::PenJoinStyle join = Qt::BevelJoin);

    //!
    //! Renders the delegate using the given painter and style option for the item specified by index.
    //! Reimplemented from QStyledItemDelegate.
    //! @param [in] painter The painter to use.
    //! @param [in] option The style to use. Typically gives the geometry of the item.
    //! @param [in] index The index of the item.
    //!
    virtual void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    const QColor           _color; //!< Color of the rectangle.
    const int              _width; //!< Line width.
    const Qt::PenStyle     _style; //!< Line style.
    const Qt::PenJoinStyle _join;  //!< Join style (for angles).

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlSelectionRectDelegate)
};

#endif // QTLSELECTIONRECTDELEGATE_H
