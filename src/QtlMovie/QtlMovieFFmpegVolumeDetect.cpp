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
// Define the class QtlMovieFFmpegVolumeDetect.
//
//----------------------------------------------------------------------------

#include "QtlMovieFFmpegVolumeDetect.h"
#include "QtlMovieFFmpeg.h"
#include "QtlMovie.h"
#include "QtlStringList.h"


//----------------------------------------------------------------------------
// Build the FFmpeg command line options.
//----------------------------------------------------------------------------

QStringList QtlMovieFFmpegVolumeDetect::commandLineOptions(const QString& inputFile, const QString& audioStream, const QtlMovieSettings* settings)
{
    // Complete command line. We drop all video streams (-vn).
    // But note that we must not specific an audio codec, even -codec:a copy,
    // otherwise the volumedetect filter does not display anything.
    QStringList args;
    args << "-nostdin"               // Do not attempt to read from standard input.
         << "-stats"                 // Print progress report (caught by our output analysis).
         << "-loglevel" << "info"    // Must report info.
         << QtlMovieFFmpeg::probeArguments(settings)
         << "-i" << inputFile        // Input file containing the audio.
         << "-map" << audioStream    // Audio stream selection.
         << "-af" << "volumedetect"  // Run "volumedetect" filter on this audio stream.
         << "-vn"                    // Drop video.
         << QtlMovieFFmpeg::outputArguments(settings, QProcess::nullDevice(), "null"); // Drop output
    return args;
}


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlMovieFFmpegVolumeDetect::QtlMovieFFmpegVolumeDetect(const QString& inputFile,
                                                       const QString& audioStream,
                                                       int inputDurationInSeconds,
                                                       const QString& temporaryDirectory,
                                                       const QtlMovieSettings* settings,
                                                       QtlLogger* log,
                                                       QObject* parent,
                                                       QtlDataPull* dataPull) :
    QtlMovieFFmpegProcess(commandLineOptions(inputFile, audioStream, settings),
                          inputDurationInSeconds,
                          temporaryDirectory,
                          settings,
                          log,
                          parent,
                          dataPull),
    _meanLevel(0.0),
    _peakLevel(0.0)
{
}


//----------------------------------------------------------------------------
// Process one text line from standard error.
//----------------------------------------------------------------------------

void QtlMovieFFmpegVolumeDetect::processOutputLine(QProcess::ProcessChannel channel, const QString& line)
{
    // Intercept all lines which are generated by the "volumedetect" filter.
    if (line.contains("[Parsed_volumedetect_", Qt::CaseInsensitive)) {

        // Determine which value it is, intercept mean_volume and max_volume.
        qreal* value = 0;
        if (line.contains("mean_volume:", Qt::CaseInsensitive)) {
            value = &_meanLevel;
        }
        else if (line.contains("max_volume:", Qt::CaseInsensitive)) {
            value = &_peakLevel;
        }

        // Extract the value, sample expected end of line: ": -19.4 dB"
        if (value != 0) {
            QString line1(line);
            line1.replace(QRegExp("^.*:\\s*"), "");
            line1.replace(QRegExp("\\s+.*$"), "");
            bool ok = false;
            const qreal f = line1.toDouble(&ok);
            if (ok) {
                *value = f;
            }
        }
    }

    // Invoke superclass
    QtlMovieFFmpegProcess::processOutputLine(channel, line);
}


//----------------------------------------------------------------------------
// Emit the completed() signal.
//----------------------------------------------------------------------------

void QtlMovieFFmpegVolumeDetect::emitCompleted(bool success, const QString& message)
{
    // Build and register the required audio filter if necessary.
    buildAudioFilter();

    // Invoke superclass
    QtlMovieFFmpegProcess::emitCompleted(success, message);
}


//----------------------------------------------------------------------------
// Build the audio filter for audio normalization.
//----------------------------------------------------------------------------

