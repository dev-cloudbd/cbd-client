#if SYSTEMD
#systemdunitdir = $(DESTDIR)/$(SYSTEMDLOC)
#systemdunit_DATA = nbd@.service
#endif

noinst_DATA = cbd@.service
DISTCLEANFILES = cbd@.service
EXTRA_DIST=cbd@.service.tmpl

cbd@.service: cbd@.service.sh
	sh cbd@.service.sh > cbd@.service

cbd@.service.sh.in: cbd@.service.tmpl ../man/sh.tmpl
	cat ../man/sh.tmpl cbd@.service.tmpl > cbd@.service.sh.in
	echo EOF >> cbd@.service.sh.in
