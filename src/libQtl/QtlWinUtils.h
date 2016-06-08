//----------------------------------------------------------------------------
//
// Copyright (c) 2015, Thierry Lelegard
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
//! @file QtlWinUtils.h
//!
//! Utilities for Windows systems.
//! Qtl, Qt utility library.
//!
//! The QTL_FOLDERID constants are URL strings which can be used to create QUrl
//! objects. They represent GUID's that identify standard folders registered
//! with the system as Known Folders. These folders are installed with Windows
//! Vista and later operating systems, and a computer will have only folders
//! appropriate to it installed.
//!
//! @see https://msdn.microsoft.com/en-us/library/windows/desktop/dd378457.aspx
//!
//----------------------------------------------------------------------------

#ifndef QTLWINUTILS_H
#define QTLWINUTILS_H

#include "QtlCore.h"

#if defined(Q_OS_WIN) || defined(DOXYGEN)

//!
//! Transform a CLSID string into a @c clsid: URL.
//! @param [in] clsid CLSID string literal in the form "hhhhhhhh-hhhh-hhhh-hhhh-hhhhhhhhhhhh".
//! @return An URL string.
//!
#define QTL_CLSID_URL(clsid) "clsid:" clsid

//!
//! Transform a CLSID string into an Windows CLSID.
//! @param [in] clsid CLSID string literal in the form "hhhhhhhh-hhhh-hhhh-hhhh-hhhhhhhhhhhh".
//! @return The corresponding "{hhhhhhhh-hhhh-hhhh-hhhh-hhhhhhhhhhhh}".
//!
#define QTL_CLSID(clsid) "{" clsid "}"

//!
//! Folder: Account Pictures (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/AccountPictures
//!
#define QTL_FOLDERID_ACCOUNT_PICTURES "008CA0B1-55B4-4C56-B8A8-4DE4B299D3BE"

//!
//! Folder: Get Programs (virtual)
//!
#define QTL_FOLDERID_ADD_NEW_PROGRAMS "DE61D971-5EBC-4F02-A3A9-6C82895E5C04"

//!
//! Folder: Administrative Tools (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Start Menu/Programs/Administrative Tools
//!
#define QTL_FOLDERID_ADMIN_TOOLS "724EF170-A42D-4FEF-9F26-B60E846FBA4F"

//!
//! Folder: Application Shortcuts (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows/Application Shortcuts
//!
#define QTL_FOLDERID_APPLICATION_SHORTCUTS "A3918781-E5F2-4890-B3D9-A7E54332328C"

//!
//! Folder: Applications (virtual)
//!
#define QTL_FOLDERID_APPS_FOLDER "1E87508D-89C2-42F0-8A7E-645A0F50CA58"

//!
//! Folder: Installed Updates (virtual)
//!
#define QTL_FOLDERID_APP_UPDATES "A305CE99-F527-492B-8B1A-7E76FA98D6E4"

//!
//! Folder: Camera Roll (per user)
//! Default Path: %USERPROFILE%/Pictures/Camera Roll
//!
#define QTL_FOLDERID_CAMERA_ROLL "AB5FB87B-7CE2-4F83-915D-550846C9537B"

//!
//! Folder: Temporary Burn Folder (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows/Burn/Burn
//!
#define QTL_FOLDERID_CD_BURNING "9E52AB10-F80D-49DF-ACB8-4330F5687855"

//!
//! Folder: Programs and Features (virtual)
//!
#define QTL_FOLDERID_CHANGE_REMOVE_PROGRAMS "DF7266AC-9274-4867-8D55-3BD661DE872D"

//!
//! Folder: Administrative Tools (common)
//! Default Path: %ALLUSERSPROFILE%/Microsoft/Windows/Start Menu/Programs/Administrative Tools
//!
#define QTL_FOLDERID_COMMON_ADMIN_TOOLS "D0384E7D-BAC3-4797-8F14-CBA229B392B5"

//!
//! Folder: OEM Links (common)
//! Default Path: %ALLUSERSPROFILE%/OEM Links
//!
#define QTL_FOLDERID_COMMON_OEM_LINKS "C1BAE2D0-10DF-4334-BEDD-7AA20B227A9D"

