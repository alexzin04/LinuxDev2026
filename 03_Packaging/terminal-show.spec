Name: terminal-show
Version: 1.0
Release: alt1

Summary: Simple ncurses file viewer
License: MIT
Group: Text tools

Source: %name-%version.tar.gz

BuildRequires: gcc
BuildRequires: make
BuildRequires: libncurses-devel

%description
Terminal-show is a small ncurses program that displays a text file and lets the
user scroll through it with keyboard commands.

%prep
%setup

%build
%make_build

%install
%makeinstall_std prefix=%_prefix

%files
%_bindir/Show

%changelog
* Fri Oct 09 2026 Alex <alex@example.com> 1.0-alt1
- Initial RPM package.
