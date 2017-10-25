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
AC_CHECK_FUNCS([memset socket strerror strstr])

AC_FUNC_FORK
AC_FUNC_SETVBUF_REVERSED
CBD_CLIENT_NAME="cbd-client"
AC_SEARCH_LIBS(bind, socket,, AC_MSG_ERROR([Could not find an implementation of the bind() system call]))
AC_SEARCH_LIBS(daemon, resolv,, AC_MSG_ERROR([Could not find an implementation of the daemon() system call]))
AC_CHECK_HEADERS([fcntl.h netdb.h netinet/in.h sys/ioctl.h sys/mount.h sys/socket.h sys/types.h sys/un.h syslog.h])
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
AC_HEADER_SYS_WAIT
AC_TYPE_OFF_T
AC_TYPE_PID_T


m4_ifdef([PKG_CHECK_VAR], [HAVE_PKG_CHECK_VAR=yes])

AC_MSG_CHECKING([where to put udev files])
if test "x$HAVE_PKG_CHECK_VAR" = "xyes"; then
    PKG_CHECK_VAR([UDEVDIR], [udev], [udevdir], [AC_SUBST([udevdir], [$UDEVDIR])])
else
    AC_SUBST([udevdir], ["$( pkg-config --variable=udevdir udev )"])
fi

if test -z $udevdir; then
	AC_MSG_ERROR([not found])
else
	AC_MSG_RESULT([found])
fi

dnl if test "$prefix" = "/usr" -o "$prefix" = "/usr/local" -o "$prefix" = "NONE"; then
dnl        AC_SUBST([UDEVDIR], ["$( pkg-config --variable=udevdir udev )/"])
dnl else
dnl        AC_SUBST([UDEVDIR], ["$prefix/lib/udev/"])
dnl fi

AC_ARG_ENABLE([systemd],
	AS_HELP_STRING([--enable-systemd],[Install systemd files using pkg-config to detect the correct location and install there.]),
	[
	  case "x${enableval}" in
	    xyes) SYSTEMD_ENABLED=yes ;;
	    xno)  SYSTEMD_ENABLED=no ;;
	    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-systemd]) ;;
	  esac
	],
	[SYSTEMD_ENABLED=no])

AC_MSG_CHECKING([where to put systemd unit files])
if test "x$SYSTEMD_ENABLED" = "xyes"; then
	if test "x$HAVE_PKG_CHECK_VAR" = "xyes"; then
		PKG_CHECK_VAR([SYSTEMDUNIT], [systemd], [systemdsystemunitdir], [AC_SUBST([systemdunitdir], [$SYSTEMDUNIT])])
		PKG_CHECK_VAR([SYSTEMDGEN], [systemd], [systemdsystemgeneratordir], [AC_SUBST([systemdgendir], [$SYSTEMDGEN])])
	else
		AC_SUBST([systemdunitdir], ["$( pkg-config --variable=systemdsystemunitdir systemd )"])
		AC_SUBST([systemdgendir], ["$( pkg-config --variable=systemdsystemgeneratordir systemd )"])
	fi
fi

AM_CONDITIONAL([SYSTEMD], [test -n "$systemdunitdir" && test -n "$systemdgendir"])

if test "x$SYSTEMD_ENABLED" = "xyes" && test -n $systemdunitdir && test -n $systemdgendir; then
	AC_MSG_RESULT([enabled])
else
	case $SYSTEMD_ENABLED in
		yes)
			AC_MSG_ERROR([systemd not found])
		;;
		*)
			AC_MSG_RESULT([disabled])
		;;
	esac
fi

AM_CPPFLAGS=$AM_CPPFLAGS" -DSYSCONFDIR='\"$sysconfdir\"' -DRUNSTATEDIR='\"$localstatedir/run\"'"
AC_SUBST(AM_CPPFLAGS)
AC_CONFIG_SRCDIR([src/nbd.h])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile
		 doc/Doxyfile
		 doc/Makefile
		 man/Makefile
		 man/cbd-client.8.sh
		 man/cbdtab.5.sh
		 src/Makefile
		 system/Makefile
		 system/cbdsetup@.service.sh
		 system/cbdsetup-generator.sh
		 system/cbd_id.sh
		 system/cbd_dmsetup.sh
		 system/cbddisks.sh
		 system/cbddisks_start.sh
		 system/cbddisks_stop.sh
		 system/cbdsetup.functions.sh
		])
AC_OUTPUT

