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
// Qtl, Qt utility library.
// Define the class QtlIsoLanguages.
//
//----------------------------------------------------------------------------

#include "QtlIsoLanguages.h"


//----------------------------------------------------------------------------
// Static member initialization.
//----------------------------------------------------------------------------

const QtlIsoLanguages* QtlIsoLanguages::_instance(0);
QMutex QtlIsoLanguages::_instanceMutex;


//----------------------------------------------------------------------------
// Get the single instance of QtlIsoLanguages.
//----------------------------------------------------------------------------

const QtlIsoLanguages*QtlIsoLanguages::instance()
{
    // Double-check locking.
    if (_instance == 0) {
        QMutexLocker lock(&_instanceMutex);
        if (_instance == 0) {
            _instance = new QtlIsoLanguages();
        }
    }
    return _instance;
}


//----------------------------------------------------------------------------
// Get the sorted list of all codes for a given map.
//----------------------------------------------------------------------------

QStringList QtlIsoLanguages::codeList(const QtlIsoLanguages::StringMap& map) const
{
    QStringList keys(map.keys());
    keys.sort(Qt::CaseInsensitive);
    return keys;
}


//----------------------------------------------------------------------------
// Get a localized language name from a language code.
//----------------------------------------------------------------------------

QString QtlIsoLanguages::languageName(const QString& code, const QString& defaultValue) const
{
    const QString& def(defaultValue.isNull() ? code : defaultValue);
    const int len = code.length();
    if (len == 2) {
        return _languageBy2Letter.value(code.toLower(), def);
    }
    else if (len == 3) {
        return _languageBy3Letter.value(code.toLower(), def);
    }
    else {
        return def;
    }
}


//----------------------------------------------------------------------------
// Get a localized country name from a country code.
//----------------------------------------------------------------------------

QString QtlIsoLanguages::countryName(const QString& code, const QString& defaultValue) const
{
    const QString& def(defaultValue.isNull() ? code : defaultValue);
    const int len = code.length();
    if (len == 2) {
        return _countryBy2Letter.value(code.toUpper(), def);
    }
    else if (len == 3) {
        // If any character is not a digit, use 3-letter code.
        for (int i = 0; i < len; i++) {
            if (!code.at(i).isDigit()) {
                return _countryBy3Letter.value(code.toUpper(), def);
            }
        }
        // All characters are digits, use 3-digit code.
        return _countryBy3Digit.value(code.toUpper(), def);
    }
    else {
        return def;
    }
}


//----------------------------------------------------------------------------
// Add an ISO 639 language code.
//----------------------------------------------------------------------------

void QtlIsoLanguages::addLanguage(const QString& code3, const QString& code3alt, const QString& code2, const QString& name)
{
    if (!name.isEmpty()) {
        if (!code3.isEmpty()) {
            _languageBy3Letter.insert(code3, name);
        }
        if (!code3alt.isEmpty()) {
            _languageBy3Letter.insert(code3alt, name);
        }
        if (!code2.isEmpty()) {
            _languageBy2Letter.insert(code2, name);
        }
    }
}


//----------------------------------------------------------------------------
// Add an ISO 3166-1 country code.
//----------------------------------------------------------------------------

void QtlIsoLanguages::addCountry(const QString& code2, const QString& code3, const QString& digit3, const QString& name)
{
    if (!name.isEmpty()) {
        if (!code2.isEmpty()) {
            _countryBy2Letter.insert(code2, name);
        }
        if (!code3.isEmpty()) {
            _countryBy3Letter.insert(code3, name);
        }
        if (!digit3.isEmpty()) {
            _countryBy3Digit.insert(digit3, name);
        }
    }
}


//----------------------------------------------------------------------------
// Private constructor.
//----------------------------------------------------------------------------

