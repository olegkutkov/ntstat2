#
#   Copyright 2012  Oleg Kutkov <elenbert@gmail.com>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.  
#

AC_INIT([ntstat2], [0.1], [elenbert@gmail.com], [], [https://github.com/elenbert/ntstat2])

AC_CANONICAL_HOST
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([config.h:config.in])

AC_PROG_CC
AC_PROG_CPP
AC_PROG_GCC_TRADITIONAL
AC_PROG_INSTALL

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])

AM_CONDITIONAL(HAVE_LINUX, false)
AM_CONDITIONAL(HAVE_ANDROID, false)
AM_CONDITIONAL(HAVE_BSD, false)

if test "$GCC" = "yes" ; then
	CFLAGS="$CFLAGS -Wall"
fi

case $host_os in
	*linux* | *linux-gnu*)
		AC_DEFINE(HAVE_LINUX, 1, [Define to 1 if your build target is Linux.])
		AM_CONDITIONAL(HAVE_LINUX, true)

		AC_CHECK_FUNCS([memset])
		;;
	*bsd* | *darwin*)
		AC_DEFINE(HAVE_BSD, 1, [Define to 1 if your build target is BSD or Darwin.])
		AM_CONDITIONAL(HAVE_BSD, true)

		AC_FUNC_MALLOC
		;;
	*android*)
		AC_DEFINE(HAVE_LINUX, 1, [Define to 1 if your build target is Linux.])
		AC_DEFINE(HAVE_ANDROID, 1, [Define to 1 if your build target is Android.])
		AM_CONDITIONAL(HAVE_LINUX, true)
		AM_CONDITIONAL(HAVE_ANDROID, true)

		;;
	*)
		AC_MSG_ERROR([Sorry! Your target operatins system is not supported])
		;;
esac

AC_CHECK_LIB(rt, timer_create)
AC_CHECK_LIB(pthread, pthread_create)

AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h unistd.hmath.h sys/param.h sys/socket.h sys/ioctl.h arpa/inet.h netinet/in.h net/if.h termios.h])

AC_HEADER_TIME

AC_OUTPUT

