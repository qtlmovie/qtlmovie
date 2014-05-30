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
// Define the class QtlMovieInputFilePropertiesDialog.
//
//----------------------------------------------------------------------------

#include "QtlMovieInputFilePropertiesDialog.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieInputFilePropertiesDialog::QtlMovieInputFilePropertiesDialog(const QtlMovieInputFile* inputFile, QtlMovieSettings* settings, QWidget *parent) :
    QDialog(parent),
    _settings(settings)
{
    // Build the UI as defined in Qt Designer.
    _ui.setupUi(this);

    // Restore the window geometry from the saved settings.
    _settings->restoreGeometry(this);

    // Define table widget headers.
    QStringList headers;
    headers << tr("FFprobe tag") << tr("Value");
    _ui.propertiesTable->setHorizontalHeaderLabels(headers);

    // Get the list of FFprobe tags.
    const QMap<QString,QString> ffTags(inputFile->ffProbeInfo());

    // Create the required number of rows in the table.
    _ui.propertiesTable->setRowCount(ffTags.size());

    // Fill the table with all input file properties.
    int row = 0;
    for (QMap<QString,QString>::ConstIterator it = ffTags.begin(); it != ffTags.end(); ++it) {
        _ui.propertiesTable->setItem(row, 0, new QTableWidgetItem(it.key()));
        _ui.propertiesTable->setItem(row, 1, new QTableWidgetItem(it.value()));
        row++;
    }

    // Make sure the table is resized to fit content.
    _ui.propertiesTable->resizeColumnToContents(0);
    _ui.propertiesTable->resizeRowsToContents();
}


//-----------------------------------------------------------------------------
// Event handler to handle window close.
//-----------------------------------------------------------------------------

void QtlMovieInputFilePropertiesDialog::closeEvent(QCloseEvent* event)
{
    // Accept to close the window.
    event->accept();

    // Save the geometry of the window.
    _settings->saveGeometry(this, true);
}
