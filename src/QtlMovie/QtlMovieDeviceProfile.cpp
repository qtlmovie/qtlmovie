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
//
// Define the class QtlMovieScreenSize, the description of the screen size
// of a device model.
//
//----------------------------------------------------------------------------

#include "QtlMovieDeviceProfile.h"
#include "QtlNumUtils.h"


//----------------------------------------------------------------------------
// Predefined families of devices.
// Always add new models at the end of the lists.
//
//----------------------------------------------------------------------------

const QVector<QtlMovieDeviceProfile> QtlMovieDeviceProfile::iPadModels
({
     // Model                    Width  Height  Frame/s  Au.bitrate  Au.sampling
     //------------------------  -----  ------  -------  ----------  -----------
     {"iPad 1, 2, Mini",          1024,    768,      30,     160000,       48000},
     {"iPad 3, 4, Air, Retina",   2048,   1536,      30,     160000,       48000},
     {"iPad Pro",                 2732,   2048,      30,     160000,       48000},
 });

const QVector<QtlMovieDeviceProfile> QtlMovieDeviceProfile::iPhoneModels
({
     // Model                    Width  Height  Frame/s  Au.bitrate  Au.sampling
     //------------------------  -----  ------  -------  ----------  -----------
     {"iPhone 1, 2G, 3G, 3GS",     480,    320,      30,     160000,       48000},
     {"iPhone 4, 4S",              960,    640,      30,     160000,       48000},
     {"iPhone 5, 5S",             1136,    640,      30,     160000,       48000},
     {"iPhone 6, 6S",             1334,    750,      30,     160000,       48000},
     {"iPhone 6 Plus, 6S Plus",   2208,   1242,      30,     160000,       48000},
     {"iPhone SE",                1136,    640,      30,     160000,       48000},
 });

const QVector<QtlMovieDeviceProfile> QtlMovieDeviceProfile::androidModels
({
     // Model                               Width  Height  Frame/s  Au.bitrate  Au.sampling
     //-----------------------------------  -----  ------  -------  ----------  -----------
     {"Samsung Galaxy S, S2, S3 mini, A4",    800,    480,      30,     128000,       48000},
     {"Samsung Galaxy S3, Note 2, A5",       1280,    720,      30,     128000,       48000},
     {"Samsung Galaxy Note",                 1280,    800,      30,     128000,       48000},
     {"Samsung Galaxy S4 mini",               960,    540,      30,     128000,       48000},
     {"Samsung Galaxy S4, S5, A6, A7, A8",   1920,   1080,      30,     128000,       48000},
     {"Samsung Galaxy S5 mini",              1280,    720,      30,     128000,       48000},
     {"Samsung Galaxy S6, S7",               2560,   1440,      30,     128000,       48000},
 });


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlMovieDeviceProfile::QtlMovieDeviceProfile(const char* name,
                                             int width,
                                             int height,
                                             float frameRate,
                                             int audioBitRate,
                                             int audioSampling) :
    _name(name == 0 ? "" : name),
    _width(width),
    _height(height),
    _frameRate(frameRate),
    _audioBitRate(audioBitRate),
    _audioSampling(audioSampling)
{
}


//----------------------------------------------------------------------------
// Frame rate value as a string for FFmpeg option.
//----------------------------------------------------------------------------

QString QtlMovieDeviceProfile::frameRateString() const
{
    const int rounded = qRound(_frameRate);
    return qtlFloatEqual(_frameRate, float(rounded)) ?
                QStringLiteral("%1").arg(rounded) :
                QStringLiteral("%1").arg(_frameRate, 0, 'f', 2);
}


//----------------------------------------------------------------------------
// Compute the video bitrate from a given quality indicator.
//----------------------------------------------------------------------------

int QtlMovieDeviceProfile::videoBitRate(int qualityIndicator) const
{
    return videoBitRate(qualityIndicator, _width, _height, _frameRate);
}

int QtlMovieDeviceProfile::videoBitRate(int qualityIndicator, int width, int height, float frameRate)
{
    // Use double float intermediate value to avoid overflow.
    return qRound((double(qualityIndicator) * double(width) * double(height) * double(frameRate)) / 100);
}
