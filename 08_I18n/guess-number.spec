Name: guess-number
Version: 1.0
Release: alt1
Summary: Guess a number using binary search
License: MIT
Group: Games/Other
Source: %name-%version.tar.gz
BuildRequires: gcc autoconf automake gettext gettext-tools python3

%description
An interactive number guessing game with GNU gettext Russian localization.

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
%find_lang %name

%files -f %name.lang
%doc LICENSE
%_bindir/%name

%changelog
* Mon Nov 09 2026 Alex Student <alex@example.org> 1.0-alt1
- Initial build with Russian translation and autotools gettext support.
