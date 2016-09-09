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
//
// Command line tool to test the parsing of SSA/ASS subtitles.
//
//----------------------------------------------------------------------------

#include "TestToolCommand.h"
#include "QtlSubStationAlphaParser.h"
#include "QtlSubRipGenerator.h"

class TestSsa : public TestToolCommand
{
    Q_OBJECT
public:
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;

    TestSsa() :
        TestToolCommand("ssa",
                        "file-name [-html]",
                        "Parse an SSA/ASS subtitles file and convert to SubRip (SRT).\n"
                        "With -html, add HTML tags in the SubRip output."),
        _useHtml(false)
    {
    }

private slots:
    void sectionChanged(const QString& sectionName);
    void subtitleFrame(const QtlSubStationAlphaFramePtr& frame);

private:
    bool _useHtml;
};

//----------------------------------------------------------------------------

int TestSsa::run(const QStringList& args)
{
    if (args.empty()) {
        return syntaxError();
    }

    QString fileName(args[0]);
    _useHtml = args.size() > 1 && args[1] == "-html";

    QtlSubStationAlphaParser parser(&log);
    connect(&parser, &QtlSubStationAlphaParser::sectionChanged, this, &TestSsa::sectionChanged);
    connect(&parser, &QtlSubStationAlphaParser::subtitleFrame, this, &TestSsa::subtitleFrame);
    parser.addFile(fileName);

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

void TestSsa::sectionChanged(const QString& sectionName)
{
    out << "Found section \"" << sectionName << "\"" << endl;
}

//----------------------------------------------------------------------------

void TestSsa::subtitleFrame(const QtlSubStationAlphaFramePtr& frame)
{
    out << "Found subtitle frame: " << QtlSubRipGenerator::formatDuration(frame->showTimestamp(), frame->hideTimestamp()) << endl
        << "    SSA text: " << frame->text() << endl
        << "    SRT text: " << frame->toSubRip(_useHtml).join("\n              ") << endl;
}

//----------------------------------------------------------------------------

#include "TestSsa.moc"
namespace {TestSsa thisTest;}
