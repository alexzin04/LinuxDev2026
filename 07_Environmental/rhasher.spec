Name: rhasher
Version: 1.0
Release: alt1
Summary: Simple interactive file and string hashing utility
License: MIT
Group: File tools
Packager: Alex Student <alex@example.org>
Source: %name-%version.tar.gz
BuildRequires: gcc autoconf automake librhash-devel libreadline-devel coreutils

%description
A small REPL using librhash to compute MD5, SHA1 and TTH in hexadecimal
or Base64, with optional readline support.

%prep
%setup

%build
%autoreconf
%configure
%make_build

%check
%make_build check

%install
%makeinstall_std

%files
%doc LICENSE
%_bindir/rhasher

%changelog
* Fri Oct 30 2026 Alex Student <alex@example.org> 1.0-alt1
- Initial version.
