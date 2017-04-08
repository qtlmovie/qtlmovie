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
//! @file QtsCore.h
//!
//! Core definitions for Qts.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSCORE_H
#define QTSCORE_H

#include "QtlCore.h"
#include <bitset>


//----------------------------------------------------------------------------
// Transport packets sizes.
//----------------------------------------------------------------------------

//! MPEG Transport Stream packet size.
const int QTS_PKT_SIZE = 188;

//! Size of Reed-Solomon outer FEC.
const int QTS_REED_SOLOMON_SIZE = 16;

//! TS packet size with trailing Reed-Solomon outer FEC
const int QTS_PKT_RS_SIZE = QTS_PKT_SIZE + QTS_REED_SOLOMON_SIZE;

//! Size of timestamp before TS packet in M2TS files (Blu-ray disc).
const int QTS_M2TS_HEADER_SIZE = 4;

//! MPEG packet size in M2TS files (Blu-ray disc).
const int QTS_PKT_M2TS_SIZE = QTS_M2TS_HEADER_SIZE + QTS_PKT_SIZE;

//! Value of sync byte (first in TS packet)
const quint8 QTS_SYNC_BYTE = 0x47;

//! PES packet start code prefix (24 bits).
const quint32 QTS_PES_START = 0x000001;

//! Size (in bits) of a Continuity Counter (CC) field.
const int QTS_CC_BITS = 4;

//! Bit mask of a Continuity Counter (CC) field.
const quint8 QTS_CC_MASK = 0x0F;

//! Number of values of a Continuity Counter (CC) field (=16).
const quint8 QTS_CC_MAX = 1 << QTS_CC_BITS;


//---------------------------------------------------------------------
// PID (TS packet identifier).
//---------------------------------------------------------------------

//! PID (TS packet identifier value.
typedef quint16 QtsPid;

//! Size (in bits) of PID field.
const int QTS_PID_BITS = 13;

//! Max number of PID's (=8192).
const QtsPid QTS_PID_MAX = 1 << QTS_PID_BITS;

//! A bit mask for PID values. Useful to implement PID filtering.
typedef std::bitset<QTS_PID_MAX> QtsPidSet;

//! A constant QtsPidSet with no PID set.
extern const QtsPidSet QtsNoPid;

//! A constant QtsPidSet with all PID's set.
extern const QtsPidSet QtsAllPids;

//
// Valid in all MPEG contexts:
//
const QtsPid QTS_PID_PAT     = 0x0000; //!< Program Association Table PAT
const QtsPid QTS_PID_CAT     = 0x0001; //!< Conditional Access Table
const QtsPid QTS_PID_TSDT    = 0x0002; //!< Transport Stream Description Table
const QtsPid QTS_PID_NULL    = 0x1FFF; //!< Null packets (stuffing)
//
// Valid in DVB context:
//
const QtsPid QTS_PID_NIT     = 0x0010; //!< Network Information Table
const QtsPid QTS_PID_SDT     = 0x0011; //!< Service Description Table
const QtsPid QTS_PID_BAT     = 0x0011; //!< Bouquet Association Table
const QtsPid QTS_PID_EIT     = 0x0012; //!< Event Information Table
const QtsPid QTS_PID_RST     = 0x0013; //!< Running Status Table
const QtsPid QTS_PID_TDT     = 0x0014; //!< Time & Date Table
const QtsPid QTS_PID_TOT     = 0x0014; //!< Time Offset Table
const QtsPid QTS_PID_NETSYNC = 0x0015; //!< Network synchronization
const QtsPid QTS_PID_RNT     = 0x0016; //!< TV-Anytime
const QtsPid QTS_PID_INBSIGN = 0x001C; //!< Inband Signalling
const QtsPid QTS_PID_MEASURE = 0x001D; //!< Measurement
const QtsPid QTS_PID_DIT     = 0x001E; //!< Discontinuity Information Table
const QtsPid QTS_PID_SIT     = 0x001F; //!< Selection Information Table


//----------------------------------------------------------------------------
// Bit rates.
//----------------------------------------------------------------------------

//!
//! Bitrate value in bits/second.
//!
typedef qint32 QtsBitRate;

//!
//! Number of milli-seconds (64 bits).
//!
typedef qint64 QtsMilliSecond;

//!
//! Transport Stream packets are counted using 64-bit integers.
//! Thus, PacketCounter will never overflow: at 100 Mb/s, 2^64 188-byte
//! packets will take 8.7 million years to transmit. No process will
//! ever run that long. On the contrary, using 32-bit integer would
//! be insufficient: at 100 Mb/s, 2^32 188-byte packets will take
//! only 17 hours to transmit.
//!
typedef qint64 QtsPacketCounter;

//!
//! Sections are counted using 64-bit integers.
//! @see QtsPacketCounter
//!
typedef qint64 QtsSectionCounter;


//!
//! Convert 188-byte packet bitrate into 204-byte packet bitrate.
//! @param [in] bitrate188 TS bitrate in b/s based on 188-byte packets.
//! @return TS bitrate in b/s based on 204-byte packets.
//!
inline QtsBitRate qtsToBitrate204(QtsBitRate bitrate188)
{
    return QtsBitRate((qint64(bitrate188) * 204L) / 188L);
}

//!
//! Convert 204-byte packet bitrate into 188-byte packet bitrate.
//! @param [in] bitrate204 TS bitrate in b/s based on 204-byte packets.
//! @return TS bitrate in b/s based on 188-byte packets.
//!
inline QtsBitRate qtsToBitrate188(QtsBitRate bitrate204)
{
    return QtsBitRate((qint64(bitrate204) * 188L) / 204L);
}

//!
//! Compute the interval, in milliseconds, between two packets.
//! @param [in] bitrate Transport stream bitrate.
//! @param [in] distance Number of packets between the two packets.
//! @return Number of milliseconds between the two packets.
//!
inline QtsMilliSecond qtsPacketInterval(QtsBitRate bitrate, QtsPacketCounter distance = 1)
{
    return bitrate == 0 ? 0 : (distance * 8 * QTS_PKT_SIZE * 1000) / QtsMilliSecond(bitrate);
}

//!
//! Compute the number of packets transmitted during a given duration in milliseconds.
//! @param [in] bitrate Transport stream bitrate.
//! @param [in] duration Number of milliseconds.
//! @return Number of packets.
//!
inline QtsPacketCounter qtsPacketDistance(QtsBitRate bitrate, QtsMilliSecond duration)
{
    return (QtsPacketCounter(bitrate) * (duration >= 0 ? duration : -duration)) / (1000 * 8 * QTS_PKT_SIZE);
}

//!
//! Compute the minimum number of TS packets required to transport a section.
//! @param [in] sectionSize Section size in bytes.
//! @return Number of TS packets.
//!
inline QtsPacketCounter qtsSectionPacketCount(int sectionSize)
{
    // The required size for a section is section_size + 1 (1 for pointer_field in first packet).
    // In each packet, the useable size is 184 bytes.
    return QtsPacketCounter((sectionSize + 184) / 184);
}


//----------------------------------------------------------------------------
// Scrambling control.
//----------------------------------------------------------------------------

//! Size in bits of a DVB Common Scrambling (DVB-CS) Control Word (CW).
const int QTS_CW_BITS = 64;

//! Size in bytes of a DVB Common Scrambling (DVB-CS) Control Word (CW).
const int QTS_CW_BYTES = QTS_CW_BITS / 8;

