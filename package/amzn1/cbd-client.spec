Name:           cbd-client
Version:        %{_version}
Release:        %{_release}%{dist}
Summary:        CloudBD Network Block Device user-space tools
License:        GPLv2
URL:            https://www.github.com/dev-cloudbd/cbd-client
Source0:        %{name}-%{version}.tar.gz
BuildRequires:  glib2-devel
Requires(pre): /usr/sbin/useradd, /usr/bin/getent, /usr/sbin/groupadd
Requires:       cloudbd >= 2.2.4, cbdkit >= 1.4.4

%pre
/usr/bin/getent group cloudbd > /dev/null || /usr/sbin/groupadd -r cloudbd
/usr/bin/getent passwd cloudbd > /dev/null || /usr/sbin/useradd -r -g cloudbd cloudbd

%post -p /bin/bash
if [ $1 -eq 1 ]; then
  chkconfig --add cbddisks
# Upgrade script for Multisock no longer needed but left as example
#elif [ $1 -eq 2 ]; then
#  for sock in /var/run/cloudbd/*.socket; do
#    if expr "x$sock" : "^x/var/run/cloudbd/[[:alnum:]_-]\+:[[:alnum:]_-]\+\.socket\$" >/dev/null 2>&1; then
#      mv "$sock" "${sock%%\.socket}:0.socket"
#    fi
#  done
fi

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

mkdir -p %{buildroot}%{_sysconfdir}/profile.d/
echo 'source %{_sysconfdir}/bash_completion.d/cloudbd' > %{buildroot}%{_sysconfdir}/profile.d/cloudbd-cli.sh

%files
%license COPYING
%{_mandir}/man*/cbdtab*
%{_sbindir}/cbd-client
%{_sbindir}/cbddisks_start
%{_sbindir}/cbddisks_stop
%config(noreplace) %{_sysconfdir}/cloudbd/cbdtab
%dir %attr(0750, root, cloudbd) %{_sysconfdir}/cloudbd/remotes.d
%{_sysconfdir}/cloudbd/remotes.d/aws.conf.sample
%{_sysconfdir}/cloudbd/remotes.d/gcs.conf.sample
%{_sysconfdir}/cloudbd/remotes.d/openstack-keystone-v3.conf.sample
%{_sysconfdir}/cloudbd/remotes.d/openstack-keystone-v2.conf.sample
%{_sysconfdir}/cloudbd/remotes.d/openstack-swiftstack.conf.sample
%{_sysconfdir}/init.d/cbddisks
%{_sysconfdir}/bash_completion.d/cloudbd
%{_sysconfdir}/profile.d/cloudbd-cli.sh
%{_libdir}/udev/cbd_id
%{_libdir}/udev/cbd_dmsetup
%{_libdir}/udev/rules.d/99-cbd.rules
%{_libdir}/cbdsetup/cbdsetup.functions

%changelog
* Fri Jan 4 2019 Shaun McDowell <smcdowell@cloudbd.io> - 4.0.8
- Production Release

* Wed Dec 19 2018 Shaun McDowell <smcdowell@cloudbd.io> - 4.0.7
- Production Release

* Tue Nov 20 2018 Shaun McDowell <smcdowell@cloudbd.io> - 4.0.5
- Production release

* Fri Nov 16 2018 Shaun McDowell <smcdowell@cloudbd.io> - 4.0.4
- Production release

* Mon Aug 27 2018 Shaun McDowell <smcdowell@cloudbd.io> - 3.15.3-1
- Production release

* Tue Sep 12 2017 Shaun McDowell <smcdowell@cloudbd.io> - 3.15.2-1
- Initial amzn rpmbuild
