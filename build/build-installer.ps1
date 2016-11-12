#-----------------------------------------------------------------------------
#
#  Copyright (c) 2013-2016, Thierry Lelegard
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#  1. Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGE.
#
#-----------------------------------------------------------------------------
#
#  Windows PowerShell script to build the binary installer for Windows.
#  Also build a source file zip archive.
#
#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Build the binary installers for Windows.

 .DESCRIPTION

  Build the binary installers for Windows. The release version of the project
  is automatically rebuilt before building the installer.

  By default, installers are built for 32-bit and 64-bit systems, full
  executable binary installers, standalone binaries (without admin rights),
  source code and wintools archives.

 .PARAMETER ProductName

  Name of the product. The default is the base name of the unique project file
  in the same directory as this script or in src or ..\src subdirectory.

 .PARAMETER Version

  Version of the product. The default is extracted from the source file
  {ProductName}Version.h.

 .PARAMETER OpenSslDir
  Installation directory of OpenSSL, from slproweb.com.
  Default is "C:\OpenSSL-Win32".

 .PARAMETER NoPause

  Do not wait for the user to press <enter> at end of execution. By default,
  execute a "pause" instruction at the end of execution, which is useful
  when the script was run from Windows Explorer.

 .PARAMETER NoBuild

  Do not rebuild the product, assumed to be still valid.

 .PARAMETER NoInstaller

  Do not build the binary installer.

 .PARAMETER NoStandalone

  Do not build the standalone binaries.

 .PARAMETER NoSource

  Do not build the source archive.
#>
[CmdletBinding()]
param(
    [Parameter(Position=1,Mandatory=$false)][string]$ProductName,
    [Parameter(Position=2,Mandatory=$false)][string]$Version,
    [string]$OpenSslDir = "C:\OpenSSL-Win32",
    [switch]$NoPause = $false,
    [switch]$NoBuild = $false,
    [switch]$NoInstaller = $false,
    [switch]$NoStandalone = $false,
    [switch]$NoSource = $false
)

# PowerShell execution policy.
Set-StrictMode -Version 3
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process -Force
Import-Module -Name (Join-Path $PSScriptRoot WindowsPowerShellTools.psm1)

# A function to exit this script.
function Exit-Script ([string]$Message = "")
{
    $Code = 0
    if ($Message -ne "") {
        Write-Output "ERROR: $Message"
        $Code = 1
    }
    if (-not $NoPause) {
        pause
    }
    exit $Code
}

# A function to search a file and exit if not found.
function Get-FileInPath ([string]$File, $SearchPath)
{
    $Path = Search-File $File $SearchPath
    if (-not $Path) {
        Exit-Script "$File not found"
    }
    return $Path
}

# Requires .NET 4.5 to build zip files (need compression methods).
if (-not $NoSource -or -not $NoStandalone) {
    $DotNetVersion = Get-DotNetVersion
    if ($DotNetVersion -lt 405) {
        $DotNetString = "v" + [int]($DotNetVersion / 100) + "." + ($DotNetVersion % 100)
        Exit-Script "Require .NET Framework v4.5, running $DotNetString"
    }
}

# Get the project directories.
$RootDir = (Split-Path -Parent $PSScriptRoot)
$SrcDir = (Join-Path $RootDir "src")
$InstallerDir = (Join-Path $RootDir "installers")
$ProjectFile = Get-QtProjectFile $SrcDir
$BuildDir = (Join-Path $RootDir "build-Win32-Release")

# Get the product name.
if (-not $ProductName -and $ProjectFile) {
    $ProductName = (Get-Item $ProjectFile).BaseName
}
if (-not $ProductName) {
    Exit-Script "Product name not found"
}

# Get the product version.
if (-not $Version) {
    $VersionFile = Search-File "${ProductName}Version.h" ($SrcDir, (Join-Path $SrcDir $ProductName))
    if ($VersionFile) {
        $Version = (Get-Content $VersionFile | Select-String '^ *#define .*_VERSION  *"') -replace '^ *#define .*_VERSION  *"' -replace '".*$'
    }
}
if (-not $Version) {
    Exit-Script "Product version not found"
}

# Locate NSIS, the Nullsoft Scriptable Installation System.
if (-not $NoInstaller) {
    $NsisExe = Get-FileInPath makensis.exe "$env:Path;C:\Program Files\NSIS;C:\Program Files (x86)\NSIS"
    $NsisScript = Get-FileInPath "${ProductName}.nsi" $PSScriptRoot
}

# Locate Qt and OpenSSL installation directory.
if (-not $NoInstaller -or -not $NoStandalone) {
    Set-QtPath
    $QtDir = Split-Path -Parent (Split-Path -Parent (Get-FileInPath qmake.exe $env:Path))
    $WinDeployExe = Get-FileInPath windeployqt.exe $env:Path
    $LibEay32Dll = Get-FileInPath libeay32.dll (Join-Path $OpenSslDir "bin")
    $SslEay32Dll = Get-FileInPath ssleay32.dll (Join-Path $OpenSslDir "bin")
}

# Build the project in release mode.
if (-not $NoBuild) {
    & (Join-Path $PSScriptRoot build.ps1) -Release -NoPause
}

