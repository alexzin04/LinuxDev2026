Name: ioccc-kurdyukov
Version: 2025
Release: alt1

Summary: IOCCC 2025 kurdyukov magic equations program
License: CC-BY-SA-4.0
Group: Games/Other
Url: https://www.ioccc.org/2025/kurdyukov/

Source: 2025_kurdyukov.tar.bz2

BuildRequires: gcc
BuildRequires: make

%description
The IOCCC 2025 kurdyukov entry finds short arithmetic expressions that use
given numbers and the +, -, *, / operators to represent target values.

%prep
%setup -n 2025

%build
cd kurdyukov
%make_build all

%install
install -d %buildroot%_bindir
install -m 0755 kurdyukov/prog %buildroot%_bindir/kurdyukov

%files
%doc kurdyukov/README.md
%_bindir/kurdyukov

%changelog
* Fri Oct 09 2026 dep <dep@localhost.localdomain> 2025-alt1
- Initial package for IOCCC 2025 kurdyukov entry.
