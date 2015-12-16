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
// Default values for general options.
//
#define QTL_MAX_LOG_LINE                    1000  //!< Maximum lines in the log window.
#define QTL_DEFAULT_OUTDIR_INPUT            true  //!< Default output directory is input directory.
#define QTL_TRANSCODE_COMPLETE              true  //!< Transcode complete video.
#define QTL_TRANSCODE_SECONDS                  0  //!< Number of seconds to transcode if not complete video.
#define QTL_KEEP_INTERMEDIATE_FILES        false  //!< Keep intermediate transcoding files.
#define QTL_FFMPEG_PROBE_SECONDS             200  //!< Initial probe size in media playout seconds for ffprobe / ffmpeg.
#define QTL_SRT_USE_VIDEO_SIZE_HINT         true  //!< Check if the insertion of SRT/SSA/ASS subtitles shall use the original video size as a hint.
#define QTL_CHAPTER_MINUTES                    5  //!< Duration of chapters to create.
#define QTL_DVD_REMUX_AFTER_TRANSCODE       true  //!< If audio/video must be remuxed after transcode for DVD output.
#define QTL_CREATE_PAL_DVD                  true  //!< If the format of created DVD is PAL or NTSC.
#define QTL_IPAD_SCREEN_SIZE          Ipad12Size  //!< Default iPad screen size.
#define QTL_IPHONE_SCREEN_SIZE       Iphone4Size  //!< Default iPhone screen size.
#define QTL_FORCE_DVD_TRANSCODE            false  //!< Force transcoding even if input file is already DVD-compliant.
#define QTL_NEW_VERSION_CHECK               true  //!< Check for a new version of QtlMovie on startup.
#define QTL_AUTO_ROTATE_VIDEO               true  //!< If automatic rotation of video shall be applied.
#define QTL_PLAY_SOUND_ON_COMPLETION       false  //!< If a sound shall be played on transcoding completion.
#define QTL_DEFAULT_LANGUAGES "fr,fre,fra,french" //!< Default audience is French-speaking.
#define QTL_CLEAR_LOG_BEFORE_TRANSCODE     false  //!< Clear the log panel before starting a transcode operation.
#define QTL_SAVE_LOG_AFTER_TRANSCODE       false  //!< Automatically save the log after transcoding completion.
#define QTL_LOG_FILE_EXTENSION            ".log"  //!< Default extension for log files.
#define QTL_FFPROBE_EXECUTION_TIMEOUT         40  //!< FFprobe execution timeout in seconds.
#define QTL_USE_BATCH_MODE                 false  //!< Do not use batch mode, use single-file mode.
#define QTL_EDIT_TASK_WIDTH                  680  //!< Width (in pixels) of QtlMovieEditTaskDialog
#define QTL_EDIT_TASK_HEIGHT                 500  //!< Height (in pixels) of QtlMovieEditTaskDialog
#define QTL_SELECT_ORIGINAL_AUDIO           true  //!< Automatically select original audio track.
#define QTL_SELECT_TARGET_SUBTITLES         true  //!< Automatically select subtitles for the target language.

#define QTL_DAR_4_3      (float(4.0)/float(3.0))  //!< Display aspect ratio value for 4:3.
#define QTL_DAR_16_9    (float(16.0)/float(9.0))  //!< Display aspect ratio value for 16:9.

//
// Transcoding presets.
//
#define QTL_DVD_AUDIO_BITRATE             192000  //!< MPEG-1 Level 2 audio bitrate (bits per second) for DVD.
#define QTL_DVD_AUDIO_SAMPLING             48000  //!< MPEG-1 Level 2 audio sampling rate (Hz) for DVD.
#define QTL_DVD_DEFAULT_VIDEO_BITRATE    4000000  //!< MPEG-2 video default bitrate for DVD.
#define QTL_DVD_PAL_VIDEO_WIDTH              720  //!< Video width (pixels) for PAL DVD.
#define QTL_DVD_PAL_VIDEO_HEIGHT             576  //!< Video height (pixels) for PAL DVD.
#define QTL_DVD_PAL_FRAME_RATE                25  //!< Frames per second for PAL DVD.
#define QTL_DVD_NTSC_VIDEO_WIDTH             720  //!< Video width (pixels) for NTSC DVD.
#define QTL_DVD_NTSC_VIDEO_HEIGHT            480  //!< Video height (pixels) for NTSC DVD.
#define QTL_DVD_NTSC_FRAME_RATE               30  //!< Frames per second for NTSC DVD.
#define QTL_DVD_DAR                 QTL_DAR_16_9  //!< Display aspect ratio for DVD.
#define QTL_DVD_DAR_FFMPEG                "16:9"  //!< Same as QTL_DVD_DAR, used with ffmpeg -aspect option.

