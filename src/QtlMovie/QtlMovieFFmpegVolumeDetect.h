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
//! @file QtlMovieFFmpegVolumeDetect.h
//!
//! Declare the class QtlMovieFFmpegVolumeDetect.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEFFMPEGVOLUMEDETECT_H
#define QTLMOVIEFFMPEGVOLUMEDETECT_H

#include "QtlMovieFFmpegProcess.h"

//!
//! An execution of FFmpeg with the audio filter "volumedetect".
//!
//! At end of execution, determine if audio normalization is required and
//! compute the corresponding audio filter. This audio filter is stored in
//! the job variables and will be applied in any subsequent FFmpeg transcoding
//! process.
//!
class QtlMovieFFmpegVolumeDetect : public QtlMovieFFmpegProcess
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] inputFile Audio file input specification.
    //! @param [in] audioStream Audio stream specification.
    //! @param [in] inputDurationInSeconds Input file play duration in seconds
    //! or zero if unknown. Used as a hint to indicate progress.
    //! @param [in] temporaryDirectory Directory of temporary files.
    //! @param [in] settings Application settings.
    //! @param [in] log Message logger.
    //! @param [in] parent Optional parent object.
    //!
    QtlMovieFFmpegVolumeDetect(const QString& inputFile,
                               const QString& audioStream,
                               int inputDurationInSeconds,
                               const QString& temporaryDirectory,
                               const QtlMovieSettings* settings,
                               QtlLogger* log,
                               QObject *parent = 0);

protected:
    //!
    //! Process one text line from standard error.
    //! @param [in] channel Origin of the line (QProcess::StandardOutput or QProcess::StandardError).
    //! @param [in] line Text line.
    //!
    virtual void processOutputLine(QProcess::ProcessChannel channel, const QString& line);

    //!
    //! Emit the completed() signal.
    //! @param [in] success True when the action completed successfully, false otherwise.
    //! @param [in] message Optional error message to log.
    //!
    virtual void emitCompleted(bool success, const QString& message = QString());

private:
    qreal _meanLevel;  //!< Measured audio mean level in dBFS.
    qreal _peakLevel;  //!< Measured audio peak level in dBFS.

    //!
    //! Build the FFmpeg command line options.
    //! @param [in] inputFile Audio file input specification.
    //! @param [in] audioStream Audio stream specification.
    //! @param [in] settings Application settings.
    //! @return Command line options.
    //!
    static QStringList commandLineOptions(const QString& inputFile,
                                          const QString& audioStream,
                                          const QtlMovieSettings* settings);

    //!
    //! Build the audio filter for audio normalization.
    //!
    void buildAudioFilter();

    //!
    //! Build a the string image of a real number.
    //! Force "+" sign for positive number.
    //! @param [in] value Real value.
    //! @param [in] fractional Number of fractional digits.
    //! @return Image string.
    //!
    static QString realToString(qreal value, int fractional = 1)
    {
        return QStringLiteral("%1%2").arg(value > 0.0 ? "+" : "").arg(value, 0, 'f', fractional);
    }

    //!
    //! Build a "compand" filter.
    //! @param [in] inMean Input mean volume.
    //! @param [in] inPeak Input peak volume.
    //! @param [in] outMean Output mean volume.
    //! @param [in] outPeak Output peak volume.
    //! @return Filter string.
    //!
    QString compandFilter(qreal inMean, qreal inPeak, qreal outMean, qreal outPeak);

    // Unaccessible operations.
    QtlMovieFFmpegVolumeDetect() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieFFmpegVolumeDetect)
};

#endif // QTLMOVIEFFMPEGVOLUMEDETECT_H
