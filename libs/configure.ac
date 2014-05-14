AC_PREREQ(2.57)
AC_INIT([itl],
	[0.8.0],
	[developer@arabeyes.org],
	[itl],
	[http://projects.arabeyes.org/project.php?proj=ITL])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11.1 no-define subdir-objects])
AM_SILENT_RULES([yes])

LT_PREREQ([2.2])
LT_INIT([disable-static])

AC_PROG_CC
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])
AC_C_CONST
AC_TYPE_UID_T

AC_CHECK_FUNCS([strcasecmp])
AC_CHECK_LIB([m], [floor], , [AC_MSG_ERROR(library 'libm' is missing floor)])
AC_CHECK_LIB([m], [modf], , [AC_MSG_ERROR(library 'libm' is missing modf)])
AC_CHECK_LIB([m], [pow], , [AC_MSG_ERROR(library 'libm' is missing pow)])
AC_CHECK_LIB([m], [atan2], , [AC_MSG_ERROR(library 'libm' is missing atan2)])

# Version
m4_define(itl_version_triplet,m4_split(AC_PACKAGE_VERSION,[[.]]))
m4_define(itl_version_major,m4_argn(1,itl_version_triplet))
m4_define(itl_version_minor,m4_argn(2,itl_version_triplet))
m4_define(itl_version_micro,m4_argn(3,itl_version_triplet))
dnl ITL_VERSION_MAJOR=itl_version_major
dnl ITL_VERSION_MINOR=itl_version_minor
dnl ITL_VERSION_MICRO=itl_version_micro
dnl ITL_VERSION=AC_PACKAGE_VERSION
dnl AC_SUBST(ITL_VERSION_MAJOR)
dnl AC_SUBST(ITL_VERSION_MINOR)
dnl AC_SUBST(ITL_VERSION_MICRO)
dnl AC_SUBST(ITL_VERSION)

# Libtool version
m4_define([itl_version_int], m4_eval(itl_version_major*10000 + itl_version_minor*100 + itl_version_micro))
m4_define([itl_libtool_revision], itl_version_micro)
m4_define([itl_libtool_age], m4_eval(itl_version_int - itl_libtool_revision))
m4_define([itl_libtool_current], m4_eval(itl_version_major + itl_libtool_age))
ITL_LIBTOOL_VERSION_INFO=itl_libtool_current:itl_libtool_revision:itl_libtool_age
AC_SUBST(ITL_LIBTOOL_VERSION_INFO)

AC_CONFIG_FILES([
 Makefile
 itl.pc
])
AC_OUTPUT
