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
//! @file QtlMovieDeviceProfile.h
//!
//! Declare the class QtlMovieDeviceProfile, the description of a device model.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEDEVICEPROFILE_H
#define QTLMOVIEDEVICEPROFILE_H

#include "QtlCore.h"

//!
//! This structure defines the type and screen size of a device (iPad, iPhone, etc).
//!
class QtlMovieDeviceProfile
{
public:
    //!
    //! Constructor.
    //! @param [in] name Device model name.
    //! @param [in] width Screen width in pixels.
    //! @param [in] height Screen height in pixels.
    //! @param [in] frameRate Frame rate.
    //! @param [in] audioBitRate Audio bitrate.
    //! @param [in] audioSampling Audio samples per second.
    //!
    QtlMovieDeviceProfile(const char* name  = "undefined",
                          int width         = 100,
                          int height        = 100,
                          float frameRate   = 30,
                          int audioBitRate  = 128000,
                          int audioSampling = 48000);

    //!
    //! Device model name.
    //! @return Device model name.
    //!
    QString name() const
    {
        return _name;
    }

    //!
    //! Screen width in pixels.
    //! @return Screen width in pixels.
    //!
    int width() const
    {
        return _width;
    }

    //!
    //! Screen height in pixels.
    //! @return Screen height in pixels.
    //!
    int height() const
    {
        return _height;
    }

    //!
    //! Frame rate.
    //! @return Frame rate in frames per second.
    //!
    float frameRate() const
    {
        return _frameRate;
    }

    //!
    //! Frame rate value as a string for FFmpeg option.
    //! @return Frame rate value as a string.
    //!
    QString frameRateString() const;

    //!
    //! Audio bitrate.
    //! @return Audio bitrate.
    //!
    int audioBitRate() const
    {
        return _audioBitRate;
    }

    //!
    //! Audio samples per second.
    //! @return Audio samples per second.
    //!
    int audioSampling() const
    {
        return _audioSampling;
    }

    //!
    //! Compute the video bitrate from a given quality indicator.
    //! A <em>quality indicator</em> is a way to represent the video quality
    //! independently from the actual video size and frame rate.
    //!
    //! @param [in] qualityIndicator Specified video quality. This is the number of bits per pixel per 100 frames.
    //! @return The computed bitrate in bits per second.
    //!
    int videoBitRate(int qualityIndicator) const;

    //!
    //! Compute the video bitrate from a given quality indicator.
    //! A <em>quality indicator</em> is a way to represent the video quality
    //! independently from the actual video size and frame rate.
    //!
    //! @param [in] qualityIndicator Specified video quality. This is the number of bits per pixel per 100 frames.
    //! @param [in] width Screen width in pixels.
    //! @param [in] height Screen height in pixels.
    //! @param [in] frameRate Frames per second.
    //! @return The computed bitrate in bits per second.
    //!
    static int videoBitRate(int qualityIndicator, int width, int height, float frameRate);

    static const QVector<QtlMovieDeviceProfile> iPadModels;     //!< List of supported iPad models.
    static const QVector<QtlMovieDeviceProfile> iPhoneModels;   //!< List of supported iPhone models.
    static const QVector<QtlMovieDeviceProfile> androidModels;  //!< List of supported Android devices.

private:
    QString _name;           //!< Device model name.
    int     _width;          //!< Screen width in pixels.
    int     _height;         //!< Screen height in pixels.
    float   _frameRate;      //!< Frames per second.
    int     _audioBitRate;   //!< Audio bitrate.
    int     _audioSampling;  //!< Audio samples per second.
};

#endif // QTLMOVIEDEVICEPROFILE_H
