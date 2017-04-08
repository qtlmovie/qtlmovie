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
// Qtl, Qt utility library.
// Definition of utilities for QTableWidget.
//
//----------------------------------------------------------------------------

#include "QtlTableWidgetUtils.h"


//-----------------------------------------------------------------------------
// Set a header in a QTableWidget.
//-----------------------------------------------------------------------------

QTableWidgetItem* qtlSetTableHorizontalHeader(QTableWidget* table, int column, const QString& text, int alignment)
{
    if (table == 0) {
        return 0;
    }
    else {
        // Increase the number of columns if required.
        if (table->columnCount() <= column) {
            table->setColumnCount(column + 1);
        }

        // Fill the header.
        QTableWidgetItem* item = new QTableWidgetItem(text);
        item->setTextAlignment(alignment);
        table->setHorizontalHeaderItem(column, item);
        return item;
    }
}


//-----------------------------------------------------------------------------
// Remove all items in a row of a QTableWidget and return them.
//-----------------------------------------------------------------------------

QList<QTableWidgetItem*> qtlTakeTableRow(QTableWidget* table, int row)
{
    QList<QTableWidgetItem*> items;
    if (table != 0 && row >= 0 && row < table->rowCount()) {
        for (int column = 0; column < table->columnCount(); ++column) {
            items << table->takeItem(row, column);
        }
    }
    return items;
}


//-----------------------------------------------------------------------------
// Set all items in a row of a QTableWidget.
//-----------------------------------------------------------------------------

int qtlSetTableRow(QTableWidget* table, int row, const QList<QTableWidgetItem*>& items)
{
    if (table != 0 && !items.isEmpty() && row >= 0) {

        // Enlarge the table if required.
        if (row >= table->rowCount()) {
            table->setRowCount(row + 1);
        }

        // Set all columns in the row.
        for (int column = 0; column < items.size() && column < table->columnCount(); ++column) {
            QTableWidgetItem* const item = items[column];
            if (item != 0) {
                table->setItem(row, column, item);
                // The row may have changed if automatic sorting was enabled.
                row = item->row();
            }
        }
    }

    return row;
}


//-----------------------------------------------------------------------------
// Set the texts of all items in a row of a QTableWidget.
//-----------------------------------------------------------------------------

int qtlSetTableRow(QTableWidget* table, int row, const QStringList& texts, bool copyHeaderTextAlignment, Qt::ItemFlags flags, int checkBoxColumn)
{
    if (table != 0 && !texts.isEmpty() && row >= 0) {

        // Enlarge the table if required.
        if (row >= table->rowCount()) {
            table->setRowCount(row + 1);
        }

        // Set all columns in the row.
        for (int column = 0; column < texts.size() && column < table->columnCount(); ++column) {

            // Get the item, create it if necessary.
            QTableWidgetItem* item = table->item(row, column);
            if (item == 0) {
                item = new QTableWidgetItem();
                table->setItem(row, column, item);
            }

            // Update the text in the item.
            item->setText(texts[column]);
            item->setFlags(flags);

            // Copy text alignment from header if requested.
            if (copyHeaderTextAlignment) {
                QTableWidgetItem* header = table->horizontalHeaderItem(column);
                if (header != 0) {
                    item->setTextAlignment(header->textAlignment());
                }
            }

            // Add a checkbox if requested.
            if (column == checkBoxColumn) {
                item->setFlags(flags | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                item->setCheckState(Qt::Unchecked);
            }

            // The row may have changed if automatic sorting was enabled.
            row = item->row();
        }
    }

    return row;
}


//-----------------------------------------------------------------------------
// Set the text style of a region of a QTableWidget.
//-----------------------------------------------------------------------------

void qtlSetTableStyle(QTableWidget* table,
                      int firstRow,
                      int lastRow,
                      int firstColumn,
                      int lastColumn,
                      const QColor& background,
                      const QColor& foreground,
                      Qt::BrushStyle style)
{
    if (table != 0) {
        for (int row = qMax(0, firstRow); row <= qMin(lastRow, table->rowCount() - 1); ++row) {
            for (int column = qMax(0, firstColumn); column <= qMin(lastColumn, table->columnCount() - 1); ++column) {
                QTableWidgetItem* item = table->item(row, column);
                if (item != 0) {
                    if (background.isValid()) {
                        item->setBackground(QBrush(background, style));
                    }
                    if (foreground.isValid()) {
                        item->setForeground(QBrush(foreground));
                    }
                }
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Set the text style of a row of a QTableWidget.
//-----------------------------------------------------------------------------

void qtlSetTableRowStyle(QTableWidget* table, int row, const QColor& background, const QColor& foreground, Qt::BrushStyle style)
{
    if (table != 0 && row >= 0 && row < table->rowCount()) {
        qtlSetTableStyle(table, row, row, 0, table->columnCount() - 1, background, foreground, style);
    }
}
