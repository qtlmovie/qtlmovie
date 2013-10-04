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
//
// Define some FFmpeg utilities.
//
//----------------------------------------------------------------------------

#include "QtlMovieFFmpeg.h"
#include "QtlMovie.h"
#include "QtlStringList.h"


//----------------------------------------------------------------------------
// Build FFmpeg command line arguments for the initial probe size and duration.
//----------------------------------------------------------------------------

QStringList QtlMovieFFmpeg::probeArguments(const QtlMovieSettings* settings)
{
    // Get the analyze duration. Our minimum value is 2 seconds.
    // The maximum value is such that the number of micro-seconds fits in an int (31 bits).
    const int seconds = qBound(2, settings->ffmpegProbeSeconds(), 2100);

    // Now read carefully the following trick;
    //
    // Although this is not clear in the documentation, ffmpeg -analyzeduration
    // alone does not do anything. The probe size (in bytes) shall be large enough
    // to contain this duration. To compute this size from the duration, we need
    // the average bitrate of the file. But, we need to run ffprobe / ffmpeg to
    // obtain the bitrate. To avoid the chicken & egg issue, we assume that the
    // largest bitrate is 8 Mb/s and we compute the corresponding probe size.
    // It should be large enough for most files.
    //
    // And now the second trick;
    //
    // Note that 8 Mbits/s = 1 Mbyte per second. And so the probe size in megabytes
    // is equal to the number of seconds. Since the unit of -analyzeduration is the
    // micro-second, the arguments -analyzeduration and -probesize are identical.
    const QString sizeSpec(QStringLiteral("%1M").arg(seconds));

    QStringList args;
    args << "-probesize" << sizeSpec << "-analyzeduration" << sizeSpec;
    return args;
}


//----------------------------------------------------------------------------
// Build FFprobe command line arguments for an input file.
//----------------------------------------------------------------------------

QStringList QtlMovieFFmpeg::ffprobeArguments(const QtlMovieSettings* settings, const QString& fileName)
{
    QStringList args;
    args << probeArguments(settings)
         << fileName
         << "-print_format" << "flat"
         << "-show_format"
         << "-show_streams";
    return args;
}


//----------------------------------------------------------------------------
// Build FFmpeg command line arguments for input file.
//----------------------------------------------------------------------------

QStringList QtlMovieFFmpeg::inputArguments(const QtlMovieSettings* settings, const QString& fileName, const QtlByteBlock& palette)
{
    QStringList args;
    args << "-nostdin"               // Do not attempt to read from standard input.
         << "-stats"                 // Print progress report (caught by our output analysis).
         << "-loglevel" << QTL_FFMPEG_LOGLEVEL
         << probeArguments(settings)
         << "-fflags" << "+genpts"   // Make FFmpeg generate PTS (time stamps).
         << paletteOptions(palette); // Input file palette if specified.

    if (!fileName.isEmpty()) {
        args << "-i" << fileName;
    }
    return args;
}


//----------------------------------------------------------------------------
// Build FFmpeg command line arguments for output file.
//----------------------------------------------------------------------------

QStringList QtlMovieFFmpeg::outputArguments(const QtlMovieSettings* settings, const QString& fileName, const QString& fileFormat)
{
    QStringList args;

    // If a maximum transcoding time is requested, limit the output.
    if (!settings->transcodeComplete()) {
        args << "-t" << QString::number(settings->transcodeSeconds());
    }

    // Optional output file format.
    if (!fileFormat.isEmpty()) {
        args << "-f" << fileFormat;
    }

    // Output file name
    args << "-y" << fileName;
    return args;
}


//----------------------------------------------------------------------------
// Build the palette FFmpeg options list .
//----------------------------------------------------------------------------

QStringList QtlMovieFFmpeg::paletteOptions(const QtlByteBlock& palette)
{
    // Build the palette description.
    QString spec;
    for (int base = 0; base + 4 <= palette.size(); base += 4) {
        const quint8 r = palette[base+1];
        const quint8 g = palette[base+2];
        const quint8 b = palette[base+3];
        if (!spec.isEmpty()) {
            spec.append(",");
        }
        spec.append(QStringLiteral("%1%2%3").arg(r,2,16,QChar('0')).arg(g,2,16,QChar('0')).arg(b,2,16,QChar('0')));
    }

    // Build the ffmpeg option.
    if (spec.isEmpty()) {
        // No palette => no option.
        return QStringList();
    }
    else {
        QStringList args;
        args << "-palette" << spec;
        return args;
    }
}


//----------------------------------------------------------------------------
// Build a video frame rate FFmpeg options list for the selected output type.
//----------------------------------------------------------------------------

QStringList QtlMovieFFmpeg::frameRateOptions(const QtlMovieSettings* settings, QtlMovieOutputFile::OutputType outputType)
{
    Q_UNUSED(settings);
    const int frameRate = QtlMovieOutputFile::frameRate(outputType);
    if (frameRate <= 0) {
        // Unspecified frame rate, no option.
        return QStringList();
    }
    else {
        QStringList args;
        args << "-r" << QString::number(frameRate);
        return args;
    }
}


//----------------------------------------------------------------------------
// Build FFmpeg command line arguments to insert a video filter string.
//----------------------------------------------------------------------------

