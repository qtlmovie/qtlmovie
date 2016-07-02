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
//!
//! @file QtlMovieDeleteAction.h
//!
//! Declare the class QtlMovieDeleteAction.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEDELETEACTION_H
#define QTLMOVIEDELETEACTION_H

#include <QtCore>
#include "QtlMovieAction.h"

//!
//! An action which deletes one or more files or directories.
//! Useful to cleanup intermediate files as soon as they are no longer needed.
//! Otherwise, all intermediate files will be automatically cleaned up after
//! completion but the accumulated storage can be too large in case of
//! multiple processing.
//!
class QtlMovieDeleteAction : public QtlMovieAction
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] files Files or directories to delete.
    //! @param [in] settings Application settings.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieDeleteAction(const QStringList& files,
                         const QtlMovieSettings* settings,
                         QtlLogger* log,
                         QObject *parent = 0);

    //!
    //! Add a file or directory to delete.
    //! @param [in] file File or directory to delete.
    //!
    void addFile(const QString& file)
    {
        _files << file;
    }

    //!
    //! Start the action.
    //! All the processing is done within this method.
    //! If also signals completed().
    //! @return False if already started. True otherwise.
    //!
    virtual bool start() Q_DECL_OVERRIDE;

private:
    QStringList _files; //!< Files to delete.

    // Unaccessible operations.
    QtlMovieDeleteAction() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieDeleteAction)
};

#endif // QTLMOVIEDELETEACTION_H
