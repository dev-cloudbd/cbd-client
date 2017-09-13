ACLOCAL_AMFLAGS = -I support

noinst_LTLIBRARIES = libcliserv.la
libcliserv_la_SOURCES = cliserv.h cliserv.c nbd.h nbd-debug.h
libcliserv_la_CFLAGS = @CFLAGS@

sbin_PROGRAMS = cbd-client
client_srcs = nbd-client.c cliserv.h lfs.h netdb-compat.h
client_flags = @CFLAGS@
client_libs = libcliserv.la

EXTRA_DIST = autogen.sh README.md support/genver.sh

if GNUTLS
cbd_client_SOURCES = $(client_srcs) crypto-gnutls.c crypto-gnutls.h buffer.c buffer.h
cbd_client_CFLAGS = $(client_flags) @GnuTLS_CFLAGS@
cbd_client_LDADD = $(client_libs) @GnuTLS_LIBS@
else
cbd_client_SOURCES = $(client_srcs)
cbd_client_CFLAGS = $(client_flags)
cbd_client_LDADD = $(client_libs)
endif
