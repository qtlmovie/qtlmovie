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
// Define the class QtlButtonGrid.
//
//----------------------------------------------------------------------------

#include "QtlButtonGrid.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlButtonGrid::QtlButtonGrid(QWidget* parent) :
    SuperClass(parent),
    _buttonGroup(new QButtonGroup(this))
{
}


//----------------------------------------------------------------------------
// Add an external button in the group (not a direct child of this object).
//----------------------------------------------------------------------------

void QtlButtonGrid::addExternalButton(QAbstractButton* button, int id)
{
    // An external button must not be a child of this button grid.
    if (button != 0 && button->parent() != this) {
        if (_buttonGroup->buttons().contains(button)) {
            // The button is already in the group, simply set its id.
            _buttonGroup->setId(button, id);
        }
        else {
            // The button is not in the group, add it.
            _buttonGroup->addButton(button, id);
            // If no button is checked in the group, check this one (assumed to be the first one).
            if (_buttonGroup->checkedButton() == 0) {
                button->setChecked(true);
            }
        }
    }
}


//----------------------------------------------------------------------------
// Assign an id to a button in the group.
//----------------------------------------------------------------------------

void QtlButtonGrid::setButtonId(QAbstractButton* button, int id)
{
    if (button != 0) {
        if (_buttonGroup->buttons().contains(button)) {
            // The button is already in the group, simply set its id.
            _buttonGroup->setId(button, id);
        }
        else if (button->parent() == this) {
            // The button is not in the group but is a direct child of this button grid, add it.
            _buttonGroup->addButton(button, id);
            // If no button is checked in the group, check this one (assumed to be the first one).
            if (_buttonGroup->checkedButton() == 0) {
                button->setChecked(true);
            }
        }
    }
}



//----------------------------------------------------------------------------
// Check ("select") the button with the specified id.
//----------------------------------------------------------------------------

bool QtlButtonGrid::checkId(int id)
{
    QAbstractButton* button = _buttonGroup->button(id);
    if (button != 0) {
        button->setChecked(true);
    }
    return button != 0;
}


//----------------------------------------------------------------------------
// Invoked when a child is added, polished or removed.
//----------------------------------------------------------------------------

void QtlButtonGrid::childEvent(QChildEvent* event)
{
    // Let the superclass handle the event first.
    SuperClass::childEvent(event);

    // Automatically add new buttons in the button group.
    // Caveat: When a child is added or removed, the event is triggered by
    // the constructor or destructor of the QObject superclass of the child.
    // The child cannot be converted to a QAbstractButton*.

    // When the child is explicitely reparented after construction or when the child is
    // "polished", then the following conversion is non-zero. When zero, the object is
    // either not a button or being constructed or being destructed.
    QAbstractButton* button = qobject_cast<QAbstractButton*>(event->child());

    if (button != 0) {
        // Check if the child belongs to the button group.
        const bool inGroup = _buttonGroup->buttons().contains(button);

        if (event->removed() && inGroup) {
            // A button is removed but not destructed (since button != 0).
            // This means that the button is explicitely reparented out of the grid.
            // We remove it from the button group.
            _buttonGroup->removeButton(button);
        }
        else if ((event->added() || event->polished()) && !inGroup) {
            // A new button is added in the box. Add it in the button group.
            // Give no id. The application may set one later.
            _buttonGroup->addButton(button);
        }
    }
}
