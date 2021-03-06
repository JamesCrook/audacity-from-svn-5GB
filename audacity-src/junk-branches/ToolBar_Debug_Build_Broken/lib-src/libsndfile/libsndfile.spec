
%define name    libsndfile
%define version 0.0.26
%define release 1
%define prefix  /usr

Summary: A library to handle various audio file formats.
Name: %{name}
Version: %{version}
Release: %{release}
Prefix: %{prefix}
Copyright: LGPL
Group: Libraries/Sound
Source: http://www.zip.com.au/~erikd/libsndfile/libsndfile-%{version}.tar.gz
URL: http://www.zip.com.au/~erikd/libsndfile/
BuildRoot: /var/tmp/%{name}-%{version}

%description
libsndfile is a C library for reading and writing sound files such as
AIFF, AU and WAV files through one standard interface. It can currently
read/write 8, 16, 24 and 32-bit PCM files as well as 32-bit floating
point WAV files and a number of compressed formats.

%package devel
Summary: Libraries, includes, etc to develop libsndfile applications
Group: Libraries

%description devel
Libraries, include files, etc you can use to develop libsndfile applications.

%prep
%setup

%build
./configure --prefix=%{prefix}
make

%install
if [ -d $RPM_BUILD_ROOT ]; then rm -rf $RPM_BUILD_ROOT; fi
mkdir -p $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
if [ -d $RPM_BUILD_ROOT ]; then rm -rf $RPM_BUILD_ROOT; fi

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README TODO doc
%prefix/lib/libsndfile.so.*

%files devel
%defattr(-,root,root)
%{prefix}/lib/libsndfile.a
%{prefix}/lib/libsndfile.la
%{prefix}/lib/libsndfile.so
%{prefix}/include/sndfile.h

%changelog
* Thu Jul 6 2000 Josh Green <jgreen@users.sourceforge.net>
- Created libsndfile.spec.in
