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
//
// Main window class for the DVD Ripper in QtlMovie application.
//
//----------------------------------------------------------------------------

#include "QtlMovieDvdRipWindow.h"
#include "QtlMovie.h"
#include "QtlFile.h"
#include "QtlProcess.h"
#include "QtlStringUtils.h"
#include "QtlMessageBoxUtils.h"
#include "QtlTableWidgetUtils.h"
#include "QtlCheckableHeaderView.h"


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------

QtlMovieDvdRipWindow::QtlMovieDvdRipWindow(QWidget *parent, bool logDebug) :
    QtlMovieMainWindowBase(parent, logDebug),
    _dvdList()
{
    // Build the UI as defined in Qt Designer.
    _ui.setupUi(this);

    // Set log debug mode.
    _ui.actionLogDebug->setChecked(logDebug);

    // Load settings and finish up base class part of user interface.
    setupUserInterface(_ui.log, _ui.actionAboutQt);

    // Restore the position of the splitter from the settings.
    settings()->restoreState(_ui.splitter);

    // The "ISO file name" cannot contain directory separators.
    _ui.editIsoFile->setValidator(new QRegExpValidator(QRegExp("[^/\\\\]+"), this));

    // Configure the VTS and Files tables.
    setupTable(_ui.tableTitleSets);
    qtlSetTableHorizontalHeader(_ui.tableTitleSets, 0, "");
    qtlSetTableHorizontalHeader(_ui.tableTitleSets, 1, tr("VTS"), Qt::AlignCenter);
    qtlSetTableHorizontalHeader(_ui.tableTitleSets, 2, tr("Duration"), Qt::AlignRight | Qt::AlignVCenter);
    qtlSetTableHorizontalHeader(_ui.tableTitleSets, 3, tr("Size"), Qt::AlignLeft | Qt::AlignVCenter);

    setupTable(_ui.tableFiles);
    qtlSetTableHorizontalHeader(_ui.tableFiles, 0, "");
    qtlSetTableHorizontalHeader(_ui.tableFiles, 1, tr("File"), Qt::AlignLeft | Qt::AlignVCenter);
    qtlSetTableHorizontalHeader(_ui.tableFiles, 2, tr("Size"), Qt::AlignLeft | Qt::AlignVCenter);




    /*@@@@@@@@@@@@@@@@@@@
    for (int row = 0; row < 3; ++row) {
        _ui.tableTitleSets->setRowCount(row + 1);
        int col = 0;

        QTableWidgetItem* item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(Qt::Unchecked);
        item->setTextAlignment(Qt::AlignCenter);
        _ui.tableTitleSets->setItem(row, col++, item);

        item = new QTableWidgetItem(QString::number(row + 1));
        item->setFlags(Qt::ItemIsEnabled);
        item->setTextAlignment(Qt::AlignCenter);
        _ui.tableTitleSets->setItem(row, col++, item);

        item = new QTableWidgetItem(qtlSecondsToString(61 + row * 7));
        item->setFlags(Qt::ItemIsEnabled);
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        _ui.tableTitleSets->setItem(row, col++, item);

        item = new QTableWidgetItem(qtlSizeToString(1000000 + row * 30000));
        item->setFlags(Qt::ItemIsEnabled);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        _ui.tableTitleSets->setItem(row, col++, item);
    }

    for (int row = 0; row < 3; ++row) {
        _ui.tableFiles->setRowCount(row + 1);
        int col = 0;

        QTableWidgetItem* item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(Qt::Unchecked);
        item->setTextAlignment(Qt::AlignCenter);
        _ui.tableFiles->setItem(row, col++, item);

        item = new QTableWidgetItem("foo/bar");
        item->setFlags(Qt::ItemIsEnabled);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        _ui.tableFiles->setItem(row, col++, item);

        item = new QTableWidgetItem(qtlSizeToString(1000000 + row * 30000));
        item->setFlags(Qt::ItemIsEnabled);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        _ui.tableFiles->setItem(row, col++, item);
    }
    @@@@@@@@@@@@@@*/

    // Extraction is initially stopped.
    extractionUpdateUi(false);

    // Read the list of DVD and selects the first one.
    refresh();
}