//!
//! Folder: Programs (common)
//! Default Path: %ALLUSERSPROFILE%/Microsoft/Windows/Start Menu/Programs
//!
#define QTL_FOLDERID_COMMON_PROGRAMS "0139D44E-6AFE-49F2-8690-3DAFCAE6FFB8"

//!
//! Folder: Start Menu (common)
//! Default Path: %ALLUSERSPROFILE%/Microsoft/Windows/Start Menu
//!
#define QTL_FOLDERID_COMMON_START_MENU "A4115719-D62E-491D-AA7C-E74B8BE3B067"

//!
//! Folder: Startup (common)
//! Default Path: %ALLUSERSPROFILE%/Microsoft/Windows/Start Menu/Programs/StartUp
//!
#define QTL_FOLDERID_COMMON_STARTUP "82A5EA35-D9CD-47C5-9629-E15D2F714E6E"

//!
//! Folder: Templates (common)
//! Default Path: %ALLUSERSPROFILE%/Microsoft/Windows/Templates
//!
#define QTL_FOLDERID_COMMON_TEMPLATES "B94237E7-57AC-4347-9151-B08C6C32D1F7"

//!
//! Folder: Computer (virtual)
//!
#define QTL_FOLDERID_COMPUTER "0AC0837C-BBF8-452A-850D-79D08E667CA7"

//!
//! Folder: Conflicts (virtual)
//!
#define QTL_FOLDERID_CONFLICTS "4bfefb45-347d-4006-a5be-ac0cb0567192"

//!
//! Folder: Network Connections (virtual)
//!
#define QTL_FOLDERID_CONNECTIONS "6F0CD92B-2E97-45D1-88FF-B0D186B8DEDD"

//!
//! Folder: Contacts (per user)
//! Default Path: %USERPROFILE%/Contacts
//!
#define QTL_FOLDERID_CONTACTS "56784854-C6CB-462b-8169-88E350ACB882"

//!
//! Folder: Control Panel (virtual)
//!
#define QTL_FOLDERID_CONTROL_PANEL "82A74AEB-AEB4-465C-A014-D097EE346D63"

//!
//! Folder: Cookies (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Cookies
//!
#define QTL_FOLDERID_COOKIES "2B0F765D-C0E9-4171-908E-08A611B84FF6"

//!
//! Folder: Desktop (per user)
//! Default Path: %USERPROFILE%/Desktop
//!
#define QTL_FOLDERID_DESKTOP "B4BFCC3A-DB2C-424C-B029-7FE99A87C641"

//!
//! Folder: DeviceMetadataStore (common)
//! Default Path: %ALLUSERSPROFILE%/Microsoft/Windows/DeviceMetadataStore
//!
#define QTL_FOLDERID_DeviceMetadataStore "5CE4A5E9-E4EB-479D-B89F-130C02886155"

//!
//! Folder: Documents (per user)
//! Default Path: %USERPROFILE%/Documents
//!
#define QTL_FOLDERID_DOCUMENTS "FDD39AD0-238F-46AF-ADB4-6C85480369C7"

//!
//! Folder: Documents (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Libraries/Documents.library-ms
//!
#define QTL_FOLDERID_DOCUMENTS_LIBRARY "7B0DB17D-9CD2-4A93-9733-46CC89022E7C"

//!
//! Folder: Downloads (per user)
//! Default Path: %USERPROFILE%/Downloads
//!
#define QTL_FOLDERID_DOWNLOADS "374DE290-123F-4565-9164-39C4925E467B"

//!
//! Folder: Favorites (per user)
//! Default Path: %USERPROFILE%/Favorites
//!
#define QTL_FOLDERID_FAVORITES "1777F761-68AD-4D8A-87BD-30B759FA33DD"

//!
//! Folder: Fonts (fixed)
//! Default Path: %windir%/Fonts
//!
#define QTL_FOLDERID_FONTS "FD228CB7-AE11-4AE3-864C-16F3910AB8FE"

//!
//! Folder: Games (virtual)
//!
#define QTL_FOLDERID_GAMES "CAC52C1A-B53D-4EDC-92D7-6B2E8AC19434"

//!
//! Folder: Game Explorer (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows/GameExplorer
//!
#define QTL_FOLDERID_GAME_TASKS "054FAE61-4DD8-4787-80B6-090220C4B700"

