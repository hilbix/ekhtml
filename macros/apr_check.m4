dnl
dnl EK_CHECK_APR - Implement the '--with-apr' configure flag, indicating
dnl                the source directory containing APR
dnl 
dnl Substitutions:  APR_DIR          = the value of --with-apr
dnl                 APR_CPPFLAGS     = EXTRA_CPPFLAGS used by APR when building
dnl                 APR_CFLAGS       = EXTRA_CFLAGS used by APR when building
dnl                 APR_LIBTOOL_LIBS = Additional libtool libraries needed
dnl
dnl Side-effects:   CPPFLAGS is adjusted to contain the APR include path,
dnl                 including the architecture path
dnl

AC_DEFUN([EK_CHECK_APR],[
AC_MSG_CHECKING(for APR)
AC_ARG_WITH(apr,[  --with-apr            Specify path to APR source directory],
[
        searchfile=$withval/include/apr.h

	AC_MSG_CHECKING(for APR directory)
	if test -f $searchfile ; then
                APR_DIR=`cd $withval; pwd`
		CPPFLAGS="$CPPFLAGS -I$APR_DIR/include"
	        AC_MSG_RESULT(found $APR_DIR)
	else
		AC_MSG_ERROR($searchfile not found.)
	fi
	
	dnl Save stuff which APRVARS defines
	OLD_EXTRA_CPPFLAGS="$EXTRA_CPPFLAGS"
	OLD_EXTRA_CFLAGS="$EXTRA_CFLAGS"
	OLD_EXTRA_LIBS="$EXTRA_LIBS"
	OLD_LIBTOOL_LIBS="$LIBTOOL_LIBS"
	EXTRA_CPPFLAGS=""
	EXTRA_CFLAGS=""
	EXTRA_LIBS=""
	LIBTOOL_LIBS=""

	. $APR_DIR/APRVARS
	APR_CPPFLAGS="$EXTRA_CPPFLAGS"
	APR_CFLAGS="$EXTRA_CFLAGS"
	APR_EXTRA_LIBS="$EXTRA_LIBS"
	APR_LIBTOOL_LIBS="$LIBTOOL_LIBS"

	EXTRA_CPPFLAGS="$OLD_EXTRA_CPPFLAGS"
	EXTRA_CFLAGS="$OLD_EXTRA_CFLAGS"
	EXTRA_LIBS="$OLD_EXTRA_LIBS"
	LIBTOOL_LIBS="$OLD_LIBTOOL_LIBS"		

	AC_SUBST(APR_DIR)
	AC_SUBST(APR_CPPFLAGS)
	AC_SUBST(APR_CFLAGS)
	AC_SUBST(APR_EXTRA_LIBS)
	AC_SUBST(APR_LIBTOOL_LIBS)
],[
	AC_MSG_ERROR(--with-apr not given)
])
])