//-----------------------------------------------------------------------------
// Common setup for the VTS and Files tables.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::setupTable(QTableWidget* table)
{
    // Use a table header with a checkbox in first column.
    table->setHorizontalHeader(new QtlCheckableHeaderView(table));
    table->horizontalHeader()->setVisible(true);
    table->horizontalHeader()->setCascadingSectionResizes(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setStretchLastSection(true);

    // No vertical header.
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Table general properties.
    table->setShowGrid(false);

    // Setup properties that can be set only using stylesheets.
    table->setStyleSheet("::item {border: 0px; padding-left: 5px; padding-right: 5px;};");
}


//-----------------------------------------------------------------------------
// Event handler to handle window close.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::closeEvent(QCloseEvent* event)
{
    // Let the superclass handle the event.
    QtlMovieMainWindowBase::closeEvent(event);

    // If the event was accepted by the superclass, save our state.
    if (event->isAccepted()) {
        settings()->saveState(_ui.splitter);
        settings()->sync();
    }
}


//-----------------------------------------------------------------------------
// Update the UI when extraction starts or stops.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::extractionUpdateUi(bool started)
{
    // Toggle start/cancel labels in buttons.
    // Change their target slot to start/cancel extraction.
    if (started) {
        _ui.actionStart->setText(tr("Cancel ..."));
        disconnect(_ui.actionStart, &QAction::triggered, this, &QtlMovieDvdRipWindow::startExtraction);
        connect(_ui.actionStart, &QAction::triggered, this, &QtlMovieDvdRipWindow::cancelExtraction);

        _ui.buttonStart->setText(tr("Cancel ..."));
        disconnect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieDvdRipWindow::startExtraction);
        connect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieDvdRipWindow::cancelExtraction);
    }
    else {
        _ui.actionStart->setText(tr("Start ..."));
        disconnect(_ui.actionStart, &QAction::triggered, this, &QtlMovieDvdRipWindow::cancelExtraction);
        connect(_ui.actionStart, &QAction::triggered, this, &QtlMovieDvdRipWindow::startExtraction);

        _ui.buttonStart->setText(tr("Start ..."));
        disconnect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieDvdRipWindow::cancelExtraction);
        connect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieDvdRipWindow::startExtraction);
    }

    // Enable / disable other buttons.
    _ui.actionRefresh->setEnabled(!started);
    _ui.buttonRefreshDvd->setEnabled(!started);

    // Set the progress bar.
    // When extraction starts, we set the maximum to zero to force a "busy" bar.
    // If a more precise progression indicator is set later, an exact percentage will be displayed.
    _ui.progressBar->setTextVisible(false);
    _ui.progressBar->setRange(0, started ? 0 : 100);
    _ui.progressBar->reset();
    _ui.labelRemainingTime->setVisible(false);

    // Set the Windows task bar button.
    setIconTaskBarVisible(started);
}


//-----------------------------------------------------------------------------
// Invoked by the "Refresh ..." buttons.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::refresh()
{
    refreshDvdList();

    //@@@@
}


