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
//!
//! @file QtlAutoGrid.h
//!
//! Declare the class QtlAutoGrid.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLAUTOGRID_H
#define QTLAUTOGRID_H

#include <QtWidgets>

//!
//! A simple grid container which rearrange the children based on its size.
//!
//! When the container is resized, the children are rearranged to fill
//! rows first, ie. to occupy the maximum width.
//!
//! Limitations:
//! - This class should be implemented as a layout. It is not.
//! - The geometry of the children is only based on their sizeHint().
//!   Their sizePolicy() is ignored.
//! - This is quick and dirty in fact...
//!
class QtlAutoGrid : public QWidget
{
#if !defined (DOXYGEN)
    Q_OBJECT
#endif

public:
    //!
    //! Reference to superclass.
    //!
    typedef QWidget SuperClass;

    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlAutoGrid(QWidget *parent = 0);

    //!
    //! Get the left margin in pixels.
    //! @return The left margin in pixels.
    //!
    int leftMargin() const
    {
        return _leftMargin;
    }

    //!
    //! Set the left margin in pixels.
    //! @param [in] leftMargin The left margin in pixels.
    //!
    void setLeftMargin(int leftMargin);

    //!
    //! Get the right margin in pixels.
    //! @return The right margin in pixels.
    //!
    int rightMargin() const
    {
        return _rightMargin;
    }

    //!
    //! Set the right margin in pixels.
    //! @param [in] rightMargin The right margin in pixels.
    //!
    void setRightMargin(int rightMargin);

    //!
    //! Get the top margin in pixels.
    //! @return The top margin in pixels.
    //!
    int topMargin() const
    {
        return _topMargin;
    }

    //!
    //! Set the top margin in pixels.
    //! @param [in] topMargin The top margin in pixels.
    //!
    void setTopMargin(int topMargin);

    //!
    //! Get the bottom margin in pixels.
    //! @return The bottom margin in pixels.
    //!
    int bottomMargin() const
    {
        return _bottomMargin;
    }

    //!
    //! Set the bottom margin in pixels.
    //! @param [in] bottomMargin The bottom margin in pixels.
    //!
    void setBottomMargin(int bottomMargin);

    //!
    //! Get the horizontal spacing in pixels.
    //! @return The horizontal spacing in pixels.
    //!
    int horizontalSpacing() const
    {
        return _horizontalSpacing;
    }

    //!
    //! Set the horizontal spacing in pixels.
    //! @param [in] horizontalSpacing The horizontal spacing in pixels.
    //!
    void setHorizontalSpacing(int horizontalSpacing);

    //!
    //! Get the vertical spacing in pixels.
    //! @return The vertical spacing in pixels.
    //!
    int verticalSpacing() const
    {
        return _verticalSpacing;
    }

    //!
    //! Set the vertical spacing in pixels.
    //! @param [in] verticalSpacing The vertical spacing in pixels.
    //!
    void setVerticalSpacing(int verticalSpacing);

    //!
    //! Return the recommended minimum size for the widget.
    //! Reimplemented from QWidget.
    //! @return The recommended minimum size.
    //!
    virtual QSize minimumSizeHint() const;

    //!
    //! Return the widget size hint.
    //! Reimplemented from QWidget.
    //! @return The size hint.
    //!
    virtual QSize sizeHint() const;

    //!
    //! Check if the preferred height of the widget depends on its width.
    //! Reimplemented from QWidget.
    //! @return True.
    //!
    virtual bool hasHeightForWidth() const
    {
        return true;
    }

    //!
    //! Compute the widget height for a given width.
    //! Reimplemented from QWidget.
    //! @param [in] width The proposed width for the widget.
    //! @return The corresponding height.
    //!
    virtual int	heightForWidth(int width) const;

protected:
    //!
    //! General event handler.
    //! Reimplemented from QWidget and QObject.
    //! @param event Event.
    //! @return True if the event was recognized and processed.
    //!
    virtual bool event(QEvent* event);

    //!
    //! Invoked when a child is added, polished or removed.
    //! Reimplemented from QObject.
    //! @param [in] event The event to intercept.
    //!
    virtual void childEvent(QChildEvent* event);

    //!
    //! Invoked when the widget is resized.
    //! Reimplemented from QWidget.
    //! @param [in] event The event to intercept.
    //!
    virtual void resizeEvent(QResizeEvent* event);

private:
    //!
    //! Description of one child widget in the grid.
    //!
    class ChildBox
    {
    public:
        QWidget* child; //!< The child object.
        QSize    size;  //!< Last known (cached) size of the child.
        //!
        //! Constructor.
        //! @param [in] child Optional child widget.
        //!
        ChildBox(QWidget* child);
        //!
        //! Update the known @link size @endlink of @link child @endlink.
        //! @return True if the size has changed, false otherwise.
        //!
        bool updateSize();
    };

    // Private members.
    int _leftMargin;           //!< Left margin in pixels.
    int _rightMargin;          //!< Right margin in pixels.
    int _topMargin;            //!< Top  margin in pixels.
    int _bottomMargin;         //!< Bottom margin in pixels.
    int _horizontalSpacing;    //!< Horizontal spacing between pixels.
    int _verticalSpacing;      //!< Vertical spacing between pixels.
    QList<ChildBox> _children; //!< List of managed children.

    //!
    //! Compute the number of columns for a given widget width.
    //! @param [in] width The proposed width for the widget.
    //! @return The corresponding number of columns.
    //!
    int columnsForWidth(int width) const;

    //!
    //! Compute the widget width for a given number of columns.
    //! @param [in] columns The proposed number of columns.
    //! @return The corresponding width for the widget.
    //!
    int widthForColumns(int columns) const;

    //!
    //! Compute the number of rows for a given number of columns.
    //! @param [in] columns The number of columns.
    //! @return The corresponding number of rows.
    //!
    int rowsForColumns(int columns) const;

    //!
    //! Compute the height of a given row, assuming a given row/column layout.
    //! @param [in] row Index of the row.
    //! @param [in] rows Number of rows in the grid.
    //! @return Height in pixels of @a row.
    //!
    int rowHeight(int row, int rows) const;

    //!
    //! Compute the width of a given column, assuming a given row/column layout.
    //! @param [in] column Index of the column.
    //! @param [in] rows Number of rows in the grid.
    //! @return Width in pixels of @a column.
    //!
    int columnWidth(int column, int rows) const;

    //!
    //! Register a new child, if not yet known.
    //! @param [in] child The child to add.
    //! @return True if the child was added, false if it was
    //! already known or is not a direct child of this object.
    //!
    bool addNewChild(QWidget* child);

    //!
    //! Reorganize the layout of the children widgets.
    //! @param [in] needUpdateGeometry If true, invoke updateGeometry()
    //! even if no widget has been moved.
    //!
    void reorganizeLayout(bool needUpdateGeometry);

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlAutoGrid)
};

#endif // QTLAUTOGRID_H
