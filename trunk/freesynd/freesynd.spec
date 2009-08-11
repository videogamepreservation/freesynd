Summary: 	freesynd - a Free Syndicate Clone
Name: 	freesynd
Version: 	0.0.1
Release: 	1.1
Source: 	%{name}-%{version}.tar.bz2
Packager: 	Dominique Leuenberger <dominique-rpm@leuenberger.net>
License: 	GPL
Group: 	Amusement
BuildRoot:  %{_tmppath}/%{name}-%{version}-build
BuildRequires: gcc-c++ SDL-devel SDL_mixer-devel

%description
Freesynd is a cross-platform, 
GPLed reimplementation of the classic Bullfrog game, Syndicate.

%prep
%setup -q -n %{name}

%build 
make %{?jobs:-j %jobs}


%install
install -D -m 755 freesynd %{buildroot}%{_prefix}/games/%{name}
install -d -m 755 %{buildroot}%{_datadir}/games/%{name} 
install -m 644 -t %{buildroot}%{_datadir}/games/%{name} data/*

# Creating the Start Menu entry
mkdir -p %{buildroot}%{_datadir}/applications/
cat <<EOF > %{buildroot}%{_datadir}/applications/%{name}.desktop
[Desktop Entry]
Encoding=UTF-8
Version=1.0
Name=Free Syndicate
Type=Application
GenericName=Strategic Role Play Game
Exec=%{name} -p /usr/share/games/freesynd/
Icon=%{name}.png
Path=
Terminal=false

Categories=Game;StrategyGame;Game
EOF

%clean
rm -rf "$RPM_BUILD_ROOT"

%files
%defattr(-,root,root)
%{_prefix}/games/%{name}
%{_datadir}/games/%{name}
%{_datadir}/applications/%{name}.desktop


%changelog
* Fri Oct 20 2006 Dominique Leuenberger <dominique-rpm@leuenberger.net>
- Initial release of an RPM Package

