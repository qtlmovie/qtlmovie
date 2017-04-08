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
// Qtl, Qt utility library.
// Define the class QtlSubRipGenerator.
//
//----------------------------------------------------------------------------

#include "QtlSubRipGenerator.h"


//----------------------------------------------------------------------------
// Constructors and destructor.
//----------------------------------------------------------------------------

QtlSubRipGenerator::QtlSubRipGenerator(const QString& fileName) :
    _outputFile(),
    _outputStream(&_outputFile),
    _stream(0),
    _frameCount(0)
{
    open(fileName);
}

QtlSubRipGenerator::QtlSubRipGenerator(QTextStream* stream) :
    _outputFile(),
    _outputStream(&_outputFile),
    _stream(0),
    _frameCount(0)
{
    setStream(stream);
}

QtlSubRipGenerator::~QtlSubRipGenerator()
{
    close();
}


//----------------------------------------------------------------------------
// Open or re-open the generator.
//----------------------------------------------------------------------------

bool QtlSubRipGenerator::open(const QString& fileName)
{
    close();
    _outputFile.setFileName(fileName);
    const bool ok = !fileName.isEmpty() && _outputFile.open(QFile::WriteOnly);
    if (ok) {
        _outputStream.reset();
        _outputStream.resetStatus();
        _stream = &_outputStream;
        configureStream();
    }
    return ok;
}

bool QtlSubRipGenerator::setStream(QTextStream* stream)
{
    close();
    _stream = stream;
    configureStream();
    return _stream != 0;
}


//----------------------------------------------------------------------------
// Configure the output stream.
//----------------------------------------------------------------------------

void QtlSubRipGenerator::configureStream()
{
    if (_stream != 0) {
        _stream->setCodec("UTF-8");
        _stream->setGenerateByteOrderMark(true);
    }
}


//----------------------------------------------------------------------------
// Close the generator.
//----------------------------------------------------------------------------

void QtlSubRipGenerator::close()
{
    if (_stream != 0) {
        _stream->flush();
        _stream = 0;
    }
    if (_outputFile.isOpen()) {
        _outputFile.close();
    }
    _frameCount = 0;
}


//----------------------------------------------------------------------------
// Add a multi-lines subtitle frame.
//----------------------------------------------------------------------------

void QtlSubRipGenerator::addFrame(quint64 showTimestamp, quint64 hideTimestamp, const QStringList& lines)
{
    // Empty lines are illegal in SRT. Make sure we have at least one non-empty line.
    bool notEmpty = false;
    foreach (const QString& line, lines) {
        if (!line.isEmpty()) {
            notEmpty = true;
            break;
        }
    }

    // Generate the frame only when it is possible to do so.
    if (notEmpty && _stream != 0) {
        // First line: Frame count, starting at 1.
        // Second line: Start and end timestamps.
        *_stream << ++_frameCount << endl
                 << formatDuration(showTimestamp, hideTimestamp) << endl;
        // Subsequent lines: Subtitle text.
        foreach (const QString& line, lines) {
            // Empty lines are illegal in SRT, skip them.
            if (!line.isEmpty()) {
                *_stream << line << endl;
            }
        }
        // Trailing empty line to mark the end of frame.
        *_stream << endl;
    }
}


//----------------------------------------------------------------------------
// Format SRT times.
//----------------------------------------------------------------------------

QString QtlSubRipGenerator::formatTime(quint64 timestamp)
{
    // Tims stamp is in milliseconds.
    const int h = int(timestamp / 3600000);
    const int m = int(timestamp / 60000 - 60 * h);
    const int s = int(timestamp / 1000 - 3600 * h - 60 * m);
    const int u = int(timestamp - 3600000 * h - 60000 * m - 1000 * s);
    return QStringLiteral("%1:%2:%3,%4")
            .arg(h, 2, 10, QLatin1Char('0'))
            .arg(m, 2, 10, QLatin1Char('0'))
            .arg(s, 2, 10, QLatin1Char('0'))
            .arg(u, 3, 10, QLatin1Char('0'));
}

QString QtlSubRipGenerator::formatDuration(quint64 showTimestamp, quint64 hideTimestamp)
{
    return formatTime(showTimestamp) + " --> " + formatTime(hideTimestamp);
}