//!
//! Scrambling_control values (used in TS and PES packets headers)
//!
const quint8 QTS_SC_CLEAR        = 0;  //!< Not scrambled (MPEG-defined)
const quint8 QTS_SC_DVB_RESERVED = 1;  //!< Reserved for future use by DVB
const quint8 QTS_SC_EVEN_KEY     = 2;  //!< Scrambled with even key (DVB-defined)
const quint8 QTS_SC_ODD_KEY      = 3;  //!< Scrambled with odd key (DVB-defined)


//---------------------------------------------------------------------
// MPEG clock representation:
// - PCR (Program Clock Reference)
// - PTS (Presentation Time Stamp)
// - DTS (Decoding Time Stamp)
//---------------------------------------------------------------------

//! MPEG-2 System Clock frequency: 27 Mb/s.
//! Used by PCR (Program Clock Reference).
const quint32 QTS_SYSTEM_CLOCK_FREQ = 27000000;

//! MPEG-2 System Clock subfrequency factor.
//! Used by PTS (Presentation Time Stamp) and DTS (Decoding Time Stamp).
const quint32 QTS_SYSTEM_CLOCK_SUBFACTOR = 300;

//! MPEG-2 System Clock subfrequency: 90 kb/s.
//! Used by PTS (Presentation Time Stamp) and DTS (Decoding Time Stamp).
const quint32 QTS_SYSTEM_CLOCK_SUBFREQ = QTS_SYSTEM_CLOCK_FREQ / QTS_SYSTEM_CLOCK_SUBFACTOR;

//! PTS and DTS bitmask (wrap up at 2**33).
const quint64 QTS_PTS_DTS_MASK  = Q_UINT64_C(0x00000001FFFFFFFF);

//! PTS and DTS scale (wrap up at 2**33).
const quint64 QTS_PTS_DTS_SCALE = Q_UINT64_C(0x0000000200000000);

//! Size in bytes of a PCR (Program Clock Reference)
const int QTS_PCR_SIZE = 6;

//!
//! Encoding of Modified Julian Dates (MJD).
//! The origin of MJD is 17 Nov 1858 00:00:00.
//! The UNIX epoch (1 Jan 1970) is 40587 days from julian time origin
//!
const quint32 QTS_MJD_EPOCH = 40587;

//!
//! Extract a PCR (Program Clock Reference) from a stream.
//! @param [in] b Address of a 6-byte area where to extract the PCR.
//! @return A 42-bit PCR value.
//!
quint64 qtsGetPcr(const void* b);

//!
//! Insert a PCR (Program Clock Reference) in a stream.
//! @param [out] b Address of a 6-byte area where to insert the PCR.
//! @param [in] pcr A 42-bit PCR value.
//!
void qtsPutPcr(void* b, const quint64& pcr);

//!
//! Extract a PTS or DTS from a stream.
//! @param [in] b Address of a 5-byte area where to extract the PTS or DTS.
//! @return A 33-bit PTS or DTS value.
//!
quint64 qtsGetPtsDts(const void* b);

//!
//! Insert a PTS or DTS in a stream.
//! @param [out] b Address of a 5-byte area where to insert the PTS or DTS.
//! @param [in] xts A 33-bit PTS or DTS value.
//!
void qtsPutPtsDts(void* b, const quint64& xts);

//!
//! Check if PTS2 follows PTS1 after wrap up.
//! @param pts1 First Presentation Time Stamp (PTS).
//! @param pts2 Second Presentation Time Stamp (PTS).
//! @return True if @a pts2 follows @a pts1 after wrapping up at @link QTS_PTS_DTS_SCALE @endlink.
//!
inline bool qtsWrapUpPts(quint64 pts1, quint64 pts2)
{
    return pts2 < pts1 && (pts1 - pts2) > Q_UINT64_C(0x00000001F0000000);
}

//!
//! Check if two Presentation Time Stamps (PTS) are in sequence.
//! In MPEG video, B-frames are transported out-of-sequence.
//! Their PTS is typically lower than the previous D-frame or I-frame
//! in the transport. A "sequenced" PTS is one that is higher than
//! the previous sequenced PTS (with possible wrap up).
//! @param pts1 First Presentation Time Stamp (PTS).
//! @param pts2 Second Presentation Time Stamp (PTS).
//! @return True if @a pts2 follows @a pts1 with or without wrapping up at @link QTS_PTS_DTS_SCALE @endlink.
//!
inline bool qtsSequencedPts(quint64 pts1, quint64 pts2)
{
    return pts1 <= pts2 || qtsWrapUpPts(pts1, pts2);
}


//---------------------------------------------------------------------
// SID (Stream identifier) values, as used in PES header
//---------------------------------------------------------------------

//!
//! SID (Stream identifier) values, as used in PES header.
//!
//! Note that QTS_SID_PACK, the SID for pack headers in program stream,
//! is not a real PES packet. In DVD VOB files, for instance, each
//! 2048-byte sector is a pack and starts with a pack header
//! (see ISO 13818-1, §2.5.3.3).
//!
typedef quint8 QtsPesStreamId;

const QtsPesStreamId QTS_SID_PACK       = 0xBA; //!< Pack header in program stream.
const QtsPesStreamId QTS_SID_SYSTEM     = 0xBB; //!< System header in program stream.
const QtsPesStreamId QTS_SID_PSMAP      = 0xBC; //!< Program stream map
const QtsPesStreamId QTS_SID_PRIV1      = 0xBD; //!< Private stream 1
const QtsPesStreamId QTS_SID_PAD        = 0xBE; //!< Padding stream
const QtsPesStreamId QTS_SID_PRIV2      = 0xBF; //!< Private stream 2
const QtsPesStreamId QTS_SID_AUDIO      = 0xC0; //!< Audio stream; with number
const QtsPesStreamId QTS_SID_AUDIO_MASK = 0x1F; //!< Mask to get audio stream number
const QtsPesStreamId QTS_SID_VIDEO      = 0xE0; //!< Video stream; with number
const QtsPesStreamId QTS_SID_VIDEO_MASK = 0x0F; //!< Mask to get video stream number
const QtsPesStreamId QTS_SID_ECM        = 0xF0; //!< ECM stream
const QtsPesStreamId QTS_SID_EMM        = 0xF1; //!< EMM stream
const QtsPesStreamId QTS_SID_DSMCC      = 0xF2; //!< DSM-CC data
const QtsPesStreamId QTS_SID_ISO13522   = 0xF3; //!< ISO 13522 (hypermedia)
const QtsPesStreamId QTS_SID_H222_1_A   = 0xF4; //!< H.222.1 type A
const QtsPesStreamId QTS_SID_H222_1_B   = 0xF5; //!< H.222.1 type B
const QtsPesStreamId QTS_SID_H222_1_C   = 0xF6; //!< H.222.1 type C
const QtsPesStreamId QTS_SID_H222_1_D   = 0xF7; //!< H.222.1 type D
const QtsPesStreamId QTS_SID_H222_1_E   = 0xF8; //!< H.222.1 type E
const QtsPesStreamId QTS_SID_ANCILLARY  = 0xF9; //!< Ancillary stream
const QtsPesStreamId QTS_SID_MP4_SLPACK = 0xFA; //!< MPEG-4 SL-packetized stream
const QtsPesStreamId QTS_SID_MP4_FLEXM  = 0xFB; //!< MPEG-4 FlexMux stream
const QtsPesStreamId QTS_SID_METADATA   = 0xFC; //!< MPEG-7 metadata stream
const QtsPesStreamId QTS_SID_EXTENDED   = 0xFD; //!< Extended stream id
const QtsPesStreamId QTS_SID_RESERVED   = 0xFE; //!< Reserved value
const QtsPesStreamId QTS_SID_PSDIR      = 0xFF; //!< Program stream directory