# Build product installation tree.
if (-not $NoInstaller -or -not $NoStandalone) {

    # Create a temporary directory.
    $TempDir = New-TempDirectory

    try {
        # Product installation root.
        $TempRoot = New-Directory @($TempDir, $ProductName)

        # Copy and strip application binary.
        Copy-Item (Join-MultiPath @($BuildDir, "QtlMovie", "QtlMovie.exe")) $TempRoot
        $ProductExe = (Join-Path $TempRoot "QtlMovie.exe")
        strip $ProductExe

        # Deploy Qt requirements.
        & $WinDeployExe $ProductExe --release --no-translations --no-quick-import --no-system-d3d-compiler --no-webkit2 --no-angle --no-opengl-sw

        # OpenSSL files.
        Copy-Item $LibEay32Dll $TempRoot
        Copy-Item $SslEay32Dll $TempRoot
        Get-ChildItem (Split-Path -Parent $LibEay32Dll) -Filter "msvcr*.dll" | ForEach-Object { Copy-Item $_.FullName $TempRoot }

        # Documentation files.
        Copy-Item (Join-MultiPath @($RootDir, "LICENSE.txt")) $TempRoot
        Copy-Item (Join-MultiPath @($RootDir, "CHANGELOG.txt")) $TempRoot

        # Translation files.
        $TempTranslations = New-Directory @($TempRoot, "translations")
        Get-ChildItem (Join-Path $QtDir "translations") -Filter "qt*_fr.qm" | ForEach-Object { Copy-Item $_.FullName $TempTranslations }
        Copy-Item (Join-MultiPath @($BuildDir, "libQtl", "locale", "qtl_fr.qm")) $TempTranslations
        Copy-Item (Join-MultiPath @($BuildDir, "libQts", "locale", "qts_fr.qm")) $TempTranslations
        Copy-Item (Join-MultiPath @($BuildDir, "QtlMovie", "locale", "qtlmovie_fr.qm")) $TempTranslations

        # Help files.
        Copy-Item -Recurse (Join-Path $RootDir "help") $TempRoot

        # Font files.
        $TempFonts = New-Directory @($TempRoot, "fonts")
        Copy-Item (Join-MultiPath @($RootDir, "fonts", "fonts.conf.template")) $TempFonts
        Copy-Item (Join-MultiPath @($RootDir, "fonts", "subfont.ttf")) $TempFonts

        # Copy 32-bit Windows tools.
        Copy-Item -Recurse (Join-Path $RootDir "wintools") $TempRoot

        # Build the 32-bit installer.
        if (-not $NoInstaller) {
            & $NsisExe `
                "/DProductVersion=$Version" `
                "/DRootDir=$RootDir" `
                "/DBuildDir=$TempRoot" `
                "/DInstallerDir=$InstallerDir" `
                "$NsisScript"
        }

        # Create the 32-bit standalone zip file.
        if (-not $NoStandalone) {
            Get-ChildItem -Recurse $TempDir | `
                New-ZipFile (Join-Path $InstallerDir "${ProductName}-Win32-Standalone-${Version}.zip") -Force -Root $TempDir
        }

        # Overwrite 64-bits tools.
        Get-ChildItem (Join-Path $RootDir "wintools64") | ForEach-Object { Copy-Item -Force $_.FullName (Join-Path $TempRoot "wintools") }

        # Build the 64-bit installer.
        if (-not $NoInstaller) {
            & $NsisExe `
                "/DWin64" `
                "/DProductVersion=$Version" `
                "/DRootDir=$RootDir" `
                "/DBuildDir=$TempRoot" `
                "/DInstallerDir=$InstallerDir" `
                "$NsisScript"
        }

        # Create the 64-bit standalone zip file.
        if (-not $NoStandalone) {
            Get-ChildItem -Recurse $TempDir | `
                New-ZipFile (Join-Path $InstallerDir "${ProductName}-Win64-Standalone-${Version}.zip") -Force -Root $TempDir
        }
    }
    finally {
        # Delete the temporary directory.
        if (Test-Path $TempDir) {
            Remove-Item $TempDir -Recurse -Force
        }
    }
}

# Build the source archives.
if (-not $NoSource) {

    # Source archive name.
    $SrcArchive = (Join-Path $InstallerDir "${ProductName}-${Version}-src.zip")
    $WintoolsArchive = (Join-Path $InstallerDir "${ProductName}-${Version}-wintools.zip")

    # Create a temporary directory.
    $TempDir = New-TempDirectory

    Push-Location $TempDir
    try {
        # Copy project tree into temporary directory.
        $TempRoot = (Join-Path $TempDir "${ProductName}-${Version}")
        Copy-Item $RootDir $TempRoot -Recurse

        # Create the wintools zip file.
        Get-ChildItem -Recurse (Join-Path $TempRoot wintools*) | New-ZipFile $WintoolsArchive -Force -Root $TempRoot

        # Cleanup the temporary tree.
        & (Join-MultiPath @($TempRoot, "build", "cleanup.ps1")) -Deep -NoPause -Silent

        # Create the source zip file.
        Get-ChildItem -Recurse (Split-Path $TempRoot) | New-ZipFile $SrcArchive -Force -Root $TempDir
    }
    finally {
        # Delete the temporary directory.
        Pop-Location
        if (Test-Path $TempDir) {
            Remove-Item $TempDir -Recurse -Force
        }
    }
}

Exit-Script