//-----------------------------------------------------------------------------
// Refresh the list of DVD's.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::refreshDvdList()
{
    // Get currently selected DVD. Used to check if it is still there after refresh.
    const QString previousSelected(_ui.comboDvd->currentText());
    int indexToPreviousSelected = -1;

    // Clear current contents.
    _ui.comboDvd->clear();
    _dvdList.clear();

    // Refresh the list of currenly inserted DVD's.
    foreach (const QStorageInfo& si, QStorageInfo::mountedVolumes()) {

        // Reading DVD's maybe a long operation, process pending events in the meantime.
        qApp->processEvents();

        // DVD's are read-only and use UDF file systems. Note that the file system type
        // name is OS-dependent. We "assume" here that the name contains "UDF".
        if (si.isReadOnly() && QString(si.fileSystemType()).contains(QStringLiteral("udf"), Qt::CaseInsensitive)) {
            // This is maybe a DVD. Maybe. Let's try to open it.
            QtlDvdMedia* dvd = new QtlDvdMedia(si.rootPath(), log(), this);
            if (!dvd->isOpen()) {
                // Finally not a DVD.
                delete dvd;
            }
            else {
                // This is a DVD, keep it.
                _dvdList << dvd;
                const QString id(dvd->volumeId());
                QString name(dvd->deviceName());
                if (!id.isEmpty()) {
                    name += " (" + id + ")";
                }
                // Add the DVD in the combo box and keep the DVD pointer as user data.
                _ui.comboDvd->addItem(name, QVariant::fromValue(dvd));
                if (name == previousSelected) {
                    indexToPreviousSelected = _ui.comboDvd->count() - 1;
                }
            }
        }
    }

    // If the previous selected DVD is still there, reselect it.
    if (indexToPreviousSelected >= 0) {
        // Reselect same DVD, but possibly at a different index.
        _ui.comboDvd->setCurrentIndex(indexToPreviousSelected);
    }
    else if (_ui.comboDvd->count() > 0) {
        // Otherwise, select the first DVD.
        _ui.comboDvd->setCurrentIndex(0);
        // Preset the ISO file name at the volume id.
        QtlDvdMedia* dvd = _ui.comboDvd->currentData().value<QtlDvdMedia*>();
        if (dvd != 0) {
            _ui.editIsoFile->setText(dvd->volumeId());
        }
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Start ..." buttons.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::startExtraction()
{
    //@@@@
}


//-----------------------------------------------------------------------------
// Invoked by the "Cancel ..." buttons.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::cancelExtraction()
{
    // Ask the user to confirm the cancelation of the current extraction.
    proposeToCancel();
}


//-----------------------------------------------------------------------------
// Invoked when extraction starts.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::extractionStarted()
{
    // Clear the log if required.
    if (settings()->clearLogBeforeTranscode()) {
        _ui.log->clear();
    }

    // Update user interface.
    extractionUpdateUi(true);
}


//-----------------------------------------------------------------------------
// Invoked when some progress is made in the extraction process.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::extractionProgress(const QString& description, int current, int maximum, int elapsedSeconds, int remainingSeconds)
{
    Q_UNUSED(elapsedSeconds);

    // Restrict current within range.
    current = qBound(0, current, maximum);

    // Progress bar.
    if (maximum <= 0) {
        // Cannot evaluate the progression, display a "busy" progress bar without percentage.
        _ui.progressBar->setRange(0, 0);
        _ui.progressBar->setTextVisible(false);
    }
    else {
        // Display the progression.
        _ui.progressBar->setRange(0, maximum);
        _ui.progressBar->setValue(current);
        _ui.progressBar->setTextVisible(true);
    }

    // Job description and remaining time.
    if (remainingSeconds >= 0) {
        _ui.labelRemainingTime->setText(tr("%1%2Remaining time: %3")
                                        .arg(description)
                                        .arg(description.isEmpty() ? "" : " - ")
                                        .arg(qtlSecondsToString(int(remainingSeconds))));
        _ui.labelRemainingTime->setVisible(true);
    }
    else if (!description.isEmpty()) {
        _ui.labelRemainingTime->setText(description);
        _ui.labelRemainingTime->setVisible(true);
    }
    else {
        _ui.labelRemainingTime->setVisible(false);
    }

    // Update the Windows task bar button.
    /*@@@@
    if (_job != 0) {
        setIconTaskBarValue(_job->currentProgress(current, maximum, iconTaskBarMaximumValue()));
    }
    @@@@@*/
}


//-----------------------------------------------------------------------------
// Invoked when extraction stops.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::extractionStopped(bool success)
{
    // Check if a extraction job was actually in progress.
    //@@@ if (_job != 0) {

        // Play notification sound at complete end of all jobs.
        if (settings()->playSoundOnCompletion()) {
            playNotificationSound();
        }

        // Save log file if required.
        if (settings()->saveLogAfterTranscode()) {
            //@@@@ const QString logFile(_job->task()->outputFile()->fileName() + settings()->logFileExtension());
            //@@@@ _ui.log->saveToFile(logFile);
            //@@@@ log()->line(tr("Saved log to %1").arg(logFile));
        }

        // Delete extraction job.
        //@@@@ _job->deleteLater();
        //@@@@ _job = 0;
    //@@@@ }

    // Update UI.
    extractionUpdateUi(false);

    // If the application needs to be closed, close it now.
    if (closePending()) {
        close();
    }
}


//-----------------------------------------------------------------------------
// Check if an extraction is currently in progress. Propose to abort.
//-----------------------------------------------------------------------------

QtlMovieDvdRipWindow::CancelStatus QtlMovieDvdRipWindow::proposeToCancel()
{
    return NothingToCancel;  //@@@@
}


//-----------------------------------------------------------------------------
// Apply the settings which affect the UI.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::applyUserInterfaceSettings()
{
    _ui.log->setMaximumBlockCount(settings()->maxLogLines());
    if (_ui.editDestination->text().isEmpty()) {
        _ui.editDestination->setText(settings()->defaultDvdExtractionDir());
    }
}


//-----------------------------------------------------------------------------
// Update the label containing the ISO full path.
//-----------------------------------------------------------------------------

void QtlMovieDvdRipWindow::updateIsoFullPath()
{
    QString path(_ui.editDestination->text());
    if (!path.isEmpty()) {
        path.append(QDir::separator());
    }
    path.append(_ui.editIsoFile->text());
    if (!path.endsWith(".iso", Qt::CaseInsensitive)) {
        path.append(".iso");
    }
    _ui.valueFullPath->setText(QtlFile::absoluteNativeFilePath(path));
}