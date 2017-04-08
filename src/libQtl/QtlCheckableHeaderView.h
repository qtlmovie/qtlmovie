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
//! @file QtlCheckableHeaderView.h
//!
//! Declare the class QtlCheckableHeaderView.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLCHECKABLEHEADERVIEW_H
#define QTLCHECKABLEHEADERVIEW_H

#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>

//!
//! A subclass of QHeaderView with a checkbox in the first column.
//! The checkbox in the header allows to select / deselect all checkboxes in the first column.
//! The header view is always horizontal (top row).
//!
class QtlCheckableHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlCheckableHeaderView(QWidget* parent = 0);

    //!
    //! Get the state of the checkbox in the header.
    //! @return The state of the checkbox in the header.
    //!
    Qt::CheckState checkState() const
    {
        return _checkState;
    }

signals:
    //!
    //! Emitted when the state of the checkbox in the header changes.
    //! @param [in] state New state of the checkbox.
    //!
    void checkStateChanged(Qt::CheckState state);

protected:
    //!
    //! Process a change of check state in the header.
    //!
    void processCheckStateChanged();

    //!
    //! Paint the section specified by the given @a logicalIndex, using the given @a painter and @a rect.
    //! Reimplemented from QHeaderView
    //! @param [in] painter Painter to use.
    //! @param [in] rect Area to paint.
    //! @param [in] logicalIndex Section index, 0 being the first section where the checkbox is located.
    //!
    virtual void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const Q_DECL_OVERRIDE;

    //!
    //! Process a mouse-press event.
    //! Reimplemented from QHeaderView
    //! @param [in] event The mouse event.
    //!
    virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
    //!
    //! Compute the position and size of the checkbox in the header.
    //! @param [in] reference Optional reference point. If non-zero, the returned rectangle
    //! is set relatively to this point.
    //! @return Position and size of the checkbox in the header.
    //!
    QRect checkBoxRect(const QPoint& reference = QPoint()) const;

    //!
    //! Update the checkboxes in all rows, based on the checkbox in the header.
    //!
    void updateAllRows();

    // Note: The following members are mutable since they are updated in paintSection().
    // And paintSection() is "const" in the superclass, which is strange.
    mutable Qt::CheckState _checkState;  //!< Current state of the checkbox in header.
    mutable bool           _forceCheck;  //!< Force either checked or unchecked on next paint.
};

#endif // QTLCHECKABLEHEADERVIEW_H
