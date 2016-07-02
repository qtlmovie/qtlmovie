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
//! @file QtlFixedSizeTableWidget.h
//!
//! Declare the QtlFixedSizeTableWidget class.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLFIXEDSIZETABLEWIDGET_H
#define QTLFIXEDSIZETABLEWIDGET_H

#include <QTableWidget>

//!
//! A subclass of QTableWidget which enforces a fixed table size.
//! Can be used to enforce the display of exactly all lines and columns.
//!
class QtlFixedSizeTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlFixedSizeTableWidget(QWidget* parent = 0);

    //!
    //! Constructor.
    //! @param [in] rows Number of rows in the table.
    //! @param [in] columns Number of columns in the table.
    //! @param [in] parent Optional parent widget.
    //!
    QtlFixedSizeTableWidget(int rows, int columns, QWidget* parent = 0);

    //!
    //! Return the widget size hint.
    //! Reimplemented from QWidget to always provide the exact size of all rows and columns.
    //! @return The size hint.
    //!
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    //!
    //! Show event handler.
    //! Reimplemented from QWidget.
    //! @param event Show event.
    //!
    virtual void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;

private slots:
    //!
    //! Invoked when a header section is resized.
    //!
    void sectionResized();

private:
    //!
    //! Common initialization code for constructors.
    //!
    void init();
    //!
    //! Enforce the table widget size.
    //!
    void enforceSize();
};

#endif // QTLFIXEDSIZETABLEWIDGET_H
