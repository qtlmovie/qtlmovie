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
QTableWidgetItem* qtlSetTableHorizontalHeader(QTableWidget* table, int column, const QString& text, int alignment = Qt::AlignLeft | Qt::AlignVCenter);

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
//! @param [in] row The row to set. If outside the table size, the table is enlarged.
//! @param [in] items A list of items to set. The QTableWidget takes ownership of the items.
//! @return The row number. This may not be identical to @a row if automatic sorting was enabled.
//!
int qtlSetTableRow(QTableWidget* table, int row, const QList<QTableWidgetItem*>& items);

//!
//! Set the texts of all items in a row of a QTableWidget.
//! @param [in,out] table The table to modify.
//! @param [in] row The row to set. If outside the table size, the table is enlarged.
//! @param [in] texts The texts to set in the various columns of the row.
//! @param [in] copyHeaderTextAlignment If true, set the text alignment to the same value as the header of the same column.
//! @param [in] flags The flags to set in all cells in this row. If @a checkBoxColumn is non-negative, the cell with the
//! checkbox automatically has flags @c Qt::ItemIsUserCheckable and @c Qt::ItemIsEnabled.
//! @param [in] checkBoxColumn Add a checkbox in this column. Ignored if negative.
//! @return The row number. This may not be identical to @a row if automatic sorting was enabled.
//!
int qtlSetTableRow(QTableWidget* table,
                   int row,
                   const QStringList& texts,
                   bool copyHeaderTextAlignment = false,
                   Qt::ItemFlags flags = Qt::ItemIsEnabled,
                   int checkBoxColumn = -1);

//!
//! Set the text style of a region of a QTableWidget.
//! @param [in,out] table The table to modify.
//! @param [in] firstRow First row in @a table to modify.
//! @param [in] lastRow Last row in @a table to modify.
//! @param [in] firstColumn First column in @a table to modify.
//! @param [in] lastColumn Last column in @a table to modify.
//! @param [in] background Background color. Ignored if omitted.
//! @param [in] foreground Foreground color. Ignored if omitted.
//! @param [in] style Background color style.
//!
void qtlSetTableStyle(QTableWidget* table,
                      int firstRow,
                      int lastRow,
                      int firstColumn,
                      int lastColumn,
                      const QColor& background = QColor(),
                      const QColor& foreground = QColor(),
                      Qt::BrushStyle style = Qt::SolidPattern);

//!
//! Set the text style of a row of a QTableWidget.
//! @param [in,out] table The table to modify.
//! @param [in] row Row in @a table to modify.
//! @param [in] background Background color. Ignored if omitted.
//! @param [in] foreground Foreground color. Ignored if omitted.
//! @param [in] style Background color style.
//!
void qtlSetTableRowStyle(QTableWidget* table,
                         int row,
                         const QColor& background = QColor(),
                         const QColor& foreground = QColor(),
                         Qt::BrushStyle style = Qt::SolidPattern);

#endif // QTLTABLEWIDGETUTILS_H