//!
//! Folder: History (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows/History
//!
#define QTL_FOLDERID_HISTORY "D9DC8A3B-B784-432E-A781-5A1130A75963"

//!
//! Folder: Homegroup (virtual)
//!
#define QTL_FOLDERID_HOME_GROUP "52528A6B-B9E3-4ADD-B60D-588C2DBA842D"

//!
//! Folder: The user's username (%USERNAME%) (virtual)
//!
#define QTL_FOLDERID_HOME_GROUP_CURRENT_USER "9B74B6A3-0DFD-4F11-9E78-5F7800F2E772"

//!
//! Folder: ImplicitAppShortcuts (per user)
//! Default Path: %APPDATA%/Microsoft/Internet Explorer/Quick Launch/User Pinned/ImplicitAppShortcuts
//!
#define QTL_FOLDERID_IMPLICIT_APP_SHORTCUTS "BCB5256F-79F6-4CEE-B725-DC34E402FD46"

//!
//! Folder: Temporary Internet Files (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows/Temporary Internet Files
//!
#define QTL_FOLDERID_INTERNET_CACHE "352481E8-33BE-4251-BA85-6007CAEDCF9D"

//!
//! Folder: The Internet (virtual)
//!
#define QTL_FOLDERID_INTERNET "4D9F7874-4E0C-4904-967B-40B0D20C3E4B"

//!
//! Folder: Libraries (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Libraries
//!
#define QTL_FOLDERID_LIBRARIES "1B3EA5DC-B587-4786-B4EF-BD1DC332AEAE"

//!
//! Folder: Links (per user)
//! Default Path: %USERPROFILE%/Links
//!
#define QTL_FOLDERID_LINKS "BFB9D5E0-C6A9-404C-B2B2-AE6DB6AF4968"

//!
//! Folder: Local (per user)
//! Default Path: %LOCALAPPDATA% (%USERPROFILE%/AppData/Local)
//!
#define QTL_FOLDERID_LOCAL_APP_DATA "F1B32785-6FBA-4FCF-9D55-7B8E7F157091"

//!
//! Folder: LocalLow (per user)
//! Default Path: %USERPROFILE%/AppData/LocalLow
//!
#define QTL_FOLDERID_LOCAL_APP_DATA_LOW "A520A1A4-1780-4FF6-BD18-167343C5AF16"

//!
//! Folder: None (fixed)
//! Default Path: %windir%/resources/0409 (code page)
//!
#define QTL_FOLDERID_LOCALIZED_RESOURCES_DIR "2A00375E-224C-49DE-B8D1-440DF7EF3DDC"

//!
//! Folder: Music (per user)
//! Default Path: %USERPROFILE%/Music
//!
#define QTL_FOLDERID_MUSIC "4BD8D571-6D19-48D3-BE97-422220080E43"

//!
//! Folder: Music (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Libraries/Music.library-ms
//!
#define QTL_FOLDERID_MUSIC_LIBRARY "2112AB0A-C86A-4FFE-A368-0DE96E47012E"

//!
//! Folder: Network Shortcuts (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Network Shortcuts
//!
#define QTL_FOLDERID_NETHOOD "C5ABBF53-E17F-4121-8900-86626FC2C973"

//!
//! Folder: Network (virtual)
//!
#define QTL_FOLDERID_NETWORK "D20BEEC4-5CA8-4905-AE3B-BF251EA09B53"

//!
//! Folder: Original Images (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows Photo Gallery/Original Images
//!
#define QTL_FOLDERID_ORIGINAL_IMAGES "2C36C0AA-5812-4B87-BFD0-4CD0DFB19B39"

//!
//! Folder: Slide Shows (per user)
//! Default Path: %USERPROFILE%/Pictures/Slide Shows
//!
#define QTL_FOLDERID_PHOTO_ALBUMS "69D2CF90-FC33-4FB7-9A0C-EBB0F0FCB43C"

//!
//! Folder: Pictures (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Libraries/Pictures.library-ms
//!
#define QTL_FOLDERID_PICTURES_LIBRARY "A990AE9F-A03B-4E80-94BC-9912D7504104"

