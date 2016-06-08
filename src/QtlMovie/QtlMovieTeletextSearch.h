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
//! @file QtlMovieTeletextSearch.h
//!
//! Declare the class QtlMovieTeletextSearch.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIETELETEXTSEARCH_H
#define QTLMOVIETELETEXTSEARCH_H

#include <QtCore>
#include "QtlMovieTsDemux.h"
#include "QtlMediaStreamInfo.h"
#include "QtsSectionDemux.h"

//!
//! This class scans an MPEG-TS file and searches Teletext subtitle streams.
//!
class QtlMovieTeletextSearch : public QtlMovieTsDemux, private QtsTableHandlerInterface
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] fileName MPEG-TS file name.
    //! @param [in] settings Application settings.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //!
    QtlMovieTeletextSearch(const QString& fileName,
                           const QtlMovieSettings* settings,
                           QtlLogger* log,
                           QObject *parent = 0);

signals:
    //!
    //! Emitted when a Teletext subtitle stream is found.
    //! @param [in] stream A smart pointer to the stream info data.
    //!
    void foundTeletextSubtitles(QtlMediaStreamInfoPtr stream);

protected:
    //!
    //! Get the demux.
    //! Reimplemented from QtlMovieTsDemux.
    //! @return The current demux. Cannot be null.
    //!
    virtual QtsDemux* demux()
    {
        return &_demux;
    }

private:
    QtsSectionDemux _demux;   //!< Extract the sections from the file.

    //!
    //! Invoked when a complete table is available.
    //! Implementation of QtsTableHandlerInterface.
    //! @param [in,out] demux The section demux.
    //! @param [in] table The table.
    //!
    virtual void handleTable(QtsSectionDemux& demux, const QtsTable& table);

    // Unaccessible operations.
    QtlMovieTeletextSearch() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieTeletextSearch)
};

#endif // QTLMOVIETELETEXTSEARCH_H
