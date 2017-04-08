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
//
// Declare the class QtlMovieTeletextSearch.
//
//----------------------------------------------------------------------------

#include "QtlMovieTeletextSearch.h"
#include "QtlMovie.h"
#include "QtsProgramAssociationTable.h"
#include "QtsProgramMapTable.h"
#include "QtsTeletextDescriptor.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieTeletextSearch::QtlMovieTeletextSearch(const QString& fileName,
                                               const QtlMovieSettings* settings,
                                               QtlLogger* log,
                                               QObject* parent) :
    QtlMovieTsDemux(fileName, settings, log, parent),
    _demux(this)
{
    // No need to report in search phase.
    setSilent(true);

    // Set the demux to collect the PAT.
    _demux.addPid(QTS_PID_PAT);
}


//----------------------------------------------------------------------------
// Invoked when a complete table is available.
//----------------------------------------------------------------------------

void QtlMovieTeletextSearch::handleTable(QtsSectionDemux& demux, const QtsTable& table)
{
    debug(tr("Teletext search: Received table id %1 on PID %2").arg(table.tableId()).arg(table.sourcePid()));

    // Process the table.
    switch (table.tableId()) {

    case QTS_TID_PAT: {
        // Got a PAT, we can locate the various services.
        const QtsProgramAssociationTable pat(table);
        if (pat.isValid()) {
            // No longer filter the PAT.
            demux.removePid(QTS_PID_PAT);
            // Add a filter on the PMT PID of all services.
            foreach (const QtsProgramAssociationTable::ServiceEntry& service, pat.serviceList) {
                debug(tr("Teletext search: Found service id %1, PMT PID %2").arg(service.serviceId).arg(service.pmtPid));
                demux.addPid(service.pmtPid);
            }
        }
        break;
    }

    case QTS_TID_PMT: {
        // Got a PMT, we can locate the various elementary streams.
        const QtsProgramMapTable pmt(table);
        if (pmt.isValid()) {
            // Loop through all elementary streams in the service.
            foreach (const QtsProgramMapTable::StreamEntry& stream, pmt.streams) {
                // Loop through all teletext descriptors in the stream.
                for (int index = 0; (index = stream.descs.search(QTS_DID_TELETEXT, index)) < stream.descs.size(); ++index) {
                    // Deserialize teletext descriptor.
                    const QtsTeletextDescriptor td(*(stream.descs[index]));
                    if (td.isValid()) {
                        // Found a valid teletext descriptor, loop through all language entries.
                        foreach (const QtsTeletextDescriptor::Entry& entry, td.entries) {
                            if (entry.type == QTS_TELETEXT_SUBTITLES || entry.type == QTS_TELETEXT_SUBTITLES_HI) {
                                // Found teletext subtitles.
                                // Create a stream description.
                                const QtlMediaStreamInfoPtr info(new QtlMediaStreamInfo());
                                info->setStreamType(QtlMediaStreamInfo::Subtitle);
                                info->setSubtitleType(QtlMediaStreamInfo::SubTeletext);
                                info->setStreamId(stream.pid); // stream id = PID for MPEG-TS files.
                                info->setTeletextPage(entry.page);
                                info->setLanguage(entry.language);
                                info->setImpaired(entry.type == QTS_TELETEXT_SUBTITLES_HI);
                                // Send the new stream information to clients.
                                emit foundTeletextSubtitles(info);
                            }
                        }
                    }
                }
            }
            // No longer filter this PMT.
            demux.removePid(table.sourcePid());
        }
        break;
    }

    default:
        // Other tables are ignored.
        break;
    }

    // When no more PID is demuxed, terminate the search.
    if (demux.filteredPidCount() == 0) {
        emitCompleted(true);
    }
}