//!
//! Folder: Pictures (per user)
//! Default Path: %USERPROFILE%/Pictures
//!
#define QTL_FOLDERID_PICTURES "33E28130-4E1E-4676-835A-98395C3BC3BB"

//!
//! Folder: Playlists (per user)
//! Default Path: %USERPROFILE%/Music/Playlists
//!
#define QTL_FOLDERID_PLAYLISTS "DE92C1C7-837F-4F69-A3BB-86E631204A23"

//!
//! Folder: Printers (virtual)
//!
#define QTL_FOLDERID_PRINTERS "76FC4E2D-D6AD-4519-A663-37BD56068185"

//!
//! Folder: Printer Shortcuts (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Printer Shortcuts
//!
#define QTL_FOLDERID_PRINT_HOOD "9274BD8D-CFD1-41C3-B35E-B13F55A758F4"

//!
//! Folder: The user's username (%USERNAME%) (fixed)
//! Default Path: %USERPROFILE% (%SystemDrive%/Users/%USERNAME%)
//!
#define QTL_FOLDERID_PROFILE "5E6C858F-0E22-4760-9AFE-EA3317B67173"

//!
//! Folder: Program Data (fixed)
//! Default Path: %ALLUSERSPROFILE% (%ProgramData%, %SystemDrive%/ProgramData)
//!
#define QTL_FOLDERID_PROGRAM_DATA "62AB5D82-FDC1-4DC3-A9DD-070D1D495D97"

//!
//! Folder: Frogam Files (fixed)
//! Default Path: %ProgramFiles% (%SystemDrive%/Program Files)
//!
#define QTL_FOLDERID_PROGRAM_FILES "905E63B6-C1BF-494E-B29C-65B732D3D21A"

//!
//! Folder: Program Files for 64-bit applications (fixed)
//! Default Path: %ProgramFiles% (%SystemDrive%/Program Files)
//!
#define QTL_FOLDERID_PROGRAM_FILES_X64 "6D809377-6AF0-444B-8957-A3773F02200E"

//!
//! Folder: Program Files for 32-bit applications on 64-bit systems (fixed)
//! Default Path: %ProgramFiles% (%SystemDrive%/Program Files)
//!
#define QTL_FOLDERID_PROGRAM_FILES_X86 "7C5A40EF-A0FB-4BFC-874A-C0F2E0B9FA8E"

//!
//! Folder: Common Program Files (fixed)
//! Default Path: %ProgramFiles% (%SystemDrive%/Program Files)
//!
#define QTL_FOLDERID_PROGRAM_FILES_COMMON "F7F1ED05-9F6D-47A2-AAAE-29D317C6F066"

//!
//! Folder: Common Files for 64-bit applications (fixed)
//! Default Path: %ProgramFiles%/Common Files
//!
#define QTL_FOLDERID_PROGRAM_FILES_COMMON_X64 "6365D5A7-0F0D-45E5-87F6-0DA56B6A4F7D"

//!
//! Folder: Common Files for 32-bit applications on 64-bit systems (fixed)
//! Default Path: %ProgramFiles%/Common Files
//!
#define QTL_FOLDERID_PROGRAM_FILES_COMMON_X86 "DE974D24-D9C6-4D3E-BF91-F4455120B917"

//!
//! Folder: Programs (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Start Menu/Programs
//!
#define QTL_FOLDERID_PROGRAMS "A77F5D77-2E2B-44C3-A6A2-ABA601054A51"

//!
//! Folder: Public (fixed)
//! Default Path: %PUBLIC% (%SystemDrive%/Users/Public)
//!
#define QTL_FOLDERID_PUBLIC "DFDF76A2-C82A-4D63-906A-5644AC457385"

//!
//! Folder: Public Desktop (common)
//! Default Path: %PUBLIC%/Desktop
//!
#define QTL_FOLDERID_PUBLIC_DESKTOP "C4AA340D-F20F-4863-AFEF-F87EF2E6BA25"

//!
//! Folder: Public Documents (common)
//! Default Path: %PUBLIC%/Documents
//!
#define QTL_FOLDERID_PUBLIC_DOCUMENTS "ED4824AF-DCE4-45A8-81E2-FC7965083634"

