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
//! @file QtsTeletextDemux.h
//!
//! Declare the class QtsTeletextDemux.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSTELETEXTDEMUX_H
#define QTSTELETEXTDEMUX_H

#include "QtsPesDemux.h"
#include "QtsTimeStamper.h"
#include "QtsTeletextCharset.h"
#include "QtsTeletextHandlerInterface.h"

//!
//! This class extracts Teletext subtitles from TS packets.
//!
class QtsTeletextDemux : public QtsPesDemux
{
public:
    //!
    //! Constructor
    //! @param [in] handler User handler for Teletext notification.
    //! @param [in] pidFilter Set of PID's to filter.
    //!
    explicit QtsTeletextDemux(QtsTeletextHandlerInterface* handler = 0, const QtsPidSet& pidFilter = QtsAllPids);

    //!
    //! Destructor.
    //!
    virtual ~QtsTeletextDemux();

    //!
    //! Activate or deactivate the font color tags in the output text.
    //! @param [in] addColors If true, font colors tags will be inserted in the output text.
    //!
    void setAddColors(bool addColors)
    {
        _addColors = addColors;
    }

    //!
    //! Check if font colors tags are inserted in the output text.
    //! @return True when font colors tags are inserted in the output text.
    //!
    bool getAddColors() const
    {
        return _addColors;
    }

    //!
    //! Flush any pending Teletext message.
    //! Useful only after receiving the last packet in the stream.
    //! Implicitly called by destructor.
    //!
    void flushTeletext();

    //!
    //! Set the Teletext handler.
    //! @param [in] handler The Teletext handler.
    //!
    void setTeletextHandler(QtsTeletextHandlerInterface* handler)
    {
        _txtHandler = handler;
    }

    //!
    //! Get the number of Teletext frames found in a given page.
    //! @param [in] page Teletext page number.
    //! @param [in] pid Teletext PID. If omitted, use the first PID containing frames from the specified @a page.
    //! @return Number of Teletext frames found so far on @a page.
    //!
    int frameCount(int page, QtsPid pid = QTS_PID_NULL) const;

    //!
    //! Reset the analysis context.
    //! Useful when the transport stream changes.
    //! The PID filter and the handlers are not modified.
    //!
    virtual void reset();

    //!
    //! Reset the analysis context for one single PID.
    //! @param [in] pid PID to reset.
    //!
    virtual void resetPid(QtsPid pid);

protected:
    //!
    //! This hook is invoked when a complete PES packet is available.
    //! Overloaded from QtsPesDemux.
    //! @param [in] packet The PES packet.
    //!
    virtual void handlePesPacket(const QtsPesPacket& packet);

private:
    //!
    //! Teletext transmission mode.
    //! Don't change values, they must match the binary format.
    //!
    enum TransMode {
        TRANSMODE_PARALLEL = 0,  //!< Parallel mode.
        TRANSMODE_SERIAL   = 1   //!< Serial mode.
    };

    //!
    //! Structure of a Teletext page.
    //!
    class TeletextPage
    {
    public:
        quint32            frameCount;    //!< Number of produced frames in this page.
        quint64            showTimestamp; //!< Show at timestamp (in ms).
        quint64            hideTimestamp; //!< Hide at timestamp (in ms).
        bool               tainted;       //!< True if text variable contains any data.
        QtsTeletextCharset charset;       //!< Charset to use.
        quint16            text[25][40];  //!< 25 lines x 40 cols (1 screen/page) of wide chars.
        //!
        //! Default constructor.
        //!
        TeletextPage();
        //!
        //! Reset to a new page with a new starting timestamp.
        //! @param [in] timestamp New starting timestamp.
        //!
        void reset(quint64 timestamp);
    };

    //!
    //! Map of TeletextPage, indexed by page number.
    //!
    typedef QMap<int,TeletextPage> TeletextPageMap;

    //!
    //! This internal structure contains the analysis context for one PID.
    //!
    class PidContext
    {
    public:
        QtsTimeStamper   timeStamper;    //!< Generator of time stamps on this PID.
        bool             resetPending;   //!< Delayed reset on this PID.
        bool             receivingData;  //!< Incoming data should be processed or ignored.
        TransMode        transMode;      //!< Teletext transmission mode.
        int              currentPage;    //!< Current Teletext page number.
        TeletextPageMap  pages;          //!< Working Teletext page buffers, indexed by page number.
        //!
        //! Default constructor.
        //!
        PidContext();
    };

    //!
    //! Map of PID analysis contexts, indexed by PID value.
    //!
    typedef QMap<QtsPid,PidContext> PidContextMap;

    //!
    //! Process one Teletext packet.
    //! @param [in] pid PID number.
    //! @param [in,out] pc PID context.
    //! @param [in] dataUnitId Teletext packet data unit id.
    //! @param [in] pkt Address of Teletext packet (44 bytes, QTS_TELETEXT_PACKET_SIZE).
    //!
    void processTeletextPacket(QtsPid pid, PidContext& pc, quint8 dataUnitId, const quint8* pkt);

    //!
    //! Process one Teletext page.
    //! @param [in] pid PID number.
    //! @param [in,out] pc PID context.
    //! @param [in] pageNumber Page number.
    //!
    void processTeletextPage(QtsPid pid, PidContext& pc, int pageNumber);

    //!
    //! Remove 8/4 Hamming code.
    //! @param [in] a Hamming-encoded byte.
    //! @return Decoded byte.
    //! @see ETSI 300 706, section 8.2.
    //!
    static quint8 unham_8_4(quint8 a);

    //!
    //! Remove 24/18 Hamming code.
    //! @param [in] a Hamming-encoded word.
    //! @return Decoded word.
    //! @see ETSI 300 706, section 8.3.
    //!
    static quint32 unham_24_18(quint32 a);

    //!
    //! Extract Teletext magazine number from Teletext page.
    //! @param [in] page Teletext page.
    //! @return The Teletext magazine number.
    //!
    int magazineOf(int page)
    {
        return (page >> 8) & 0x0F;
    }

    //!
    //! Extract Teletext page number from Teletext page.
    //! @param [in] page Teletext page.
    //! @return The Teletext page number.
    //!
    int pageOf(int page)
    {
        return page & 0xFF;
    }

    //!
    //! Converts a page number from BCD to binary.
    //! Teletext page numbers are stored in Binary-Coded Decimal.
    //! @param [in] bcd BCD page number.
    //! @return Binary page number.
    //!
    static int pageBcdToBinary(int bcd);

    //!
    //! Converts a page number from binary to BCD.
    //! Teletext page numbers are stored in Binary-Coded Decimal.
    //! @param [in] bin Binary page number.
    //! @return BCD page number.
    //!
    static int pageBinaryToBcd(int bin);

    // Private members:
    QtsTeletextHandlerInterface* _txtHandler;    //!< User handler.
    PidContextMap                _pids;          //!< Map of PID analysis contexts.
    bool                         _addColors;     //!< Add font color tags.
    bool                         _inHandler;     //!< True when in the context of a handler
    QtsPid                       _pidInHandler;  //!< PID which is currently processed by handler
    bool                         _resetPending;  //!< Delayed reset().

    // Unaccessible operations.
    QtsTeletextDemux() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtsTeletextDemux)
};

#endif // QTSTELETEXTDEMUX_H