#define QTL_IOS_AUDIO_BITRATE             160000  //!< Common AAC audio bitrate (bits per second) for iPad/iPhone.
#define QTL_IOS_AUDIO_SAMPLING             48000  //!< Common AAC audio sampling rate (Hz) for iPad/iPhone.
#define QTL_IOS_FRAME_RATE                    30  //!< Common frames per second for iPad/iPhone.

#define QTL_IPAD_DEFAULT_VIDEO_QUALITY        11  //!< H.264 video default quality indicator for iPad.
#define QTL_IPAD12_VIDEO_WIDTH              1024  //!< Video width (pixels) for iPad 1, 2 & mini.
#define QTL_IPAD12_VIDEO_HEIGHT              768  //!< Video height (pixels) for iPad 1, 2 & mini.
#define QTL_IPAD34_VIDEO_WIDTH              2048  //!< Video width (pixels) for iPad 3 & 4.
#define QTL_IPAD34_VIDEO_HEIGHT             1536  //!< Video height (pixels) for iPad 3 & 4.

#define QTL_IPHONE_DEFAULT_VIDEO_QUALITY      11  //!< H.264 video default quality indicator for iPhone.
#define QTL_IPHONE3_VIDEO_WIDTH              480  //!< Video width (pixels) for iPhone 1/2G/3G/3GS.
#define QTL_IPHONE3_VIDEO_HEIGHT             320  //!< Video height (pixels) for iPhone 1/2G/3G/3GS.
#define QTL_IPHONE4_VIDEO_WIDTH              960  //!< Video width (pixels) for iPhone 4/4S.
#define QTL_IPHONE4_VIDEO_HEIGHT             640  //!< Video height (pixels) for iPhone 4/4S.
#define QTL_IPHONE5_VIDEO_WIDTH             1136  //!< Video width (pixels) for iPhone 5C/5S.
#define QTL_IPHONE5_VIDEO_HEIGHT             640  //!< Video height (pixels) for iPhone 5C/5S.
#define QTL_IPHONE6_VIDEO_WIDTH             1334  //!< Video width (pixels) for iPhone 6.
#define QTL_IPHONE6_VIDEO_HEIGHT             750  //!< Video height (pixels) for iPhone 6.
#define QTL_IPHONE6P_VIDEO_WIDTH            2208  //!< Video width (pixels) for iPhone 6 Plus.
#define QTL_IPHONE6P_VIDEO_HEIGHT           1242  //!< Video height (pixels) for iPhone 6 Plus.

#define QTL_AVI_AUDIO_BITRATE             128000  //!< MP3 audio bitrate (bits per second) for AVI.
#define QTL_AVI_AUDIO_SAMPLING             48000  //!< MP3 audio sampling rate (Hz) for AVI.
#define QTL_AVI_DEFAULT_VIDEO_QUALITY         18  //!< H.263/Xvid/DivX video default quality indicator for AVI.
#define QTL_AVI_DEFAULT_MAX_VIDEO_WIDTH      880  //!< Default max video width (pixels) for AVI.
#define QTL_AVI_DEFAULT_MAX_VIDEO_HEIGHT     352  //!< Default max video height (pixels) for AVI.
#define QTL_AVI_FRAME_RATE                    24  //!< Frames per second for AVI.

#define QTL_AUDIO_NORMALIZE_MODE        Compress  //!< How to normalize audio when the input dynamic range is too large.
#define QTL_DEFAULT_AUDIO_NORMALIZE        false  //!< Do not normalize audio level by default.
#define QTL_DEFAULT_AUDIO_MEAN_LEVEL       (-20)  //!< Normalized mean audio level, in dBFS.
#define QTL_DEFAULT_AUDIO_PEAK_LEVEL        (-1)  //!< Normalized peak audio level, in dBFS.
#define QTL_AUDIO_NORMALIZATION_TOLERANCE    1.0  //!< Do not normalize audio if mean level is that close to target level.

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

//!
//! Name of the "Job Variable" (see @link QtlMovieJob::setVariable() @endlink) for audio filter.
//!
#define QTL_AUDIO_FILTER_VARNAME "AUDIOFILTER"

//!
//! Reference to the "Job Variable" (see @link QtlMovieJob::setVariable() @endlink) for audio filter.
//!
#define QTL_AUDIO_FILTER_VARREF "{" QTL_AUDIO_FILTER_VARNAME "}"

#endif // QTLMOVIE_H