//!
//! Check if a PES stream identifier value indicates a video stream.
//! @param [in] sid PES stream identifier.
//! @return True for video stream, false otherwise.
//!
inline bool qtsIsVideoPesStreamId(QtsPesStreamId sid)
{
    return (sid & ~QTS_SID_VIDEO_MASK) == QTS_SID_VIDEO;
}

//!
//! Check if a PES stream identifier value indicates an audio stream.
//! @param [in] sid PES stream identifier.
//! @return True for audio stream, false otherwise.
//!
inline bool qtsIsAudioPesStreamId(QtsPesStreamId sid)
{
    return (sid & ~QTS_SID_AUDIO_MASK) == QTS_SID_AUDIO;
}

//!
//! Check if a PES stream identifier value indicates a PES packet with long header.
//! @param [in] sid PES stream identifier.
//! @return True for a PES packet with long header, false otherwise.
//!
bool qtsIsLongHeaderPesStreamId(QtsPesStreamId sid);


//---------------------------------------------------------------------
// PES start code values.
//---------------------------------------------------------------------

//!
//! PES start codes values, as used in PES headers and data.
//!
//! A PES start sequence is 32-bit long. The first 24 bits are always
//! 0x000001 (defined as QTS_PES_START), followed by the 8-bit start
//! code value.
//!
//! A PES start sequence is used to "start a sequence". A sequence can
//! be a complete PES packet, a unit inside a PES packet or system data
//! outside PES packets (such as pack headers in program streams).
//!
//! Most PES start codes in the "system range" (0xB9 to 0xFF) are
//! typically used as stream id values in PES packet headers (when
//! the PES start sequence is in fact the start of a PES packet).
//!
typedef quint8 QtsPesStartCode;

const QtsPesStartCode QTS_PST_PICTURE         = 0x00;
const QtsPesStartCode QTS_PST_SLICE_MIN       = 0x01;
const QtsPesStartCode QTS_PST_SLICE_MAX       = 0xAF;
const QtsPesStartCode QTS_PST_RESERVED_B0     = 0xB0;
const QtsPesStartCode QTS_PST_RESERVED_B1     = 0xB1;
const QtsPesStartCode QTS_PST_USER_DATA       = 0xB2;
const QtsPesStartCode QTS_PST_SEQUENCE_HEADER = 0xB3;
const QtsPesStartCode QTS_PST_SEQUENCE_ERROR  = 0xB4;
const QtsPesStartCode QTS_PST_EXTENSION       = 0xB5;
const QtsPesStartCode QTS_PST_RESERVED_B6     = 0xB6;
const QtsPesStartCode QTS_PST_SEQUENCE_END    = 0xB7;
const QtsPesStartCode QTS_PST_GROUP           = 0xB8;
const QtsPesStartCode QTS_PST_SYSTEM_MIN      = 0xB9;  //!< Stream id values (QTS_SID_*)
const QtsPesStartCode QTS_PST_SYSTEM_MAX      = 0xFF;


//---------------------------------------------------------------------
// Video encoding.
//---------------------------------------------------------------------

//!
//! Video macroblock width.
//! Valid for:
//! - ISO 11172-2 (MPEG-1 video)
//! - ISO 13818-2 (MPEG-2 video)
//! - ISO 14496-10 (MPEG-4 Advanced Video Coding, AVC, ITU H.264)
//!
const int QTS_MACROBLOCK_WIDTH  = 16;

//!
//! Video macroblock height.
//! Valid for:
//! - ISO 11172-2 (MPEG-1 video)
//! - ISO 13818-2 (MPEG-2 video)
//! - ISO 14496-10 (MPEG-4 Advanced Video Coding, AVC, ITU H.264)
//!
const int QTS_MACROBLOCK_HEIGHT = 16;

//
// Aspect ratio values (in MPEG-1/2 video sequence header)
//
const int QTS_AR_SQUARE = 1;  //!< 1:1
const int QTS_AR_4_3    = 2;  //!< 4:3
const int QTS_AR_16_9   = 3;  //!< 16:9
const int QTS_AR_221    = 4;  //!< 2.21:1

//
// Chroma format values (in MPEG-1/2 video sequence header)
//
const int QTS_CHROMA_MONO = 0;  //!< Monochrome
const int QTS_CHROMA_420  = 1;  //!< 4:2:0
const int QTS_CHROMA_422  = 2;  //!< 4:2:2
const int QTS_CHROMA_444  = 3;  //!< 4:4:4

//!
//! AVC / H.264 access unit types
//!
const int QTS_AVC_AUT_NON_IDR      =  1; //!< Coded slice of a non-IDR picture
const int QTS_AVC_AUT_SLICE_A      =  2; //!< Coded slice data partition A
const int QTS_AVC_AUT_SLICE_B      =  3; //!< Coded slice data partition B
const int QTS_AVC_AUT_SLICE_C      =  4; //!< Coded slice data partition C
const int QTS_AVC_AUT_IDR          =  5; //!< Coded slice of an IDR picture
const int QTS_AVC_AUT_SEI          =  6; //!< Supplemental enhancement information (SEI)
const int QTS_AVC_AUT_SEQPARAMS    =  7; //!< Sequence parameter set
const int QTS_AVC_AUT_PICPARAMS    =  8; //!< Picture parameter set
const int QTS_AVC_AUT_DELIMITER    =  9; //!< Access unit delimiter
const int QTS_AVC_AUT_END_SEQUENCE = 10; //!< End of sequence
const int QTS_AVC_AUT_END_STREAM   = 11; //!< End of stream
const int QTS_AVC_AUT_FILLER       = 12; //!< Filler data
const int QTS_AVC_AUT_SEQPARAMSEXT = 13; //!< Sequence parameter set extension
const int QTS_AVC_AUT_PREFIX       = 14; //!< Prefix NAL unit in scalable extension
const int QTS_AVC_AUT_SUBSETPARAMS = 15; //!< Subset sequence parameter set
const int QTS_AVC_AUT_SLICE_NOPART = 19; //!< Coded slice without partitioning
const int QTS_AVC_AUT_SLICE_SCALE  = 20; //!< Coded slice in scalable extension


//---------------------------------------------------------------------
// Stream type values, as used in the PMT
//---------------------------------------------------------------------

//! Stream type values, as used in the PMT
typedef quint8 QtsStreamType;

