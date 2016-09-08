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
// Define the class QtlMovieDvdExtractionSession.
//
//----------------------------------------------------------------------------

#include "QtlMovieDvdExtractionSession.h"
#include "QtlMessageBoxUtils.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieDvdExtractionSession::QtlMovieDvdExtractionSession(const QString& dvdDeviceName,
                                                           const QtlMovieSettings* settings,
                                                           QtlLogger* log,
                                                           QObject* parent) :
    QtlMovieAction(settings, log, parent),
    _dvdDeviceName(dvdDeviceName),
    _totalSectors(0),
    _completedSectors(0),
    _totalFiles(0),
    _completedFiles(0),
    _transferList()
{
}


//----------------------------------------------------------------------------
// Constructor of inner private class describing one transfer.
//----------------------------------------------------------------------------

QtlMovieDvdExtractionSession::OutputFile::OutputFile(const QString& outputFileName,
                                                     const QString& dvdDeviceName,
                                                     int startSector,
                                                     int sectorCount,
                                                     bool useMaxReadSpeed,
                                                     Qts::BadSectorPolicy badSectorPolicy,
                                                     QtlLogger* log) :
    totalSectors(sectorCount),
    file(outputFileName),
    dataPull(dvdDeviceName,
             startSector,
             sectorCount,
             badSectorPolicy,
             QTS_DEFAULT_DVD_TRANSFER_SIZE,
             QtlDataPull::DEFAULT_MIN_BUFFER_SIZE,
             log,
             0,
             useMaxReadSpeed)
{
}


//----------------------------------------------------------------------------
// Add a slice of DVD to extract in a file.
//----------------------------------------------------------------------------

void QtlMovieDvdExtractionSession::addFile(const QString& outputFileName,
                                           int startSector,
                                           int sectorCount,
                                           Qts::BadSectorPolicy badSectorPolicy)
{
    // Cannot do that after start.
    if (isStarted()) {
        line(tr("Internal error: adding a DVD transfer after start"));
    }
    else {
        // Add a new transfer in the list.
        _transferList << OutputFilePtr(new OutputFile(outputFileName, _dvdDeviceName, startSector, sectorCount, settings()->dvdUseMaxSpeed(), badSectorPolicy, this));
        debug(tr("Queued file %1, sectors %2 to %3").arg(outputFileName).arg(startSector).arg(startSector + sectorCount - 1));

        // Accumulate total transfer size.
        _totalSectors += sectorCount;
        _totalFiles++;
    }
}


//----------------------------------------------------------------------------
// Ask the user if the output files may be overwritten.
//----------------------------------------------------------------------------

bool QtlMovieDvdExtractionSession::askOverwriteOutput()
{
    // Get the list of files which already exist.
    QStringList existing;
    foreach (const OutputFilePtr& ofp, _transferList) {
        if (!ofp.isNull() && ofp->file.exists()) {
            existing << QtlFile::absoluteNativeFilePath(ofp->file.fileName());
        }
    }

    // Ask for confirmation to overwrite.
    // Exact question depends if there is one or more files.
    if (existing.size() == 1) {
        if (!qtlConfirm(this, tr("File %1 already exists.\nOverwrite it?").arg(existing.first()))) {
            // Don't overwrite, give up.
            return false;
        }
    }
    else if (!existing.isEmpty()) {
        if (!qtlConfirm(this, tr("The following files already exist:\n%1\nOverwrite them?").arg(existing.join("\n")))) {
            // Don't overwrite, give up.
            return false;
        }
    }

    // Delete the previous output files.
    foreach (const QString& fileName, existing) {
        if (!QFile(fileName).remove()) {
            line(tr("Failed to delete %1").arg(fileName));
        }
    }

    // Yes, we can overwrite the output file (non existent or just deleted in fact).
    return true;
}


//----------------------------------------------------------------------------
// Start the extraction.
//----------------------------------------------------------------------------

bool QtlMovieDvdExtractionSession::start()
{
    // Do not start twice.
    if (!QtlMovieAction::start()) {
        return false;
    }

    // Start the first action.
    if (!startNextExtraction()) {
        emitCompleted(false, tr("Error starting DVD extraction"));
    }

    return true;
}


