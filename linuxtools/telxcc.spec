Name:           telxcc
Version:        %{version}
Release:        %{release}
Summary:        Teletext extractor

Group:          Applications/Multimedia
License:        GPL
URL:            https://github.com/forers/telxcc
Vendor:         Petr Kutalek
Source0:        telxcc-%{version}.zip
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  gcc
BuildRequires:  make
BuildRequires:  binutils

%description
Telxcc is a utility extracting teletext Closed Captions from Transport Stream
binary files (TS & M2TS) into SubRip text files (SRT).

%prep
%setup -q -n telxcc-master

%build
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin $RPM_BUILD_ROOT/%{_mandir}
cp telxcc $RPM_BUILD_ROOT/usr/bin
cp telxcc.1 $RPM_BUILD_ROOT/%{_mandir}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
/usr/bin/telxcc
%{_mandir}/telxcc.1

%doc LICENSE README.md