const QtsStreamType QTS_ST_MPEG1_VIDEO = 0x01; //!< MPEG-1 Video
const QtsStreamType QTS_ST_MPEG2_VIDEO = 0x02; //!< MPEG-2 Video
const QtsStreamType QTS_ST_MPEG1_AUDIO = 0x03; //!< MPEG-1 Audio
const QtsStreamType QTS_ST_MPEG2_AUDIO = 0x04; //!< MPEG-2 Audio
const QtsStreamType QTS_ST_PRIV_SECT   = 0x05; //!< MPEG-2 Private sections
const QtsStreamType QTS_ST_PES_PRIV    = 0x06; //!< MPEG-2 PES private data
const QtsStreamType QTS_ST_MHEG        = 0x07; //!< MHEG
const QtsStreamType QTS_ST_DSMCC       = 0x08; //!< DSM-CC
const QtsStreamType QTS_ST_MPEG2_ATM   = 0x09; //!< MPEG-2 over ATM
const QtsStreamType QTS_ST_DSMCC_MPE   = 0x0A; //!< DSM-CC Multi-Protocol Encapsulation
const QtsStreamType QTS_ST_DSMCC_UN    = 0x0B; //!< DSM-CC User-to-Network messages
const QtsStreamType QTS_ST_DSMCC_SD    = 0x0C; //!< DSM-CC Stream Descriptors
const QtsStreamType QTS_ST_DSMCC_SECT  = 0x0D; //!< DSM-CC Sections
const QtsStreamType QTS_ST_MPEG2_AUX   = 0x0E; //!< MPEG-2 Auxiliary
const QtsStreamType QTS_ST_AAC_AUDIO   = 0x0F; //!< Advanced Audio Coding (ISO 13818-7)
const QtsStreamType QTS_ST_MPEG4_VIDEO = 0x10; //!< MPEG-4 Video
const QtsStreamType QTS_ST_MPEG4_AUDIO = 0x11; //!< MPEG-4 Audio
const QtsStreamType QTS_ST_MPEG4_PES   = 0x12; //!< MPEG-4 SL or FlexMux in PES packets
const QtsStreamType QTS_ST_MPEG4_SECT  = 0x13; //!< MPEG-4 SL or FlexMux in sections
const QtsStreamType QTS_ST_DSMCC_DLOAD = 0x14; //!< DSM-CC Synchronized Download Protocol
const QtsStreamType QTS_ST_MDATA_PES   = 0x15; //!< MPEG-7 MetaData in PES packets
const QtsStreamType QTS_ST_MDATA_SECT  = 0x16; //!< MPEG-7 MetaData in sections
const QtsStreamType QTS_ST_MDATA_DC    = 0x17; //!< MPEG-7 MetaData in DSM-CC Data Carousel
const QtsStreamType QTS_ST_MDATA_OC    = 0x18; //!< MPEG-7 MetaData in DSM-CC Object Carousel
const QtsStreamType QTS_ST_MDATA_DLOAD = 0x19; //!< MPEG-7 MetaData in DSM-CC Sync Downl Proto
const QtsStreamType QTS_ST_MPEG2_IPMP  = 0x1A; //!< MPEG-2 IPMP stream
const QtsStreamType QTS_ST_AVC_VIDEO   = 0x1B; //!< AVC video
const QtsStreamType QTS_ST_IPMP        = 0x7F; //!< IPMP stream
const QtsStreamType QTS_ST_AC3_AUDIO   = 0x81; //!< AC-3 Audio (ATSC only)
const QtsStreamType QTS_ST_EAC3_AUDIO  = 0x87; //!< Enhanced-AC-3 Audio (ATSC only)

//!
//! Check if an ST (Stream Type) value indicates a PES stream.
//! @param [in] st Stream type as used in PMT.
//! @return True for a PES stream, false otherwise.
//!
bool qtsIsPesStreamType(QtsStreamType st);

//!
//! Check if an ST (Stream Type) value indicates a video stream.
//! @param [in] st Stream type as used in PMT.
//! @return True for a video stream, false otherwise.
//!
bool qtsIsVideoStreamType(QtsStreamType st);

//!
//! Check if an ST (Stream Type) value indicates an audio stream.
//! @param [in] st Stream type as used in PMT.
//! @return True for an audio stream, false otherwise.
//!
bool qtsIsAudioStreamType(QtsStreamType st);

//!
//! Check if an ST (Stream Type) value indicates a stream carrying sections.
//! @param [in] st Stream type as used in PMT.
//! @return True for a section stream, false otherwise.
//!
bool qtsIsSectionStreamType(QtsStreamType st);


//---------------------------------------------------------------------
// PSI, SI and data sections and tables
//---------------------------------------------------------------------

//! Size (in bits) of a section version field
const int QTS_SVERSION_BITS = 5;

//! Bit mask of a section version field
const quint8 QTS_SVERSION_MASK = 0x1F;

//! Number of values of a section version field (=32).
const quint8 QTS_SVERSION_MAX = 1 << QTS_SVERSION_BITS;

//! Maximum size of a descriptor.
//! 255 + 2-byte header.
const int QTS_MAX_DESCRIPTOR_SIZE = 257;

//! Maximum size of a descriptor payload.
const int QTS_MAX_DESCRIPTOR_PAYLOAD_SIZE = QTS_MAX_DESCRIPTOR_SIZE - 2;

//! Header size for "short sections" (ie sections with a short header).
const int QTS_SHORT_SECTION_HEADER_SIZE = 3;

//! Header size for "long sections" (ie sections with a long header).
const int QTS_LONG_SECTION_HEADER_SIZE = 8;

//! Size of the CRC of a long section.
//! Short sections do not have a CRC.
const int QTS_SECTION_CRC32_SIZE = 4;

//! Maximum size of a PSI section.
const int QTS_MAX_PSI_SECTION_SIZE = 1024;

//! Maximum size of a private section.
const int QTS_MAX_PRIVATE_SECTION_SIZE = 4096;

//! Minimum size of a "short section" (ie a section with a short header).
const int QTS_MIN_SHORT_SECTION_SIZE = QTS_SHORT_SECTION_HEADER_SIZE;

//! Minimum size of a "long section" (ie a section with a short header).
const int QTS_MIN_LONG_SECTION_SIZE = QTS_LONG_SECTION_HEADER_SIZE + QTS_SECTION_CRC32_SIZE;

//! Maximum size of the payload of a PSI short section.
const int QTS_MAX_PSI_SHORT_SECTION_PAYLOAD_SIZE = QTS_MAX_PSI_SECTION_SIZE - QTS_SHORT_SECTION_HEADER_SIZE;

//! Maximum size of the payload of a PSI long section.
const int QTS_MAX_PSI_LONG_SECTION_PAYLOAD_SIZE = QTS_MAX_PSI_SECTION_SIZE - QTS_LONG_SECTION_HEADER_SIZE - QTS_SECTION_CRC32_SIZE;

//! Maximum size of the payload of a private short section.
const int QTS_MAX_PRIVATE_SHORT_SECTION_PAYLOAD_SIZE = QTS_MAX_PRIVATE_SECTION_SIZE - QTS_SHORT_SECTION_HEADER_SIZE;

//! Maximum size of the payload of a private long section.
const int QTS_MAX_PRIVATE_LONG_SECTION_PAYLOAD_SIZE = QTS_MAX_PRIVATE_SECTION_SIZE - QTS_LONG_SECTION_HEADER_SIZE - QTS_SECTION_CRC32_SIZE;


//---------------------------------------------------------------------
// PSI/SI table identifier.
//---------------------------------------------------------------------

//!
//! PSI/SI table identifier.
//!
typedef quint8 QtsTableId;

//!
//! PSI/SI table identifier extension (long header).
//!
typedef quint16 QtsTableIdExt;

