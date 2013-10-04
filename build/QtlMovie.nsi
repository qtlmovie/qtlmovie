;-----------------------------------------------------------------------------
; 
;  Copyright (c) 2013, Thierry Lelegard
;  All rights reserved.
; 
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions are met:
; 
;  1. Redistributions of source code must retain the above copyright notice,
;     this list of conditions and the following disclaimer. 
;  2. Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution. 
; 
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
;  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
;  THE POSSIBILITY OF SUCH DAMAGE.
; 
;-----------------------------------------------------------------------------
; 
;  NSIS script to build the QtlMovie binary installer for Windows.
;
;  Do not invoke NSIS directly, use build-installer.ps1 to build the installer.
;  If Win64 is defined, generate a 64-bit installer (default: 32-bit installer).
;
;  This script uses the following required symbols:
;  ProductVersion, BuildDir, RootDir, QtDllDir, OutDir.
;
;-----------------------------------------------------------------------------

Name "QtlMovie"

!verbose push
!verbose 0
!include "MUI2.nsh"
!include "WinMessages.nsh"
!include "x64.nsh"
!verbose pop

; In 64-bit installers, don't use registry redirection. Also prevent execution
; of 64-bit installers on 32-bit systems (the installer itself is 32-bit and
; can run on 32-bit systems but it contains some 64-bit executables).
!ifdef Win64
    function .onInit
        ${If} ${RunningX64}
            SetRegView 64
        ${Else}
            MessageBox MB_OK|MB_ICONSTOP \
                "This is a 64-bit version of QtlMovie.$\r$\n\
                You have a 32-bit version of Windows.$\r$\n\
                Please use a 32-bit version of QtlMovie on this system."
            Quit
        ${EndIf}
    functionEnd
    function un.onInit
        ${If} ${RunningX64}
            SetRegView 64
        ${EndIf}
    functionEnd
!endif

; Installer file name.
!ifdef Win64
    OutFile "${OutDir}\QtlMovie-Win64-${ProductVersion}.exe"
!else
    OutFile "${OutDir}\QtlMovie-Win32-${ProductVersion}.exe"
!endif

; Use XP manifest.
XPStyle on

; Request administrator privileges for Windows Vista and higher.
RequestExecutionLevel admin

; "Modern User Interface" (MUI) settings
!define MUI_ABORTWARNING
!define MUI_ICON "${RootDir}\src\QtlMovie\images\qtlmovie-logo.ico"
!define MUI_UNICON "${RootDir}\src\QtlMovie\images\qtlmovie-logo.ico"

; Installer pages
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

; Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Languages
!insertmacro MUI_LANGUAGE "English"

; Registry entry for product info and uninstallation info.
!define ProductKey "Software\QtlMovie"
!define UninstallKey "Software\Microsoft\Windows\CurrentVersion\Uninstall\QtlMovie"

; Default installation folder
!ifdef Win64
    InstallDir "$PROGRAMFILES64\QtlMovie"
!else
    InstallDir "$PROGRAMFILES\QtlMovie"
!endif

; Get installation folder from registry if available from a previous installation.
InstallDirRegKey HKLM "${ProductKey}" "InstallDir"


;-----------------------------------------------------------------------------
; Installation section
;-----------------------------------------------------------------------------

