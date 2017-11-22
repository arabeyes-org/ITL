#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
define([CURRENT], [1])
define([REV], [8])
define([AGE], [1])

AC_PREREQ([2.68])
AC_INIT([libitl], 0.8.0, [https://github.com/arabeyes-org/ITL/issues/new])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([hijri])
#AC_CONFIG_HEADERS([config.h])
LTVERSION=CURRENT:REV:AGE
AC_SUBST(LTVERSION)

AC_PROG_LIBTOOL

# Enable new method:
AC_ARG_ENABLE(newmethod,
  AC_HELP_STRING([--enable-newmethod],[Enable new prayertime calculation method (still under development)]),
  AM_SUBDIRS="$AM_SUBDIRS new_method",
  AM_SUBDIRS="")

AC_SUBST(AM_SUBDIRS)

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.
AC_CHECK_LIB([m], [pow])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor memset modf pow sqrt strtol])

AC_CONFIG_FILES([Makefile libitl.pc new_method/Makefile])
AC_OUTPUT