//
// Valid in all MPEG contexts:
//
const QtsTableId QTS_TID_PAT           = 0x00; //!< Program Association Table PAT
const QtsTableId QTS_TID_CAT           = 0x01; //!< Conditional Access Table
const QtsTableId QTS_TID_PMT           = 0x02; //!< Program Map Table
const QtsTableId QTS_TID_TSDT          = 0x03; //!< Transport Stream Description Table
const QtsTableId QTS_TID_MP4SDT        = 0x04; //!< MPEG-4 Scene Description Table
const QtsTableId QTS_TID_MP4ODT        = 0x05; //!< MPEG-4 Object Descriptor Table
const QtsTableId QTS_TID_MDT           = 0x06; //!< MetaData Table
const QtsTableId QTS_TID_DSMCC_MPE     = 0x3A; //!< DSM-CC Multi-Protocol Encapsulated data
const QtsTableId QTS_TID_DSMCC_UNM     = 0x3B; //!< DSM-CC User-to-Network Messages
const QtsTableId QTS_TID_DSMCC_DDM     = 0x3C; //!< DSM-CC Download Data Messages
const QtsTableId QTS_TID_DSMCC_SD      = 0x3D; //!< DSM-CC Stream Descriptors
const QtsTableId QTS_TID_DSMCC_PD      = 0x3E; //!< DSM-CC Private Data
const QtsTableId QTS_TID_NULL          = 0xFF; //!< Reserved; end of TS packet PSI payload
//
// Valid in DVB context:
//
const QtsTableId QTS_TID_NIT_ACT       = 0x40; //!< Network Information Table - Actual netw
const QtsTableId QTS_TID_NIT_OTH       = 0x41; //!< Network Information Table - Other netw
const QtsTableId QTS_TID_SDT_ACT       = 0x42; //!< Service Description Table - Actual TS
const QtsTableId QTS_TID_SDT_OTH       = 0x46; //!< Service Description Table - Other TS
const QtsTableId QTS_TID_BAT           = 0x4A; //!< Bouquet Association Table
const QtsTableId QTS_TID_EIT_PF_ACT    = 0x4E; //!< EIT present/following - Actual network
const QtsTableId QTS_TID_EIT_PF_OTH    = 0x4F; //!< EIT present/following - Other network
const QtsTableId QTS_TID_EIT_S_ACT_MIN = 0x50; //!< EIT schedule - Actual network
const QtsTableId QTS_TID_EIT_S_ACT_MAX = 0x5F; //!< EIT schedule - Actual network
const QtsTableId QTS_TID_EIT_S_OTH_MIN = 0x60; //!< EIT schedule - Other network
const QtsTableId QTS_TID_EIT_S_OTH_MAX = 0x6F; //!< EIT schedule - Other network
const QtsTableId QTS_TID_TDT           = 0x70; //!< Time & Date Table
const QtsTableId QTS_TID_RST           = 0x71; //!< Running Status Table
const QtsTableId QTS_TID_ST            = 0x72; //!< Stuffing Table
const QtsTableId QTS_TID_TOT           = 0x73; //!< Time Offset Table
const QtsTableId QTS_TID_RNT           = 0x74; //!< Resolution Notification T. (TV-Anytime)
const QtsTableId QTS_TID_CT            = 0x75; //!< Container Table (TV-Anytime)
const QtsTableId QTS_TID_RCT           = 0x76; //!< Related Content Table (TV-Anytime)
const QtsTableId QTS_TID_CIT           = 0x77; //!< Content Identifier Table (TV-Anytime)
const QtsTableId QTS_TID_MPE_FEC       = 0x78; //!< MPE-FEC Table (Data Broadcasting)
const QtsTableId QTS_TID_DIT           = 0x7E; //!< Discontinuity Information Table
const QtsTableId QTS_TID_SIT           = 0x7F; //!< Selection Information Table
const QtsTableId QTS_TID_ECM_80        = 0x80; //!< ECM
const QtsTableId QTS_TID_ECM_81        = 0x81; //!< ECM
const QtsTableId QTS_TID_EMM_FIRST     = 0x82; //!< Start of EMM range
const QtsTableId QTS_TID_EMM_LAST      = 0x8F; //!< End of EMM range
//
// Ranges by type
//
const QtsTableId QTS_TID_EIT_MIN       = 0x4E; //!< EIT; first TID
const QtsTableId QTS_TID_EIT_MAX       = 0x6F; //!< EIT; last TID
const QtsTableId QTS_TID_CAS_FIRST     = 0x80; //!< Start of CAS range
const QtsTableId QTS_TID_CAS_LAST      = 0x8F; //!< End of CAS range


//---------------------------------------------------------------------
// PSI/SI descriptor tag.
//---------------------------------------------------------------------

//!
//! PSI/SI descriptor tag.
//!
typedef quint8 QtsDescTag;

//
// Valid in all MPEG contexts:
//
const QtsDescTag QTS_DID_VIDEO               = 0x02; //!< video_stream_descriptor
const QtsDescTag QTS_DID_AUDIO               = 0x03; //!< audio_stream_descriptor
const QtsDescTag QTS_DID_HIERARCHY           = 0x04; //!< hierarchy_descriptor
const QtsDescTag QTS_DID_REGISTRATION        = 0x05; //!< registration_descriptor
const QtsDescTag QTS_DID_DATA_ALIGN          = 0x06; //!< data_stream_alignment_descriptor
const QtsDescTag QTS_DID_TGT_BG_GRID         = 0x07; //!< target_background_grid_descriptor
const QtsDescTag QTS_DID_VIDEO_WIN           = 0x08; //!< video_window_descriptor
const QtsDescTag QTS_DID_CA                  = 0x09; //!< CA_descriptor
const QtsDescTag QTS_DID_LANGUAGE            = 0x0A; //!< ISO_639_language_descriptor
const QtsDescTag QTS_DID_SYS_CLOCK           = 0x0B; //!< system_clock_descriptor
const QtsDescTag QTS_DID_MUX_BUF_USE         = 0x0C; //!< multiplex_buffer_utilization_desc
const QtsDescTag QTS_DID_COPYRIGHT           = 0x0D; //!< copyright_descriptor
const QtsDescTag QTS_DID_MAX_BITRATE         = 0x0E; //!< maximum bitrate descriptor
const QtsDescTag QTS_DID_PRIV_DATA_IND       = 0x0F; //!< private data indicator descriptor
const QtsDescTag QTS_DID_SMOOTH_BUF          = 0x10; //!< smoothing buffer descriptor
const QtsDescTag QTS_DID_STD                 = 0x11; //!< STD_descriptor
const QtsDescTag QTS_DID_IBP                 = 0x12; //!< IBP descriptor
                                                     // Values 0x13 to 0x1A are defined by DSM-CC
