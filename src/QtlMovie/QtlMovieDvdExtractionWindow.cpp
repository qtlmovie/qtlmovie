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

#include "QtlMovieDvdExtractionWindow.h"
#include "QtlMovie.h"
#include "QtlFile.h"
#include "QtlProcess.h"
#include "QtlStringList.h"
#include "QtlStringUtils.h"
#include "QtsDvdTitleSet.h"
#include "QtlMessageBoxUtils.h"
#include "QtlTableWidgetUtils.h"
#include "QtlCheckableHeaderView.h"

// List of tabs:
#define QTL_TAB_ISO    0
#define QTL_TAB_VTS    1
#define QTL_TAB_FILES  2


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------

QtlMovieDvdExtractionWindow::QtlMovieDvdExtractionWindow(QWidget *parent, bool logDebug) :
    QtlMovieMainWindowBase(parent, logDebug),
    _dvdList(),
    _extraction(0)
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
    qtlSetTableHorizontalHeader(_ui.tableTitleSets, 2, tr("Duration"), Qt::AlignLeft | Qt::AlignVCenter);
    qtlSetTableHorizontalHeader(_ui.tableTitleSets, 3, tr("Size"), Qt::AlignLeft | Qt::AlignVCenter);

    setupTable(_ui.tableFiles);
    qtlSetTableHorizontalHeader(_ui.tableFiles, 0, "");
    qtlSetTableHorizontalHeader(_ui.tableFiles, 1, tr("File"), Qt::AlignLeft | Qt::AlignVCenter);
    qtlSetTableHorizontalHeader(_ui.tableFiles, 2, tr("Size"), Qt::AlignLeft | Qt::AlignVCenter);

    // Extraction is initially stopped.
    extractionUpdateUi(false);

    // Read the list of DVD and selects the first one.
    refresh();
}


//-----------------------------------------------------------------------------
// Event handler to handle window close.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::closeEvent(QCloseEvent* event)
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
// Common setup for the VTS and Files tables.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::setupTable(QTableWidget* table)
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
    table->setAlternatingRowColors(true);

    // Setup properties that can be set only using stylesheets.
    table->setStyleSheet("QTableWidget::item {padding-right: 15px;};");
}


//-----------------------------------------------------------------------------
// Apply the settings which affect the UI.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::applyUserInterfaceSettings(bool initial)
{
    _ui.log->setMaximumBlockCount(settings()->maxLogLines());
    if (_ui.editDestination->text().isEmpty()) {
        _ui.editDestination->setText(settings()->defaultDvdExtractionDir());
    }
}


//-----------------------------------------------------------------------------
// Update the label containing the ISO full path.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::updateIsoFullPath()
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


//-----------------------------------------------------------------------------
// Update the UI when extraction starts or stops.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::extractionUpdateUi(bool started)
{
    // Toggle start/cancel labels in buttons.
    // Change their target slot to start/cancel extraction.
    if (started) {
        _ui.actionStart->setText(tr("Cancel ..."));
        disconnect(_ui.actionStart, &QAction::triggered, this, &QtlMovieDvdExtractionWindow::startExtraction);
        connect(_ui.actionStart, &QAction::triggered, this, &QtlMovieDvdExtractionWindow::cancelExtraction);

        _ui.buttonStart->setText(tr("Cancel ..."));
        disconnect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieDvdExtractionWindow::startExtraction);
        connect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieDvdExtractionWindow::cancelExtraction);
    }
    else {
        _ui.actionStart->setText(tr("Start ..."));
        disconnect(_ui.actionStart, &QAction::triggered, this, &QtlMovieDvdExtractionWindow::cancelExtraction);
        connect(_ui.actionStart, &QAction::triggered, this, &QtlMovieDvdExtractionWindow::startExtraction);

        _ui.buttonStart->setText(tr("Start ..."));
        disconnect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieDvdExtractionWindow::cancelExtraction);
        connect(_ui.buttonStart, &QPushButton::clicked, this, &QtlMovieDvdExtractionWindow::startExtraction);
    }

    // Enable / disable other widgets.
    _ui.actionRefresh->setEnabled(!started);
    _ui.buttonRefreshDvd->setEnabled(!started);
    _ui.tabDvd->setEnabled(!started);

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
// Get the currently selected DVD.
//-----------------------------------------------------------------------------

