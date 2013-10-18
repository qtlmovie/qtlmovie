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
// Main program for the QtlMovie application.
//
//----------------------------------------------------------------------------

#include "QtlMovieMainWindow.h"
#include "QtlTranslator.h"
#include "QtsTsPacket.h"
#include <QtCore>
#include <QtDebug>

int main(int argc, char *argv[])
{
    // Application initialization.
    QApplication app(argc, argv);

    // Decode command line arguments.
    const QStringList args(QCoreApplication::arguments());
    QString locale;
    QString inputFile;
    for (int i = 1; i < args.size(); ++i) {
        if (args[i] == "-l" && i+1 < args.size()) {
            // Option "-l" means locale name
            locale = args[++i];
        }
        else if (!args[i].startsWith("-")) {
            // Not an option, this is the input file.
            inputFile = args[i];
        }
        else {
            qWarning() << "Unknow option:" << args[i];
        }
    }

    // Install translations.
    QtlTranslator trQt("qt", locale);
    QtlTranslator trQtl("qtl", locale);
    QtlTranslator trQts("qts", locale);
    QtlTranslator trQtlMovie("qtlmovie", locale);

    // Various sanity checks.
    QtsTsPacket::sanityCheck();

    // Run the application GUI.
    QtlMovieMainWindow win(0, inputFile);
    win.show();
    return app.exec();
}