const QtsDescTag QTS_DID_MPEG4_VIDEO         = 0x1B; //!< MPEG-4_video_descriptor
const QtsDescTag QTS_DID_MPEG4_AUDIO         = 0x1C; //!< MPEG-4_audio_descriptor
const QtsDescTag QTS_DID_IOD                 = 0x1D; //!< IOD_descriptor
const QtsDescTag QTS_DID_SL                  = 0x1E; //!< SL_descriptor
const QtsDescTag QTS_DID_FMC                 = 0x1F; //!< FMC_descriptor
const QtsDescTag QTS_DID_EXT_ES_ID           = 0x20; //!< External_ES_id_descriptor
const QtsDescTag QTS_DID_MUXCODE             = 0x21; //!< MuxCode_descriptor
const QtsDescTag QTS_DID_FMX_BUFFER_SIZE     = 0x22; //!< FmxBufferSize_descriptor
const QtsDescTag QTS_DID_MUX_BUFFER          = 0x23; //!< MultiplexBuffer_descriptor
const QtsDescTag QTS_DID_CONTENT_LABELING    = 0x24; //!< Content_labeling_descriptor
const QtsDescTag QTS_DID_METADATA_ASSOC      = 0x25; //!< Metadata_association_descriptor
const QtsDescTag QTS_DID_METADATA            = 0x26; //!< Metadata_descriptor
const QtsDescTag QTS_DID_METADATA_STD        = 0x27; //!< Metadata_STD_descriptor
const QtsDescTag QTS_DID_AVC_VIDEO           = 0x28; //!< AVC_video_descriptor
const QtsDescTag QTS_DID_MPEG2_IPMP          = 0x29; //!< MPEG-2_IPMP_descriptor
const QtsDescTag QTS_DID_AVC_TIMING_HRD      = 0x2A; //!< AVC_timing_and_HRD_descriptor
//
// Valid in DVB context:
//
const QtsDescTag QTS_DID_NETWORK_NAME        = 0x40; //!< network_name_descriptor
const QtsDescTag QTS_DID_SERVICE_LIST        = 0x41; //!< service_list_descriptor
const QtsDescTag QTS_DID_STUFFING            = 0x42; //!< stuffing_descriptor
const QtsDescTag QTS_DID_SAT_DELIVERY        = 0x43; //!< satellite_delivery_system_desc
const QtsDescTag QTS_DID_CABLE_DELIVERY      = 0x44; //!< cable_delivery_system_descriptor
const QtsDescTag QTS_DID_VBI_DATA            = 0x45; //!< VBI_data_descriptor
const QtsDescTag QTS_DID_VBI_TELETEXT        = 0x46; //!< VBI_teletext_descriptor
const QtsDescTag QTS_DID_BOUQUET_NAME        = 0x47; //!< bouquet_name_descriptor
const QtsDescTag QTS_DID_SERVICE             = 0x48; //!< service_descriptor
const QtsDescTag QTS_DID_COUNTRY_AVAIL       = 0x49; //!< country_availability_descriptor
const QtsDescTag QTS_DID_LINKAGE             = 0x4A; //!< linkage_descriptor
const QtsDescTag QTS_DID_NVOD_REFERENCE      = 0x4B; //!< NVOD_reference_descriptor
const QtsDescTag QTS_DID_TIME_SHIFT_SERVICE  = 0x4C; //!< time_shifted_service_descriptor
const QtsDescTag QTS_DID_SHORT_EVENT         = 0x4D; //!< short_event_descriptor
const QtsDescTag QTS_DID_EXTENDED_EVENT      = 0x4E; //!< extended_event_descriptor
const QtsDescTag QTS_DID_TIME_SHIFT_EVENT    = 0x4F; //!< time_shifted_event_descriptor
const QtsDescTag QTS_DID_COMPONENT           = 0x50; //!< component_descriptor
const QtsDescTag QTS_DID_MOSAIC              = 0x51; //!< mosaic_descriptor
const QtsDescTag QTS_DID_STREAM_ID           = 0x52; //!< stream_identifier_descriptor
const QtsDescTag QTS_DID_CA_ID               = 0x53; //!< CA_identifier_descriptor
const QtsDescTag QTS_DID_CONTENT             = 0x54; //!< content_descriptor
const QtsDescTag QTS_DID_PARENTAL_RATING     = 0x55; //!< parental_rating_descriptor
const QtsDescTag QTS_DID_TELETEXT            = 0x56; //!< teletext_descriptor
const QtsDescTag QTS_DID_TELEPHONE           = 0x57; //!< telephone_descriptor
const QtsDescTag QTS_DID_LOCAL_TIME_OFFSET   = 0x58; //!< local_time_offset_descriptor
const QtsDescTag QTS_DID_SUBTITLING          = 0x59; //!< subtitling_descriptor
const QtsDescTag QTS_DID_TERREST_DELIVERY    = 0x5A; //!< terrestrial_delivery_system_desc
const QtsDescTag QTS_DID_MLINGUAL_NETWORK    = 0x5B; //!< multilingual_network_name_desc
const QtsDescTag QTS_DID_MLINGUAL_BOUQUET    = 0x5C; //!< multilingual_bouquet_name_desc
const QtsDescTag QTS_DID_MLINGUAL_SERVICE    = 0x5D; //!< multilingual_service_name_desc
const QtsDescTag QTS_DID_MLINGUAL_COMPONENT  = 0x5E; //!< multilingual_component_descriptor
const QtsDescTag QTS_DID_PRIV_DATA_SPECIF    = 0x5F; //!< private_data_specifier_descriptor
const QtsDescTag QTS_DID_SERVICE_MOVE        = 0x60; //!< service_move_descriptor
const QtsDescTag QTS_DID_SHORT_SMOOTH_BUF    = 0x61; //!< short_smoothing_buffer_descriptor
const QtsDescTag QTS_DID_FREQUENCY_LIST      = 0x62; //!< frequency_list_descriptor
const QtsDescTag QTS_DID_PARTIAL_TS          = 0x63; //!< partial_transport_stream_desc
const QtsDescTag QTS_DID_DATA_BROADCAST      = 0x64; //!< data_broadcast_descriptor
const QtsDescTag QTS_DID_SCRAMBLING          = 0x65; //!< scrambling_descriptor
const QtsDescTag QTS_DID_DATA_BROADCAST_ID   = 0x66; //!< data_broadcast_id_descriptor
const QtsDescTag QTS_DID_TRANSPORT_STREAM    = 0x67; //!< transport_stream_descriptor
const QtsDescTag QTS_DID_DSNG                = 0x68; //!< DSNG_descriptor
const QtsDescTag QTS_DID_PDC                 = 0x69; //!< PDC_descriptor
const QtsDescTag QTS_DID_AC3                 = 0x6A; //!< AC-3_descriptor
const QtsDescTag QTS_DID_ANCILLARY_DATA      = 0x6B; //!< ancillary_data_descriptor
const QtsDescTag QTS_DID_CELL_LIST           = 0x6C; //!< cell_list_descriptor
const QtsDescTag QTS_DID_CELL_FREQ_LINK      = 0x6D; //!< cell_frequency_link_descriptor
const QtsDescTag QTS_DID_ANNOUNCE_SUPPORT    = 0x6E; //!< announcement_support_descriptor
const QtsDescTag QTS_DID_APPLI_SIGNALLING    = 0x6F; //!< application_signalling_descriptor
const QtsDescTag QTS_DID_ADAPTFIELD_DATA     = 0x70; //!< adaptation_field_data_descriptor
const QtsDescTag QTS_DID_SERVICE_ID          = 0x71; //!< service_identifier_descriptor
const QtsDescTag QTS_DID_SERVICE_AVAIL       = 0x72; //!< service_availability_descriptor
const QtsDescTag QTS_DID_DEFAULT_AUTHORITY   = 0x73; //!< default_authority_descriptor
const QtsDescTag QTS_DID_RELATED_CONTENT     = 0x74; //!< related_content_descriptor
const QtsDescTag QTS_DID_TVA_ID              = 0x75; //!< TVA_id_descriptor
const QtsDescTag QTS_DID_CONTENT_ID          = 0x76; //!< content_identifier_descriptor
const QtsDescTag QTS_DID_TIME_SLICE_FEC_ID   = 0x77; //!< time_slice_fec_identifier_desc
const QtsDescTag QTS_DID_ECM_REPETITION_RATE = 0x78; //!< ECM_repetition_rate_descriptor
const QtsDescTag QTS_DID_S2_SAT_DELIVERY     = 0x79; //!< S2_satellite_delivery_system_descriptor
const QtsDescTag QTS_DID_ENHANCED_AC3        = 0x7A; //!< enhanced_AC-3_descriptor
const QtsDescTag QTS_DID_DTS                 = 0x7B; //!< DTS_descriptor
const QtsDescTag QTS_DID_AAC                 = 0x7C; //!< AAC_descriptor
const QtsDescTag QTS_DID_XAIT_LOCATION       = 0x7D; //!< XAIT_location_descriptor (DVB-MHP)
const QtsDescTag QTS_DID_FTA_CONTENT_MGMT    = 0x7E; //!< FTA_content_management_descriptor
const QtsDescTag QTS_DID_EXTENSION           = 0x7F; //!< extension_descriptor
//
// Valid in ATSC context:
//
const QtsDescTag QTS_DID_ATSC_STUFFING       = 0X80; //!< stuffing_descriptor
const QtsDescTag QTS_DID_AC3_AUDIO_STREAM    = 0x81; //!< ac3_audio_stream_descriptor
const QtsDescTag QTS_DID_ATSC_PID            = 0x85; //!< program_identifier_descriptor
const QtsDescTag QTS_DID_CAPTION             = 0x86; //!< caption_service_descriptor
const QtsDescTag QTS_DID_CONTENT_ADVIS       = 0x87; //!< content_advisory_descriptor
const QtsDescTag QTS_DID_EXT_CHAN_NAME       = 0xA0; //!< extended_channel_name_descriptor
const QtsDescTag QTS_DID_SERV_LOCATION       = 0xA1; //!< service_location_descriptor
const QtsDescTag QTS_DID_ATSC_TIME_SHIFT     = 0xA2; //!< time_shifted_event_descriptor
const QtsDescTag QTS_DID_COMPONENT_NAME      = 0xA3; //!< component_name_descriptor
const QtsDescTag QTS_DID_ATSC_DATA_BRDCST    = 0xA4; //!< data_broadcast_descriptor
const QtsDescTag QTS_DID_PID_COUNT           = 0xA5; //!< pid_count_descriptor
const QtsDescTag QTS_DID_DOWNLOAD            = 0xA6; //!< download_descriptor
const QtsDescTag QTS_DID_MPROTO_ENCAPS       = 0xA7; //!< multiprotocol_encapsulation_desc
//
// Valid after PDS_EUTELSAT private_data_specifier
//
const QtsDescTag QTS_DID_EUTELSAT_CHAN_NUM   = 0x83; //!< eutelsat_channel_number_descriptor
//
// Valid after PDS_EACEM/EICTA private_data_specifier
//
const QtsDescTag QTS_DID_LOGICAL_CHANNEL_NUM = 0x83; //!< logical_channel_number_descriptor
const QtsDescTag QTS_DID_PREF_NAME_LIST      = 0x84; //!< preferred_name_list_descriptor
const QtsDescTag QTS_DID_PREF_NAME_ID        = 0x85; //!< preferred_name_identifier_descriptor
const QtsDescTag QTS_DID_EACEM_STREAM_ID     = 0x86; //!< eacem_stream_identifier_descriptor
const QtsDescTag QTS_DID_HD_SIMULCAST_LCN    = 0x88; //!< HD_simulcast_logical_channel_number_descriptor

