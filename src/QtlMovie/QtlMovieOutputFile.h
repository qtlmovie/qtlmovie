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
//! @file QtlMovieOutputFile.h
//!
//! Declare the class QtlMovieOutputFile.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEOUTPUTFILE_H
#define QTLMOVIEOUTPUTFILE_H

#include "QtlFile.h"
#include "QtlMovieSettings.h"

//!
//! Describes an output video file.
//!
class QtlMovieOutputFile : public QtlFile
{
#if !defined (DOXYGEN)
    Q_OBJECT
#endif

public:
    //!
    //! Constructor.
    //! @param [in] fileName Output file name.
    //! @param [in] settings Application settings.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent widget.
    //!
    QtlMovieOutputFile(const QString& fileName,
                       const QtlMovieSettings* settings,
                       QtlLogger* log,
                       QObject *parent = 0);

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy (except parent).
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlMovieOutputFile(const QtlMovieOutputFile& other, QObject *parent = 0);

    //!
    //! Type of output file.
    //!
    enum OutputType {
        DvdFile,  //!< DVD MPEG-PS file.
        DvdImage, //!< DVD ISO image.
        DvdBurn,  //!< DVD ISO image and burn it.
        Ipad,     //!< iPad target.
        Iphone,   //!< iPhone target.
        Avi,      //!< AVI target (highly compressed).
        SubRip,   //!< Subtitles only in SRT format.
        None      //!< No output.
    };

    //!
    //! Get the output file type.
    //! @return Output file type.
    //!
    OutputType outputType() const
    {
        return _outputType;
    }

    //!
    //! Set the output file type.
    //! @param [in] type Output file type.
    //!
    void setOutputType(OutputType type)
    {
        _outputType = type;
    }

    //!
    //! Get the default output directory for an input file name, based on the output type.
    //! @param [in] inputFileName Input file name. Can be empty.
    //!
    QString defaultOutputDirectory(const QString& inputFileName);

    //!
    //! Set the default output file name from an input file name, based on the output type.
    //! @param [in] inputFileName Input file name. Can be empty.
    //!
    void setDefaultFileName(const QString& inputFileName);

    //!
    //! Get a list of all output types, in GUI presentation order.
    //! @return A list of all output types, in GUI presentation order.
    //!
    static QList<OutputType> outputTypes();

    //!
    //! Get a user friendly name for an output type.
    //! @param [in] outputType Output type.
    //! @return User friendly name.
    //!
    static QString outputTypeName(OutputType outputType);

    //!
    //! Get an identifier name for an output type.
    //! @param [in] outputType Output type.
    //! @return Identifier name.
    //!
    static QString outputIdName(OutputType outputType);

    //!
    //! Get the default output file extension for a given output type.
    //! @param [in] outputType Output type.
    //! @return The output file extension.
    //!
    static QString fileExtension(OutputType outputType);

private:
    QtlLogger*              _log;         //!< Where to log errors.
    const QtlMovieSettings* _settings;    //!< Application settings.
    OutputType              _outputType;  //!< Output format.

    // Unaccessible operations.
    QtlMovieOutputFile() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieOutputFile)
};

#endif // QTLMOVIEOUTPUTFILE_H
