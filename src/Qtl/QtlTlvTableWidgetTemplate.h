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
// Define the template methods of the class QtlTlvTableWidget
//
//-----------------------------------------------------------------------------

#ifndef QTLTLVTABLEWIDGETTEMPLATE_H
#define QTLTLVTABLEWIDGETTEMPLATE_H

#include "QtlTlvTableWidget.h"
#include "QtlHexa.h"


//-----------------------------------------------------------------------------
// Get the content of the table widget as a list of TLV.
//-----------------------------------------------------------------------------

template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
void QtlTlvTableWidget::getTlvList(QtlTlvList<TAG,LENGTH,ORDER>& tlvList) const
{
    tlvList.clear();
    for (int row = 0; row < rowCount(); ++row) {
        typename QtlTlvList<TAG,LENGTH,ORDER>::Tlv tlv;
        if (getTlvRow(row, tlv)) {
            tlvList << tlv;
        }
    }
}


//-----------------------------------------------------------------------------
// Set the content of the table widget from a list of TLV.
//-----------------------------------------------------------------------------

template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
void QtlTlvTableWidget::setTlvList(const QtlTlvList<TAG,LENGTH,ORDER>& tlvList)
{
    // Enlarge / shrink the table if necessary.
    setRowCount(tlvList.size());

    // Fill the TLV table with TLV items.
    for (int row = 0; row < tlvList.size(); ++row) {
        setTlvRow(row, tlvList[row]);
    }
}


//-----------------------------------------------------------------------------
// Get the binary serialized value of the edited TLV list.
//-----------------------------------------------------------------------------

template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
QtlByteBlock QtlTlvTableWidget::binaryValue() const
{
    // Get all TLV values.
    QtlTlvList<TAG,LENGTH,ORDER> tlvList;
    getTlvList(tlvList);

    // Serialize the TLV list.
    QtlByteBlock value;
    tlvList.appendTo(value);
    return value;
}


//-----------------------------------------------------------------------------
// Set the edited TLV list from a binary serialized TLV list.
//-----------------------------------------------------------------------------

template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
void QtlTlvTableWidget::setBinaryValue(const QtlByteBlock& value)
{
    // Parse the TLV list. If invalid, only keep the first items which can be parsed.
    QtlTlvList<TAG,LENGTH,ORDER> tlvList;
    int index = 0;
    tlvList.readAt(value, index);
    setTlvList(tlvList);
}


//-----------------------------------------------------------------------------
// Get the content of a row in the TLV editor.
//-----------------------------------------------------------------------------

template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
bool QtlTlvTableWidget::getTlvRow(int row, QtlTlv<TAG,LENGTH,ORDER>& tlv) const
{
    // Get row items.
    QTableWidgetItem* itemTag = this->item(row, 0);
    QTableWidgetItem* itemValue = this->item(row, 1);
    if (itemTag == 0 || itemValue == 0) {
        // If any item is absent, cannot get the value.
        return false;
    }

    // Decode the row items.
    tlv.setTag(_tagNames.value(itemTag->text()));
    tlv.setValue(qtlHexaDecode(itemValue->text()));
    return true;
}

#endif // QTLTLVTABLEWIDGETTEMPLATE_H
