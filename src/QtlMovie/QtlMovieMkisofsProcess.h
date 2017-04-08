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
//! @file QtlMovieMkisofsProcess.h
//!
//! Declare the class QtlMovieMkisofsProcess.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEMKISOFSPROCESS_H
#define QTLMOVIEMKISOFSPROCESS_H

#include <QtCore>
#include "QtlLogger.h"
#include "QtlMovieProcess.h"

//!
//! An execution of mkisofs.
//!
class QtlMovieMkisofsProcess : public QtlMovieProcess
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] arguments Mkisofs command line arguments.
    //! @param [in] settings Application settings.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieMkisofsProcess(const QStringList& arguments,
                           const QtlMovieSettings* settings,
                           QtlLogger* log,
                           QObject *parent = 0);

protected:
    //!
    //! Process one text line from standard error.
    //! @param [in] channel Origin of the line (QProcess::StandardOutput or QProcess::StandardError).
    //! @param [in] line Text line.
    //!
    virtual void processOutputLine(QProcess::ProcessChannel channel, const QString& line) Q_DECL_OVERRIDE;

private:
    // Unaccessible operations.
    QtlMovieMkisofsProcess() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieMkisofsProcess)
};

#endif // QTLMOVIEMKISOFSPROCESS_H
