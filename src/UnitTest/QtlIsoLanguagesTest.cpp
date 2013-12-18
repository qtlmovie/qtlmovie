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
// Unit test for class QtlIsoLanguages
//
//----------------------------------------------------------------------------

#include "QtlTest.h"
#include "QtlIsoLanguages.h"

class QtlIsoLanguagesTest : public QObject
{
    Q_OBJECT
private slots:
    void testInstance();
    void testLanguageName();
    void testCountryName();
    void testLanguage2LetterCodes();
    void testLanguage3LetterCodes();
    void testCountry2LetterCodes();
    void testCountry3LetterCodes();
    void testCountry3DigitCodes();
};

#include "QtlIsoLanguagesTest.moc"
QTL_TEST_CLASS(QtlIsoLanguagesTest);

//----------------------------------------------------------------------------

void QtlIsoLanguagesTest::testInstance()
{
    const QtlIsoLanguages* instance1 = QtlIsoLanguages::instance();
    const QtlIsoLanguages* instance2 = QtlIsoLanguages::instance();

    QVERIFY(instance1 != 0);
    QVERIFY(instance1 == instance2);
}


void QtlIsoLanguagesTest::testLanguageName()
{
    QVERIFY(QtlIsoLanguages::instance()->languageName("en") == "English");
    QVERIFY(QtlIsoLanguages::instance()->languageName("eng") == "English");
    QVERIFY(QtlIsoLanguages::instance()->languageName("fr") == "French");
    QVERIFY(QtlIsoLanguages::instance()->languageName("fre") == "French");
    QVERIFY(QtlIsoLanguages::instance()->languageName("fra") == "French");
    QVERIFY(QtlIsoLanguages::instance()->languageName("FR") == "French");
    QVERIFY(QtlIsoLanguages::instance()->languageName("Fre") == "French");
    QVERIFY(QtlIsoLanguages::instance()->languageName("FRa") == "French");
    QVERIFY(QtlIsoLanguages::instance()->languageName("") == "");
    QVERIFY(QtlIsoLanguages::instance()->languageName("foo") == "foo");
    QVERIFY(QtlIsoLanguages::instance()->languageName("foo", "") == "");
    QVERIFY(QtlIsoLanguages::instance()->languageName("foo", "bar") == "bar");
}

void QtlIsoLanguagesTest::testCountryName()
{
    QVERIFY(QtlIsoLanguages::instance()->countryName("ad") == "Andorra");
    QVERIFY(QtlIsoLanguages::instance()->countryName("and") == "Andorra");
    QVERIFY(QtlIsoLanguages::instance()->countryName("020") == "Andorra");
    QVERIFY(QtlIsoLanguages::instance()->countryName("AD") == "Andorra");
    QVERIFY(QtlIsoLanguages::instance()->countryName("And") == "Andorra");
    QVERIFY(QtlIsoLanguages::instance()->countryName("") == "");
    QVERIFY(QtlIsoLanguages::instance()->countryName("20") == "20");
    QVERIFY(QtlIsoLanguages::instance()->countryName("20", "") == "");
    QVERIFY(QtlIsoLanguages::instance()->countryName("20", "foo") == "foo");
}

void QtlIsoLanguagesTest::testLanguage2LetterCodes()
{
    const QStringList languages2Letter(QtlIsoLanguages::instance()->language2LetterCodes());

    QVERIFY(languages2Letter.size() == 173);

    foreach (const QString& code, languages2Letter) {
        QVERIFY(code.length() == 2);
        foreach (QChar c, code) {
            QVERIFY(c.isLower());
        }
    }
}

void QtlIsoLanguagesTest::testLanguage3LetterCodes()
{
    const QStringList languages3Letter(QtlIsoLanguages::instance()->language3LetterCodes());

    QVERIFY(languages3Letter.size() == 192);

    foreach (const QString& code, languages3Letter) {
        QVERIFY(code.length() == 3);
        foreach (QChar c, code) {
            QVERIFY(c.isLower());
        }
    }
}

void QtlIsoLanguagesTest::testCountry2LetterCodes()
{
    const QStringList country2Letter(QtlIsoLanguages::instance()->country2LetterCodes());

    QVERIFY(country2Letter.size() == 239);

    foreach (const QString& code, country2Letter) {
        QVERIFY(code.length() == 2);
        foreach (QChar c, code) {
            QVERIFY(c.isUpper());
        }
    }
}

void QtlIsoLanguagesTest::testCountry3LetterCodes()
{
    const QStringList country3Letter(QtlIsoLanguages::instance()->country3LetterCodes());

    QVERIFY(country3Letter.size() == 239);

    foreach (const QString& code, country3Letter) {
        QVERIFY(code.length() == 3);
        foreach (QChar c, code) {
            QVERIFY(c.isUpper());
        }
    }
}

void QtlIsoLanguagesTest::testCountry3DigitCodes()
{
    const QStringList country3Digit(QtlIsoLanguages::instance()->country3DigitCodes());

    QVERIFY(country3Digit.size() == 239);

    foreach (const QString& code, country3Digit) {
        QVERIFY(code.length() == 3);
        foreach (QChar c, code) {
            QVERIFY(c.isDigit());
        }
    }
}
