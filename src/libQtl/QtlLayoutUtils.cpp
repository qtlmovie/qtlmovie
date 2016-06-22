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
// Definition of utilities for QLayout.
//
//----------------------------------------------------------------------------

#include "QtlLayoutUtils.h"


//----------------------------------------------------------------------------
// Insert one or more rows in a QGridLayout.
//----------------------------------------------------------------------------

void qtlLayoutInsertRows(QGridLayout* layout, int atRow, int rowCount)
{
    // Filter invalid parameters or nothing to do.
    if (layout == 0 || rowCount < 1 || atRow < 0 || atRow >= layout->rowCount()) {
        return;
    }

    const int rows = layout->rowCount();
    const int columns = layout->columnCount();

    for (int row = rows - 1; row >= atRow; --row) {
        for (int col = 0; col < columns; ++col) {
            QLayoutItem* item = layout->itemAtPosition(row, col);
            if (item != 0) {
                Qt::Alignment align = item->alignment();
                layout->removeItem(item);
                layout->addItem(item, row + rowCount, col, 1, 1, align);
            }
        }
    }
}