QtlIsoLanguages::QtlIsoLanguages()
{
    // Add coutry codes.
    // Based on http://www.davros.org/misc/iso3166.txt
    // "Table 1: current codes" only.

    addCountry("AF", "AFG", "004", QObject::tr("Afghanistan"));
    addCountry("AL", "ALB", "008", QObject::tr("Albania"));
    addCountry("DZ", "DZA", "012", QObject::tr("Algeria"));
    addCountry("AS", "ASM", "016", QObject::tr("American Samoa"));
    addCountry("AD", "AND", "020", QObject::tr("Andorra"));
    addCountry("AO", "AGO", "024", QObject::tr("Angola"));
    addCountry("AI", "AIA", "660", QObject::tr("Anguilla"));
    addCountry("AQ", "ATA", "010", QObject::tr("Antarctica"));
    addCountry("AG", "ATG", "028", QObject::tr("Antigua and Barbuda"));
    addCountry("AR", "ARG", "032", QObject::tr("Argentina"));
    addCountry("AM", "ARM", "051", QObject::tr("Armenia"));
    addCountry("AW", "ABW", "533", QObject::tr("Aruba"));
    addCountry("AU", "AUS", "036", QObject::tr("Australia"));
    addCountry("AT", "AUT", "040", QObject::tr("Austria"));
    addCountry("AZ", "AZE", "031", QObject::tr("Azerbaijan"));
    addCountry("BS", "BHS", "044", QObject::tr("Bahamas"));
    addCountry("BH", "BHR", "048", QObject::tr("Bahrain"));
    addCountry("BD", "BGD", "050", QObject::tr("Bangladesh"));
    addCountry("BB", "BRB", "052", QObject::tr("Barbados"));
    addCountry("BY", "BLR", "112", QObject::tr("Belarus"));
    addCountry("BE", "BEL", "056", QObject::tr("Belgium"));
    addCountry("BZ", "BLZ", "084", QObject::tr("Belize"));
    addCountry("BJ", "BEN", "204", QObject::tr("Benin"));
    addCountry("BM", "BMU", "060", QObject::tr("Bermuda"));
    addCountry("BT", "BTN", "064", QObject::tr("Bhutan"));
    addCountry("BO", "BOL", "068", QObject::tr("Bolivia"));
    addCountry("BA", "BIH", "070", QObject::tr("Bosnia and Herzegovina"));
    addCountry("BW", "BWA", "072", QObject::tr("Botswana"));
    addCountry("BV", "BVT", "074", QObject::tr("Bouvet Island"));
    addCountry("BR", "BRA", "076", QObject::tr("Brazil"));
    addCountry("IO", "IOT", "086", QObject::tr("British Indian Ocean Territory"));
    addCountry("VG", "VGB", "092", QObject::tr("British Virgin Islands"));
    addCountry("BN", "BRN", "096", QObject::tr("Brunei"));
    addCountry("BG", "BGR", "100", QObject::tr("Bulgaria"));
    addCountry("BF", "BFA", "854", QObject::tr("Burkina Faso"));
    addCountry("BI", "BDI", "108", QObject::tr("Burundi"));
    addCountry("KH", "KHM", "116", QObject::tr("Cambodia"));
    addCountry("CM", "CMR", "120", QObject::tr("Cameroon"));
    addCountry("CA", "CAN", "124", QObject::tr("Canada"));
    addCountry("CV", "CPV", "132", QObject::tr("Cape Verde"));
    addCountry("KY", "CYM", "136", QObject::tr("Cayman Islands"));
    addCountry("CF", "CAF", "140", QObject::tr("Central African Republic"));
    addCountry("TD", "TCD", "148", QObject::tr("Chad"));
    addCountry("CL", "CHL", "152", QObject::tr("Chile"));
    addCountry("CN", "CHN", "156", QObject::tr("China"));
    addCountry("CX", "CXR", "162", QObject::tr("Christmas Island"));
    addCountry("CC", "CCK", "166", QObject::tr("Cocos Islands"));
    addCountry("CO", "COL", "170", QObject::tr("Colombia"));
    addCountry("KM", "COM", "174", QObject::tr("Comoros"));
    addCountry("CD", "COD", "180", QObject::tr("Congo-Kinshasa"));
    addCountry("CG", "COG", "178", QObject::tr("Congo-Brazzaville"));
    addCountry("CK", "COK", "184", QObject::tr("Cook Islands"));
    addCountry("CR", "CRI", "188", QObject::tr("Costa Rica"));
    addCountry("CI", "CIV", "384", QObject::tr("Ivory Coast"));
    addCountry("CU", "CUB", "192", QObject::tr("Cuba"));
    addCountry("CY", "CYP", "196", QObject::tr("Cyprus"));
    addCountry("CZ", "CZE", "203", QObject::tr("Czech Republic"));
    addCountry("DK", "DNK", "208", QObject::tr("Denmark"));
    addCountry("DJ", "DJI", "262", QObject::tr("Djibouti"));
    addCountry("DM", "DMA", "212", QObject::tr("Dominica"));
    addCountry("DO", "DOM", "214", QObject::tr("Dominican Republic"));
    addCountry("EC", "ECU", "218", QObject::tr("Ecuador"));
    addCountry("EG", "EGY", "818", QObject::tr("Egypt"));
    addCountry("SV", "SLV", "222", QObject::tr("El Salvador"));
    addCountry("GQ", "GNQ", "226", QObject::tr("Equatorial Guinea"));
    addCountry("ER", "ERI", "232", QObject::tr("Eritrea"));
    addCountry("EE", "EST", "233", QObject::tr("Estonia"));
    addCountry("ET", "ETH", "231", QObject::tr("Ethiopia"));
    addCountry("FO", "FRO", "234", QObject::tr("Faeroe Islands"));
    addCountry("FK", "FLK", "238", QObject::tr("Falklands"));
    addCountry("FJ", "FJI", "242", QObject::tr("Fiji"));
    addCountry("FI", "FIN", "246", QObject::tr("Finland"));
    addCountry("FR", "FRA", "250", QObject::tr("France"));
    addCountry("GF", "GUF", "254", QObject::tr("French Guiana"));
    addCountry("PF", "PYF", "258", QObject::tr("French Polynesia"));
    addCountry("TF", "ATF", "260", QObject::tr("French Southern Territories"));
    addCountry("GA", "GAB", "266", QObject::tr("Gabon"));
    addCountry("GM", "GMB", "270", QObject::tr("Gambia"));
    addCountry("GE", "GEO", "268", QObject::tr("Georgia"));
    addCountry("DE", "DEU", "276", QObject::tr("Germany"));
    addCountry("GH", "GHA", "288", QObject::tr("Ghana"));
    addCountry("GI", "GIB", "292", QObject::tr("Gibraltar"));
    addCountry("GR", "GRC", "300", QObject::tr("Greece"));
    addCountry("GL", "GRL", "304", QObject::tr("Greenland"));
    addCountry("GD", "GRD", "308", QObject::tr("Grenada"));
    addCountry("GP", "GLP", "312", QObject::tr("Guadaloupe"));
    addCountry("GU", "GUM", "316", QObject::tr("Guam"));
    addCountry("GT", "GTM", "320", QObject::tr("Guatemala"));
    addCountry("GN", "GIN", "324", QObject::tr("Guinea"));
    addCountry("GW", "GNB", "624", QObject::tr("Guinea-Bissau"));
    addCountry("GY", "GUY", "328", QObject::tr("Guyana"));
    addCountry("HT", "HTI", "332", QObject::tr("Haiti"));
    addCountry("HM", "HMD", "334", QObject::tr("Heard and McDonald Islands"));
    addCountry("VA", "VAT", "336", QObject::tr("Vatican"));
    addCountry("HN", "HND", "340", QObject::tr("Honduras"));
    addCountry("HK", "HKG", "344", QObject::tr("Hong Kong"));
    addCountry("HR", "HRV", "191", QObject::tr("Croatia"));
    addCountry("HU", "HUN", "348", QObject::tr("Hungary"));
    addCountry("IS", "ISL", "352", QObject::tr("Iceland"));
    addCountry("IN", "IND", "356", QObject::tr("India"));
    addCountry("ID", "IDN", "360", QObject::tr("Indonesia"));
    addCountry("IR", "IRN", "364", QObject::tr("Iran"));
    addCountry("IQ", "IRQ", "368", QObject::tr("Iraq"));
    addCountry("IE", "IRL", "372", QObject::tr("Ireland"));
    addCountry("IL", "ISR", "376", QObject::tr("Israel"));
    addCountry("IT", "ITA", "380", QObject::tr("Italy"));
    addCountry("JM", "JAM", "388", QObject::tr("Jamaica"));
    addCountry("JP", "JPN", "392", QObject::tr("Japan"));
    addCountry("JO", "JOR", "400", QObject::tr("Jordan"));
    addCountry("KZ", "KAZ", "398", QObject::tr("Kazakhstan"));
    addCountry("KE", "KEN", "404", QObject::tr("Kenya"));
    addCountry("KI", "KIR", "296", QObject::tr("Kiribati"));
    addCountry("KP", "PRK", "408", QObject::tr("North Korea"));
    addCountry("KR", "KOR", "410", QObject::tr("South Korea"));
    addCountry("KW", "KWT", "414", QObject::tr("Kuwait"));
    addCountry("KG", "KGZ", "417", QObject::tr("Kyrgyz Republic"));
    addCountry("LA", "LAO", "418", QObject::tr("Laos"));
    addCountry("LV", "LVA", "428", QObject::tr("Latvia"));
    addCountry("LB", "LBN", "422", QObject::tr("Lebanon"));
    addCountry("LS", "LSO", "426", QObject::tr("Lesotho"));
    addCountry("LR", "LBR", "430", QObject::tr("Liberia"));
    addCountry("LY", "LBY", "434", QObject::tr("Libya"));
    addCountry("LI", "LIE", "438", QObject::tr("Liechtenstein"));
    addCountry("LT", "LTU", "440", QObject::tr("Lithuania"));
    addCountry("LU", "LUX", "442", QObject::tr("Luxembourg"));
    addCountry("MO", "MAC", "446", QObject::tr("Macao"));
    addCountry("MK", "MKD", "807", QObject::tr("Macedonia"));
    addCountry("MG", "MDG", "450", QObject::tr("Madagascar"));
    addCountry("MW", "MWI", "454", QObject::tr("Malawi"));
    addCountry("MY", "MYS", "458", QObject::tr("Malaysia"));
    addCountry("MV", "MDV", "462", QObject::tr("Maldives"));
    addCountry("ML", "MLI", "466", QObject::tr("Mali"));
    addCountry("MT", "MLT", "470", QObject::tr("Malta"));
    addCountry("MH", "MHL", "584", QObject::tr("Marshall Islands"));
    addCountry("MQ", "MTQ", "474", QObject::tr("Martinique"));
    addCountry("MR", "MRT", "478", QObject::tr("Mauritania"));
    addCountry("MU", "MUS", "480", QObject::tr("Mauritius"));
    addCountry("YT", "MYT", "175", QObject::tr("Mayotte"));
    addCountry("MX", "MEX", "484", QObject::tr("Mexico"));
    addCountry("FM", "FSM", "583", QObject::tr("Micronesia"));
    addCountry("MD", "MDA", "498", QObject::tr("Moldova"));
    addCountry("MC", "MCO", "492", QObject::tr("Monaco"));
    addCountry("MN", "MNG", "496", QObject::tr("Mongolia"));
    addCountry("MS", "MSR", "500", QObject::tr("Montserrat"));
    addCountry("MA", "MAR", "504", QObject::tr("Morocco"));
    addCountry("MZ", "MOZ", "508", QObject::tr("Mozambique"));
    addCountry("MM", "MMR", "104", QObject::tr("Myanmar"));
    addCountry("NA", "NAM", "516", QObject::tr("Namibia"));
    addCountry("NR", "NRU", "520", QObject::tr("Nauru"));
    addCountry("NP", "NPL", "524", QObject::tr("Nepal"));
    addCountry("AN", "ANT", "530", QObject::tr("Netherlands Antilles"));
    addCountry("NL", "NLD", "528", QObject::tr("Netherlands"));
    addCountry("NC", "NCL", "540", QObject::tr("New Caledonia"));
    addCountry("NZ", "NZL", "554", QObject::tr("New Zealand"));
    addCountry("NI", "NIC", "558", QObject::tr("Nicaragua"));
    addCountry("NE", "NER", "562", QObject::tr("Niger"));
    addCountry("NG", "NGA", "566", QObject::tr("Nigeria"));
    addCountry("NU", "NIU", "570", QObject::tr("Niue"));
    addCountry("NF", "NFK", "574", QObject::tr("Norfolk Island"));
    addCountry("MP", "MNP", "580", QObject::tr("Northern Mariana Islands"));
    addCountry("NO", "NOR", "578", QObject::tr("Norway"));
    addCountry("OM", "OMN", "512", QObject::tr("Oman"));
    addCountry("PK", "PAK", "586", QObject::tr("Pakistan"));
    addCountry("PW", "PLW", "585", QObject::tr("Palau"));
    addCountry("PS", "PSE", "275", QObject::tr("Palestine"));
    addCountry("PA", "PAN", "591", QObject::tr("Panama"));
    addCountry("PG", "PNG", "598", QObject::tr("Papua New Guinea"));
    addCountry("PY", "PRY", "600", QObject::tr("Paraguay"));
    addCountry("PE", "PER", "604", QObject::tr("Peru"));
    addCountry("PH", "PHL", "608", QObject::tr("Philippines"));
    addCountry("PN", "PCN", "612", QObject::tr("Pitcairn Island"));
    addCountry("PL", "POL", "616", QObject::tr("Poland"));
    addCountry("PT", "PRT", "620", QObject::tr("Portugal"));
    addCountry("PR", "PRI", "630", QObject::tr("Puerto Rico"));
    addCountry("QA", "QAT", "634", QObject::tr("Qatar"));
    addCountry("RE", "REU", "638", QObject::tr("Reunion"));
    addCountry("RO", "ROU", "642", QObject::tr("Romania"));
    addCountry("RU", "RUS", "643", QObject::tr("Russia"));
    addCountry("RW", "RWA", "646", QObject::tr("Rwanda"));
    addCountry("SH", "SHN", "654", QObject::tr("St. Helena"));
    addCountry("KN", "KNA", "659", QObject::tr("St. Kitts and Nevis"));
    addCountry("LC", "LCA", "662", QObject::tr("St. Lucia"));
    addCountry("PM", "SPM", "666", QObject::tr("St. Pierre and Miquelon"));
    addCountry("VC", "VCT", "670", QObject::tr("St. Vincent and the Grenadines"));
    addCountry("WS", "WSM", "882", QObject::tr("Samoa"));
    addCountry("SM", "SMR", "674", QObject::tr("San Marino"));
    addCountry("ST", "STP", "678", QObject::tr("Sao Tome and Principe"));
    addCountry("SA", "SAU", "682", QObject::tr("Saudi Arabia"));
    addCountry("SN", "SEN", "686", QObject::tr("Senegal"));
    addCountry("CS", "SCG", "891", QObject::tr("Serbia and Montenegro"));
    addCountry("SC", "SYC", "690", QObject::tr("Seychelles"));
    addCountry("SL", "SLE", "694", QObject::tr("Sierra Leone"));
    addCountry("SG", "SGP", "702", QObject::tr("Singapore"));
    addCountry("SK", "SVK", "703", QObject::tr("Slovakia"));
    addCountry("SI", "SVN", "705", QObject::tr("Slovenia"));
    addCountry("SB", "SLB", "090", QObject::tr("Solomon Islands"));
    addCountry("SO", "SOM", "706", QObject::tr("Somalia"));
    addCountry("ZA", "ZAF", "710", QObject::tr("South Africa"));
    addCountry("GS", "SGS", "239", QObject::tr("South Georgia and the South Sandwich Islands"));
    addCountry("ES", "ESP", "724", QObject::tr("Spain"));
    addCountry("LK", "LKA", "144", QObject::tr("Sri Lanka"));
    addCountry("SD", "SDN", "736", QObject::tr("Sudan"));
    addCountry("SR", "SUR", "740", QObject::tr("Suriname"));
    addCountry("SJ", "SJM", "744", QObject::tr("Svalbard & Jan Mayen Islands"));
    addCountry("SZ", "SWZ", "748", QObject::tr("Swaziland"));
    addCountry("SE", "SWE", "752", QObject::tr("Sweden"));
    addCountry("CH", "CHE", "756", QObject::tr("Switzerland"));
    addCountry("SY", "SYR", "760", QObject::tr("Syria"));
    addCountry("TW", "TWN", "158", QObject::tr("Taiwan"));
    addCountry("TJ", "TJK", "762", QObject::tr("Tajikistan"));
    addCountry("TZ", "TZA", "834", QObject::tr("Tanzania"));
    addCountry("TH", "THA", "764", QObject::tr("Thailand"));
    addCountry("TL", "TLS", "626", QObject::tr("Timor-Leste"));
    addCountry("TG", "TGO", "768", QObject::tr("Togo"));
    addCountry("TK", "TKL", "772", QObject::tr("Tokelau Islands"));
    addCountry("TO", "TON", "776", QObject::tr("Tonga"));
    addCountry("TT", "TTO", "780", QObject::tr("Trinidad and Tobago"));
    addCountry("TN", "TUN", "788", QObject::tr("Tunisia"));
    addCountry("TR", "TUR", "792", QObject::tr("Turkey"));
    addCountry("TM", "TKM", "795", QObject::tr("Turkmenistan"));
    addCountry("TC", "TCA", "796", QObject::tr("Turks and Caicos Islands"));
    addCountry("TV", "TUV", "798", QObject::tr("Tuvalu"));
    addCountry("VI", "VIR", "850", QObject::tr("US Virgin Islands"));
    addCountry("UG", "UGA", "800", QObject::tr("Uganda"));
    addCountry("UA", "UKR", "804", QObject::tr("Ukraine"));
    addCountry("AE", "ARE", "784", QObject::tr("United Arab Emirates"));
    addCountry("GB", "GBR", "826", QObject::tr("Great Britain"));
    addCountry("UM", "UMI", "581", QObject::tr("United States Minor Outlying Islands"));
    addCountry("US", "USA", "840", QObject::tr("United States of America"));
    addCountry("UY", "URY", "858", QObject::tr("Uruguay"));
    addCountry("UZ", "UZB", "860", QObject::tr("Uzbekistan"));
    addCountry("VU", "VUT", "548", QObject::tr("Vanuatu"));
    addCountry("VE", "VEN", "862", QObject::tr("Venezuela"));
    addCountry("VN", "VNM", "704", QObject::tr("Viet Nam"));
    addCountry("WF", "WLF", "876", QObject::tr("Wallis and Futuna"));
    addCountry("EH", "ESH", "732", QObject::tr("Western Sahara"));
    addCountry("YE", "YEM", "887", QObject::tr("Yemen"));
    addCountry("ZM", "ZMB", "894", QObject::tr("Zambia"));
    addCountry("ZW", "ZWE", "716", QObject::tr("Zimbabwe"));

    // Add language codes.
    // Based on http://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt
    // Ancient and rare languages are not added.
    // Specifically, all languages without a 2-letter code are ignored.

    addLanguage("abk",    "", "ab", QObject::tr("Abkhazian"));
    addLanguage("afr",    "", "af", QObject::tr("Afrikaans"));
    addLanguage("alb", "sqi", "sq", QObject::tr("Albanian"));
    addLanguage("ara",    "", "ar", QObject::tr("Arabic"));
    addLanguage("arg",    "", "an", QObject::tr("Aragonese"));
    addLanguage("arm", "hye", "hy", QObject::tr("Armenian"));
    addLanguage("aze",    "", "az", QObject::tr("Azerbaijani"));
    addLanguage("bam",    "", "bm", QObject::tr("Bambara"));
    addLanguage("baq", "eus", "eu", QObject::tr("Basque"));
    addLanguage("bel",    "", "be", QObject::tr("Belarusian"));
    addLanguage("ben",    "", "bn", QObject::tr("Bengali"));
    addLanguage("bih",    "", "bh", QObject::tr("Bihari"));
    addLanguage("bis",    "", "bi", QObject::tr("Bislama"));
    addLanguage("bos",    "", "bs", QObject::tr("Bosnian"));
    addLanguage("bre",    "", "br", QObject::tr("Breton"));
    addLanguage("bul",    "", "bg", QObject::tr("Bulgarian"));
    addLanguage("bur", "mya", "my", QObject::tr("Burmese"));
    addLanguage("cat",    "", "ca", QObject::tr("Catalan"));
    addLanguage("cha",    "", "ch", QObject::tr("Chamorro"));
    addLanguage("che",    "", "ce", QObject::tr("Chechen"));
    addLanguage("chi", "zho", "zh", QObject::tr("Chinese"));
    addLanguage("chv",    "", "cv", QObject::tr("Chuvash"));
    addLanguage("cor",    "", "kw", QObject::tr("Cornish"));
    addLanguage("cos",    "", "co", QObject::tr("Corsican"));
    addLanguage("cre",    "", "cr", QObject::tr("Cree"));
    addLanguage("cze", "ces", "cs", QObject::tr("Czech"));
    addLanguage("dan",    "", "da", QObject::tr("Danish"));
    addLanguage("div",    "", "dv", QObject::tr("Divehi"));
    addLanguage("dut", "nld", "nl", QObject::tr("Dutch"));
    addLanguage("dzo",    "", "dz", QObject::tr("Dzongkha"));
    addLanguage("eng",    "", "en", QObject::tr("English"));
    addLanguage("epo",    "", "eo", QObject::tr("Esperanto"));
    addLanguage("est",    "", "et", QObject::tr("Estonian"));
    addLanguage("ewe",    "", "ee", QObject::tr("Ewe"));
    addLanguage("fao",    "", "fo", QObject::tr("Faroese"));
    addLanguage("fij",    "", "fj", QObject::tr("Fijian"));
    addLanguage("fin",    "", "fi", QObject::tr("Finnish"));
    addLanguage("fre", "fra", "fr", QObject::tr("French"));
    addLanguage("fry",    "", "fy", QObject::tr("Western Frisian"));
    addLanguage("ful",    "", "ff", QObject::tr("Fulah"));
    addLanguage("geo", "kat", "ka", QObject::tr("Georgian"));
    addLanguage("ger", "deu", "de", QObject::tr("German"));
    addLanguage("gla",    "", "gd", QObject::tr("Gaelic"));
    addLanguage("gle",    "", "ga", QObject::tr("Irish"));
    addLanguage("glg",    "", "gl", QObject::tr("Galician"));
    addLanguage("glv",    "", "gv", QObject::tr("Manx"));
    addLanguage("gre", "ell", "el", QObject::tr("Greek"));
    addLanguage("grn",    "", "gn", QObject::tr("Guarani"));
    addLanguage("guj",    "", "gu", QObject::tr("Gujarati"));
    addLanguage("hat",    "", "ht", QObject::tr("Haitian Creole"));
    addLanguage("hau",    "", "ha", QObject::tr("Hausa"));
    addLanguage("heb",    "", "he", QObject::tr("Hebrew"));
    addLanguage("her",    "", "hz", QObject::tr("Herero"));
    addLanguage("hin",    "", "hi", QObject::tr("Hindi"));
    addLanguage("hmo",    "", "ho", QObject::tr("Hiri Motu"));
    addLanguage("hrv",    "", "hr", QObject::tr("Croatian"));
    addLanguage("hun",    "", "hu", QObject::tr("Hungarian"));
    addLanguage("ibo",    "", "ig", QObject::tr("Igbo"));
    addLanguage("ice", "isl", "is", QObject::tr("Icelandic"));
    addLanguage("ido",    "", "io", QObject::tr("Ido"));
    addLanguage("iii",    "", "ii", QObject::tr("Sichuan Yi"));
    addLanguage("iku",    "", "iu", QObject::tr("Inuktitut"));
    addLanguage("ind",    "", "id", QObject::tr("Indonesian"));
    addLanguage("ipk",    "", "ik", QObject::tr("Inupiaq"));
    addLanguage("ita",    "", "it", QObject::tr("Italian"));
    addLanguage("jav",    "", "jv", QObject::tr("Javanese"));
    addLanguage("jpn",    "", "ja", QObject::tr("Japanese"));
    addLanguage("kal",    "", "kl", QObject::tr("Greenlandic"));
    addLanguage("kan",    "", "kn", QObject::tr("Kannada"));
    addLanguage("kas",    "", "ks", QObject::tr("Kashmiri"));
    addLanguage("kau",    "", "kr", QObject::tr("Kanuri"));
    addLanguage("kaz",    "", "kk", QObject::tr("Kazakh"));
    addLanguage("khm",    "", "km", QObject::tr("Central Khmer"));
    addLanguage("kik",    "", "ki", QObject::tr("Kikuyu"));
    addLanguage("kin",    "", "rw", QObject::tr("Kinyarwanda"));
    addLanguage("kir",    "", "ky", QObject::tr("Kirghiz"));
    addLanguage("kom",    "", "kv", QObject::tr("Komi"));
    addLanguage("kon",    "", "kg", QObject::tr("Kongo"));
    addLanguage("kor",    "", "ko", QObject::tr("Korean"));
    addLanguage("kua",    "", "kj", QObject::tr("Kuanyama"));
    addLanguage("kur",    "", "ku", QObject::tr("Kurdish"));
    addLanguage("lao",    "", "lo", QObject::tr("Lao"));
    addLanguage("lav",    "", "lv", QObject::tr("Latvian"));
    addLanguage("lim",    "", "li", QObject::tr("Limburgan"));
    addLanguage("lin",    "", "ln", QObject::tr("Lingala"));
    addLanguage("lit",    "", "lt", QObject::tr("Lithuanian"));
    addLanguage("ltz",    "", "lb", QObject::tr("Luxembourgish"));
    addLanguage("lub",    "", "lu", QObject::tr("Luba-Katanga"));
    addLanguage("lug",    "", "lg", QObject::tr("Ganda"));
    addLanguage("mac", "mkd", "mk", QObject::tr("Macedonian"));
    addLanguage("mah",    "", "mh", QObject::tr("Marshallese"));
    addLanguage("mal",    "", "ml", QObject::tr("Malayalam"));
    addLanguage("mao", "mri", "mi", QObject::tr("Maori"));
    addLanguage("mar",    "", "mr", QObject::tr("Marathi"));
    addLanguage("may", "msa", "ms", QObject::tr("Malay"));
    addLanguage("mlg",    "", "mg", QObject::tr("Malagasy"));
    addLanguage("mlt",    "", "mt", QObject::tr("Maltese"));
    addLanguage("mon",    "", "mn", QObject::tr("Mongolian"));
    addLanguage("nau",    "", "na", QObject::tr("Nauru"));
    addLanguage("nav",    "", "nv", QObject::tr("Navajo"));
    addLanguage("nbl",    "", "nr", QObject::tr("South Ndebele"));
    addLanguage("nde",    "", "nd", QObject::tr("North Ndebele"));
    addLanguage("ndo",    "", "ng", QObject::tr("Ndonga"));
    addLanguage("nep",    "", "ne", QObject::tr("Nepali"));
    addLanguage("nno",    "", "nn", QObject::tr("Norwegian Nynorsk"));
    addLanguage("nob",    "", "nb", QObject::tr("Norwegian Bokmal"));
    addLanguage("nor",    "", "no", QObject::tr("Norwegian"));
    addLanguage("nya",    "", "ny", QObject::tr("Chichewa"));
    addLanguage("oci",    "", "oc", QObject::tr("Occitan"));
    addLanguage("oji",    "", "oj", QObject::tr("Ojibwa"));
    addLanguage("ori",    "", "or", QObject::tr("Oriya"));
    addLanguage("orm",    "", "om", QObject::tr("Oromo"));
    addLanguage("oss",    "", "os", QObject::tr("Ossetian"));
    addLanguage("pan",    "", "pa", QObject::tr("Punjabi"));
    addLanguage("per", "fas", "fa", QObject::tr("Persian"));
    addLanguage("pli",    "", "pi", QObject::tr("Pali"));
    addLanguage("pol",    "", "pl", QObject::tr("Polish"));
    addLanguage("por",    "", "pt", QObject::tr("Portuguese"));
    addLanguage("pus",    "", "ps", QObject::tr("Pushto"));
    addLanguage("que",    "", "qu", QObject::tr("Quechua"));
    addLanguage("roh",    "", "rm", QObject::tr("Romansh"));
    addLanguage("rum", "ron", "ro", QObject::tr("Romanian"));
    addLanguage("run",    "", "rn", QObject::tr("Rundi"));
    addLanguage("rus",    "", "ru", QObject::tr("Russian"));
    addLanguage("sag",    "", "sg", QObject::tr("Sango"));
    addLanguage("san",    "", "sa", QObject::tr("Sanskrit"));
    addLanguage("sin",    "", "si", QObject::tr("Sinhala"));
    addLanguage("slo", "slk", "sk", QObject::tr("Slovak"));
    addLanguage("slv",    "", "sl", QObject::tr("Slovenian"));
    addLanguage("sme",    "", "se", QObject::tr("Northern Sami"));
    addLanguage("smo",    "", "sm", QObject::tr("Samoan"));
    addLanguage("sna",    "", "sn", QObject::tr("Shona"));
    addLanguage("snd",    "", "sd", QObject::tr("Sindhi"));
    addLanguage("som",    "", "so", QObject::tr("Somali"));
    addLanguage("sot",    "", "st", QObject::tr("Sotho"));
    addLanguage("spa",    "", "es", QObject::tr("Spanish"));
    addLanguage("srd",    "", "sc", QObject::tr("Sardinian"));
    addLanguage("srp",    "", "sr", QObject::tr("Serbian"));
    addLanguage("ssw",    "", "ss", QObject::tr("Swati"));
    addLanguage("sun",    "", "su", QObject::tr("Sundanese"));
    addLanguage("swa",    "", "sw", QObject::tr("Swahili"));
    addLanguage("swe",    "", "sv", QObject::tr("Swedish"));
    addLanguage("tah",    "", "ty", QObject::tr("Tahitian"));
    addLanguage("tam",    "", "ta", QObject::tr("Tamil"));
    addLanguage("tat",    "", "tt", QObject::tr("Tatar"));
    addLanguage("tel",    "", "te", QObject::tr("Telugu"));
    addLanguage("tgk",    "", "tg", QObject::tr("Tajik"));
    addLanguage("tgl",    "", "tl", QObject::tr("Tagalog"));
    addLanguage("tha",    "", "th", QObject::tr("Thai"));
    addLanguage("tib", "bod", "bo", QObject::tr("Tibetan"));
    addLanguage("tir",    "", "ti", QObject::tr("Tigrinya"));
    addLanguage("ton",    "", "to", QObject::tr("Tonga"));
    addLanguage("tsn",    "", "tn", QObject::tr("Tswana"));
    addLanguage("tso",    "", "ts", QObject::tr("Tsonga"));
    addLanguage("tuk",    "", "tk", QObject::tr("Turkmen"));
    addLanguage("tur",    "", "tr", QObject::tr("Turkish"));
    addLanguage("twi",    "", "tw", QObject::tr("Twi"));
    addLanguage("uig",    "", "ug", QObject::tr("Uighur"));
    addLanguage("ukr",    "", "uk", QObject::tr("Ukrainian"));
    addLanguage("urd",    "", "ur", QObject::tr("Urdu"));
    addLanguage("uzb",    "", "uz", QObject::tr("Uzbek"));
    addLanguage("ven",    "", "ve", QObject::tr("Venda"));
    addLanguage("vie",    "", "vi", QObject::tr("Vietnamese"));
    addLanguage("vol",    "", "vo", QObject::tr("Volapuk"));
    addLanguage("wel", "cym", "cy", QObject::tr("Welsh"));
    addLanguage("wln",    "", "wa", QObject::tr("Walloon"));
    addLanguage("wol",    "", "wo", QObject::tr("Wolof"));
    addLanguage("xho",    "", "xh", QObject::tr("Xhosa"));
    addLanguage("yid",    "", "yi", QObject::tr("Yiddish"));
    addLanguage("yor",    "", "yo", QObject::tr("Yoruba"));
    addLanguage("zha",    "", "za", QObject::tr("Zhuang"));
    addLanguage("zul",    "", "zu", QObject::tr("Zulu"));
}
