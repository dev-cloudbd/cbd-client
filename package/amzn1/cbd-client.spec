Name:           cbd-client
Version:        3.15.3
Release:        1%{dist}
Summary:        CloudBD Network Block Device user-space tools
License:        GPLv2
URL:            https://www.github.com/dev-cloudbd/cbd-client
Source0:        %{name}-%{version}.tar.gz
BuildRequires:  glib2-devel
Requires(pre): /usr/sbin/useradd, /usr/bin/getent, /usr/sbin/groupadd

%pre
/usr/bin/getent group cloudbd > /dev/null || /usr/sbin/groupadd -r cloudbd
/usr/bin/getent passwd cloudbd > /dev/null || /usr/sbin/useradd -r -g cloudbd cloudbd

%description 
Tools for the Linux Kernel's network block device, allowing you to use
remote block devices over a TCP/IP network.

%prep
%setup -q

%build
%configure
%make_build

%install
%make_install

%files
%license COPYING
%{_mandir}/man*/cbd*
%{_sbindir}/cbd-client
%{_sbindir}/cbddisks_start
%{_sbindir}/cbddisks_stop
%config(noreplace) %{_sysconfdir}/cloudbd/cbdtab
%{_sysconfdir}/init.d/cbddisks
%{_libdir}/udev/cbd_id
%{_libdir}/udev/cbd_dmsetup
%{_libdir}/udev/rules.d/99-cbd.rules
%{_libdir}/cbdsetup/cbdsetup.functions

%changelog
* Tue Sep 12 2017 Shaun McDowell <smcdowell@cloudbd.io> - 3.15.2-1
- Initial amzn rpmbuild
