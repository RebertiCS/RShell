Name:           RShell
Version:        master
Release:        1%{?dist}
Summary:        A minimal shell

License:        GPLv2
URL:            https://gitlab.com/KisuYami/RShell
Source0:        https://gitlab.com/KisuYami/RShell/-/archive/master/RShell-master.tar.gz

BuildRequires:  readline-devel, make
Requires:       readline

%define pkg_dir %{_topdir}/BUILDROOT/RShell-master-1%{?dist}.%{_arch}
%global debug_package %{nil}

%description
A minimal shell with my name.

%prep
%autosetup

%build
%make_build

%install
mkdir -p %{pkg_dir}/usr/bin
mkdir -p %{pkg_dir}/usr/share/man/man1
mkdir -p %{pkg_dir}/usr/share/man/pt_BR/man1

cd %{_topdir}/BUILD/RShell-master/

install -Dm644 man/rshell.1 %{pkg_dir}/usr/share/man/man1/
install -Dm644 man/pt_BR/rshell.1 %{pkg_dir}/usr/share/man/pt_BR/man1/

install -Dm644 build/rshell %{pkg_dir}/usr/bin
chmod +x %{pkg_dir}/usr/bin/rshell

%files
/usr/bin/rshell
/usr/share/man/man1/rshell.1.gz
/usr/share/man/pt_BR/man1/rshell.1.gz
