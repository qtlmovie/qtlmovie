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
//! @file QtlSubStationAlphaFrame.h
//!
//! Declare the class QtlSubStationAlphaFrame.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLSUBSTATIONALPHAFRAME_H
#define QTLSUBSTATIONALPHAFRAME_H

#include "QtlCore.h"
#include "QtlSmartPointer.h"
#include "QtlSubStationAlphaStyle.h"

class QtlSubStationAlphaFrame;

//!
//! Smart pointer to QtlSubStationAlphaFrame, non thread-safe.
//!
typedef QtlSmartPointer<QtlSubStationAlphaFrame,QtlNullMutexLocker> QtlSubStationAlphaFramePtr;
Q_DECLARE_METATYPE(QtlSubStationAlphaFramePtr)

//!
//! Description of a frame of Sub Station Alpha (SSA/ASS) subtitles.
//!
class QtlSubStationAlphaFrame
{
public:
    //!
    //! Constructor.
    //! @param [in] definition Definition the frame as in a "Dialogue:" line in a SSA/ASS file.
    //! @param [in] format List of style fields, as in a "Format:" line, with normalized names.
    //! @param [in] styles Map of style definitions, using normalized names as key.
    //!
    QtlSubStationAlphaFrame(const QString& definition = QString(),
                            const QStringList& format = QStringList(),
                            const QtlSubStationAlphaStyleMap& styles = QtlSubStationAlphaStyleMap());

    //!
    //! Get one field of the dialog.
    //! @param [in] name Field name, as specified in the "Format:" line.
    //! @param [in] defaultValue Default value if the field is not found.
    //! @return The field value as a string.
    //!
    QString getInfo(const QString& name, const QString& defaultValue = QString()) const;

    //!
    //! Show frame at this timestamp (in ms from start of stream).
    //! @return The start time in milliseconds.
    //!
    quint64 showTimestamp() const
    {
        return _start;
    }

    //!
    //! Hide frame at this timestamp (in ms from start of stream).
    //! @return The end time in milliseconds.
    //!
    quint64 hideTimestamp() const
    {
        return _end;
    }

    //!
    //! The text of this subtitle frame in SSA/ASS format.
    //! @return The text of this subtitle frame in SSA/ASS format.
    //!
    QString text() const
    {
        return _text;
    }

    //!
    //! Convert the text of this subtitle frame in SubRip format.
    //! @param [in] useHtmlTags If true, add the HTML SubRip extensions.
    //! @return The text of this subtitle frame in SubRip format.
    //! Never empty, there is at least one string.
    //!
    QStringList toSubRip(bool useHtmlTags = true) const;

private:
    QString                    _text;  //!< Subtitle text, in SSA/ASS format.
    QtlSubStationAlphaStylePtr _style; //!< Style to apply to _text.
    quint64                    _start; //!< Show frame at this timestamp (in ms from start of stream).
    quint64                    _end;   //!< Hide frame at this timestamp (in ms from start of stream).
    QMap<QString,QString>      _info;  //!< Map of info, using normalized names as keys.

    //!
    //! Convert an SSA/ASS timestamp specification to a value in milliseconds.
    //! @param [in] value SSA/ASS timestamp specification.
    //! @return Value in milliseconds or 0 if incorrect.
    //!
    static quint64 toTimestamp(const QString& value);

    //!
    //! Convert a color code in RGB format into a SubRip color specification.
    //! @param [in] color Color code in RGB format.
    //! @return Corresponding SubRip color specification.
    //!
    static QString rgbToSubRip(quint32 color);
};

#endif // QTLSUBSTATIONALPHAFRAME_H
