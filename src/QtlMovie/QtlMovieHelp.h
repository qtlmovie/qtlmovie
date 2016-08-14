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
//! @file QltMovieHelp.h
//!
//! Declare the class QtlMovieHelp.
//!
//----------------------------------------------------------------------------

#ifndef QLTMOVIEHELP_H
#define QLTMOVIEHELP_H

#include "QtlCore.h"

//!
//! A class to display help files in an external browser.
//!
class QtlMovieHelp
{
public:
    //!
    //! Constructor.
    //!
    QtlMovieHelp();

    //!
    //! Virtual destructor.
    //!
    virtual ~QtlMovieHelp();

    //!
    //! The help file to open.
    //!
    enum HelpFileType {
        Home,       //!< Main help file.
        User,       //!< Help file for "using QtlMovie".
        Developer   //!< Help file for "building QtlMovie".
    };

    //!
    //! Display a help HTML file in the external browser.
    //! @param [in] parent Parent object/widget for error messages. If not a widget, find first widget in its hierarchy.
    //! @param [in] type File type to open.
    //! @param [in] fragment Option fragment inside the help file (trailing "#fragment" in URL).
    //!
    void showHelp(QObject* parent, HelpFileType type, const QString& fragment = QString());

    //!
    //! Build the URL for a help HTML file.
    //! @param [in] parent Parent object/widget for error messages. If not a widget, find first widget in its hierarchy.
    //! @param [in] type File type to open.
    //! @param [in] fragment Option fragment inside the help file (trailing "#fragment" in URL).
    //!
    QUrl urlOf(QObject* parent, HelpFileType type, const QString& fragment = QString());

private:
    QTemporaryDir* _tempDir;  //!< Directory for temporary files.

    // Unaccessible operations.
    Q_DISABLE_COPY(QtlMovieHelp)
};

#endif // QLTMOVIEHELP_H
