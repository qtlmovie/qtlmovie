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
//! @file QtlMovieFFprobeTags.h
//!
//! Declare the class QtlMovieFFprobeTags.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEFFPROBETAGS_H
#define QTLMOVIEFFPROBETAGS_H

#include <QtCore>
#include "QtlMediaStreamInfo.h"
#include "QtlStringUtils.h"

//!
//! Define a map of tags/values from ffprobe output.
//! The keys in the map are ffprobe tags. They are always trimmed and lowercase'd.
//! The values are trimmed but are left otherwise unchanged from ffprobe output.
//!
class QtlMovieFFprobeTags: public QMap<QString,QString>
{
public:
    //!
    //! Parse the text output of ffprobe and load this object.
    //! @param [in] ffprobeOutput FFprobe output, in one single string with embedded new-lines.
    //!
    void loadFFprobeOutput(const QString& ffprobeOutput);

    //!
    //! Analyze the FFprobe tags and add in a list of stream info.
    //! @param [in,out] streams The list of streams to build.
    //!
    void buildStreamInfo(QtlMediaStreamInfoPtrVector& streams);

    //!
    //! Get the value of a global tag in FFprobe output.
    //! @param [in] name Tag name.
    //! @param [in] defaultValue Default value if tag not present.
    //! @return Tag value.
    //!
    QString value(const QString& name, const QString& defaultValue = QString()) const;

    //!
    //! Get the integer value of a global tag in FFprobe output.
    //! Decimal or hexadecimal notation is recognized.
    //! Low-level method, dependent on ffprobe output, prefer specialized method when possible.
    //! @param [in] name Tag name.
    //! @param [in] defaultValue Default value if tag not present or not an integer.
    //! @return Tag value.
    //!
    int intValue(const QString& name, int defaultValue = 0) const;

    //!
    //! Get the float value of a global tag in FFprobe output.
    //! Syntax like "x/y" and "x:y" are interpreted as rational numbers.
    //! Low-level method, dependent on ffprobe output, prefer specialized method when possible.
    //! @param [in] name Tag name.
    //! @param [in] defaultValue Default value if tag not present or not an integer.
    //! @return Tag value.
    //!
    float floatValue(const QString& name, float defaultValue = 0.0) const
    {
        return qtlToFloat(value(name), defaultValue);
    }

    //!
    //! Get the full name of a stream-specific tag in FFprobe output.
    //! Low-level method, dependent on ffprobe output, prefer specialized method when possible.
    //! @param [in] ffIndex FFprobe index of the stream to query.
    //! @param [in] name Tag name in the stream.
    //! @return Full tag name.
    //!
    static QString tagOfStream(int ffIndex, const QString& name)
    {
        return QStringLiteral("streams.stream.%1.%2").arg(ffIndex).arg(name);
    }

    //!
    //! Get the value of a stream tag in FFprobe output.
    //! Low-level method, dependent on ffprobe output, prefer specialized method when possible.
    //! @param [in] ffIndex FFprobe index of the stream to query.
    //! @param [in] name Tag name in the stream.
    //! @param [in] defaultValue Default value if tag not present.
    //! @return Tag value.
    //!
    QString valueOfStream(int ffIndex, const QString& name, const QString& defaultValue = QString()) const
    {
        return value(tagOfStream(ffIndex, name), defaultValue);
    }

    //!
    //! Get the integer value of a stream tag in FFprobe output.
    //! Low-level method, dependent on ffprobe output, prefer specialized method when possible.
    //! @param [in] ffIndex FFprobe index of the stream to query.
    //! @param [in] name Tag name in the stream.
    //! @param [in] defaultValue Default value if tag not present or not an integer.
    //! @return Tag value.
    //!
    int intValueOfStream(int ffIndex, const QString& name, int defaultValue = 0) const
    {
        return intValue(tagOfStream(ffIndex, name), defaultValue);
    }

    //!
    //! Get the float value of a stream tag in FFprobe output.
    //! Syntax like "x/y" and "x:y" are interpreted as rational numbers.
    //! Low-level method, dependent on ffprobe output, prefer specialized method when possible.
    //! @param [in] ffIndex FFprobe index of the stream to query.
    //! @param [in] name Tag name.
    //! @param [in] defaultValue Default value if tag not present or not an integer.
    //! @return Tag value.
    //!
    float floatValueOfStream(int ffIndex, const QString& name, float defaultValue = 0.0) const
    {
        return floatValue(tagOfStream(ffIndex, name), defaultValue);
    }
};

#endif // QTLMOVIEFFPROBETAGS_H