void QtlMovieFFmpegVolumeDetect::buildAudioFilter()
{
    // No audio level found or no normalization required.
    if (_meanLevel >= 0.0 || _peakLevel <= _meanLevel || !settings()->audioNormalize()) {
        return;
    }

    // Report audio levels.
    line(tr("Audio volume analysis completed, mean level = %1 dBFS, peak level = %2 dBFS").arg(_meanLevel).arg(_peakLevel));

    // Target audio levels.
    const qreal outMean = qreal(settings()->audioNormalizeMean());
    const qreal outPeak = qreal(settings()->audioNormalizePeak());

    // Check configuration consistency.
    if (outPeak <= outMean) {
        line(tr("Inconsistent audio normalization settings, max peak level (%1 dBFS) must be greater than mean level (%2 dBFS)").arg(outPeak).arg(outMean), QColor("red"));
        return;
    }

    // Perform audio normalization only if the input mean level is far enough from the target.
    if (qAbs(_meanLevel - outMean) <= QTL_AUDIO_NORMALIZATION_TOLERANCE) {
        line(tr("Audio volume is close enough to the normalized level, will not normalize"));
        return;
    }

    // Compute input and output dynamic ranges.
    const qreal inDynamics = _peakLevel - _meanLevel;
    const qreal outDynamics = outPeak - outMean;

    // Compute the audio filter.
    QString audioFilter;
    if (inDynamics <= outDynamics || settings()->audioNormalizeMode() == QtlMovieSettings::Clip) {
        // The input dynamics is in the output range, simply adjust the volume to the target mean level.
        // Same if we clip the audio in case the input dynamics is too large.
        audioFilter = "volume=" + realToString(outMean - _meanLevel) + "dB";
    }
    else if (settings()->audioNormalizeMode() == QtlMovieSettings::AlignPeak) {
        // The input dynamics is too large and we simply adjust the volume to align the peak level.
        audioFilter = "volume=" + realToString(outPeak - _peakLevel) + "dB";
    }
    else {
        // The input dynamics is too large and we must compress the dynamics.
        audioFilter = compandFilter(_meanLevel, _peakLevel, outMean, outPeak);
    }
    debug(tr("Audio filter: \"%1\"").arg(audioFilter));

    // Store the audio filter options for FFmpeg in the job.
    if (!audioFilter.isEmpty()) {
        setJobVariable(QTL_AUDIO_FILTER_VARNAME, QtlStringList("-af", audioFilter));
    }
}


//----------------------------------------------------------------------------
// Build a "compand" filter.
//----------------------------------------------------------------------------

QString QtlMovieFFmpegVolumeDetect::compandFilter(qreal inMean, qreal inPeak, qreal outMean, qreal outPeak)
{
    // Check validity.
    if (inMean > inPeak || inPeak > 0.0 || outMean > outPeak || outPeak > 0.0) {
        return "";
    }

    // General parameters, currently hardcoded.
    const qreal attacks = 0.002; // seconds
    const qreal decays  = 0.2;   // seconds
    const qreal knee    = 10.0;  // ?
    const qreal gain    = 0.0;   // dB
    const qreal volume  = 0.0;   // dB
    const qreal delay   = 0.0;   // seconds

    // List of points in the transfer function in dBFS.
    QList<QPointF> transfer;

    // The first point is at "noise level", -70dB but at least inMean - 10dB.
    // The noise level remains unchanged to avoid upraising the noise.
    const qreal noise = qMin(-70.0, inMean - 10.0);
    transfer << QPointF(noise, noise);

    // The mean level is shifted. The mid points in [noise, mean] and [mean, peak]
    // are shifted by the same amount but must remain in the range noise + 1/3 (mean - noise)
    // to peak - 1/3 (peak - mean).
    const qreal meanShift = outMean - inMean;
    const qreal inMidBelowMean = noise + (inMean - noise) / 2.0;
    const qreal outMidBelowMean = qMax(inMidBelowMean + meanShift, noise + 0.33 * (outMean - noise));
    const qreal inMidAboveMean = inMean + (inPeak - inMean) / 2.0;
    const qreal outMidAboveMean = qMin(inMidAboveMean + meanShift, outPeak - 0.33 * (outPeak - outMean));

    // Mid point between noise and mid-below-mean.
    transfer << QPointF(noise + (inMidBelowMean - noise) / 2.0, noise + (outMidBelowMean - noise) / 2.0);

    // Mean level and its surrounding mid points.
    transfer << QPointF(inMidBelowMean, outMidBelowMean);
    transfer << QPointF(inMean, outMean);
    transfer << QPointF(inMidAboveMean, outMidAboveMean);

    // Mid point between mid-above-mean and peak.
    transfer << QPointF(inMidAboveMean + (inPeak - inMidAboveMean) / 2.0, outMidAboveMean + (outPeak - outMidAboveMean) / 2.0);

    // End of transfer function: inPeak and Full Scale (0 dBFS) map to outPeak.
    transfer << QPointF(inPeak, outPeak);
    if (inPeak < 0.0) {
        transfer << QPointF(0.0, outPeak);
    }

    // Build the string representation of the transfer function.
    QString transferString;
    foreach (const QPointF& point, transfer) {
        if (!transferString.isEmpty()) {
            transferString += ' ';
        }
        transferString += realToString(point.x());
        transferString += '/';
        transferString += realToString(point.y());
    }

    // Build the audio filter string.
    return "compand=attacks=" + realToString(attacks, 3) +
            ":decays=" + realToString(decays, 3) +
            ":points=" + transferString +
            ":soft-knee=" + realToString(knee) +
            ":gain=" + realToString(gain) +
            ":volume=" + realToString(volume) +
            ":delay=" + realToString(delay, 3);
}
