Name: safemove
Version: 1.0
Release: alt1
Packager: Alex Student <alex@example.org>
Summary: File copying move with preload deletion protection
License: MIT
Group: File tools
Source: %name-%version.tar.gz
BuildRequires: gcc make python3 strace

%description
Move regular files with checked I/O and protect filenames containing PROTECT
using an LD_PRELOAD library and the safemove wrapper.

%prep
%setup

%build
%make_build LIBDIR=%_libdir BINDIR=%_bindir

%check
%make test

%install
%makeinstall_std LIBDIR=%_libdir BINDIR=%_bindir

%files
%doc README.md
%_bindir/move
%_bindir/safemove
%_libdir/libprotect.so

%changelog
* Fri Oct 23 2026 Alex Student <alex@example.org> 1.0-alt1
- Initial version.
