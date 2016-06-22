//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
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
//! @file QtlLayoutUtils.h
//!
//! Declare some utilities functions for QLayout.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLLAYOUTUTILS_H
#define QTLLAYOUTUTILS_H

#include "QtlCore.h"

//!
//! Insert one or more rows in a QGridLayout.
//!
//! All widgets below the inserted row are moved downwards.
//! The inserted rows are empty.
//!
//! Limitation: The moved widgets shall use only one cell. If any item to move
//! uses a row span or column span larger that 1, it will be moved to one single
//! cell after row insertion.
//!
//! @param [in,out] layout The layout to modify.
//! @param [in] atRow Position of the row to insert.
//! @param [in] rowCount Number of rows to insert, 1 by default.
//!
void qtlLayoutInsertRows(QGridLayout* layout, int atRow, int rowCount = 1);

#endif // QTLLAYOUTUTILS_H
