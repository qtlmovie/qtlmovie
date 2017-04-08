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
//! @file QtlTlvTableWidget.h
//!
//! Declare the class QtlTlvTableWidget
//! Qtl, Qt utility library.
//!
//-----------------------------------------------------------------------------

#ifndef QTLTLVTABLEWIDGET_H
#define QTLTLVTABLEWIDGET_H

#include "QtlCore.h"
#include "QtlNamedIntSet.h"
#include "QtlTlvList.h"

//!
//! A subclass of QTableWidget to edit a list of TLV items.
//!
//! The corresponding table has two columns.
//! Each row contains an editable pair of tag / value.
//!
//! The first column contains the tags. The tags may have known names
//! defined by a QtlNamedIntSet instance. The tag names are used for display
//! and edition using a combo box.
//!
//! The second column contains the values. Each value is simply an
//! editable haxadecimal string.
//!
//! A context menu (right-click) proposes the following options:
//! - Insert before this one
//! - Insert after this one
//! - Move up (Ctrl-Up arrow)
//! - Move down (Ctrl-Down arrow)
//! - Delete
//!
class QtlTlvTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlTlvTableWidget(QWidget *parent = 0);

    //!
    //! Set the list of known tag names.
    //! This list is used to display and edit the tags.
    //! @param [in] tagNames The list of known tag names.
    //!
    void setTagNames(const QtlNamedIntSet& tagNames)
    {
        _tagNames = tagNames;
    }

    //!
    //! Get a constant reference to the list of known tag names.
    //! @return A constant reference to the list of known tag names.
    //!
    const QtlNamedIntSet& tagNames() const
    {
        return _tagNames;
    }

    //!
    //! Set the number of hex digits per group as displayed in value.
    //! @param [in] valueHexDigitGroupSize The number of hex digits per group as displayed in value.
    //! When negative or zero, there is no digit grouping and all hex digits are contiguous.
    //!
    void setValueHexDigitGroupSize(int valueHexDigitGroupSize)
    {
        _valueHexDigitGroupSize = valueHexDigitGroupSize < 0 ? 0 : valueHexDigitGroupSize;
    }

    //!
    //! Get the number of hex digits per group as displayed in value.
    //! @return The number of hex digits per group as displayed in value.
    //! When zero, there is no digit grouping and all hex digits are contiguous.
    //!
    int valueHexDigitGroupSize() const
    {
        return _valueHexDigitGroupSize;
    }

    //!
    //! Get the content of the table widget as a list of TLV.
    //! @tparam TAG An integer type representing tag fields.
    //! @tparam LENGTH An integer type representing length fields.
    //! @tparam ORDER Byte order to use for serialization.
    //! @param [out] tlvList The return list of TLV.
    //!
    template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
    void getTlvList(QtlTlvList<TAG,LENGTH,ORDER>& tlvList) const;

    //!
    //! Set the content of the table widget from a list of TLV.
    //! @tparam TAG An integer type representing tag fields.
    //! @tparam LENGTH An integer type representing length fields.
    //! @tparam ORDER Byte order to use for serialization.
    //! @param [in] tlvList The list of TLV to set.
    //!
    template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
    void setTlvList(const QtlTlvList<TAG,LENGTH,ORDER>& tlvList);

    //!
    //! Get the binary serialized value of the edited TLV list.
    //! @tparam TAG An integer type representing tag fields.
    //! @tparam LENGTH An integer type representing length fields.
    //! @tparam ORDER Byte order to use for serialization.
    //! @return The binary serialized value of the edited TLV list.
    //!
    template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
    QtlByteBlock binaryValue() const;

    //!
    //! Set the edited TLV list from a binary serialized TLV list.
    //! @tparam TAG An integer type representing tag fields.
    //! @tparam LENGTH An integer type representing length fields.
    //! @tparam ORDER Byte order to use for serialization.
    //! @param [in] value The binary serialized value of a TLV list.
    //! If invalid, only keep the first items which can be parsed.
    //!
    template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
    void setBinaryValue(const QtlByteBlock& value);

    //!
    //! Get the content of a row in the TLV editor.
    //! @tparam TAG An integer type representing tag fields.
    //! @tparam LENGTH An integer type representing length fields.
    //! @tparam ORDER Byte order to use for serialization.
    //! @param [in] row Row index.
    //! @param [out] tlv The value extracted from the row. Unchanged on error.
    //! @return True on success, false on error (non-existent row, invalid value).
    //!
    template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
    bool getTlvRow(int row, QtlTlv<TAG,LENGTH,ORDER>& tlv) const;

    //!
    //! Set the content of a row in the TLV editor.
    //! @tparam TAG An integer type representing tag fields.
    //! @tparam LENGTH An integer type representing length fields.
    //! @tparam ORDER Byte order to use for serialization.
    //! @param [in] row Row index.
    //! @param [in] tlv The value to set in the row.
    //!
    template<typename TAG, typename LENGTH, QtlByteBlock::ByteOrder ORDER>
    void setTlvRow(int row, const QtlTlv<TAG,LENGTH,ORDER>& tlv)
    {
        setTlvRowData(row, tlv.tag(), tlv.value());
    }

    //!
    //! Get the first selected row in the TLV editor.
    //! @return The first selected row or -1 if none is selected.
    //!
    int getSelectedTlvRow() const;

public slots:
    //!
    //! Clear the table content.
    //! Reimplemented from QTableWidget.
    //!
    void clear();

private slots:
    //!
    //! Insert one row in TLV editor before selected one.
    //!
    void tlvInsertBeforeSelected();
    //!
    //! Insert one row in TLV editor after selected one.
    //!
    void tlvInsertAfterSelected();
    //!
    //! Move selected row up in TLV editor.
    //!
    void tlvMoveSelectedUp();
    //!
    //! Move selected row down in TLV editor.
    //!
    void tlvMoveSelectedDown();
    //!
    //! Delete selected row in TLV editor.
    //!
    void tlvDeleteSelected();

private:
    QtlNamedIntSet _tagNames;                //!< List of known tags.
    int            _valueHexDigitGroupSize;  //!< Number of hex digits per group as displayed in value.

    //!
    //! Set the content of a row in the TLV editor.
    //! @param [in] row Row index.
    //! @param [in] tag The tag to set in the row.
    //! @param [in] value The value to set in the row.
    //!
    void setTlvRowData(int row, quint32 tag, const QtlByteBlock& value);

    // Unaccessible operations.
    QtlTlvTableWidget() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlTlvTableWidget)
};

#include "QtlTlvTableWidgetTemplate.h"
#endif // QTLTLVTABLEWIDGET_H
