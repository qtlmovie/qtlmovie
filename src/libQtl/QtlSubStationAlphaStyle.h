//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
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
//! @file QtlSubStationAlphaStyle.h
//!
//! Declare the class QtlSubStationAlphaStyle.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLSUBSTATIONALPHASTYLE_H
#define QTLSUBSTATIONALPHASTYLE_H

#include "QtlCore.h"
#include "QtlSmartPointer.h"

class QtlSubStationAlphaStyle;

//!
//! Smart pointer to QtlSubStationAlphaStyle, non thread-safe.
//!
typedef QtlSmartPointer<QtlSubStationAlphaStyle,QtlNullMutexLocker> QtlSubStationAlphaStylePtr;
Q_DECLARE_METATYPE(QtlSubStationAlphaStylePtr)

//!
//! Map of styles, using normalized names as keys.
//! @see QtlSubStationAlphaParser::normalized()
//!
typedef QMap<QString,QtlSubStationAlphaStylePtr> QtlSubStationAlphaStyleMap;
Q_DECLARE_METATYPE(QtlSubStationAlphaStyleMap)

//!
//! Description of a style of Sub Station Alpha (SSA/ASS) subtitles.
//!
class QtlSubStationAlphaStyle
{
public:
    //!
    //! Constructor.
    //! @param [in] definition Definition the style as in a "Style:" line in a SSA/ASS file.
    //! @param [in] format List of style fields, as in a "Format:" line, with normalized names.
    //!
    QtlSubStationAlphaStyle(const QString& definition = QString(), const QStringList& format = QStringList());

    //!
    //! Get the style name.
    //! @return The style name.
    //!
    QString name() const
    {
        return _name;
    }

    //!
    //! Check if the style is bold.
    //! @return True if the style is bold.
    //!
    bool isBold() const
    {
        return _isBold;
    }

    //!
    //! Check if the style is italic.
    //! @return True if the style is italic.
    //!
    bool isItalic() const
    {
        return _isItalic;
    }

    //!
    //! Check if the style is underline.
    //! @return True if the style is underline.
    //!
    bool isUnderline() const
    {
        return _isUnderline;
    }

    //!
    //! Get the font color in RGB format.
    //! @return The font color in RGB format.
    //!
    quint32 color() const
    {
        return _color;
    }

    //!
    //! Get one field of the style.
    //! @param [in] name Field name, as specified in the "Format:" line.
    //! @param [in] defaultValue Default value if the field is not found.
    //! @return The field value as a string.
    //!
    QString getInfo(const QString& name, const QString& defaultValue = QString()) const;

    //!
    //! Interpret a string as an RGB color specification.
    //! @param [in] value A string value, typically a field of the style.
    //! @param [in] defaultColor The default color if @a value is incorrect, white by default.
    //! @return The color specification.
    //!
    static quint32 toRgbColor(const QString& value, quint32 defaultColor = 0x00FFFFFF);

private:
    QString               _name;         //!< Style name.
    bool                  _isBold;       //!< Style is bold.
    bool                  _isItalic;     //!< Style is italic.
    bool                  _isUnderline;  //!< Style is underline.
    quint32               _color;        //!< Primary color.
    QMap<QString,QString> _info;         //!< Map of style info, using normalized type names as keys.
};

#endif // QTLSUBSTATIONALPHASTYLE_H
