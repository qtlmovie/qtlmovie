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
//! @file QtlTest.h
//!
//! Support macros and class for Qt unit testing.
//!
//----------------------------------------------------------------------------

#ifndef QTLTEST_H
#define QTLTEST_H

#include <QtCore>
#include <QtTest>
#include <QtDebug>
#include <typeinfo>

//!
//! This macro checks that a given statement throws a given exception.
//! Similar to Qt-defined QVERIFY macro but QtTest has no way to verify that an exception is thrown.
//! @param statement The statement to execute.
//! @param exceptionClass The expected exception class.
//!
#define QTLVERIFY_THROW(statement, exceptionClass) \
do { \
    bool caught_ = false; \
    try { \
        (statement); \
    } \
    catch (const exceptionClass& e_) { \
        caught_ = typeid (e_) == typeid (exceptionClass); \
    } \
    catch (...) { \
    } \
    if (!QTest::qVerify(caught_, #statement, "", __FILE__, __LINE__)) { \
        return; \
    } \
} while (0)

//!
//! This macro is used to declare a unit test class.
//!
#define QTL_TEST_CLASS(className) \
    QtlTest qtlTestFor##className(new className(), #className)

//!
//! Support class for unit testing of a class.
//!
class QtlTest
{
public:
    //!
    //! Constructor: one test is added in the unit tests.
    //! Normally used from macro @link QTL_TEST_CLASS @endlink.
    //! @param [in] testObject A test object instance.
    //! @param [in] className Name of the tested class.
    //!
    QtlTest(QObject* testObject, const QString& className);

    //!
    //! Test unit main program.
    //! The command line arguments are: [[classname] [options]
    //!
    //! Where [options] are QtTest standard options (use -help for a list of options).
    //!
    //! @param [in] argc Command line argument count.
    //! @param [in] argv Command line arguments.
    //! @return Exit code.
    //!
    static int mainProgram(int argc, char** argv);

private:
    //!
    //! Describe the unit tests for one class.
    //!
    struct TestEntry
    {
        QObject* testObject;
        QString  className;
        //!
        //! Default constructor.
        //! @param [in] obj Test test object.
        //! @param [in] name The class name.
        //!
        TestEntry(QObject* obj = 0, const QString& name = QString()) :
            testObject(obj),
            className(name)
        {
        }
        //!
        //! Comparison operator (useful for sort operations).
        //! @param [in] other Other instance to compare.
        //! @return True if this object is logically less than @a other
        //! (use lexical order of class names).
        //!
        bool operator<(const TestEntry& other) const;
    };

    //!
    //! List of tests.
    //! Since the list is accessed before main(), we cannot enforce that
    //! the list object is initialized before its used. So we use a pointer.
    //!
    static QList<TestEntry>* _allTests;

    //!
    //! Run all tests.
    //! @param [in] argc Command line argument count.
    //! @param [in] argv Command line arguments (QtTest standard arguments only).
    //! @return Exit code.
    //!
    static int runAllTests(int argc, char** argv);

    //!
    //! Run tests for one class.
    //! @param [in] className Name of test class.
    //! Since the test class for class Xyz is often XyzTest, we accept
    //! a class name with or without "Test" suffix.
    //! @param [in] argc Command line argument count.
    //! @param [in] argv Command line arguments (QtTest standard arguments only).
    //! @return Exit code.
    //!
    static int runOneTest(const QString& className, int argc, char** argv);

    // Unaccessible operations.
    QtlTest() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlTest)
};

#endif // QTLTEST_H