//!
//! Extended descriptor tag (in extension_descriptor)
//!
typedef quint8 QtsExtDescTag;

const QtsExtDescTag QTS_EDID_IMAGE_ICON         = 0x00; //!< image_icon_descriptor
const QtsExtDescTag QTS_EDID_CPCM_DELIVERY_SIG  = 0x01; //!< cpcm_delivery_signalling_descriptor
const QtsExtDescTag QTS_EDID_CP                 = 0x02; //!< CP_descriptor
const QtsExtDescTag QTS_EDID_CP_IDENTIFIER      = 0x03; //!< CP_identifier_descriptor
const QtsExtDescTag QTS_EDID_T2_DELIVERY        = 0x04; //!< T2_delivery_system_descriptor
const QtsExtDescTag QTS_EDID_SH_DELIVERY        = 0x05; //!< SH_delivery_system_descriptor
const QtsExtDescTag QTS_EDID_SUPPL_AUDIO        = 0x06; //!< supplementary_audio_descriptor
const QtsExtDescTag QTS_EDID_NETW_CHANGE_NOTIFY = 0x07; //!< network_change_notify_descriptor
const QtsExtDescTag QTS_EDID_MESSAGE            = 0x08; //!< message_descriptor
const QtsExtDescTag QTS_EDID_TARGET_REGION      = 0x09; //!< target_region_descriptor
const QtsExtDescTag QTS_EDID_TARGET_REGION_NAME = 0x0A; //!< target_region_name_descriptor
const QtsExtDescTag QTS_EDID_SERVICE_RELOCATED  = 0x0B; //!< service_relocated_descriptor


//---------------------------------------------------------------------
// Private data specifier (associated with QtsDescTag).
//---------------------------------------------------------------------

//!
//! Private data specifier (associated with QtsDescTag).
//!
typedef quint32 QtsPrivDataSpec;

const QtsPrivDataSpec QTS_PDS_NAGRA     = 0x00000009;
const QtsPrivDataSpec QTS_PDS_NAGRA_2   = 0x0000000A;
const QtsPrivDataSpec QTS_PDS_NAGRA_3   = 0x0000000B;
const QtsPrivDataSpec QTS_PDS_NAGRA_4   = 0x0000000C;
const QtsPrivDataSpec QTS_PDS_NAGRA_5   = 0x0000000D;
const QtsPrivDataSpec QTS_PDS_TPS       = 0x00000010;
const QtsPrivDataSpec QTS_PDS_EACEM     = 0x00000028;
const QtsPrivDataSpec QTS_PDS_EICTA     = QTS_PDS_EACEM;
const QtsPrivDataSpec QTS_PDS_LOGIWAYS  = 0x000000A2;
const QtsPrivDataSpec QTS_PDS_CANALPLUS = 0x000000C0;
const QtsPrivDataSpec QTS_PDS_EUTELSAT  = 0x0000055F;


//---------------------------------------------------------------------
// Linkage type values (in linkage_descriptor)
//---------------------------------------------------------------------

const int QTS_LINKAGE_INFO            = 0x01; //!< Information service
const int QTS_LINKAGE_EPG             = 0x02; //!< EPG service
const int QTS_LINKAGE_CA_REPLACE      = 0x03; //!< CA replacement service
const int QTS_LINKAGE_TS_NIT_BAT      = 0x04; //!< TS containing complet network/bouquet SI
const int QTS_LINKAGE_SERVICE_REPLACE = 0x05; //!< Service replacement service
const int QTS_LINKAGE_DATA_BROADCAST  = 0x06; //!< Data broadcast service
const int QTS_LINKAGE_RCS_MAP         = 0x07; //!< RCS map
const int QTS_LINKAGE_HAND_OVER       = 0x08; //!< Mobile hand-over
const int QTS_LINKAGE_SSU             = 0x09; //!< System software update service
const int QTS_LINKAGE_SSU_TABLE       = 0x0A; //!< TS containing SSU BAT or NIT
const int QTS_LINKAGE_IP_NOTIFY       = 0x0B; //!< IP/MAC notification service
const int QTS_LINKAGE_INT_BAT_NIT     = 0x0C; //!< TS containing INT BAT or NIT


//---------------------------------------------------------------------
// Teletext type values (in teletext_descriptor).
//---------------------------------------------------------------------

const int QTS_TELETEXT_INITIAL      = 0x01; //!< Initial Teletext page.
const int QTS_TELETEXT_SUBTITLES    = 0x02; //!< Teletext subtitles.
const int QTS_TELETEXT_ADD_INFO     = 0x03; //!< Additional information page.
const int QTS_TELETEXT_SCHEDULE     = 0x04; //!< Programme schedule page.
const int QTS_TELETEXT_SUBTITLES_HI = 0x05; //!< Teletext subtitles for hearing impaired.


//---------------------------------------------------------------------
// Data broadcast id values (in data_broadcast[_id]_descriptor)
//---------------------------------------------------------------------

