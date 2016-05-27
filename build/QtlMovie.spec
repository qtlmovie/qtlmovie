Name:          qtlmovie
Version:       %{version}
Release:       %{release}
Summary:       Graphical frontend to FFmpeg and other media tools

Group:         Applications/Multimedia
License:       BSD
URL:           http://qtlmovie.sourceforge.net/
Vendor:        Thierry Lelegard
Source0:       http://sourceforge.net/projects/qtlmovie/files/src/QtlMovie-%{version}-src.zip
BuildRoot:     %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: binutils
BuildRequires: qt5-qtbase
BuildRequires: qt5-qttools
BuildRequires: qt5-qtbase-devel
BuildRequires: qt5-qtbase-x11
BuildRequires: qt5-qtmultimedia-devel
BuildRequires: qt5-qttools-devel

# Installation requirements:
# The package ffmpeg can be found in the RPMfusion repository. The packages
# genisoimage and dvd+rw-tools respectively provide mkisofs and growisofs.
# The package ccextractor is not available in standard
# repositories but packages are available on the QtlMovie site.

Requires: ffmpeg >= 1.2
Requires: genisoimage
Requires: dvd+rw-tools
Requires: dvdauthor
Requires: ccextractor >= 0.67

%description
QtlMovie is a graphical frontend to FFmpeg, DVD Author, CCExtractor, mkisofs
and growisofs.

QtlMovie is not a general purpose frontend to FFmpeg. It focuses on a small
number of tasks such as the generation of DVD or iPad movie files from any
video file or DVD. The unique feature of QtlMovie is the ability to correctly
handle most types of subtitles: DVD, DVB, SRT, SSA/ASS, Teletext, Closed
Captions and burn them into the output video in one click.

# Disable debuginfo package. Required on Fedora 23.
%global debug_package %{nil}

%prep
%setup -q -n QtlMovie-%{version}

%build
build/build.sh

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p \
    $RPM_BUILD_ROOT/usr/bin \
    $RPM_BUILD_ROOT/usr/share/qt5/translations \
    $RPM_BUILD_ROOT/usr/share/applications
cp build-QtlMovie-Desktop-Release/QtlMovie/QtlMovie $RPM_BUILD_ROOT/usr/bin
strip $RPM_BUILD_ROOT/usr/bin/QtlMovie
cp build-QtlMovie-Desktop-Release/Qtl/locale/qtl_fr.qm \
   build-QtlMovie-Desktop-Release/QtlMovie/locale/qtlmovie_fr.qm \
   build-QtlMovie-Desktop-Release/Qts/locale/qts_fr.qm \
   $RPM_BUILD_ROOT/usr/share/qt5/translations
cp build/QtlMovie.desktop $RPM_BUILD_ROOT/usr/share/applications
ICON_SIZES="16 24 32 48 64 72 96 128 256 512"
for sz in $ICON_SIZES; do
    mkdir -p $RPM_BUILD_ROOT/usr/share/icons/hicolor/${sz}x${sz}/apps
    cp src/QtlMovie/images/qtlmovie-${sz}.png $RPM_BUILD_ROOT/usr/share/icons/hicolor/${sz}x${sz}/apps/qtlmovie.png
done

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
/usr/bin/QtlMovie
/usr/share/qt5/translations/qtl_fr.qm
/usr/share/qt5/translations/qtlmovie_fr.qm
/usr/share/qt5/translations/qts_fr.qm
/usr/share/applications/QtlMovie.desktop
/usr/share/icons/hicolor/16x16/apps/qtlmovie.png
/usr/share/icons/hicolor/24x24/apps/qtlmovie.png
/usr/share/icons/hicolor/32x32/apps/qtlmovie.png
/usr/share/icons/hicolor/48x48/apps/qtlmovie.png
/usr/share/icons/hicolor/64x64/apps/qtlmovie.png
/usr/share/icons/hicolor/72x72/apps/qtlmovie.png
/usr/share/icons/hicolor/96x96/apps/qtlmovie.png
/usr/share/icons/hicolor/128x128/apps/qtlmovie.png
/usr/share/icons/hicolor/256x256/apps/qtlmovie.png
/usr/share/icons/hicolor/512x512/apps/qtlmovie.png
%doc CHANGELOG.txt LICENSE.txt TODO.txt
