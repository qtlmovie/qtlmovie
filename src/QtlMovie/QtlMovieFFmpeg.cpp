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
#include "QtlStringUtils.h"
#include "QtlStringList.h"
#include "QtlNumUtils.h"


//----------------------------------------------------------------------------
// Build FFmpeg command line arguments for the initial probe size and duration.
//----------------------------------------------------------------------------

QStringList QtlMovieFFmpeg::probeArguments(const QtlMovieSettings* settings, int probeTimeDivisor)
{
    // Get the analyze duration. Our minimum value is 2 seconds.
    // The maximum value (2147) is such that the number of micro-seconds fits in an int (31 bits).
    int seconds = qBound(2, settings->ffmpegProbeSeconds(), 2147);

    // Apply a custom time reduction if requested. Do not drop below 2 seconds.
    if (probeTimeDivisor > 1) {
        seconds = qMax(2, seconds / probeTimeDivisor);
    }

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
    // Note that 8 Mbit/s = 1 Mbyte/s. And so the probe size in megabytes
    // is equal to the number of seconds. Since the unit of -analyzeduration is the
    // micro-second, the arguments -analyzeduration and -probesize are identical.
    const QString sizeSpec(QStringLiteral("%1M").arg(seconds));

    QStringList args;
    args << "-probesize" << sizeSpec << "-analyzeduration" << sizeSpec;
    return args;
}


//----------------------------------------------------------------------------
// Build FFmpeg command line arguments for input file.
//----------------------------------------------------------------------------

