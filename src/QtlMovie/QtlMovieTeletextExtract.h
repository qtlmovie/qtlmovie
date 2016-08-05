//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2016, Thierry Lelegard
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
//! @file QtlMovieTeletextExtract.h
//!
//! Declare the class QtlMovieTeletextExtract.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIETELETEXTEXTRACT_H
#define QTLMOVIETELETEXTEXTRACT_H

#include <QtCore>
#include "QtlMovieTsDemux.h"
#include "QtsTeletextDemux.h"
#include "QtlSubRipGenerator.h"

//!
//! This class extracts one Teletext subtitle stream from an MPEG-TS file into an SRT file.
//!
class QtlMovieTeletextExtract : public QtlMovieTsDemux, private QtsTeletextHandlerInterface
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] inputFileName Input MPEG-TS file name.
    //! @param [in] pid PID containing Teletext to extract.
    //! @param [in] teletextPage Teletext page.
    //! @param [in] outputFileName Output SRT file name.
    //! @param [in] settings Application settings.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //!
    QtlMovieTeletextExtract(const QString& inputFileName,
                            QtsPid pid,
                            int teletextPage,
                            const QString& outputFileName,
                            const QtlMovieSettings* settings,
                            QtlLogger* log,
                            QObject *parent = 0);

    //!
    //! Start the extraction.
    //! Reimplemented from QtlMovieTsDemux.
    //! @return False if already started. True otherwise.
    //!
    virtual bool start() Q_DECL_OVERRIDE;

protected:
    //!
    //! Emit the completed() signal.
    //! Reimplemented from QtlMovieTsDemux.
    //! @param [in] success True when the action completed successfully, false otherwise.
    //! @param [in] message Optional error message to log.
    //!
    virtual void emitCompleted(bool success, const QString& message = QString()) Q_DECL_OVERRIDE;

    //!
    //! Get the demux.
    //! Reimplemented from QtlMovieTsDemux.
    //! @return The current demux. Cannot be null.
    //!
    virtual QtsDemux* demux() Q_DECL_OVERRIDE
    {
        return &_demux;
    }

private:
    QtsTeletextDemux   _demux;          //!< Extract the Teletext frames from the file.
    QtlSubRipGenerator _subrip;         //!< SRT file generator.
    QString            _outputFileName; //!< Output SRT file name.
    QtsPid             _pid;            //!< PID containing Teletext to extract.
    int                _page;           //!< Teletext page.

    //!
    //! Invoked when a complete Teletext message is available.
    //! Implementation of QtsTeletextHandlerInterface.
    //! @param [in,out] demux The Teletext demux.
    //! @param [in] frame Teletext frame.
    //!
    virtual void handleTeletextMessage(QtsTeletextDemux& demux, const QtsTeletextFrame& frame) Q_DECL_OVERRIDE;

    // Unaccessible operations.
    QtlMovieTeletextExtract() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieTeletextExtract)
};

#endif // QTLMOVIETELETEXTEXTRACT_H
