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
//! @file QtlLineEdit.h
//!
//! Declare the class QtlLineEdit.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLLINEEDIT_H
#define QTLLINEEDIT_H

#include "QtlCore.h"

//!
//! A subclass of QLineEdit with enhancements.
//! Accept objects containing URL's (file names from Explorer for instance) to be dropped.
//!
class QtlLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlLineEdit(QWidget *parent = 0);

    //!
    //! Constructor.
    //! @param [in] contents Initial text content.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlLineEdit(const QString& contents, QWidget *parent = 0);

protected:
    //!
    //! Invoked when a drag operation enters the widget.
    //! @param [in,out] event The notified event.
    //!
    virtual void dragEnterEvent(QDragEnterEvent* event);
    //!
    //! Invoked when a drop operation is performed.
    //! @param [in,out] event The notified event.
    //!
    virtual void dropEvent(QDropEvent* event);
};

#endif // QTLLINEEDIT_H