QStringList QtlMovieFFmpeg::inputArguments(const QtlMovieSettings* settings, const QtlMovieInputFile* file)
{
    // Common options.
    QStringList args;
    args << "-nostdin"               // Do not attempt to read commands from standard input.
         << "-stats"                 // Print progress report (caught by our output analysis).
         << "-loglevel" << QTL_FFMPEG_LOGLEVEL
         << probeArguments(settings)
         << "-fflags" << "+genpts";  // Make FFmpeg generate PTS (time stamps).

    // If no input file specified, nothing more to add.
    if (file == 0) {
        return args;
    }

    // Build the color palette options. This is typically used for DVD subtitles where the
    // color palette of the subtitles bitmaps is stored outside the video file (in the .IFO).
    const QtlByteBlock palette(file->palette());
    QString paletteDescription;
    for (int base = 0; base + 4 <= palette.size(); base += 4) {
        const quint8 r = palette[base+1];
        const quint8 g = palette[base+2];
        const quint8 b = palette[base+3];
        if (!paletteDescription.isEmpty()) {
            paletteDescription.append(",");
        }
        paletteDescription.append(QStringLiteral("%1%2%3").arg(r,2,16,QChar('0')).arg(g,2,16,QChar('0')).arg(b,2,16,QChar('0')));
    }
    if (!paletteDescription.isEmpty()) {
        args << "-palette" << paletteDescription;
    }

    // Add explicit file format. This is typically used when the input is stdin
    // and the file format cannot be derived from the file name.
    const QString fileFormat(file->ffmpegInputFileFormat());
    if (!fileFormat.isEmpty()) {
        args << "-f" << fileFormat;
    }

    // Add the input file name. This is not really a file name but the "ffmpeg specification"
    // which can be '-' (stdin) or 'concat:...' (concatenation of DVD VTS files).
    const QString fileName(file->ffmpegInputFileSpecification());
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
// Build an audio conversion FFmpeg options list for DVD transcoding.
//----------------------------------------------------------------------------

QStringList QtlMovieFFmpeg::dvdAudioOptions(const QtlMovieSettings* settings, const QtlMediaStreamInfoPtr& audioStream)
{
    QStringList args;
    if (!audioStream.isNull()) {
        args << "-map" << audioStream->ffSpecifier()
             << QTL_AUDIO_FILTER_VARREF  // Potential audio filter will be inserted here.
             << "-codec:a" << "ac3"      // AC-3 audio (Dolby Digital).
             << "-ac" << "2"             // Remix to 2 channels (stereo).
             << "-ar" << QString::number(QTL_DVD_AUDIO_SAMPLING)
             << "-b:a" << QString::number(QTL_DVD_AUDIO_BITRATE);
    }
    return args;
}


//----------------------------------------------------------------------------
// Format the argument of an ffmpeg filter using proper escaping.
//----------------------------------------------------------------------------

QString QtlMovieFFmpeg::escapedFilterArgument(const QString& argument)
{
    // The string we transform. At the end, we will enclose it in single quotes.
    QString escaped(argument);

    // All \ are escaped with another \.
    // We must do it now since we are going to insert a lot of other \ afterwards.
    escaped.replace("\\", "\\\\");

    // All =:,; are escaped with a \.
    escaped.replace(QRegExp("([=:,;])"), "\\\\1");

    // The quotes cannot be escaped within quotes. We must close the quoted string,
    // escape the quote and reopen the quote sequence.
    escaped.replace("'", "'\\\\\\''");

    // Return the escaped argument between quotes.
    return "'" + escaped + "'";
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
    if (qtlFloatZero(darIn) && heightIn > 0) {
        darIn = float(widthIn) / float(heightIn);
    }
    if (qtlFloatZero(darOut) && heightOut > 0) {
        darOut = float(widthOut) / float(heightOut);
    }

    // If the input or output size is not known, cannot resize.
    if (widthIn == 0 || heightIn == 0 || qtlFloatZero(darIn) || widthOut == 0 || heightOut == 0 || qtlFloatZero(darOut)) {
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
    if (qtlFloatZero(darIn) && heightIn > 0) {
        darIn = float(widthIn) / float(heightIn);
    }
    if (qtlFloatZero(parOut)) {
        parOut = 1.0;
    }

    // Initially, assume no need to resize.
    widthOut = widthIn;
    heightOut = heightIn;
    float darOut = darIn;

    // We can resize only of the video size is known.
    if (widthIn <= 0 || heightIn <= 0 || maxWidthOut <= 0 || maxHeightOut <= 0 || qtlFloatZero(parOut)) {
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
    if (widthIn > maxWidthOut || heightIn > maxHeightOut || !qtlFloatEqual(parIn, parOut)) {

        // Compute input and max output sample aspect ratios.
        const float sarIn = float(widthIn) / float(heightIn);
        const float sarOut = float(maxWidthOut) / float(maxHeightOut);

        // Compute output size in pixels. Round to lower even value using "~1 & ...".
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
        darOut = (parOut * widthOut) / heightOut;

        // Add a video filter to resize.
        if (!videoFilters.isEmpty()) {
            videoFilters.append(",");
        }
        videoFilters.append(QStringLiteral("scale=width=%1:height=%2").arg(widthOut).arg(heightOut));
    }

    // Add a command line option to set aspect ratio.
    ffmpegArguments << "-aspect" << QString::number(darOut, 'f', 3);
}


//----------------------------------------------------------------------------
// Build a video rotation FFmpeg options list .
//----------------------------------------------------------------------------

void QtlMovieFFmpeg::addRotateOptions(const QtlMovieSettings* settings,
                                      const QtlMediaStreamInfoPtr& videoStream,
                                      QStringList& ffmpegArguments,
                                      QString& videoFilters,
                                      int& width,
                                      int& height,
                                      float& dar)
{
    // Check video stream.
    if (videoStream.isNull()) {
        width = height = 0;
        dar = 1.0;
        return;
    }

    // Get initial video size.
    width = videoStream->width();
    height = videoStream->height();
    dar = videoStream->displayAspectRatio();

    // Always use rotation angle in range [0..360[ degrees.
    const int rotation = videoStream->rotation() % 360;

    // If the auto rotate option is disabled, do nothing.
    if (!settings->autoRotateVideo() || rotation == 0) {
        return;
    }

    // Make sure the "rotate" metadata is removed from output file.
    ffmpegArguments << ("-metadata:s:" + videoStream->ffSpecifier()) << "rotate=";

    // If the rotation is exactly a portrait/landscape swap, swap input width and height.
    if (rotation == 90 || rotation == 270) {
        qSwap(width, height);
        if (dar != 0.0) {
            dar = 1.0 / dar;
        }
    }

    // Add a video filter to rotate.
    if (!videoFilters.isEmpty()) {
        videoFilters.append(",");
    }
    if (rotation == 90) {
        videoFilters.append(QStringLiteral("rotate=PI/2:ow=%1:oh=%2").arg(width).arg(height));
    }
    else if (rotation == 180) {
        videoFilters.append("rotate=PI");
    }
    else if (rotation == 270) {
        videoFilters.append(QStringLiteral("rotate=-PI/2:ow=%1:oh=%2").arg(width).arg(height));
    }
    else {
        // Convert degrees to radians.
        videoFilters.append(QStringLiteral("rotate=%1").arg(float(rotation) * 3.1415 / 180.0, 0, 'f', 3));
    }
}