//----------------------------------------------------------------------------
// Start the next extraction in the list.
//----------------------------------------------------------------------------

bool QtlMovieDvdExtractionSession::startNextExtraction()
{
    // If list is empty, the extraction is completed.
    if (_transferList.isEmpty()) {
        emitCompleted(true);
        return true;
    }

    // Get the next transfer (but keep it in the list).
    OutputFilePtr out(_transferList.first());

    // Build a description.
    QString desc(tr("Extracting %1").arg(QtlFile::absoluteNativeFilePath(out->file.fileName())));
    if (_totalFiles > 1) {
        desc.append(QStringLiteral(" (%1/%2)").arg(_completedFiles + 1).arg(_totalFiles));
    }
    setDescription(desc);

    // Create the parent directory of the output file is necessary.
    const QString dir(QtlFile::parentPath(out->file.fileName()));
    if (!QtlFile::createDirectory(dir)) {
        line(tr("Error creating directory %1").arg(dir), QColor(Qt::red));
        dataPullCompleted(false);
        return false;
    }

    // Open the output file.
    if (!out->file.open(QFile::WriteOnly)) {
        line(tr("Error creating %1").arg(out->file.fileName()), QColor(Qt::red));
        dataPullCompleted(false);
        return false;
    }

    // Get notified of the transfer progress.
    connect(&out->dataPull, &QtlDataPull::progress, this, &QtlMovieDvdExtractionSession::dataPullProgressed);

    // Get notified of the transfer progress. Important: We need a queued connection, not a direct one.
    // Our slot dataPullCompleted() will destroy the QtlDataPull instance, we cannot do this inside a
    // direct call from the object to destroy.
    connect(&out->dataPull, &QtlDataPull::completed, this, &QtlMovieDvdExtractionSession::dataPullCompleted, Qt::QueuedConnection);

    // Start the transfer.
    if (!out->dataPull.start(&out->file)) {
        line(tr("Error starting data transfer to %1").arg(out->file.fileName()), QColor(Qt::red));
        dataPullCompleted(false);
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
// Abort the extraction.
//----------------------------------------------------------------------------

void QtlMovieDvdExtractionSession::abort()
{
    // Is there anything to abort?
    if (!isStarted() || _transferList.isEmpty()) {
        return;
    }

    // Remove all transfers except current one.
    while (_transferList.size() > 1 || (!_transferList.isEmpty() && !_transferList.first()->dataPull.isStarted())) {
        _transferList.removeLast();
    }

    if (_transferList.isEmpty()) {
        // No transfer will notify the completion, we need to do it now.
        emitCompleted(false, "DVD extraction aborted");
    }
    else {
        // Abort the current transfer.
        _transferList.first()->dataPull.stop();
        // The completion of the aborted transfer will trigger the completion of the complete extraction.
    }
}


//----------------------------------------------------------------------------
// Invoked when some progress in the current data transfer is available.
//----------------------------------------------------------------------------

void QtlMovieDvdExtractionSession::dataPullProgressed(qint64 current, qint64 maximum)
{
    // Number of sectors from previous transfers plus sectors in current transfer.
    emitProgress(_completedSectors + int(current / QTS_DVD_SECTOR_SIZE), _totalSectors);
}


//----------------------------------------------------------------------------
// Invoked each time a transfer completes.
//----------------------------------------------------------------------------

void QtlMovieDvdExtractionSession::dataPullCompleted(bool success)
{
    // Cleanup terminated extraction.
    Q_ASSERT(!_transferList.isEmpty());
    OutputFilePtr out(_transferList.takeFirst());
    _completedSectors += out->totalSectors;
    _completedFiles++;
    out->file.close();
    out.clear();

    // Emit a progress indicator.
    dataPullProgressed(0, 0);

    // Process the next extraction.
    if (success) {
        if (_transferList.isEmpty()) {
            // Extraction successfully completed.
            emitCompleted(true);
            return;
        }
        else if (startNextExtraction()) {
            // Next action started.
            return;
        }
    }

    // Notify failure.
    emitCompleted(false, "DVD extraction failed, see messages above.");
}
