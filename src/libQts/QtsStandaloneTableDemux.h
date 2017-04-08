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
//! @file QtsStandaloneTableDemux.h
//!
//! Declare the class QtsStandaloneTableDemux.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSSTANDALONETABLEDEMUX_H
#define QTSSTANDALONETABLEDEMUX_H

#include "QtsSectionDemux.h"

//!
//! A QtsSectionDemux which extracts MPEG tables without external handler.
//!
class QtsStandaloneTableDemux : public QtsSectionDemux, private QtsTableHandlerInterface
{
public:
    //!
    //! Constructor.
    //! @param [in] pidFilter Set of PID's to filter. Default: none.
    //!
    QtsStandaloneTableDemux(const QtsPidSet& pidFilter = QtsNoPid);

    //!
    //! Get the number of demuxed tables.
    //! @return The number of demuxed tables.
    //!
    int tableCount() const
    {
        return _tables.size ();
    }

    //!
    //! Get a pointer to a demuxed table.
    //! @param [in] index Index of demuxed table, in the range 0 to tableCount().
    //! @return A pointer to the demuxed table at @a index.
    //!
    const QtsTablePtr tableAt(int index) const
    {
        return index >= 0 && index < _tables.size() ? _tables[index] : 0;
    }

    //!
    //! Reset the analysis context (partially built sections and tables).
    //! Useful when the transport stream changes.
    //! The PID filter and the handlers are not modified.
    //! Inherited from QtsSectionDemux
    //!
    virtual void reset() Q_DECL_OVERRIDE;

    //!
    //! Reset the analysis context for one single PID.
    //! Inherited from QtsSectionDemux
    //! @param [in] pid PID to reset.
    //!
    virtual void resetPid(QtsPid pid) Q_DECL_OVERRIDE;

private:
    QtsTablePtrVector _tables; //!< Demuxed tables.

    //!
    //! This hook is invoked when a complete table is available.
    //! Tables with long sections are reported only when a new version is available.
    //! @param [in,out] demux The section demux.
    //! @param [in] table The table.
    //!
    virtual void handleTable(QtsSectionDemux& demux, const QtsTable& table) Q_DECL_OVERRIDE;

    // Make these methods inaccessible
    void setTableHandler(QtsTableHandlerInterface* handler);
    void setSectionHandler(QtsSectionHandlerInterface* handler);
};

#endif // QTSSTANDALONETABLEDEMUX_H
