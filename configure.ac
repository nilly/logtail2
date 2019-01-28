#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([logtail2], [0.5.3], [andlov@nowise.se])
AC_CONFIG_SRCDIR([src/logtail2.c])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

AC_PROG_CC
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h unistd.h])
AC_C_CONST
AC_TYPE_SIZE_T
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([atexit memset mkdir realpath strerror strstr])

AC_CONFIG_FILES([Makefile
		 src/Makefile])
AC_OUTPUT
