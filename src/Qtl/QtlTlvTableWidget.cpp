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
// Define the class QtlTlvTableWidget
// Qtl, Qt utility library.
//
//----------------------------------------------------------------------------

#include "QtlTlvTableWidget.h"
#include "QtlNamedIntComboBox.h"
#include "QtlPlainTextEdit.h"
#include "QtlTableWidgetUtils.h"
#include "QtlMessageBoxUtils.h"
#include "QtlStringUtils.h"
#include "QtlFontUtils.h"
#include "QtlHexa.h"


//-----------------------------------------------------------------------------
// Delegated editor for TLV tags.
//-----------------------------------------------------------------------------

namespace {
    //!
    //! A subclass of QStyledItemDelegate for editing tag values in tables.
    //!
    class TagItemDelegate : public QStyledItemDelegate
    {
    public:
        //!
        //! Constructor.
        //! @param [in] tagNames List of known tags.
        //! @param [in] parent Optional parent widget.
        //!
        TagItemDelegate(const QtlNamedIntSet& tagNames, QObject *parent = 0) :
            QStyledItemDelegate(parent),
            _tagNames(tagNames)
        {
        }
        //!
        //! Create the editor widget.
        //! @param [in] parent Parent widget for the new editor.
        //! @param [in] option How to draw the editor.
        //! @param [in] index Index in the data model.
        //! @return The new editor widget.
        //!
        virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex &index) const
        {
            QtlNamedIntComboBox* combo = new QtlNamedIntComboBox(parent);
            combo->setNames(_tagNames);
            combo->setEditable(true);
            return combo;
        }
        //!
        //! Transfer data from the model to the editor widget.
        //! Typically invoked before editing the item.
        //! @param [in] editor The editor widget.
        //! @param [in] index Index in the data model.
        //!
        virtual void setEditorData(QWidget* editor, const QModelIndex& index) const
        {
            QtlNamedIntComboBox* combo = qobject_cast<QtlNamedIntComboBox*>(editor);
            if (combo == 0) {
                // Not an editor we created, pass to superclass.
                QStyledItemDelegate::setEditorData(editor, index);
            }
            else {
                // Get the current text of the item, convert it from a tag name to a tag value.
                combo->setCurrentValue(_tagNames.value(index.data(Qt::EditRole).toString()));
            }
        }
        //!
        //! Transfer data from the editor widget to the data model.
        //! Typically invoked after editing the item.
        //! @param [in] editor The editor widget.
        //! @param [in] model The data model.
        //! @param [in] index Index in the data model.
        //!
        virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
        {
            QtlNamedIntComboBox* combo = qobject_cast<QtlNamedIntComboBox*>(editor);
            if (combo == 0) {
                // Not an editor we created, pass to superclass.
                QStyledItemDelegate::setModelData(editor, model, index);
            }
            else {
                // Set the model data to the corresponding tag name.
                model->setData(index, combo->currentText(), Qt::EditRole);
            }
        }
    private:
        const QtlNamedIntSet& _tagNames;  //!< List of known tags.

        // Unaccessible operations.
        TagItemDelegate() Q_DECL_EQ_DELETE;
        Q_DISABLE_COPY(TagItemDelegate)
    };
}


//-----------------------------------------------------------------------------
// Delegated editor for TLV value.
//-----------------------------------------------------------------------------

namespace {
    //!
    //! A subclass of QStyledItemDelegate for editing TLV values in tables.
    //!
    class TlvValueItemDelegate : public QStyledItemDelegate
    {
    public:
        //!
        //! Constructor.
        //! @param [in] parent Optional parent widget.
        //!
        TlvValueItemDelegate(QObject *parent = 0) :
            QStyledItemDelegate(parent)
        {
        }
        //!
        //! Create the editor widget.
        //! @param [in] parent Parent widget for the new editor.
        //! @param [in] option How to draw the editor.
        //! @param [in] index Index in the data model.
        //! @return The new editor widget.
        //!
        virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex &index) const
        {
            // Create an editor for hexadecimal content.
            return new QtlPlainTextEdit(parent, QtlPlainTextEdit::Monospace | QtlPlainTextEdit::ToUpper | QtlPlainTextEdit::Spaces | QtlPlainTextEdit::Hexa);
        }
        //!
        //! Transfer data from the model to the editor widget.
        //! Typically invoked before editing the item.
        //! @param [in] editor The editor widget.
        //! @param [in] index Index in the data model.
        //!
        virtual void setEditorData(QWidget* editor, const QModelIndex& index) const
        {
            QtlPlainTextEdit* edit = qobject_cast<QtlPlainTextEdit*>(editor);
            if (edit == 0) {
                // Not an editor we created, pass to superclass.
                QStyledItemDelegate::setEditorData(editor, index);
            }
            else {
                // Get the current text of the item.
                edit->setPlainText(index.data(Qt::EditRole).toString());
            }
        }
        //!
        //! Transfer data from the editor widget to the data model.
        //! Typically invoked after editing the item.
        //! @param [in] editor The editor widget.
        //! @param [in] model The data model.
        //! @param [in] index Index in the data model.
        //!
        virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
        {
            QtlPlainTextEdit* edit = qobject_cast<QtlPlainTextEdit*>(editor);
            if (edit == 0) {
                // Not an editor we created, pass to superclass.
                QStyledItemDelegate::setModelData(editor, model, index);
            }
            else {
                // Set the model data to the editor content.
                model->setData(index, edit->toPlainText(), Qt::EditRole);
            }
        }
    private:
        // Unaccessible operations.
        TlvValueItemDelegate() Q_DECL_EQ_DELETE;
        Q_DISABLE_COPY(TlvValueItemDelegate)
    };
}


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------