//!
//! Folder: Public Downloads (common)
//! Default Path: %PUBLIC%/Downloads
//!
#define QTL_FOLDERID_PUBLIC_DOWNLOADS "3D644C9B-1FB8-4F30-9B45-F670235F79C0"

//!
//! Folder: GameExplorer (common)
//! Default Path: %ALLUSERSPROFILE%/Microsoft/Windows/GameExplorer
//!
#define QTL_FOLDERID_PUBLIC_GAME_TASKS "DEBF2536-E1A8-4C59-B6A2-414586476AEA"

//!
//! Folder: Libraries (common)
//! Default Path: %ALLUSERSPROFILE%/Microsoft/Windows/Libraries
//!
#define QTL_FOLDERID_PUBLIC_LIBRARIES "48DAF80B-E6CF-4F4E-B800-0E69D84EE384"

//!
//! Folder: Public Music (common)
//! Default Path: %PUBLIC%/Music
//!
#define QTL_FOLDERID_PUBLIC_MUSIC "3214FAB5-9757-4298-BB61-92A9DEAA44FF"

//!
//! Folder: Public Pictures (common)
//! Default Path: %PUBLIC%/Pictures
//!
#define QTL_FOLDERID_PUBLIC_PICTURES "B6EBFB86-6907-413C-9AF7-4FC2ABF07CC5"

//!
//! Folder: Ringtones (common)
//! Default Path: %ALLUSERSPROFILE%/Microsoft/Windows/Ringtones
//!
#define QTL_FOLDERID_PUBLIC_RINGTONES "E555AB60-153B-4D17-9F04-A5FE99FC15EC"

//!
//! Folder: Public Account Pictures (common)
//! Default Path: %PUBLIC%/AccountPictures
//!
#define QTL_FOLDERID_PUBLIC_USER_TILES "0482af6c-08f1-4c34-8c90-e17ec98b1e17"

//!
//! Folder: Public Videos (common)
//! Default Path: %PUBLIC%/Videos
//!
#define QTL_FOLDERID_PUBLIC_VIDEOS "2400183A-6185-49FB-A2D8-4A392A602BA3"

//!
//! Folder: Quick Launch (per user)
//! Default Path: %APPDATA%/Microsoft/Internet Explorer/Quick Launch
//!
#define QTL_FOLDERID_QUICK_LAUNCH "52a4f021-7b75-48a9-9f6b-4b87a210bc8f"

//!
//! Folder: Recent Items (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Recent
//!
#define QTL_FOLDERID_RECENT "AE50C081-EBD2-438A-8655-8A092E34987A"

//!
//! Folder: Recorded TV (common)
//! Default Path: %PUBLIC%/RecordedTV.library-ms
//!
#define QTL_FOLDERID_RECORDED_TV_LIBRARY "1A6FDBA2-F42D-4358-A798-B74D745926C5"

//!
//! Folder: Recycle Bin (virtual)
//!
#define QTL_FOLDERID_RECYCLE_BIN "B7534046-3ECB-4C18-BE4E-64CD4CB7D6AC"

//!
//! Folder: Resources (fixed)
//! Default Path: %windir%/Resources
//!
#define QTL_FOLDERID_RESOURCES "8AD10C31-2ADB-4296-A8F7-E4701232C972"

//!
//! Folder: Ringtones (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows/Ringtones
//!
#define QTL_FOLDERID_RINGTONES "C870044B-F49E-4126-A9C3-B52A1FF411E8"

//!
//! Folder: Roaming (per user)
//! Default Path: %APPDATA% (%USERPROFILE%/AppData/Roaming)
//!
#define QTL_FOLDERID_ROAMING_APP_DATA "3EB685DB-65F9-4CF6-A03A-E3EF65729F3D"

//!
//! Folder: RoamedTileImages (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows/RoamedTileImages
//!
#define QTL_FOLDERID_ROAMED_TILE_IMAGES "AAA8D5A5-F1D6-4259-BAA8-78E7EF60835E"

//!
//! Folder: RoamingTiles (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows/RoamingTiles
//!
#define QTL_FOLDERID_ROAMING_TILES "00BCFC5A-ED94-4e48-96A1-3F6217F21990"