const int QTS_DBID_DATA_PIPE            = 0x0001; //!< Data pipe
const int QTS_DBID_ASYNC_DATA_STREAM    = 0x0002; //!< Asynchronous data stream
const int QTS_DBID_SYNC_DATA_STREAM     = 0x0003; //!< Synchronous data stream
const int QTS_DBID_SYNCED_DATA_STREAM   = 0x0004; //!< Synchronised data stream
const int QTS_DBID_MPE                  = 0x0005; //!< Multi protocol encapsulation
const int QTS_DBID_DATA_CSL             = 0x0006; //!< Data Carousel
const int QTS_DBID_OBJECT_CSL           = 0x0007; //!< Object Carousel
const int QTS_DBID_ATM                  = 0x0008; //!< DVB ATM streams
const int QTS_DBID_HP_ASYNC_DATA_STREAM = 0x0009; //!< Higher Protocols based on asynchronous data streams
const int QTS_DBID_SSU                  = 0x000A; //!< System Software Update service [TS 102 006]
const int QTS_DBID_IPMAC_NOTIFICATION   = 0x000B; //!< IP/MAC Notification service [EN 301 192]
const int QTS_DBID_MHP_OBJECT_CSL       = 0x00F0; //!< MHP Object Carousel
const int QTS_DBID_MHP_MPE              = 0x00F1; //!< Reserved for MHP Multi Protocol Encapsulation
const int QTS_DBID_EUTELSAT_DATA_PIPE   = 0x0100; //!< Eutelsat Data Piping
const int QTS_DBID_EUTELSAT_DATA_STREAM = 0x0101; //!< Eutelsat Data Streaming
const int QTS_DBID_SAGEM_IP             = 0x0102; //!< SAGEM IP encapsulation in MPEG-2 PES packets
const int QTS_DBID_BARCO_DATA_BRD       = 0x0103; //!< BARCO Data Broadcasting
const int QTS_DBID_CIBERCITY_MPE        = 0x0104; //!< CyberCity Multiprotocol Encapsulation
const int QTS_DBID_CYBERSAT_MPE         = 0x0105; //!< CyberSat Multiprotocol Encapsulation
const int QTS_DBID_TDN                  = 0x0106; //!< The Digital Network
const int QTS_DBID_OPENTV_DATA_CSL      = 0x0107; //!< OpenTV Data Carousel
const int QTS_DBID_PANASONIC            = 0x0108; //!< Panasonic
const int QTS_DBID_KABEL_DEUTSCHLAND    = 0x0109; //!< Kabel Deutschland
const int QTS_DBID_TECHNOTREND          = 0x010A; //!< TechnoTrend Gorler GmbH
const int QTS_DBID_MEDIAHIGHWAY_SSU     = 0x010B; //!< NDS France Technologies system software download
const int QTS_DBID_GUIDE_PLUS           = 0x010C; //!< GUIDE Plus+ Rovi Corporation
const int QTS_DBID_ACAP_OBJECT_CSL      = 0x010D; //!< ACAP Object Carousel
const int QTS_DBID_MICRONAS             = 0x010E; //!< Micronas Download Stream
const int QTS_DBID_POLSAT               = 0x0110; //!< Televizja Polsat
const int QTS_DBID_DTG                  = 0x0111; //!< UK DTG
const int QTS_DBID_SKYMEDIA             = 0x0112; //!< SkyMedia
const int QTS_DBID_INTELLIBYTE          = 0x0113; //!< Intellibyte DataBroadcasting
const int QTS_DBID_TELEWEB_DATA_CSL     = 0x0114; //!< TeleWeb Data Carousel
const int QTS_DBID_TELEWEB_OBJECT_CSL   = 0x0115; //!< TeleWeb Object Carousel
const int QTS_DBID_TELEWEB              = 0x0116; //!< TeleWeb
const int QTS_DBID_BBC                  = 0x0117; //!< BBC
const int QTS_DBID_ELECTRA              = 0x0118; //!< Electra Entertainment Ltd
const int QTS_DBID_BBC_2_3              = 0x011A; //!< BBC 2 - 3
const int QTS_DBID_TELETEXT             = 0x011B; //!< Teletext
const int QTS_DBID_SKY_DOWNLOAD_1_5     = 0x0120; //!< Sky Download Streams 1-5
const int QTS_DBID_ICO                  = 0x0121; //!< ICO mim
const int QTS_DBID_CIPLUS_DATA_CSL      = 0x0122; //!< CI+ Data Carousel
const int QTS_DBID_HBBTV                = 0x0123; //!< HBBTV Carousel
const int QTS_DBID_ROVI_PREMIUM         = 0x0124; //!< Premium Content from Rovi Corporation
const int QTS_DBID_MEDIA_GUIDE          = 0x0125; //!< Media Guide from Rovi Corporation
const int QTS_DBID_INVIEW               = 0x0126; //!< InView Technology Ltd
const int QTS_DBID_BOTECH               = 0x0130; //!< Botech Elektronik SAN. ve TIC. LTD.STI.
const int QTS_DBID_SCILLA_PUSHVOD_CSL   = 0x0131; //!< Scilla Push-VOD Carousel
const int QTS_DBID_CANAL_PLUS           = 0x0140; //!< Canal+
const int QTS_DBID_OIPF_OBJECT_CSL      = 0x0150; //!< OIPF Object Carousel - Open IPTV Forum
const int QTS_DBID_4TV                  = 0x4444; //!< 4TV Data Broadcast
const int QTS_DBID_NOKIA_IP_SSU         = 0x4E4F; //!< Nokia IP based software delivery
const int QTS_DBID_BBG_DATA_CSL         = 0xBBB1; //!< BBG Data Caroussel
const int QTS_DBID_BBG_OBJECT_CSL       = 0xBBB2; //!< BBG Object Caroussel
const int QTS_DBID_BBG                  = 0xBBBB; //!< Bertelsmann Broadband Group


//---------------------------------------------------------------------
// Teletext PES packets.
// See ETSI EN 300 472 V1.3.1, "DVB; Specification for conveying ITU-R
// System B Teletext in DVB bitstreams"
//---------------------------------------------------------------------

const int QTS_TELETEXT_PES_FIRST_EBU_DATA_ID = 0x10; //!< First EBU data_identifier value in PES packets conveying Teletext.
const int QTS_TELETEXT_PES_LAST_EBU_DATA_ID  = 0x1F; //!< Last EBU data_identifier value in PES packets conveying Teletext.

const int QTS_TELETEXT_DATA_UNIT_ID_NON_SUBTITLE    = 0x02; //!< Data_unit_id for EBU Teletext non-subtitle data.
const int QTS_TELETEXT_DATA_UNIT_ID_SUBTITLE        = 0x03; //!< Data_unit_id for EBU Teletext subtitle data.
const int QTS_TELETEXT_DATA_UNIT_ID_INVERTED        = 0x0C; //!< Data_unit_id for EBU EBU Teletext Inverted (extension ?).
const int QTS_TELETEXT_DATA_UNIT_ID_VPS             = 0xC3; //!< Data_unit_id for VPS (extension ?).
const int QTS_TELETEXT_DATA_UNIT_ID_CLOSED_CAPTIONS = 0xC5; //!< Data_unit_id for Closed Caption (extension ?).
const int QTS_TELETEXT_DATA_UNIT_ID_STUFFING        = 0xFF; //!< Data_unit_id for stuffing data.

const int QTS_TELETEXT_PACKET_SIZE = 44; //!< Size of a Teletext packet.

#endif //!< QTSCORE_H
