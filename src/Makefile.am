ACLOCAL_AMFLAGS = -I support

noinst_LTLIBRARIES = libcliserv.la
libcliserv_la_SOURCES = cliserv.h cliserv.c nbd.h
libcliserv_la_CFLAGS = @CFLAGS@

sbin_PROGRAMS = cbd-client
client_srcs = cbd-client.c cliserv.h netdb-compat.h
client_flags = @CFLAGS@
client_libs = libcliserv.la

cbd_client_SOURCES = $(client_srcs)
cbd_client_CFLAGS = $(client_flags)
cbd_client_LDADD = $(client_libs)
