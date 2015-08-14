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
// Define the class QtlTest and the main program of the unit test.
//
//----------------------------------------------------------------------------

#include "QtlTest.h"

//
// The list of all tests.
// The actual object is allocated the first time it is used.
// Using a QList object instead of a pointer would require the guarantee that
// the QList object is initialized before being used, which is not the case.
//
QList<QtlTest::TestEntry>* QtlTest::_allTests = 0;


//----------------------------------------------------------------------------
// Constructor: one test is added in the unit tests.
//----------------------------------------------------------------------------

QtlTest::QtlTest(QObject* testObject, const QString& className)
{
    // Allocate the list the first time.
    if (_allTests == 0) {
        _allTests = new QList<TestEntry>();
    }

    // Add the test.
    _allTests->append(TestEntry(testObject, className));
}


//----------------------------------------------------------------------------
// Comparison operator on test entries (useful for sort operations).
//----------------------------------------------------------------------------

bool QtlTest::TestEntry::operator <(const TestEntry& other) const
{
    return className.compare(other.className, Qt::CaseInsensitive) < 0;
}


//----------------------------------------------------------------------------
// Test unit main program.
//----------------------------------------------------------------------------

int QtlTest::mainProgram(int argc, char** argv)
{
    // Do not remove the QApplication instance. On Windows, it ensures that
    // CoInitialize is invoked and this is required for some classes in Qtl.
    QApplication app(argc, argv);

    // If no test is recorded, success.
    if (_allTests == 0) {
        qWarning("No unit test defined");
        return 1;
    }

    // Sort the list of test per class name.
    qSort(*_allTests);

    // Analyze first argument if any.
    QString className;
    if (argc > 1 && argv[1][0] != '-') {
        // There is at least one argument and this not an option.
        // Check if this name contains "::".
        const char* colon = ::strstr(argv[1], "::");
        if (colon == 0) {
            // "::" not found, first argument is a class name.
            className = QString::fromLatin1(argv[1]);
            // Remove first argument from argument list.
            argc--;
            argv++;
        }
        else {
            // "::" found, extract class name.
            className = QString::fromLatin1(argv[1], colon - argv[1]);
            // Remove class name from first argument.
            argv[1] += colon - argv[1] + 2;
        }
    }

    // Run the unit test.
    if (className.isEmpty()) {
        // Without class name, run all tests.
        return runAllTests(argc, argv);
    }
    else {
        // If arguments are specified, the first one must be a class name.
        // Subsequent arguments are QtTest standard command line arguments.
        return runOneTest(className, argc, argv);
    }
}


//----------------------------------------------------------------------------
// Run all tests.
//----------------------------------------------------------------------------

int QtlTest::runAllTests(int argc, char** argv)
{
    // Build a list of failed classes.
    QStringList failed;

    // Run all tests.
    foreach (const TestEntry& test, *_allTests) {
        if (QTest::qExec(test.testObject, argc, argv) != 0) {
            failed << test.className;
        }
    }

    // Report failed classes.
    if (failed.isEmpty()) {
        qWarning("All tests successfully passed");
    }
    else {
        qCritical("***");
        qCritical("*** Failures in classes: %s", qPrintable(failed.join(", ")));
        qCritical("***");
    }

    // Return the number of failures as exit code (0 means success).
    return failed.size();
}


//----------------------------------------------------------------------------
// Run tests for one class.
//----------------------------------------------------------------------------

int QtlTest::runOneTest(const QString& className, int argc, char** argv)
{
    // Look for the class name.
    // Since the test class for class Xyz is often XyzTest, we accept
    // a class name with or without "Test" suffix.
    foreach (const TestEntry& test, *_allTests) {
        if (test.className.compare(className, Qt::CaseInsensitive) == 0 ||
            test.className.compare(className + "Test", Qt::CaseInsensitive) == 0) {
            // Found the test object for the class, run it.
            return QTest::qExec(test.testObject, argc, argv);
        }
    }

    // Class not found.
    qCritical("Class %s not found", qPrintable(className));
    return 1;
}
