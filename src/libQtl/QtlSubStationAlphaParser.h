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
//! @file QtlSubStationAlphaParser.h
//!
//! Declare the class QtlSubStationAlphaParser.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLSUBSTATIONALPHAPARSER_H
#define QTLSUBSTATIONALPHAPARSER_H

#include "QtlCore.h"
#include "QtlNullLogger.h"
#include "QtlSubStationAlphaStyle.h"
#include "QtlSubStationAlphaFrame.h"

//!
//! A basic parser for Sub Station Alpha (SSA/ASS) subtitles files.
//!
//! SSA: Sub Station Alpha version 4.
//! ASS: Advanced Sub Station Alpha version 4+.
//! SSA/ASS specification: http://moodub.free.fr/video/ass-specs.doc
//!
class QtlSubStationAlphaParser : public QObject
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtlSubStationAlphaParser(QtlLogger* log = 0, QObject *parent = 0);

    //!
    //! Provide one input line.
    //! @param [in] line One line of ASS/SSA file.
    //!
    void addLine(const QString& line);

    //!
    //! Provide input lines.
    //! @param [in] lines Lines of ASS/SSA file.
    //!
    void addLines(const QStringList& lines);

    //!
    //! Provide a complete input file.
    //! @param [in] fileName Name of an ASS/SSA file.
    //! @return True on success, false on error (file not found, etc.)
    //!
    bool addFile(const QString& fileName);

    //!
    //! Get the current section name.
    //! @return The current section name.
    //!
    QString currentSection() const
    {
        return _sectionName;
    }

    //!
    //! Check if the format is "advanced" Sub Station Alpha (aka ASS).
    //! @return True for ASS, false for SSA.
    //!
    bool isAdvanced() const
    {
        return _isAdvanced;
    }

    //!
    //! Check if the [Script Info] section has been completely parsed.
    //! @return True if the [Script Info] section has been completely parsed.
    //!
    bool scriptInfoCompleted() const
    {
        return _scriptInfoCompleted;
    }

    //!
    //! Get one field of the script info.
    //! @param [in] name Info name.
    //! @param [in] defaultValue Default value if the field is not found.
    //! @return The information value as a string.
    //!
    QString getScriptInfo(const QString& name, const QString& defaultValue = QString()) const;

    //!
    //! Convert an identifier to a normalized format.
    //! This method is useless for applications.
    //! @param [in] name Identifier.
    //! @return Normalized format, in lower case, with all spaces "simplified".
    //!
    static QString normalized(const QString& name);

signals:
    //!
    //! Emitted at the start of a section.
    //! @param [in] sectionName Name of the section, as it appears in the file.
    //!
    void sectionChanged(const QString& sectionName);

    //!
    //! Emitted when a subtitle frame is found.
    //! @param [in] frame Smart pointer to a subtitle frame.
    //!
    void subtitleFrame(const QtlSubStationAlphaFramePtr& frame);

private:
    QtlNullLogger         _nullLog;             //!< Default logger.
    QtlLogger*            _log;                 //!< Message logger.
    bool                  _isAdvanced;          //!< True for ASS, false for SSA.
    bool                  _scriptInfoCompleted; //!< True if [Script Info] section has been completely parsed.
    QString               _sectionName;         //!< Current section name.
    QString               _sectionNormalized;   //!< Current section name.
    QStringList           _formatList;          //!< Current list of formats, in normalized form.
    QMap<QString,QString> _infoMap;             //!< Map of script info, using normalized type names as keys.
    QMap<QString,QtlSubStationAlphaStylePtr> _styles;  //!< Mof styles, using normalized names as keys.

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlSubStationAlphaParser)
};

#endif // QTLSUBSTATIONALPHAPARSER_H
