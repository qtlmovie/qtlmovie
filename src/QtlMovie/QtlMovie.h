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
//! @file QtlMovie.h
//!
//! Declare generic constants.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIE_H
#define QTLMOVIE_H

#include <QtGlobal>

//
// Transcoding presets.
//
#define QTL_DVD_AUDIO_BITRATE           192000  //!< MPEG-1 Level 2 audio bitrate (bits per second) for DVD.
#define QTL_DVD_AUDIO_SAMPLING           48000  //!< MPEG-1 Level 2 audio sampling rate (Hz) for DVD.
#define QTL_DVD_DEFAULT_VIDEO_BITRATE  4000000  //!< MPEG-2 video default bitrate for DVD.
#define QTL_DVD_PAL_VIDEO_WIDTH            720  //!< Video width (pixels) for PAL DVD.
#define QTL_DVD_PAL_VIDEO_HEIGHT           576  //!< Video height (pixels) for PAL DVD.
#define QTL_DVD_PAL_FRAME_RATE              25  //!< Frames per second for PAL DVD.
#define QTL_DVD_NTSC_VIDEO_WIDTH           720  //!< Video width (pixels) for NTSC DVD.
#define QTL_DVD_NTSC_VIDEO_HEIGHT          480  //!< Video height (pixels) for NTSC DVD.
#define QTL_DVD_NTSC_FRAME_RATE             30  //!< Frames per second for NTSC DVD.
#define QTL_DVD_DAR                 (16.0/9.0)  //!< Display aspect ratio for DVD.
#define QTL_DVD_DAR_FFMPEG              "16:9"  //!< Same as QTL_DVD_DAR, used with ffmpeg -aspect option.

#define QTL_IPAD_AUDIO_BITRATE           160000  //!< AAC audio bitrate (bits per second) for iPad.
#define QTL_IPAD_AUDIO_SAMPLING           48000  //!< AAC audio sampling rate (Hz) for iPad.
#define QTL_IPAD_DEFAULT_VIDEO_BITRATE  2500000  //!< H.264 video default bitrate for iPad.
#define QTL_IPAD12_VIDEO_WIDTH             1024  //!< Video width (pixels) for iPad 1, 2 & mini.
#define QTL_IPAD12_VIDEO_HEIGHT             768  //!< Video height (pixels) for iPad 1, 2 & mini.
#define QTL_IPAD34_VIDEO_WIDTH             2048  //!< Video width (pixels) for iPad 3 & 4.
#define QTL_IPAD34_VIDEO_HEIGHT            1536  //!< Video height (pixels) for iPad 3 & 4.
#define QTL_IPAD_FRAME_RATE                  25  //!< Frames per second for iPad.

//!
//! Percentage of DVD ISO image overhead.
//!
//! This is the percentage to add to the audio size + video size to obtain
//! an estimate of the ISO image size. We assume here that the DVD contains
//! only one video title and no menu.
//!
//! This constant is notoriously oversized. Maybe 12% will be more accurate.
//!
#define QTL_DVD_ISO_OVERHEAD_PERCENT 15

//!
//! Maximum size in bytes of a DVD ISO image.
//!
#define QTL_DVD_ISO_MAX_SIZE (Q_UINT64_C(4700000000))

//!
//! Log level for FFmpeg.
//!
//! Possible values for -loglevel option:
//!
//! - quiet   : Show nothing at all; be silent.
//! - panic     Only show fatal errors which could lead the process to crash,
//!             such as and assert failure.
//!             This is not currently used for anything.
//! - fatal   : Only show fatal errors. These are errors after which the process
//!             absolutely cannot continue after.
//! - error   : Show all errors, including ones which can be recovered from.
//! - warning : Show all warnings and errors. Any message related to possibly
//!             incorrect or unexpected events will be shown.
//! - info    : Show informative messages during processing. This is in addition
//!             to warnings and errors. This is the default value.
//! - verbose : Same as info, except more verbose.
//! - debug   : Show everything, including debugging information.
//!
//! By default the program logs to stderr, if coloring is supported by the terminal,
//! colors are used to mark errors and warnings. Log coloring can be disabled setting
//! the environment variable AV_LOG_FORCE_NOCOLOR or NO_COLOR, or can be forced setting
//! the environment variable AV_LOG_FORCE_COLOR. The use of the environment variable
//! NO_COLOR is deprecated and will be dropped in a following FFmpeg version.
//!
#define QTL_FFMPEG_LOGLEVEL "warning"

//!
//! Number of MPEG Transport Stream packets to read at a time when
//! searching for Teletext subtitles.
//!
#define QTL_TS_PACKETS_CHUNK 100

#endif // QTLMOVIE_H