//!
//! Folder: Sample Music (common)
//! Default Path: %PUBLIC%/Music/Sample Music
//!
#define QTL_FOLDERID_SAMPLE_MUSIC "B250C668-F57D-4EE1-A63C-290EE7D1AA1F"

//!
//! Folder: Sample Pictures (common)
//! Default Path: %PUBLIC%/Pictures/Sample Pictures
//!
#define QTL_FOLDERID_SAMPLE_PICTURES "C4900540-2379-4C75-844B-64E6FAF8716B"

//!
//! Folder: Sample Playlists (common)
//! Default Path: %PUBLIC%/Music/Sample Playlists
//!
#define QTL_FOLDERID_SAMPLE_PLAYLISTS "15CA69B3-30EE-49C1-ACE1-6B5EC372AFB5"

//!
//! Folder: Sample Videos (common)
//! Default Path: %PUBLIC%/Videos/Sample Videos
//!
#define QTL_FOLDERID_SAMPLE_VIDEOS "859EAD94-2E85-48AD-A71A-0969CB56A6CD"

//!
//! Folder: Saved Games (per user)
//! Default Path: %USERPROFILE%/Saved Games
//!
#define QTL_FOLDERID_SAVED_GAMES "4C5C32FF-BB9D-43B0-B5B4-2D72E54EAAA4"

//!
//! Folder: Searches (per user)
//! Default Path: %USERPROFILE%/Searches
//!
#define QTL_FOLDERID_SAVED_SEARCHES "7D1D3A04-DEBB-4115-95CF-2F29DA2920DA"

//!
//! Folder: Screenshots (per user)
//! Default Path: %USERPROFILE%/Pictures/Screenshots
//!
#define QTL_FOLDERID_SCREEN_SHOTS "B7BEDE81-DF94-4682-A7D8-57A52620B86F"

//!
//! Folder: Offline Files (virtual)
//!
#define QTL_FOLDERID_SEARCH_CSC "EE32E446-31CA-4ABA-814F-A5EBD2FD6D5E"

//!
//! Folder: History (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows/ConnectedSearch/History
//!
#define QTL_FOLDERID_SEARCH_HISTORY "0D4C3DB6-03A3-462F-A0E6-08924C41B5D4"

//!
//! Folder: Search Results (virtual)
//!
#define QTL_FOLDERID_SEARCH_HOME "190337D1-B8CA-4121-A639-6D472D16972A"

//!
//! Folder: Microsoft Office Outlook (virtual)
//!
#define QTL_FOLDERID_SEARCH_MAPI "98EC0E18-2098-4D44-8644-66979315A281"

//!
//! Folder: Templates (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows/ConnectedSearch/Templates
//!
#define QTL_FOLDERID_SEARCH_TEMPLATES "7E636BFE-DFA9-4D5E-B456-D7B39851D8A9"

//!
//! Folder: SendTo (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/SendTo
//!
#define QTL_FOLDERID_SEND_TO "8983036C-27C0-404B-8F08-102D10DCFD74"

//!
//! Folder: Gadgets (common)
//! Default Path: %ProgramFiles%/Windows Sidebar/Gadgets
//!
#define QTL_FOLDERID_SIDEBAR_DEFAULT_PARTS "7B396E54-9EC5-4300-BE0A-2482EBAE1A26"

//!
//! Folder: Gadgets (per user)
//! Default Path: %LOCALAPPDATA%/Microsoft/Windows Sidebar/Gadgets
//!
#define QTL_FOLDERID_SIDEBAR_PARTS "A75D362E-50FC-4FB7-AC2C-A8BEAA314493"

//!
//! Folder: OneDrive (per user)
//! Default Path: %USERPROFILE%/OneDrive
//!
#define QTL_FOLDERID_SKY_DRIVE "A52BBA46-E9E1-435f-B3D9-28DAA648C0F6"

//!
//! Folder: Camera Roll (per user)
//! Default Path: %USERPROFILE%/OneDrive/Pictures/Camera Roll
//!
#define QTL_FOLDERID_SKY_DRIVE_CAMERA_ROLL "767E6811-49CB-4273-87C2-20F355E1085B"

//!
//! Folder: Documents (per user)
//! Default Path: %USERPROFILE%/OneDrive/Documents
//!
#define QTL_FOLDERID_SKY_DRIVE_DOCUMENTS "24D89E24-2F19-4534-9DDE-6A6671FBB8FE"

