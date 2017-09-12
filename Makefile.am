ACLOCAL_AMFLAGS = -I support
SUBDIRS = . man doc systemd
EXTRA_PROGRAMS = nbd-client
noinst_LTLIBRARIES = libcliserv.la
libcliserv_la_SOURCES = cliserv.h cliserv.c nbd.h nbd-debug.h
libcliserv_la_CFLAGS = @CFLAGS@
client_srcs = nbd-client.c cliserv.h lfs.h netdb-compat.h
client_flags = @CFLAGS@
client_libs = libcliserv.la
EXTRA_DIST = maketr CodingStyle autogen.sh README.md support/genver.sh
sbin_PROGRAMS = nbd-client
if GNUTLS
nbd_client_SOURCES = $(client_srcs) crypto-gnutls.c crypto-gnutls.h buffer.c buffer.h
nbd_client_CFLAGS = $(client_flags) @GnuTLS_CFLAGS@
nbd_client_LDADD = $(client_libs) @GnuTLS_LIBS@
else
nbd_client_SOURCES = $(client_srcs)
nbd_client_CFLAGS = $(client_flags)
nbd_client_LDADD = $(client_libs)
endif
