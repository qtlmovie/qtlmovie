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
//! @file QtlNamedIntComboBox.h
//!
//! Declare the class QtlNamedIntComboBox.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLNAMEDINTCOMBOBOX_H
#define QTLNAMEDINTCOMBOBOX_H

#include <QComboBox>
#include "QtlNamedIntSet.h"
#include "QtlIntValidator.h"

//!
//! A subclass of QComboBox which takes its list of values from a QtlNamedIntSet.
//! When the QtlNamedIntComboBox is editable, a decimal or hexadecimal value can be entered.
//!
class QtlNamedIntComboBox : public QComboBox
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlNamedIntComboBox(QWidget* parent = 0);

    //!
    //! Get the minimum acceptable value.
    //! @return The minimum acceptable value.
    //!
    quint32 minimum() const
    {
        return _validator->minimum();
    }

    //!
    //! Set the minimum acceptable value.
    //! @param [in] minimum The minimum acceptable value.
    //!
    void setMinimum(quint32 minimum)
    {
        _validator->setMinimum(minimum);
    }

    //!
    //! Get the maximum acceptable value.
    //! @return The maximum acceptable value.
    //!
    quint32 maximum() const
    {
        return _validator->maximum();
    }

    //!
    //! Set the maximum acceptable value.
    //! @param [in] maximum The maximum acceptable value.
    //!
    void setMaximum(quint32 maximum)
    {
        _validator->setMaximum(maximum);
    }

    //!
    //! Set the minimum and maximum acceptable values.
    //! @param [in] minimum The minimum acceptable value.
    //! @param [in] maximum The maximum acceptable value.
    //!
    void setRange(quint32 minimum, quint32 maximum)
    {
        _validator->setRange(minimum, maximum);
    }

    //!
    //! Get the set of named values.
    //! @return The set of named values.
    //!
    QtlNamedIntSet names() const
    {
        return _names;
    }

    //!
    //! Set the set of named values.
    //! @param [in] names The set of named values.
    //!
    void setNames(const QtlNamedIntSet& names);

    //!
    //! Get the current integer value in the combo box.
    //! @return The current integer value in the combo box.
    //!
    quint32 currentValue() const;

    //!
    //! Set the current integer value in the combo box.
    //! @param [in] currentValue The current integer value in the combo box.
    //!
    void setCurrentValue(quint32 currentValue);

    //!
    //! Set the editable flag in the combo box.
    //! Reimplemented from QComboBox.
    //! @param [in] editable The editable flag in the combo box.
    //!
    void setEditable(bool editable);

signals:
    //!
    //! Emitted when the current integer value in the combo box changes.
    //! @param [in] currentValue The current integer value in the combo box.
    //!
    void currentValueChanged(quint32 currentValue);

private slots:
    //!
    //! Received when the current text has changed in the combo box.
    //! @param [in] text The current text in the combo box.
    //!
    void receiveCurrentText(const QString& text);

private:
    QtlIntValidator* _validator;    //! Validate input (when editable).
    QtlNamedIntSet   _names;        //! The names of the values.

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlNamedIntComboBox)
};

#endif // QTLNAMEDINTCOMBOBOX_H
