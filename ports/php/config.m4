dnl $Id$
dnl config.m4 for extension itl

PHP_ARG_WITH(itl, for itl support,
[  --with-itl             Include itl support])

if test "$PHP_ITL" != "no"; then

	SEARCH_PATH="/usr/local /usr"
	SEARCH_FOR="/include/itl/prayer.h"
	if test -r $PHP_ITL/$SEARCH_FOR; then
		ITL_DIR=$PHP_ITL
	else # search default path list
		AC_MSG_CHECKING([for itl files in default path])
		for i in $SEARCH_PATH ; do
			if test -r $i/$SEARCH_FOR; then
				ITL_DIR=$i
				AC_MSG_RESULT(found in $i)
			fi
		done
	fi

	if test -z "$ITL_DIR"; then
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([Please reinstall the itl distribution])
	fi

	PHP_ADD_INCLUDE($ITL_DIR/include)

	LIBNAME=itl
	LIBSYMBOL=getPrayerTimes

	PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
	[
	PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ITL_DIR/lib/itl, ITL_SHARED_LIBADD)
	AC_DEFINE(HAVE_ITLLIB,1,[ ])
	],[
	AC_MSG_ERROR([wrong itl lib version or lib not found])
	],[
	-L$ITL_DIR/lib/itl -lm -ldl
	])

	PHP_SUBST(ITL_SHARED_LIBADD)

	PHP_NEW_EXTENSION(itl, itl.c, $ext_shared)
fi
