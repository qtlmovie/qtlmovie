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
//! @file QtlMovieFFmpeg.h
//!
//! Declare some FFmpeg utilities.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEFFMPEG_H
#define QTLMOVIEFFMPEG_H

#include "QtlByteBlock.h"
#include "QtlMovie.h"
#include "QtlMediaStreamInfo.h"
#include "QtlMovieSettings.h"
#include "QtlMovieInputFile.h"

//!
//! Namespace for ffmpeg utilities.
//!
namespace QtlMovieFFmpeg
{
    //!
    //! Build FFmpeg command line arguments for the initial probe size and duration.
    //! @param [in] settings Application settings.
    //! @param [in] probeTimeDivisor If positive, this value is used to reduce the
    //! probe time duration. The actual probe time is the value from @a settings,
    //! divided by @a probeTimeDivisor.
    //! @return FFmpeg command line arguments.
    //!
    QStringList probeArguments(const QtlMovieSettings* settings, int probeTimeDivisor = 0);

    //!
    //! Build FFmpeg command line arguments for an input file.
    //! @param [in] settings Application settings.
    //! @param [in] file Input file description. If omitted, do not specify an input file name.
    //! @return FFmpeg command line arguments.
    //!
    QStringList inputArguments(const QtlMovieSettings* settings, const QtlMovieInputFile* file = 0);

    //!
    //! Build FFmpeg command line arguments for output file.
    //! @param [in] settings Application settings.
    //! @param [in] fileName Output file name.
    //! @param [in] fileFormat If non empty, specifies the FFmpeg format of the output file.
    //! @return FFmpeg command line arguments.
    //!
    QStringList outputArguments(const QtlMovieSettings* settings,
                                const QString& fileName,
                                const QString& fileFormat = QString());

    //!
    //! Build an audio conversion FFmpeg options list for DVD transcoding.
    //! @param [in] settings Application settings.
    //! @param [in] audioStream Audio stream description. Can be null.
    //! @return FFmpeg options, possibly empty.
    //!
    QStringList dvdAudioOptions(const QtlMovieSettings* settings, const QtlMediaStreamInfoPtr& audioStream);

    //!
    //! Format the argument of an ffmpeg filter using proper escaping.
    //! @param [in] argument The argument plain text.
    //! @return The corresponding string to add in the filter description.
    //!
    QString escapedFilterArgument(const QString& argument);

    //!
    //! Build FFmpeg command line arguments to insert a video filter string.
    //! @param [in] videoFilters A video filter string.
    //! @return FFmpeg command line arguments. Can be empty if no video filter is specified.
    //!
    QStringList videoFilterOptions(const QString& videoFilters);

    //!
    //! Compute an FFmpeg video filter for a target size and aspect ratio.
    //! @param [in,out] videoFilters A video filter string which is updated.
    //! @param [in] widthIn Input video width in pixels.
    //! @param [in] heightIn Input video height in pixels.
    //! @param [in] darIn Input display aspect ratio.
    //! @param [in] widthOut Target video width in pixels.
    //! @param [in] heightOut Target video height in pixels.
    //! @param [in] darOut Target display aspect ratio.
    //!
    void addResizeVideoFilter(QString& videoFilters,
                              int widthIn,
                              int heightIn,
                              float darIn,
                              int widthOut,
                              int heightOut,
                              float darOut);

    //!
    //! Build a video resize FFmpeg options list to bind the video within a maximum size.
    //! Always resize if the input pixel aspect ratio does not match the output one.
    //! @param [in,out] ffmpegArguments A list of FFmpeg arguments which is updated.
    //! @param [in,out] videoFilters A video filter string which is updated.
    //! @param [in] widthIn Input video width in pixels.
    //! @param [in] heightIn Input video height in pixels.
    //! @param [in] darIn Input display aspect ratio.
    //! @param [in] maxWidthOut Maximum target video width in pixels.
    //! @param [in] maxHeightOut Maximum target video height in pixels.
    //! @param [in] parOut Target pixel aspect ratio.
    //! @param [out] widthOut Returned target video width in pixels.
    //! @param [out] heightOut Returned target video height in pixels.
    //!
    void addBoundedSizeOptions(QStringList& ffmpegArguments,
                               QString& videoFilters,
                               int widthIn,
                               int heightIn,
                               float darIn,
                               int maxWidthOut,
                               int maxHeightOut,
                               float parOut,
                               int& widthOut,
                               int& heightOut);

    //!
    //! Build a video rotation FFmpeg options list .
    //! @param [in] settings Application settings.
    //! @param [in] videoStream Video stream description.
    //! @param [in,out] ffmpegArguments A list of FFmpeg arguments which is updated.
    //! @param [in,out] videoFilters A video filter string which is updated.
    //! @param [out] width New video width in pixels after rotation.
    //! @param [out] height New video height in pixels after rotation.
    //! @param [out] dar New display aspect ratio after rotation.
    //!
    void addRotateOptions(const QtlMovieSettings* settings,
                          const QtlMediaStreamInfoPtr& videoStream,
                          QStringList& ffmpegArguments,
                          QString& videoFilters,
                          int& width,
                          int& height,
                          float& dar);
}

#endif // QTLMOVIEFFMPEG_H