QtsDvdMediaPtr QtlMovieDvdExtractionWindow::currentDvd() const
{
    return _ui.comboDvd->count() > 0 ? _ui.comboDvd->currentData().value<QtsDvdMediaPtr>() : QtsDvdMediaPtr();
}


//-----------------------------------------------------------------------------
// Invoked by the "Refresh ..." buttons.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::refresh()
{
    refreshDvdList();
    refreshVtsList();
    refreshFilesList();
}


//-----------------------------------------------------------------------------
// Refresh the list of DVD's.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::refreshDvdList()
{
    // Get currently selected DVD. Used to check if it is still there after refresh.
    const QString previousIsoFile(_ui.editIsoFile->text());
    const QString previousSelected(_ui.comboDvd->currentText());
    int indexToPreviousSelected = -1;

    // Clear current contents.
    _ui.comboDvd->clear();
    _ui.editIsoFile->clear();
    _dvdList.clear();

    // Refresh the list of currenly inserted DVD's.
    foreach (const QStorageInfo& si, QStorageInfo::mountedVolumes()) {

        // Reading DVD's maybe a long operation, process pending events in the meantime.
        qApp->processEvents();

        // DVD's are read-only and use UDF file systems. Note that the file system type
        // name is OS-dependent. We "assume" here that the name contains "UDF".
        if (si.isReadOnly() && QString(si.fileSystemType()).contains(QStringLiteral("udf"), Qt::CaseInsensitive)) {
            // This is maybe a DVD. Maybe. Let's try to open it.
            QtsDvdMediaPtr dvd(new QtsDvdMedia(si.rootPath(), log(), this));
            if (dvd->isOpen()) {
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
        _ui.editIsoFile->setText(previousIsoFile);
    }
    else if (_ui.comboDvd->count() > 0) {
        // Otherwise, select the first DVD.
        _ui.comboDvd->setCurrentIndex(0);
        // Preset the ISO file name at the volume id.
        QtsDvdMediaPtr dvd(_ui.comboDvd->currentData().value<QtsDvdMediaPtr>());
        if (!dvd.isNull()) {
            _ui.editIsoFile->setText(dvd->volumeId());
        }
    }
}


//-----------------------------------------------------------------------------
// Refresh the list of video title sets.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::refreshVtsList()
{
    // Clear the previous content of the table.
    _ui.tableTitleSets->setRowCount(0);

    // Get selected DVD.
    const QtsDvdMediaPtr dvd(currentDvd());
    if (dvd.isNull()) {
        return;
    }

    // Create one row per title set.
    for (int vtsNumber = 1; vtsNumber <= dvd->vtsCount(); ++vtsNumber) {

        // Get the description of the VTS.
        QtsDvdTitleSetPtr vts(new QtsDvdTitleSet(QString(), log()));
        if (!vts->load(dvd->vtsInformationFileName(vtsNumber), &*dvd)) {
            // Cannot open VTS, strange, skip it.
            log()->line(tr("Cannot open video title set %1").arg(vtsNumber));
            continue;
        }

        // Items: check box, VTS number, VTS duration, size of the VOB files.
        const QtlStringList texts("", QString::number(vtsNumber), qtlSecondsToString(vts->longestDurationInSeconds()), qtlSizeToString(vts->vobSizeInBytes(), 2));

        // Create the row, use header text alignment, set checkbox in column zero.
        const int row = qtlSetTableRow(_ui.tableTitleSets, _ui.tableTitleSets->rowCount(), texts, true, Qt::ItemIsEnabled, 0);

        // The first cell is used to store the title set description.
        _ui.tableTitleSets->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue(vts));
    }
}


//-----------------------------------------------------------------------------
// Refresh the list of files.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::refreshFilesList()
{
    // Clear the previous content of the table.
    _ui.tableFiles->setRowCount(0);

    // Get selected DVD.
    const QtsDvdMediaPtr dvd(currentDvd());
    if (dvd.isNull()) {
        return;
    }

    // Create one row per file.
    addDirectoryTree(dvd->rootDirectory());
}


//-----------------------------------------------------------------------------
// Add a tree of files and directories in the table of files.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::addDirectoryTree(const QtsDvdDirectory& dir)
{
    // First, add local files in current directory.
    foreach (const QtsDvdFilePtr& file, dir.files()) {
        if (!file.isNull()) {

            // Items: check box, file path, file size.
            const QtlStringList texts("", file->path(), qtlSizeToString(file->sizeInBytes(), 2));

            // Create the row, use header text alignment, set checkbox in column zero.
            const int row = qtlSetTableRow(_ui.tableFiles, _ui.tableFiles->rowCount(), texts, true, Qt::ItemIsEnabled, 0);

            // The first cell is used to store the file description.
            _ui.tableFiles->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue(file));
        }
    }

    // Then, add subdirectory trees.
    foreach (const QtsDvdDirectoryPtr& subdir, dir.subDirectories()) {
        if (!subdir.isNull()) {
            addDirectoryTree(*subdir);
        }
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Start ..." buttons.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::startExtraction()
{
    // Fool-proof check.
    if (_extraction != 0) {
        log()->line(tr("Internal error, DVD extraction already created."));
        return;
    }

    // Get currently selected DVD.
    QtsDvdMediaPtr dvd(currentDvd());
    if (dvd.isNull() || !dvd->isOpen()) {
        log()->line(tr("No DVD found, try to refresh"));
        return;
    }

    // Create a DVD extraction object.
    _extraction = new QtlMovieDvdExtractionSession(dvd->deviceName(), settings(), log(), this);

    // Select the various extraction. This depends on the mode, extraction of ISO, VTS or files.
    switch (_ui.tabDvd->currentIndex()) {
        case QTL_TAB_ISO: {
            // Only one big file to extract.
            // We replace bad sectors by zeroes to preserve the media layout.
            _extraction->addFile(_ui.valueFullPath->text(), 0, dvd->volumeSizeInSectors(), Qts::ReadBadSectorsAsZero);
            break;
        }
        case QTL_TAB_VTS: {
            // Loop on all rows in the title set table.
            for (int row = 0; row < _ui.tableTitleSets->rowCount(); ++ row) {
                // Get cell in first column and see if it is checked.
                // The second cell contains the VTS number.
                QTableWidgetItem* item1 = _ui.tableTitleSets->item(row, 0);
                QTableWidgetItem* item2 = _ui.tableTitleSets->item(row, 1);
                if (item1 != 0 && item1->checkState() == Qt::Checked && item2 != 0) {
                    // We need to extract this title set.
                    // The first cell is used to store the title set description.
                    const QtsDvdTitleSetPtr vts(item1->data(Qt::UserRole).value<QtsDvdTitleSetPtr>());
                    const int vtsNumber = qtlToInt(item2->text());
                    if (!vts.isNull() && vtsNumber > 0) {
                        addFileForExtraction(dvd->vtsInformationFile(vtsNumber));
                        for (int vob = 1; vob <= vts->vobCount(); ++ vob) {
                            addFileForExtraction(dvd->vtsVideoFile(vtsNumber, vob));
                        }
                    }
                }
            }
            break;
        }
        case QTL_TAB_FILES: {
            // Loop on all rows in the file table.
            for (int row = 0; row < _ui.tableFiles->rowCount(); ++ row) {
                // Get cell in first column and see if it is checked.
                QTableWidgetItem* item1 = _ui.tableFiles->item(row, 0);
                if (item1 != 0 && item1->checkState() == Qt::Checked) {
                    // We need to extract this file.
                    // The first cell is used to store the file description.
                    const QtsDvdFilePtr file(item1->data(Qt::UserRole).value<QtsDvdFilePtr>());
                    addFileForExtraction(*file);
                }
            }
            break;
        }
        default: {
            log()->line(tr("Internal error, wrong tab index %1").arg(_ui.tabDvd->currentIndex()));
            break;
        }
    }

    // Make sure we have something selected.
    if (_extraction->remainingTransferCount() <= 0) {
        log()->line(tr("Nothing to extract"));
        delete _extraction;
        _extraction = 0;
        return;
    }

    // If some output files already exist, ask confirmation.
    if (!_extraction->askOverwriteOutput()) {
        // Don't overwrite, give up.
        log()->line(tr("Overwritting output files denied"));
        delete _extraction;
        _extraction = 0;
        return;
    }

    // Get notifications from the extraction object.
    connect(_extraction, &QtlMovieDvdExtractionSession::started, this, &QtlMovieDvdExtractionWindow::extractionStarted);
    connect(_extraction, &QtlMovieDvdExtractionSession::progress, this, &QtlMovieDvdExtractionWindow::extractionProgress);
    connect(_extraction, &QtlMovieDvdExtractionSession::completed, this, &QtlMovieDvdExtractionWindow::extractionStopped);

    // Start the job.
    if (!_extraction->start()) {
       // Error starting the job, delete it now.
        delete _extraction;
        _extraction = 0;
    }
}


//-----------------------------------------------------------------------------
// Add a file in the current extraction.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::addFileForExtraction(const QtsDvdFile& file)
{
    // Check the validity of the file.
    if (file.startSector() < 0) {
        log()->line(tr("File %1 not found on DVD").arg(file.path()));
        return;
    }

    // Output file path.
    QString outputPath(_ui.editDestination->text());
    if (!outputPath.isEmpty()) {
        outputPath.append(QDir::separator());
    }
    if (settings()->dvdExtractDirTree()) {
        outputPath.append(file.path());
    }
    else {
        outputPath.append(file.name());
    }

    // Add the file.
    if (_extraction != 0) {
        // When ripping files, we skip bad sectors.
        _extraction->addFile(outputPath, file.startSector(), file.sectorCount(), Qts::SkipBadSectors);
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Cancel ..." buttons.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::cancelExtraction()
{
    // Ask the user to confirm the cancelation of the current extraction.
    proposeToCancel();
}


//-----------------------------------------------------------------------------
// Invoked when extraction starts.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::extractionStarted()
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

void QtlMovieDvdExtractionWindow::extractionProgress(const QString& description, int current, int maximum, int elapsedSeconds, int remainingSeconds)
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
    setIconTaskBarValue(current, maximum);
}


//-----------------------------------------------------------------------------
// Invoked when extraction stops.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::extractionStopped(bool success)
{
    // Check if a extraction job was actually in progress.
    if (_extraction != 0) {

        // Play notification sound at complete end of all jobs.
        if (settings()->playSoundOnCompletion()) {
            playNotificationSound();
        }

        // Delete extraction job.
        _extraction->deleteLater();
        _extraction = 0;
    }

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

QtlMovieDvdExtractionWindow::CancelStatus QtlMovieDvdExtractionWindow::proposeToCancel()
{
    // If there is nothing to cancel now, no need to ask.
    if (_extraction == 0) {
        return NothingToCancel;
    }

    // Ask the user if we should cancel the current extraction?
    const bool confirm = qtlConfirm(this, tr("Do you want to abort the current DVD extraction ?"));

    if (_extraction == 0) {
        // If extraction completed in the meantime, all clear...
        return NothingToCancel;
    }
    else if (!confirm) {
        // The user refused to cancel and the extraction is still in progress, refuse.
        return CancelRefused;
    }
    else {
        // A extraction is in progress and the user decided to abort it.
        // Defer the actual abort() when back in event loop.
        QMetaObject::invokeMethod(this, "deferredAbort", Qt::QueuedConnection);
        return CancelInProgress;
    }
}


//-----------------------------------------------------------------------------
// Abort the extraction, using a slot to defer the abort when back in the event loop.
//-----------------------------------------------------------------------------

void QtlMovieDvdExtractionWindow::deferredAbort()
{
    if (_extraction != 0) {
        _extraction->abort();
    }
}
