//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2016, Thierry Lelegard
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

#include "QtlMovieScreenSize.h"


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

QtlMovieScreenSize::QtlMovieScreenSize(const char* name_, int width_, int height_) :
    name(name_ == 0 ? "" : name_),
    width(width_),
    height(height_)
{
}


//----------------------------------------------------------------------------
// Predefined families of devices.
//----------------------------------------------------------------------------

const QVector<QtlMovieScreenSize> QtlMovieScreenSize::iPadModels({
    {"iPad 1, 2, Mini",        1024,  768},
    {"iPad 3, 4, Air, Retina", 2048, 1536},
    {"iPad Pro",               2732, 2048},
});

const QVector<QtlMovieScreenSize> QtlMovieScreenSize::iPhoneModels({
    {"iPhone 1, 2G, 3G, 3GS",   480,  320},
    {"iPhone 4, 4S",            960,  640},
    {"iPhone 5, 5S",           1136,  640},
    {"iPhone 6, 6S",           1334,  750},
    {"iPhone 6 Plus, 6S Plus", 2208, 1242},
    {"iPhone SE",              1136,  640},
});
