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
//!
//! @file QtlMovieDvdExtractionSession.h
//!
//! Declare the class QtlMovieDvdExtractionSession.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEDVDEXTRACTIONSESSION_H
#define QTLMOVIEDVDEXTRACTIONSESSION_H

#include "QtlMovieAction.h"
#include "QtlDvdDataPull.h"
#include "QtlSmartPointer.h"

//!
//! A complete DVD extraction session.
//!
class QtlMovieDvdExtractionSession : public QtlMovieAction
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] dvdDeviceName DVD device name.
    //! @param [in] settings Application settings.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieDvdExtractionSession(const QString& dvdDeviceName, const QtlMovieSettings* settings, QtlLogger* log, QObject *parent = 0);

    //!
    //! Add a slice of DVD to extract in a file.
    //! Must be done before start().
    //! @param [in] outputFileName Output file name.
    //! @param [in] startSector First sector to read.
    //! @param [in] sectorCount Total number of sectors to read.
    //! @param [in] badSectorPolicy How to handle bad sectors.
    //! @see QtlDvdMedia::BadSectorPolicy
    //!
    void addFile(const QString& outputFileName, int startSector, int sectorCount, QtlDvdMedia::BadSectorPolicy badSectorPolicy);

    //!
    //! Get the number of remaining transfers, including the current one.
    //! @return The number of remaining transfers, including the current one.
    //!
    int remainingTransferCount() const
    {
        return _transferList.size();
    }

    //!
    //! Ask the user if the output files may be overwritten.
    //! If no output file already exists, ask nothing.
    //! If some output files already exist and the user is OK to overwrite them,
    //! the previous output files are deleted.
    //! @return True if the output files do not exist or can be overwritten, false otherwise.
    //!
    bool askOverwriteOutput();

    //!
    //! Start the extraction.
    //! @return False if already started. True otherwise.
    //!
    virtual bool start() Q_DECL_OVERRIDE;

    //!
    //! Abort the extraction.
    //! If the extraction was started, the signal completed() will be emitted when it actually terminates.
    //!
    virtual void abort() Q_DECL_OVERRIDE;

private slots:
    //!
    //! Invoked when some progress in the current data transfer is available.
    //! @param [in] current Current number of input bytes.
    //! @param [in] maximum Total size of the data transfer.
    //!
    void dataPullProgressed(qint64 current, qint64 maximum);

    //!
    //! Invoked each time a transfer completes.
    //! @param [in] success Indicates whether the action succeeded or failed.
    //!
    void dataPullCompleted(bool success);

private:
    class OutputFile;
    typedef QtlSmartPointer<OutputFile, QtlNullMutexLocker> OutputFilePtr;
    typedef QList<OutputFilePtr> OutputFileList;

    QString        _dvdDeviceName;     //!< DVD device name.
    int            _totalSectors;      //!< Total size in DVD sectors of the transfer.
    int            _completedSectors;  //!< Transfered sectors in previous completed transfered.
    int            _totalFiles;        //!< Total file count.
    int            _completedFiles;    //!< Completed file count.
    OutputFileList _transferList;      //!< List of transfers.

    //!
    //! Inner private class describing one transfer.
    //!
    class OutputFile
    {
    public:
        const int      totalSectors;  //!< Total number of sectors in this transfer.
        QFile          file;          //!< Output file.
        QtlDvdDataPull dataPull;      //!< DVD transfer.

        //!
        //! Constructor.
        //! @param [in] outputFileName Output file name.
        //! @param [in] dvdDeviceName DVD device name.
        //! @param [in] startSector First sector to read.
        //! @param [in] sectorCount Total number of sectors to read.
        //! @param [in] useMaxReadSpeed Set the DVD reader to maximum speed.
        //! @param [in] badSectorPolicy How to handle bad sectors.
        //! @param [in] log Message logger.
        //!
        OutputFile(const QString& outputFileName,
                   const QString& dvdDeviceName,
                   int startSector,
                   int sectorCount,
                   bool useMaxReadSpeed,
                   QtlDvdMedia::BadSectorPolicy badSectorPolicy,
                   QtlLogger* log);
    };

    //!
    //! Start the next extraction in the list.
    //! @return True on success, false on error (not started).
    //!
    bool startNextExtraction();

    // Unaccessible operations.
    QtlMovieDvdExtractionSession() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieDvdExtractionSession)
};

#endif // QTLMOVIEDVDEXTRACTIONSESSION_H
