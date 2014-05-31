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
//! @file QtlGeometrySettingsInterface.h
//!
//! Declare the class QtlGeometrySettingsInterface.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLGEOMETRYSETTINGSINTERFACE_H
#define QTLGEOMETRYSETTINGSINTERFACE_H

#include <QWidget>

//!
//! An interface class used to save and restore the geometry of widgets.
//!
class QtlGeometrySettingsInterface
{
public:
    //!
    //! Save the geometry of a widget.
    //! A typical implementation would use the object name of @a widget
    //! as a key to index the geometry of the widget.
    //! @param [in] widget The widget to save the geometry of.
    //!
    virtual void saveGeometry(const QWidget* widget) = 0;
    //!
    //! Restore the geometry of a widget.
    //! A typical implementation would use the object name of @a widget
    //! as a key to retrieve the geometry of the widget.
    //! If no geometry was saved for a widget of that name, do not modify @a widget.
    //! @param [in,out] widget The widget to restore the geometry.
    //!
    virtual void restoreGeometry(QWidget* widget) = 0;
};

#endif // QTLGEOMETRYSETTINGSINTERFACE_H
