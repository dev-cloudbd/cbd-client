systemd_unit_dir = $(systemdunitdir)
systemd_unit__DATA = cbdsetup@.service cbdsetup.target cbdsetup-pre.target
systemd_generator_dir = $(systemdgendir)
systemd_generator__SCRIPTS = cbdsetup-generator

conf_cloudbd_dir = $(sysconfdir)/cloudbd
conf_cloudbd__DATA = cbdtab
conf_cloudbd_remotes.d_dir = $(sysconfdir)/cloudbd/remotes.d
conf_cloudbd_remotes.d__DATA = \
	aws.conf.sample \
	gcs.conf.sample \
	openstack-keystone-v3.conf.sample \
	openstack-keystone-v2.conf.sample \
	openstack-swiftstack.conf.sample

conf_init_dir = $(sysconfdir)/init.d
conf_init__SCRIPTS = cbddisks

udev_dir = $(udevdir)
udev__SCRIPTS = cbd_id cbd_dmsetup
udev_rules_dir = $(udevdir)/rules.d
udev_rules__DATA = 99-cbd.rules

sbin_SCRIPTS = cbddisks_start cbddisks_stop
cbdsetup_dir = $(libdir)/cbdsetup
cbdsetup__DATA = cbdsetup.functions

EXTRA_DIST = \
	99-cbd.rules.tmpl \
	cbd_id.tmpl \
	cbd_dmsetup.tmpl \
	cbddisks.tmpl \
	cbddisks_start.tmpl \
	cbddisks_stop.tmpl \
	cbdsetup.functions.tmpl \
	cbdsetup.target \
	cbdsetup-pre.target \
	cbdsetup@.service.tmpl \
	cbdsetup-generator.tmpl \
	cbdtab \
	aws.conf.sample \
	gcs.conf.sample \
	openstack-keystone-v3.conf.sample \
	openstack-keystone-v2.conf.sample \
	openstack-swiftstack.conf.sample \
	sh.tmpl 

cbdsetup@.service: cbdsetup@.service.sh
	sh cbdsetup@.service.sh > cbdsetup@.service

cbdsetup@.service.sh.in: cbdsetup@.service.tmpl sh.tmpl
	cat sh.tmpl cbdsetup@.service.tmpl > cbdsetup@.service.sh.in
	echo EOF >> cbdsetup@.service.sh.in

cbdsetup-generator: cbdsetup-generator.sh
	sh cbdsetup-generator.sh > cbdsetup-generator
	chmod +x cbdsetup-generator

cbdsetup-generator.sh.in: cbdsetup-generator.tmpl sh.tmpl
	cat sh.tmpl cbdsetup-generator.tmpl > cbdsetup-generator.sh.in
	echo EOF >> cbdsetup-generator.sh.in

cbd_id: cbd_id.sh
	sh cbd_id.sh > cbd_id
	chmod +x cbd_id

cbd_id.sh.in: cbd_id.tmpl sh.tmpl
	cat sh.tmpl cbd_id.tmpl > cbd_id.sh.in
	echo EOF >> cbd_id.sh.in

cbd_dmsetup: cbd_dmsetup.sh
	sh cbd_dmsetup.sh > cbd_dmsetup
	chmod +x cbd_dmsetup

cbd_dmsetup.sh.in: cbd_dmsetup.tmpl sh.tmpl
	cat sh.tmpl cbd_dmsetup.tmpl > cbd_dmsetup.sh.in
	echo EOF >> cbd_dmsetup.sh.in

cbddisks: cbddisks.sh
	sh cbddisks.sh > cbddisks
	chmod +x cbddisks

cbddisks.sh.in: cbddisks.tmpl sh.tmpl
	cat sh.tmpl cbddisks.tmpl > cbddisks.sh.in
	echo EOF >> cbddisks.sh.in

cbddisks_start: cbddisks_start.sh
	sh cbddisks_start.sh > cbddisks_start
	chmod +x cbddisks_start

cbddisks_start.sh.in: cbddisks_start.tmpl sh.tmpl
	cat sh.tmpl cbddisks_start.tmpl > cbddisks_start.sh.in
	echo EOF >> cbddisks_start.sh.in

cbddisks_stop: cbddisks_stop.sh
	sh cbddisks_stop.sh > cbddisks_stop
	chmod +x cbddisks_stop

cbddisks_stop.sh.in: cbddisks_stop.tmpl sh.tmpl
	cat sh.tmpl cbddisks_stop.tmpl > cbddisks_stop.sh.in
	echo EOF >> cbddisks_stop.sh.in

cbdsetup.functions: cbdsetup.functions.sh
	sh cbdsetup.functions.sh > cbdsetup.functions

cbdsetup.functions.sh.in: cbdsetup.functions.tmpl sh.tmpl
	cat sh.tmpl cbdsetup.functions.tmpl > cbdsetup.functions.sh.in
	echo EOF >> cbdsetup.functions.sh.in

99-cbd.rules: 99-cbd.rules.sh
	sh 99-cbd.rules.sh > 99-cbd.rules

99-cbd.rules.sh.in: 99-cbd.rules.tmpl sh.tmpl
	cat sh.tmpl 99-cbd.rules.tmpl > 99-cbd.rules.sh.in
	echo EOF >> 99-cbd.rules.sh.in

MOSTLYCLEANFILES = cbdsetup@.service cbdsetup-generator cbd_id cbd_dmsetup cbddisks_start cbddisks_stop cbdsetup.functions cbddisks 99-cbd.rules
CLEANFILES = *~
DISTCLEANFILES = cbdsetup@.service.sh cbdsetup-generator.sh cbd_id.sh cbd_dmsetup.sh cbddisks_start.sh cbddisks_stop.sh cbdsetup.functions.sh cbddisks.sh 99-cbd.rules.sh
