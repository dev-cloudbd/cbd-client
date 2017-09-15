dnl Configure script for CloudBD CBD system
dnl (c) 1998 Martin Mares <mj@ucw.cz>, (c) 2000 Pavel Machek <pavel@ucw.cz>,
dnl (c) 2003-2006 Wouter Verhelst <wouter@debian.org>
dnl (c) 2017 CloudBD, LLC
AC_INIT([cbd-client],
  [3.15.3],
  [smcdowell@cloudbd.io],,
  [https://www.github.com/dev-cloudbd/cbd-client/])
AM_INIT_AUTOMAKE(foreign subdir-objects)
AM_MAINTAINER_MODE([enable])
AC_CONFIG_MACRO_DIR([support])
LT_INIT

AC_PROG_CC_C99
AC_PROG_CPP
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG
AC_CANONICAL_HOST
AC_C_BIGENDIAN
AC_C_INLINE
AC_C_CONST
AC_CHECK_SIZEOF(unsigned short int)
AC_CHECK_SIZEOF(unsigned int)
AC_CHECK_SIZEOF(unsigned long int)
AC_CHECK_SIZEOF(unsigned long long int)
AC_STRUCT_DIRENT_D_TYPE
AC_CHECK_FUNCS([inet_ntoa memset socket strerror strstr mkstemp])

PKG_CHECK_MODULES(GnuTLS, [gnutls >= 2.12.0],[HAVE_GNUTLS=1],[HAVE_GNUTLS=0])

AC_FUNC_FORK
AC_FUNC_SETVBUF_REVERSED
CBD_CLIENT_NAME="cbd-client"
AM_CONDITIONAL(CLIENT, [test ! -z "$CBD_CLIENT_NAME"])
AC_SEARCH_LIBS(bind, socket,, AC_MSG_ERROR([Could not find an implementation of the bind() system call]))
AC_SEARCH_LIBS(inet_ntoa, nsl,, AC_MSG_ERROR([Could not find an implementation of the inet_ntoa() system call]))
AC_SEARCH_LIBS(daemon, resolv,, AC_MSG_ERROR([Could not find an implementation of the daemon() system call]))
AC_CHECK_HEADERS([sys/mount.h],,,
[[#include <sys/param.h>
]])
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h sys/ioctl.h sys/socket.h syslog.h linux/types.h sys/dirent.h])
AM_PATH_GLIB_2_0(2.26.0, [HAVE_GLIB=yes], AC_MSG_ERROR([Missing glib]), gthread)

my_save_cflags="$CFLAGS"
CFLAGS="-Wdeprecated-declarations -Werror $GLIB_CFLAGS"
AC_MSG_CHECKING([if we are using an old glib 2.0 library])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
	[[#include <glib.h>]],
	[[g_thread_init(NULL);]])],
	[
		AC_MSG_RESULT(yes)
		AC_DEFINE(HAVE_OLD_GLIB, 1, [Define to 1 if you have an old glib library])
	],
	[
		AC_MSG_RESULT(no)
		AC_DEFINE(HAVE_OLD_GLIB, 0, [Define to 1 if you have an old glib library])
	]
)
CFLAGS="$my_save_cflags"
NBD_LFS=1
AC_HEADER_SYS_WAIT
AC_TYPE_OFF_T
AC_TYPE_PID_T
AM_CPPFLAGS=$AM_CPPFLAGS" -DSYSCONFDIR='\"$sysconfdir\"' -DRUNSTATEDIR='\"$runstatedir\"'"
AC_SUBST(AM_CPPFLAGS)
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile
		 doc/Doxyfile
		 doc/Makefile
		 man/Makefile
		 man/nbd-client.8.sh
		 man/nbdtab.5.sh
		 src/Makefile
		 systemd/Makefile
		 systemd/nbd@.service.sh
		])
AC_OUTPUT

