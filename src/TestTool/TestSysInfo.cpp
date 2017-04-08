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
// Command line tool to test various system information.
//
//----------------------------------------------------------------------------

#include "TestToolCommand.h"
#include "QtlCore.h"

class TestSysInfo : public TestToolCommand
{
    Q_OBJECT
public:
    TestSysInfo() : TestToolCommand("sysinfo", "", "Display various information from QSysInfo and other system classes.") {}
    virtual int run(const QStringList& args) Q_DECL_OVERRIDE;
};

//----------------------------------------------------------------------------

int TestSysInfo::run(const QStringList& args)
{
    out << "==== Global system information" << endl
        << "qtlApplicationName() = " << qtlApplicationName() << endl
        << "qtlQtVersion() = " << QStringLiteral("0x%1").arg(qtlQtVersion(), 8, 16, QChar('0')) << endl
        << "qtVersion() = " << qVersion() << endl
        << "QSysInfo::buildAbi() = " << QSysInfo::buildAbi() << endl
        << "QSysInfo::buildCpuArchitecture() = " << QSysInfo::buildCpuArchitecture() << endl
        << "QSysInfo::currentCpuArchitecture() = " << QSysInfo::currentCpuArchitecture() << endl
        << "QSysInfo::kernelType() = " << QSysInfo::kernelType() << endl
        << "QSysInfo::kernelVersion() = " << QSysInfo::kernelVersion() << endl
        << "QSysInfo::MacintoshVersion = " << QSysInfo::MacintoshVersion << endl
        << "QSysInfo::macVersion() = " << QSysInfo::macVersion() << endl
#if QT_VERSION >= QT_VERSION_CHECK(5,6,0)
        << "QSysInfo::machineHostName() = " << QSysInfo::machineHostName() << endl
#endif
        << "QSysInfo::prettyProductName() = " << QSysInfo::prettyProductName() << endl
        << "QSysInfo::productType() = " << QSysInfo::productType() << endl
        << "QSysInfo::productVersion() = " << QSysInfo::productVersion() << endl
        << "QSysInfo::WindowsVersion = " << QSysInfo::WindowsVersion << endl
        << "QSysInfo::windowsVersion() = " << QSysInfo::windowsVersion() << endl
        << "QLocale::system().name() = " << QLocale::system().name() << endl
        << "QTimeZone::systemTimeZoneId() = " << QString(QTimeZone::systemTimeZoneId()) << endl;

    foreach (const QStorageInfo& si, QStorageInfo::mountedVolumes()) {
        out << endl
            << "==== File system information (QStorageInfo)" << endl
            << "name() = " << si.name() << endl
            << "device() = " << QString(si.device()) << endl
            << "displayName() = " << si.displayName() << endl
            << "fileSystemType() = " << QString(si.fileSystemType()) << endl
            << "rootPath() = " << si.rootPath() << endl
#if QT_VERSION >= QT_VERSION_CHECK(5,6,0)
            << "blockSize() = " << si.blockSize() << endl
#endif
            << "bytesAvailable() = " << si.bytesAvailable() << endl
            << "bytesFree() = " << si.bytesFree() << endl
            << "bytesTotal() = " << si.bytesTotal() << endl
            << "isReadOnly() = " << si.isReadOnly() << endl
            << "isReady() = " << si.isReady() << endl
            << "isRoot() = " << si.isRoot() << endl
            << "isValid() = " << si.isValid() << endl;
    }

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

#include "TestSysInfo.moc"
namespace {TestSysInfo thisTest;}
