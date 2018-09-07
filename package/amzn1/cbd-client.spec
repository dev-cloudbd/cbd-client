Name:           cbd-client
Version:        %{_version}
Release:        %{_release}%{dist}
Summary:        CloudBD Network Block Device user-space tools
License:        GPLv2
URL:            https://www.github.com/dev-cloudbd/cbd-client
Source0:        %{name}-%{version}.tar.gz
BuildRequires:  glib2-devel
Requires(pre): /usr/sbin/useradd, /usr/bin/getent, /usr/sbin/groupadd
Requires:       cloudbd >= 2.1.0

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
%{_sysconfdir}/cloudbd/remotes.d/aws.conf.sample
%{_sysconfdir}/cloudbd/remotes.d/gcs.conf.sample
%{_sysconfdir}/cloudbd/remotes.d/openstack-keystone-v3.conf.sample
%{_sysconfdir}/cloudbd/remotes.d/openstack-keystone-v2.conf.sample
%{_sysconfdir}/cloudbd/remotes.d/openstack-swiftstack.conf.sample
%{_sysconfdir}/init.d/cbddisks
%{_libdir}/udev/cbd_id
%{_libdir}/udev/cbd_dmsetup
%{_libdir}/udev/rules.d/99-cbd.rules
%{_libdir}/cbdsetup/cbdsetup.functions

%changelog
* Mon Aug 27 2018 Shaun McDowell <smcdowell@cloudbd.io> - 3.15.3-1
- Production release

* Tue Sep 12 2017 Shaun McDowell <smcdowell@cloudbd.io> - 3.15.2-1
- Initial amzn rpmbuild
