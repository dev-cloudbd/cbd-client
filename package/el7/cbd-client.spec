Name:           cbd-client
Version:        %{_version}
Release:        %{_release}%{dist}
Summary:        CloudBD Network Block Device user-space tools
License:        GPLv2
URL:            https://www.github.com/dev-cloudbd/cbd-client
Source0:        %{name}-%{version}.tar.gz
BuildRequires:  glib2-devel
%{?systemd_requires}
BuildRequires:  systemd
Requires(pre): /usr/sbin/useradd, /usr/bin/getent, /usr/sbin/groupadd
Requires(post): systemd
Requires(preun): systemd
Requires(postun): systemd
Requires:       cloudbd >= 2.2.4, cbdkit >= 1.4.4

%description 
Tools for the Linux Kernel's network block device, allowing you to use
remote block devices over a TCP/IP network.

%pre
/usr/bin/getent group cloudbd > /dev/null || /usr/sbin/groupadd -r cloudbd
/usr/bin/getent passwd cloudbd > /dev/null || /usr/sbin/useradd -r -g cloudbd cloudbd

%prep
%setup -q

%build
%configure --enable-systemd
%make_build

%install
%make_install
ln -s /dev/null %{buildroot}%{_unitdir}/cbddisks.service

%post -p /bin/bash
%systemd_post %{S:1}
if [ $1 -eq 2 ]; then
  for sock in /var/run/cloudbd/*.socket; do
    if expr "x$sock" : "^x/var/run/cloudbd/[[:alnum:]_-]\+:[[:alnum:]_-]\+\.socket\$" >/dev/null 2>&1; then
      mv "$sock" "${sock%%\.socket}:0.socket"
    fi
  done
fi

%preun
%systemd_preun %{S:1}

%postun
%systemd_postun_with_restart %{S:1}

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
%{_libdir}/udev/cbd_id
%{_libdir}/udev/cbd_dmsetup
%{_udevrulesdir}/99-cbd.rules
%{_libdir}/cbdsetup/cbdsetup.functions
%{_unitdir}/cbdsetup@.service
%{_unitdir}/cbdsetup.target
%{_unitdir}/cbdsetup-pre.target
%{_libdir}/systemd/systemd-cbdsetup
%{_libdir}/systemd/system-generators/cbdsetup-generator
%{_unitdir}/cbddisks.service

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

