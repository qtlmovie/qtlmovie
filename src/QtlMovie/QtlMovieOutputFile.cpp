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
//
// Define the class QtlMovieOutputFile.
//
//----------------------------------------------------------------------------

#include "QtlMovieOutputFile.h"
#include "QtlMovieSettings.h"
#include "QtlMovie.h"


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

QtlMovieOutputFile::QtlMovieOutputFile(const QString& fileName,
                                       const QtlMovieSettings* settings,
                                       QtlLogger* log,
                                       QObject *parent) :
    QtlFile(fileName, parent),
    _log(log),
    _settings(settings),
    _outputType(settings->defaultOutputType()),
    _forcedDar(0.0)
{
    Q_ASSERT(log != 0);
    Q_ASSERT(settings != 0);
}

QtlMovieOutputFile::QtlMovieOutputFile(const QtlMovieOutputFile& other, QObject* parent) :
    QtlFile(other, parent),
    _log(other._log),
    _settings(other._settings),
    _outputType(other._outputType),
    _forcedDar(other._forcedDar)
{
}


//----------------------------------------------------------------------------
// Set the output file type.
//----------------------------------------------------------------------------

void QtlMovieOutputFile::setOutputType(OutputType type)
{
    if (_outputType != type) {
        _outputType = type;
        emit outputTypeChanged(type);
    }
}


//----------------------------------------------------------------------------
// Get the default output directory for an input file name, based on the output type.
//----------------------------------------------------------------------------

QString QtlMovieOutputFile::defaultOutputDirectory(const QString& inputFileName)
{
    // The default output directory by output type is stored in the settings.
    // When empty, this means "use same directory as input file".
    const QString defaultOutDir(_settings->defaultOutputDir(outputIdName(_outputType)));

    if (!defaultOutDir.isEmpty()) {
        // Default directory by output type is present, use it.
        return defaultOutDir;
    }
    else if (!inputFileName.isEmpty()) {
        // An input file is present, use same directory.
        return absoluteNativeFilePath(QFileInfo(inputFileName).absolutePath());
    }
    else {
        // Last resort, use user's home directory.
        return absoluteNativeFilePath(QDir::homePath());
    }
}


//----------------------------------------------------------------------------
// Set the default output file name from an input file name.
//----------------------------------------------------------------------------

void QtlMovieOutputFile::setDefaultFileName(const QString& inputFileName, bool keepPreviousDirectory, bool keepPreviousBase)
{
    // If both input and output file names are empty, do nothing.
    if (inputFileName.isEmpty() && !isSet()) {
        return;
    }

    // Output directory.
    QString outBase;
    if (inputFileName.isEmpty() || keepPreviousDirectory) {
        outBase = QFileInfo(fileName()).absolutePath();
    }
    else {
        outBase = defaultOutputDirectory(inputFileName);
    }

    // Output base name.
    outBase += QDir::separator();
    if (inputFileName.isEmpty() || keepPreviousBase) {
        outBase += QFileInfo(fileName()).completeBaseName();
    }
    else {
        outBase += QFileInfo(inputFileName).completeBaseName();
    }

    // Output file name.
    const QString outExt(fileExtension(_outputType));
    const QString outFull(outBase + outExt);

    // Set the output file name. If clashes with input file, add a .transcoded suffix.
    // Take care of case sensitivity of the file system.
    const bool same = QString::compare(outFull, inputFileName, QTL_FILE_NAMES_CASE_SENSITIVE) == 0;
    setFileName(same ? outBase + ".transcoded" + outExt : outFull);
}


//----------------------------------------------------------------------------
// Get a list of all output types, in GUI presentation order.
//----------------------------------------------------------------------------

QList<QtlMovieOutputFile::OutputType> QtlMovieOutputFile::outputTypes()
{
    QList<OutputType> list;
    list << DvdFile << DvdImage << DvdBurn << Ipad << Iphone << Avi << SubRip;
    return list;
}


//----------------------------------------------------------------------------
// Get a user friendly name for an output type.
//----------------------------------------------------------------------------

QString QtlMovieOutputFile::outputTypeName(QtlMovieOutputFile::OutputType outputType)
{
    switch (outputType) {
    case DvdFile:  return tr("DVD MPEG File");
    case DvdImage: return tr("DVD ISO Image");
    case DvdBurn:  return tr("Burn DVD");
    case Ipad:     return tr("iPad");
    case Iphone:   return tr("iPhone");
    case Avi:      return tr("AVI");
    case SubRip:   return tr("SRT Subtitles");
    case None:     return "";
    default:       return "";
    }
}


//----------------------------------------------------------------------------
// Get an identifier name for an output type.
//----------------------------------------------------------------------------

QString QtlMovieOutputFile::outputIdName(QtlMovieOutputFile::OutputType outputType)
{
    // These are identifiers, do not translate.
    switch (outputType) {
    case DvdFile:  return "dvdfile";
    case DvdImage: return "dvdiso";
    case DvdBurn:  return "dvdburn";
    case Ipad:     return "ipad";
    case Iphone:   return "iphone";
    case Avi:      return "avi";
    case SubRip:   return "srt";
    case None:     return "none";
    default:       return "";
    }
}


//----------------------------------------------------------------------------
// Get the default output file extension for a given output type.
//----------------------------------------------------------------------------

QString QtlMovieOutputFile::fileExtension(QtlMovieOutputFile::OutputType outputType)
{
    switch (outputType) {
    case DvdFile:  return ".mpg";
    case DvdImage: return ".iso";
    case DvdBurn:  return ".iso";
    case Ipad:     return ".mp4";
    case Iphone:   return ".mp4";
    case Avi:      return ".avi";
    case SubRip:   return ".srt";
    case None:     return "";
    default:       return "";
    }
}
