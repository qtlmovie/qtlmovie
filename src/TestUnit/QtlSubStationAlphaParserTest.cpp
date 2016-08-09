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
//
// Unit test for class QtlSubStationAlphaParser
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlSubRipGenerator.h"
#include "QtlSubStationAlphaParser.h"
#include "QtlFile.h"
#include <QMetaType>


//----------------------------------------------------------------------------
// Support class: Test one file.
//----------------------------------------------------------------------------

class SubStationAlphaConverterTest : public QObject
{
    Q_OBJECT
public:
    SubStationAlphaConverterTest(const QString& inputFileName, const QString& refOutputFileName, bool useHtml);
private slots:
    void processSubtitleFrame(const QtlSubStationAlphaFramePtr& frame);
private:
    QString            _srtString;
    QTextStream        _srtStream;
    QtlSubRipGenerator _subrip;
    bool               _useHtml;
};


//----------------------------------------------------------------------------
// Test class.
//----------------------------------------------------------------------------

class QtlSubStationAlphaParserTest : public QObject
{
    Q_OBJECT
private slots:
    void testWithHtml();
    void testWithoutHtml();
};

#include "QtlSubStationAlphaParserTest.moc"
QTL_TEST_CLASS(QtlSubStationAlphaParserTest);


void QtlSubStationAlphaParserTest::testWithHtml()
{
    SubStationAlphaConverterTest test(":/test/test-substationalpha.ass", ":/test/test-substationalpha-html.srt", true);
}

void QtlSubStationAlphaParserTest::testWithoutHtml()
{
    SubStationAlphaConverterTest test(":/test/test-substationalpha.ass", ":/test/test-substationalpha-nohtml.srt", false);
}

//----------------------------------------------------------------------------
// Support class: Test one file, implementation.
//----------------------------------------------------------------------------

SubStationAlphaConverterTest::SubStationAlphaConverterTest(const QString& inputFileName, const QString& refOutputFileName, bool useHtml) :
    _srtString(),
    _srtStream(&_srtString, QIODevice::WriteOnly),
    _subrip(&_srtStream),
    _useHtml(useHtml)
{
    // Parse the SSA file, convert into SRT in _srtString.
    QtlSubStationAlphaParser parser;
    connect(&parser, &QtlSubStationAlphaParser::subtitleFrame, this, &SubStationAlphaConverterTest::processSubtitleFrame);
    QVERIFY(parser.addFile(inputFileName));
    _subrip.close();

    // Load reference SRT file.
    QStringList refLines(QtlFile::readTextLinesFile(refOutputFileName));
    QVERIFY(!refLines.isEmpty());

    // Compare with generated SRT file.
    // As a side effect of split, the trailing \n generates an additional line.
    QStringList srtLines(_srtString.split(QRegExp("\\r*\\n")));
    QVERIFY(!srtLines.isEmpty());
    srtLines.removeLast();

    QVERIFY(srtLines == refLines);
}

void SubStationAlphaConverterTest::processSubtitleFrame(const QtlSubStationAlphaFramePtr& frame)
{
    QVERIFY(!frame.isNull());
    _subrip.addFrame(frame->showTimestamp(), frame->hideTimestamp(), frame->toSubRip(_useHtml));
}
