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
// Define the class QtlNamedIntComboBox.
// Qtl, Qt utility library.
//
//----------------------------------------------------------------------------

#include "QtlNamedIntComboBox.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlNamedIntComboBox::QtlNamedIntComboBox(QWidget* parent) :
    QComboBox(parent),
    _validator(new QtlIntValidator(this)),
    _names()
{
    // Set the validator for the editable input.
    setValidator(_validator);
    setInsertPolicy(InsertAtTop);

    // Transpose notification of text input into integer data.
    connect(this, &QtlNamedIntComboBox::currentTextChanged, this, &QtlNamedIntComboBox::receiveCurrentText);
}


//----------------------------------------------------------------------------
// Set the editable flag in the combo box.
//----------------------------------------------------------------------------

void QtlNamedIntComboBox::setEditable(bool editable)
{
    // Invoke superclass.
    QComboBox::setEditable(editable);

    // Force the validator again.
    // It has been observed that setting the validator was ignored while editable is false.
    setValidator(_validator);
    setInsertPolicy(InsertAtTop);
}


//----------------------------------------------------------------------------
// Set the set of named values.
//----------------------------------------------------------------------------

void QtlNamedIntComboBox::setNames(const QtlNamedIntSet& names)
{
    // Get a copy of the QtlNamedIntSet object.
    _names = names;

    // Remove all items in the combo box.
    clear();

    // Set a fresh new list of items.
    addItems(_names.names());
}


//----------------------------------------------------------------------------
// Get the current integer value in the combo box.
//----------------------------------------------------------------------------

quint32 QtlNamedIntComboBox::currentValue() const
{
    return _names.value(currentText());
}


//----------------------------------------------------------------------------
// Set the current integer value in the combo box.
//----------------------------------------------------------------------------

void QtlNamedIntComboBox::setCurrentValue(quint32 currentValue)
{
    setCurrentText(_names.name(currentValue));
}


//----------------------------------------------------------------------------
// Received when the current text has changed in the combo box.
//----------------------------------------------------------------------------

void QtlNamedIntComboBox::receiveCurrentText(const QString& text)
{
    emit currentValueChanged(currentValue());
}
