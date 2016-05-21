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
//!
//! @file QtlMovieScreenSize.h
//!
//! Declare the class QtlMovieScreenSize, the description of the screen size
//! of a device model.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIESCREENSIZE_H
#define QTLMOVIESCREENSIZE_H

#include "QtlCore.h"

//!
//! This structure defines the type and screen size of a device (iPad, iPhone, etc).
//!
class QtlMovieScreenSize
{
public:
    //!
    //! Constructor.
    //! @param [in] name_ Device model name.
    //! @param [in] width_ Screen width in pixels.
    //! @param [in] height_ Screen height in pixels.
    //!
    QtlMovieScreenSize(const char* name_ = "undefined", int width_ = 100, int height_ = 100);

    QString name;    //!< Device model name.
    int     width;   //!< Screen width in pixels.
    int     height;  //!< Screen height in pixels.

    static const QVector<QtlMovieScreenSize> iPadModels;    //!< List of supported iPad models.
    static const QVector<QtlMovieScreenSize> iPhoneModels;  //!< List of supported iPhone models.
};

#endif // QTLMOVIESCREENSIZE_H
