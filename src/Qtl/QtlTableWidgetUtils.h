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
//! @file QtlTableWidgetUtils.h
//!
//! Declare some utilities functions for QTableWidget.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLTABLEWIDGETUTILS_H
#define QTLTABLEWIDGETUTILS_H

#include "QtlCore.h"

//!
//! Set a header in a QTableWidget.
//! @param [in,out] table The table to modify.
//! @param [in] column Header column, starting at zero.
//! @param [in] text Header text.
//! @param [in] alignment Header alignment.
//! @return The created item.
//!
QTableWidgetItem* qtlSetTableHorizontalHeader(QTableWidget* table, int column, const QString& text, int alignment = Qt::AlignLeft);

//!
//! Remove all items in a row of a QTableWidget and return them.
//! @param [in,out] table The table to modify.
//! @param [in] row The row to remove.
//! @return A list of removed items.
//!
QList<QTableWidgetItem*> qtlTakeTableRow(QTableWidget* table, int row);

//!
//! Set all items in a row of a QTableWidget.
//! @param [in,out] table The table to modify.
//! @param [in] row The row to set.
//! @param [in] items A list of items to set. The QTableWidget takes ownership of the items.
//!
void qtlSetTableRow(QTableWidget* table, int row, const QList<QTableWidgetItem*>& items);

#endif // QTLTABLEWIDGETUTILS_H
