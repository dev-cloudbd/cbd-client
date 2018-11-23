dnl Configure script for CloudBD CBD system
dnl (c) 1998 Martin Mares <mj@ucw.cz>, (c) 2000 Pavel Machek <pavel@ucw.cz>,
dnl (c) 2003-2006 Wouter Verhelst <wouter@debian.org>
dnl (c) 2017 CloudBD, LLC
AC_INIT([cbd-client],
  [4.0.6],
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

m4_ifdef([PKG_CHECK_VAR], [], [AC_DEFUN([PKG_CHECK_VAR], [
	AC_REQUIRE([PKG_PROG_PKG_CONFIG])dnl
	AC_ARG_VAR([$1], [value of $3 for $2, overriding pkg-config])dnl
	_PKG_CONFIG([$1], [variable="][$3]["], [$2])
	AS_VAR_COPY([$1], [pkg_cv_][$1])
	AS_VAR_IF([$1], [""], [$5], [$4])dnl
    ])# PKG_CHECK_VAR
])

AC_MSG_CHECKING([where to put udev files])
PKG_CHECK_VAR([UDEVDIR], [udev], [udevdir], [AC_MSG_RESULT([found])], [AC_MSG_ERROR([not found])])
if test "$prefix" = "NONE" -o "$prefix" = "/usr" -o "$prefix" = "/usr/local"; then
    AC_SUBST([udevdir], [$UDEVDIR])
else
    AC_SUBST([udevdir], ["${prefix}$UDEVDIR"])
fi

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
    PKG_CHECK_VAR([SYSTEMDUNIT], [systemd], [systemdsystemunitdir], [], [AC_MSG_ERROR([systemd not found])])
    PKG_CHECK_VAR([SYSTEMDGEN], [systemd], [systemdsystemgeneratordir], [], [AC_MSG_ERROR([systemd not found])])
    AC_MSG_RESULT([found])
    if test "$prefix" = "NONE" -o "$prefix" = "/usr" -o "$prefix" = "/usr/local"; then
        AC_SUBST([systemdunitdir], [$SYSTEMDUNIT])
        AC_SUBST([systemdgendir], [$SYSTEMDGEN])
    else
        AC_SUBST([systemdunitdir], ["${prefix}$SYSTEMDUNIT"])
        AC_SUBST([systemdgendir], ["${prefix}$SYSTEMDGEN"])
    fi
else
    AC_MSG_RESULT([disabled])
fi

AC_MSG_CHECKING([where to put bash completions])
PKG_CHECK_VAR([BASHCOMPLETIONS], [bash-completion], [completionsdir], [AC_SUBST([bashcompletionsdir], [$BASHCOMPLETIONS])], [AC_SUBST([bashcompletionsdir], ["${sysconfdir}/bash_completions.d"])])
AC_MSG_RESULT([$bashcompletionsdir])

AM_CONDITIONAL([SYSTEMD], [test -n "$systemdunitdir" && test -n "$systemdgendir"])

AM_CPPFLAGS=$AM_CPPFLAGS" -DSYSCONFDIR='\"$sysconfdir\"' -DRUNSTATEDIR='\"$localstatedir/run\"'"
AC_SUBST(AM_CPPFLAGS)
AC_SUBST(PACKAGE_TAG)
AC_SUBST(PACKAGE_DIST)
AC_CONFIG_SRCDIR([src/nbd.h])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile
		 doc/Doxyfile
		 doc/Makefile
		 man/Makefile
		 man/cbdtab.5.sh
		 src/Makefile
		 system/Makefile
		 system/cbdsetup@.service.sh
		 system/cbdsetup-generator.sh
		 system/99-cbd.rules.sh
		 system/cbd_id.sh
		 system/cbd_dmsetup.sh
		 system/cbddisks.sh
		 system/cbddisks_start.sh
		 system/cbddisks_stop.sh
		 system/cbdsetup.functions.sh
		 system/cloudbd.completion.sh
                 package/trusty/changelog
                 package/xenial/changelog
		])
AC_OUTPUT