//!
//! Folder: Pictures (per user)
//! Default Path: %USERPROFILE%/OneDrive/Pictures
//!
#define QTL_FOLDERID_SKY_DRIVE_PICTURES "339719B5-8C47-4894-94C2-D8F77ADD44A6"

//!
//! Folder: Start Menu (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Start Menu
//!
#define QTL_FOLDERID_START_MENU "625B53C3-AB48-4EC1-BA1F-A1EF4146FC19"

//!
//! Folder: Startup (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Start Menu/Programs/StartUp
//!
#define QTL_FOLDERID_STARTUP "B97D20BB-F46A-4C97-BA10-5E3608430854"

//!
//! Folder: Sync Center (virtual)
//!
#define QTL_FOLDERID_SYNC_MANAGER "43668BF8-C14E-49B2-97C9-747784D784B7"

//!
//! Folder: Sync Results (virtual)
//!
#define QTL_FOLDERID_SYNC_RESULTS "289A9A43-BE44-4057-A41B-587A76D7E7F9"

//!
//! Folder: Sync Setup (virtual)
//!
#define QTL_FOLDERID_SYNC_SETUP "0F214138-B1D3-4A90-BBA9-27CBC0C5389A"

//!
//! Folder: System32 (fixed)
//! Default Path: %windir%/system32
//!
#define QTL_FOLDERID_SYSTEM "1AC14E77-02E7-4E5D-B744-2EB1AE5198B7"

//!
//! Folder: System32 (fixed)
//! Default Path: %windir%/system32
//!
#define QTL_FOLDERID_SYSTEM_X86 "D65231B0-B2F1-4857-A4CE-A8E7C6EA7D27"

//!
//! Folder: Templates (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Templates
//!
#define QTL_FOLDERID_TEMPLATES "A63293E8-664E-48DB-A079-DF759E0509F7"

//!
//! Folder: User Pinned (per user)
//! Default Path: %APPDATA%/Microsoft/Internet Explorer/Quick Launch/User Pinned
//!
#define QTL_FOLDERID_USER_PINNED "9E3995AB-1F9C-4F13-B827-48B24B6C7174"

//!
//! Folder: Users (fixed)
//! Default Path: %SystemDrive%/Users
//!
#define QTL_FOLDERID_USER_PROFILES "0762D272-C50A-4BB0-A382-697DCD729B80"

//!
//! Folder: Programs (per user)
//! Default Path: %LOCALAPPDATA%/Programs
//!
#define QTL_FOLDERID_USER_PROGRAM_FILES "5CD7AEE2-2219-4A67-B85D-6C9CE15660CB"

//!
//! Folder: Programs (per user)
//! Default Path: %LOCALAPPDATA%/Programs/Common
//!
#define QTL_FOLDERID_USER_PROGRAM_FILES_COMMON "BCBD3057-CA5C-4622-B42D-BC56DB0AE516"

//!
//! Folder: The user's full name (for instance, Jean Philippe Bagel) entered when the user account was created. (virtual)
//!
#define QTL_FOLDERID_USERS_FILES "f3ce0f7c-4901-4acc-8648-d5d44b04ef8f"

//!
//! Folder: Libraries (virtual)
//!
#define QTL_FOLDERID_USERS_LIBRARIES "A302545D-DEFF-464b-ABE8-61C8648D939B"

//!
//! Folder: Videos (per user)
//! Default Path: %USERPROFILE%/Videos
//!
#define QTL_FOLDERID_VIDEOS "18989B1D-99B5-455B-841C-AB7C74E4DDFC"

//!
//! Folder: Videos (per user)
//! Default Path: %APPDATA%/Microsoft/Windows/Libraries/Videos.library-ms
//!
#define QTL_FOLDERID_VIDEOS_LIBRARY "491E922F-5643-4AF4-A7EB-4E7A138D8174"

//!
//! Folder: Windows (fixed)
//! Default Path: %windir%
//!
#define QTL_FOLDERID_WINDOWS "F38BF404-1D43-42F2-9305-67DE0B28FC23"

#endif // Q_OS_WIN || DOXYGEN
#endif // QTLWINUTILS_H
