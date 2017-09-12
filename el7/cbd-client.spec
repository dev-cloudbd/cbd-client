Name:           cbd-client
Version:        3.15.2
Release:        1cbd%{dist}
Summary:        Network Block Device user-space tools (TCP version)
License:        GPLv2
URL:            https://www.github.com/dev-cloudbd/cbd-client
Source0:        %{name}-%{version}.tar.gz
Source1:        nbd-server.service
Source2:        nbd-server.sysconfig
# include a file from upstream git, which is missed in tarball
Source3:        nbd@.service.tmpl
BuildRequires:  glib2-devel
BuildRequires:  zlib-devel
BuildRequires:  systemd
Requires(post): systemd
Requires(preun): systemd
Requires(postun): systemd

%description 
Tools for the Linux Kernel's network block device, allowing you to use
remote block devices over a TCP/IP network.

%prep
%setup -q
cp %{SOURCE3} systemd

%build
%configure --enable-syslog --enable-lfs --enable-gznbd
%make_build

%install
%make_install
install -pDm644 systemd/nbd@.service %{buildroot}%{_unitdir}/nbd@.service
install -pDm644 %{S:1} %{buildroot}%{_unitdir}/nbd-server.service
install -pDm644 %{S:2} %{buildroot}%{_sysconfdir}/sysconfig/nbd-server

%post
%systemd_post %{S:1}

%preun
%systemd_preun %{S:1}

%postun
%systemd_postun_with_restart %{S:1}

%files
%doc README.md doc/proto.md doc/todo.txt
%license COPYING
%{_bindir}/nbd-server
%{_bindir}/nbd-trdump
%{_bindir}/gznbd
%{_mandir}/man*/nbd*
%{_sbindir}/nbd-client
%config(noreplace) %{_sysconfdir}/sysconfig/nbd-server
%{_unitdir}/nbd-server.service
%{_unitdir}/nbd@.service

%changelog
* Wed Aug 17 2016 Robin Lee <cheeselee@fedoraproject.org> - 3.14-2
- Install the nbd@.service systemd unit file (BZ#1367679)