QStringList QtlMovieFFmpeg::videoFilterOptions(const QString& videoFilters)
{
    if (videoFilters.isEmpty()) {
        // No option.
        return QStringList();
    }
    else if (videoFilters.contains(";[") || videoFilters.contains("][")) {
        // A graph of filters with several branches.
        return QtlStringList("-filter_complex", videoFilters);
    }
    else {
        // A simple filter chain.
        return QtlStringList("-vf", videoFilters);
    }
}


//----------------------------------------------------------------------------
// Compute an FFmpeg video filter for a target size and aspect ratio.
//----------------------------------------------------------------------------

void QtlMovieFFmpeg::addResizeVideoFilter(QString& videoFilters,
                                          int widthIn,
                                          int heightIn,
                                          float darIn,
                                          int widthOut,
                                          int heightOut,
                                          float darOut)
{
    // When display aspect ratios are unspecified, assume spare pixels.
    if (darIn < 0.001 && heightIn > 0) {
        darIn = float(widthIn) / float(heightIn);
    }
    if (darOut < 0.001 && heightOut > 0) {
        darOut = float(widthOut) / float(heightOut);
    }

    // If the input or output size is not known, cannot resize.
    if (widthIn == 0 || heightIn == 0 || darIn < 0.001 || widthOut == 0 || heightOut == 0 || darOut < 0.001) {
        return;
    }

    // Compute output pixel aspect ratio.
    const float parOut = (darOut * heightOut) / widthOut;

    // To reach the destination size, we resize first and then we add black borders.
    // We always resize to even width/height values (round to the previous even value).
    const int resizeWidth = ~1 & qMin(widthOut, darIn <= darOut ? qRound((heightOut * darIn) / parOut) : widthOut);
    const int resizeHeight = ~1 & qMin(heightOut, darIn <= darOut ? heightOut : qRound((widthOut * parOut) / darIn));

    // Add an optional resize filter.
    if (resizeWidth != widthIn || resizeHeight != heightIn) {
        if (!videoFilters.isEmpty()) {
            videoFilters.append(",");
        }
        videoFilters.append(QStringLiteral("scale=width=%1:height=%2").arg(resizeWidth).arg(resizeHeight));
    }

    // Add optional black borders.
    // We always use even border sizes (round to the previous even value).
    if (resizeWidth != widthOut || resizeHeight != heightOut) {
        if (!videoFilters.isEmpty()) {
            // Add a filter separator.
            videoFilters.append(",");
        }
        videoFilters.append(QStringLiteral("pad=width=%1:height=%2:x=%3:y=%4:color=black")
                            .arg(widthOut)
                            .arg(heightOut)
                            .arg(~1 & ((widthOut - resizeWidth) / 2))
                            .arg(~1 & ((heightOut - resizeHeight) / 2)));
    }
}


//----------------------------------------------------------------------------
// Build FFmpeg a video resize options list to bind the video within a
// maximum size.
//----------------------------------------------------------------------------

void QtlMovieFFmpeg::addBoundedSizeOptions(QStringList& ffmpegArguments,
                                           QString& videoFilters,
                                           int widthIn,
                                           int heightIn,
                                           float darIn,
                                           int maxWidthOut,
                                           int maxHeightOut,
                                           float parOut,
                                           int& widthOut,
                                           int& heightOut)
{
    // When display aspect ratios are unspecified, assume spare pixels.
    if (darIn < 0.001 && heightIn > 0) {
        darIn = float(widthIn) / float(heightIn);
    }
    if (parOut < 0.001) {
        parOut = 1.0;
    }

    // We can resize only of the video size is known.
    if (widthIn <= 0 || heightIn <= 0 || maxWidthOut <= 0 || maxHeightOut <= 0 || parOut <= 0.001) {
        // Cannot resize => no option.
        return;
    }

    // Compute the input pixel aspect ratio.
    const float parIn = qRound((darIn * heightIn) / widthIn);

    // Adjust the input width if the output pixel ratio were applied.
    widthIn = qRound((heightIn * darIn) / parOut);
    if (widthIn == 0) {
        // Unlikely to happen. Cannot resize => no option.
        return;
    }

    // We need to resize if the input size is larger than max output size or input and output pixel ratios are not identical.
    if (widthIn <= maxWidthOut && heightIn <= maxHeightOut && qAbs(parIn - parOut) < 0.01) {
        // Smaller than max output size, same pixel aspect ratio => no need to resize => no option.
        return;
    }

    // Compute input and max output sample aspect ratios.
    const float sarIn = float(widthIn) / float(heightIn);
    const float sarOut = float(maxWidthOut) / float(maxHeightOut);

    // Compute output size in pixels. Round to lower even value.
    if (sarIn > sarOut) {
        // Input is "wider" than max output => bind width to max output width.
        widthOut = ~1 & qMin(maxWidthOut, widthIn);
        heightOut = ~1 & qRound(widthOut / sarIn);
    }
    else {
        // Input is "taller" than max output => bind height to max output height.
        heightOut = ~1 & qMin(maxHeightOut, heightIn);
        widthOut = ~1 & qRound(heightOut * sarIn);
    }

    // Compute the output display aspect ratio.
    Q_ASSERT(heightOut != 0);
    const float darOut = (parOut * widthOut) / heightOut;

    // Add a video filter to resize.
    if (!videoFilters.isEmpty()) {
        videoFilters.append(",");
    }
    videoFilters.append(QStringLiteral("scale=width=%1:height=%2").arg(widthOut).arg(heightOut));

    // Add a command line option to set aspect ratio.
    ffmpegArguments << "-aspect" << QString::number(darOut, 'f', 3);
}