Section "Install"

    ; Work on "all users" context, not current user.
    SetShellVarContext all

    ; Delete obsolete files from previous versions.
    Delete "$INSTDIR\fonts\fonts.win.conf"

    ; Install product files.
    SetOutPath "$INSTDIR"
    File "${BuildDir}\QtlMovie.exe"
    File "${QtDllDir}\Qt5Core.dll"
    File "${QtDllDir}\Qt5Network.dll"
    File "${QtDllDir}\Qt5Gui.dll"
    File "${QtDllDir}\Qt5Widgets.dll"
    File "${QtDllDir}\Qt5Multimedia.dll"
    File "${QtDllDir}\libgcc_s_dw2-1.dll"
    File "${QtDllDir}\libstdc++-6.dll"
    File "${QtDllDir}\libwinpthread-1.dll"
    File "${QtDllDir}\icuin51.dll"
    File "${QtDllDir}\icuuc51.dll"
    File "${QtDllDir}\icudt51.dll"
    File "${RootDir}\LICENSE.txt"

    SetOutPath "$INSTDIR\fonts"
    File "${RootDir}\fonts\fonts.conf.template"
    File "${RootDir}\fonts\subfont.ttf"

    SetOutPath "$INSTDIR\wintools"
    !ifdef Win64
        File "${RootDir}\wintools64\ffprobe.exe"
        File "${RootDir}\wintools64\ffmpeg.exe"
        File "${RootDir}\wintools64\ffmpeg.txt"
    !else
        File "${RootDir}\wintools\ffprobe.exe"
        File "${RootDir}\wintools\ffmpeg.exe"
        File "${RootDir}\wintools\ffmpeg.txt"
    !endif
    File "${RootDir}\wintools\dvdauthor.exe"
    File "${RootDir}\wintools\dvdauthor.txt"
    File "${RootDir}\wintools\mkisofs.exe"
    File "${RootDir}\wintools\mkisofs.txt"
    File "${RootDir}\wintools\growisofs.exe"
    File "${RootDir}\wintools\growisofs.txt"
    File "${RootDir}\wintools\telxcc.exe"
    File "${RootDir}\wintools\telxcc.txt"
    File "${RootDir}\wintools\dvddecrypter.exe"
    File "${RootDir}\wintools\dvddecrypter.txt"

    ; Create a shortcut in start menu.
    CreateShortCut "$SMPROGRAMS\QtlMovie.lnk" "$INSTDIR\QtlMovie.exe"

    ; Store installation folder in registry.
    WriteRegStr HKLM "${ProductKey}" "InstallDir" $INSTDIR

    ; Create uninstaller
    WriteUninstaller "$INSTDIR\QtlMovieUninstall.exe"

    ; Declare uninstaller in "Add/Remove Software" control panel
    WriteRegStr HKLM "${UninstallKey}" "DisplayName" "QtlMovie"
    WriteRegStr HKLM "${UninstallKey}" "DisplayVersion" "${ProductVersion}"
    WriteRegStr HKLM "${UninstallKey}" "DisplayIcon" "$INSTDIR\QtlMovieUninstall.exe"
    WriteRegStr HKLM "${UninstallKey}" "UninstallString" "$INSTDIR\QtlMovieUninstall.exe"

SectionEnd

;-----------------------------------------------------------------------------
; Uninstallation section
;-----------------------------------------------------------------------------

Section "Uninstall"

    ; Work on "all users" context, not current user.
    SetShellVarContext all

    ; Delete product files.
    Delete "$INSTDIR\fonts\fonts.conf.template"
    Delete "$INSTDIR\fonts\subfont.ttf"
    RMDir  "$INSTDIR\fonts"

    Delete "$INSTDIR\wintools\ffprobe.exe"
    Delete "$INSTDIR\wintools\ffmpeg.exe"
    Delete "$INSTDIR\wintools\ffmpeg.txt"
    Delete "$INSTDIR\wintools\dvdauthor.exe"
    Delete "$INSTDIR\wintools\dvdauthor.txt"
    Delete "$INSTDIR\wintools\mkisofs.exe"
    Delete "$INSTDIR\wintools\mkisofs.txt"
    Delete "$INSTDIR\wintools\growisofs.exe"
    Delete "$INSTDIR\wintools\growisofs.txt"
    Delete "$INSTDIR\wintools\telxcc.exe"
    Delete "$INSTDIR\wintools\telxcc.txt"
    Delete "$INSTDIR\wintools\dvddecrypter.exe"
    Delete "$INSTDIR\wintools\dvddecrypter.txt"
    RMDir  "$INSTDIR\wintools"

    Delete "$INSTDIR\QtlMovie.exe"
    Delete "$INSTDIR\Qt5Core.dll"
    Delete "$INSTDIR\Qt5Network.dll"
    Delete "$INSTDIR\Qt5Gui.dll"
    Delete "$INSTDIR\Qt5Widgets.dll"
    Delete "$INSTDIR\Qt5Multimedia.dll"
    Delete "$INSTDIR\libgcc_s_dw2-1.dll"
    Delete "$INSTDIR\libstdc++-6.dll"
    Delete "$INSTDIR\libwinpthread-1.dll"
    Delete "$INSTDIR\icuin51.dll"
    Delete "$INSTDIR\icuuc51.dll"
    Delete "$INSTDIR\icudt51.dll"
    Delete "$INSTDIR\LICENSE.txt"
    Delete "$INSTDIR\QtlMovieUninstall.exe"
    RMDir  "$INSTDIR"

    ; Delete start menu entries  
    Delete "$SMPROGRAMS\QtlMovie.lnk"

    ; Delete registry entries
    DeleteRegKey HKLM "${ProductKey}"
    DeleteRegKey HKLM "${UninstallKey}"

SectionEnd
