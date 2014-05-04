%define         defined()   %{expand:%%{?%{1}:1}%%{!?%{1}:0}}
%define         undefined() %{expand:%%{?%{1}:0}%%{!?%{1}:1}}

Name:           ccextractor
Version:        %{version}
Release:        %{release}
Summary:        Closed captions extractor

Group:          Applications/Multimedia
License:        GPL
URL:            http://ccextractor.sourceforge.net
Vendor:         Carlos Fernandez (cfsmp3), Volker Quetschke.
Source0:        http://sourceforge.net/projects/ccextractor/files/ccextractor/%{version}/ccextractor.src.%{version}.zip
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
%if %{defined patch}
Patch0:         %{patch}
%endif

BuildRequires:  gcc
BuildRequires:  gcc-c++
BuildRequires:  make
BuildRequires:  binutils

%description
A tool that analyzes video files and produces independent subtitle files from
the closed captions data. CCExtractor is portable, small, and very fast.
American TV captions (CEA-608 is well supported, and CEA-708 is starting to
look good) and Teletext based European subtitles.

%prep
%setup -q -n ccextractor.%{version}
%if %{defined patch}
%patch0 -p1
%endif

%build
(cd linux; ./build)

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
cp linux/ccextractor $RPM_BUILD_ROOT/usr/bin

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
/usr/bin/ccextractor
%doc docs/*
