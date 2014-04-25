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
//! @file QtlButtonGrid.h
//!
//! Declare the class QtlButtonGrid.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLBUTTONGRID_H
#define QTLBUTTONGRID_H

#include <QtWidgets>
#include "QtlAutoGrid.h"

//!
//! A containers for buttons with a grid layout and id values.
//!
class QtlButtonGrid : public QtlAutoGrid
{
    Q_OBJECT

public:
    //!
    //! Reference to superclass.
    //!
    typedef QtlAutoGrid SuperClass;

    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlButtonGrid(QWidget *parent = 0);

    //!
    //! Add an external button in the group (not a direct child of this object).
    //! @param [in] button The button to add.
    //! @param [in] id The id to assign to this button.
    //!
    void addExternalButton(QAbstractButton* button, int id = -1);

    //!
    //! Assign an id to a button in the group.
    //! @param [in] button The button to assign.
    //! @param [in] id The id to assign to this button.
    //!
    void setButtonId(QAbstractButton* button, int id = -1);

    //!
    //! Search the button which is associated to an id.
    //! @param [in] id The id to search.
    //! @return The first button which is associated to @a id or zero if none found.
    //!
    QAbstractButton* buttonOf(int id) const
    {
        return _buttonGroup->button(id);
    }

    //!
    //! Get the id of the specified button.
    //! @return The id of the specified button or -1 if the button is unknown or has no id.
    //!
    int buttonId(QAbstractButton* button) const
    {
        return _buttonGroup->id(button);
    }

    //!
    //! Get the id of the checked button.
    //! @return The id of the checked button or -1 if no button is checked
    //! or if the checked button has no id.
    //!
    int checkedId() const
    {
        return _buttonGroup->checkedId();
    }

    //!
    //! Check ("select") the button with the specified id.
    //! @param [in] id The id of the button to check.
    //! @return True if the button was found, false if not found (and consequently not checked).
    //!
    bool checkId(int id);

protected:
    //!
    //! Invoked when a child is added, polished or removed.
    //! Reimplemented from QObject.
    //! @param [in] event The event to intercept.
    //!
    virtual void childEvent(QChildEvent* event);

private:
    QButtonGroup* _buttonGroup; //!< The button group.

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlButtonGrid)
};

#endif // QTLBUTTONGRID_H