QtlTlvTableWidget::QtlTlvTableWidget(QWidget *parent) :
    QTableWidget(parent),
    _tagNames(),
    _valueHexDigitGroupSize(0)
{
    // There are two colums: tag and value.
    setColumnCount(2);

    // Setup the headers.
    verticalHeader()->setVisible(false);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader()->setVisible(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader()->setStretchLastSection(true);

    qtlSetTableHorizontalHeader(this, 0, tr("Tag"));
    qtlSetTableHorizontalHeader(this, 1, tr("Value (hexadecimal)"));

    // Setup the delegated editors for the TLV editor.
    setItemDelegateForColumn(0, new TagItemDelegate(_tagNames, this));
    setItemDelegateForColumn(1, new TlvValueItemDelegate(this));

    // Setup the context menu in the TLV editor.
    QAction* action = new QAction(tr("Insert before this one"), this);
    connect(action, &QAction::triggered, this, &QtlTlvTableWidget::tlvInsertBeforeSelected);
    addAction(action);

    action = new QAction(tr("Insert after this one"), this);
    connect(action, &QAction::triggered, this, &QtlTlvTableWidget::tlvInsertAfterSelected);
    addAction(action);

    action = new QAction(tr("Move up"), this);
    action->setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_Up));
    connect(action, &QAction::triggered, this, &QtlTlvTableWidget::tlvMoveSelectedUp);
    addAction(action);

    action = new QAction(tr("Move down"), this);
    action->setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_Down));
    connect(action, &QAction::triggered, this, &QtlTlvTableWidget::tlvMoveSelectedDown);
    addAction(action);

    action = new QAction(tr("Delete"), this);
    action->setShortcut(QKeySequence(QKeySequence::Delete));
    connect(action, &QAction::triggered, this, &QtlTlvTableWidget::tlvDeleteSelected);
    addAction(action);

    setContextMenuPolicy(Qt::ActionsContextMenu);
}


//-----------------------------------------------------------------------------
// Clear the table content. Reimplemented from QTableWidget.
//-----------------------------------------------------------------------------

void QtlTlvTableWidget::clear()
{
    // Delete all rows but keep the headers.
    setRowCount(0);
}


//-----------------------------------------------------------------------------
// Get the first selected row in the TLV editor.
//-----------------------------------------------------------------------------

int QtlTlvTableWidget::getSelectedTlvRow() const
{
    const QList<QTableWidgetItem*> items(selectedItems());
    return items.isEmpty() || items.first() == 0 ? -1 : items.first()->row();
}


//-----------------------------------------------------------------------------
// Set the content of a row in the TLV editor.
//-----------------------------------------------------------------------------

void QtlTlvTableWidget::setTlvRowData(int row, quint32 tag, const QtlByteBlock& value)
{
    // Get first column item or create it.
    QTableWidgetItem* item = this->item(row, 0);
    if (item == 0) {
        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
        setItem(row, 0, item);
    }

    // Set the tag name or value in the item.
    item->setText(_tagNames.name(tag));

    // Get second column item or create it.
    item = this->item(row, 1);
    if (item == 0) {
        item = new QTableWidgetItem();
        item->setFont(qtlMonospaceFont());
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
        setItem(row, 1, item);
    }

    // Set the TLV value in the item.
    item->setText(qtlStringSpace(qtlHexa(value, Qtl::HexCompact), _valueHexDigitGroupSize));
}


//-----------------------------------------------------------------------------
// Private slots for editing the TLV table.
//-----------------------------------------------------------------------------

void QtlTlvTableWidget::tlvInsertBeforeSelected()
{
    const int row = getSelectedTlvRow();
    if (row >= 0) {
        insertRow(row);
        setTlvRowData(row, 0, QtlByteBlock());
        clearSelection();
        selectRow(row);
    }
}

void QtlTlvTableWidget::tlvInsertAfterSelected()
{
    const int row = getSelectedTlvRow();
    if (row >= 0) {
        insertRow(row + 1);
        setTlvRowData(row + 1, 0, QtlByteBlock());
        clearSelection();
        selectRow(row + 1);
    }
}

void QtlTlvTableWidget::tlvMoveSelectedUp()
{
    const int row = getSelectedTlvRow();
    if (row > 0) {
        // Get content of line up.
        const QList<QTableWidgetItem*> up(qtlTakeTableRow(this, row - 1));
        // Get content of current line.
        const QList<QTableWidgetItem*> current(qtlTakeTableRow(this, row));
        // Swap the content of the line.
        qtlSetTableRow(this, row - 1, current);
        qtlSetTableRow(this, row, up);
        // Keep selection on the moved row.
        clearSelection();
        selectRow(row - 1);
    }
}

void QtlTlvTableWidget::tlvMoveSelectedDown()
{
    const int row = getSelectedTlvRow();
    if (row >= 0 && row < rowCount() - 1) {
        // Get content of line down.
        const QList<QTableWidgetItem*> down(qtlTakeTableRow(this, row + 1));
        // Get content of current line.
        const QList<QTableWidgetItem*> current(qtlTakeTableRow(this, row));
        // Swap the content of the line.
        qtlSetTableRow(this, row + 1, current);
        qtlSetTableRow(this, row, down);
        // Keep selection on the moved row.
        clearSelection();
        selectRow(row + 1);
    }
}

void QtlTlvTableWidget::tlvDeleteSelected()
{
    const int row = getSelectedTlvRow();
    if (row >= 0) {
        QTableWidgetItem* item = this->item(row, 0);
        if (qtlConfirm(this, tr("Delete tag %1?").arg(item == 0 ? "" : item->text()))) {
            removeRow(row);
        }
    }
}
