dnl
dnl EK_CHECK_APR - Implement the '--with-apr' configure flag, indicating
dnl                the source directory containing APR
dnl 
dnl Substitutions:  APR_LDFLAGS      = LDFLAGS including APR's lib
dnl                 APR_CPPFLAGS     = CPPFLAGS to use against APR
dnl                 APR_CFLAGS       = CFLAGS to use against APR
dnl

AC_DEFUN([EK_CHECK_APR],[
AC_MSG_CHECKING(for APR)
AC_ARG_WITH(apr,[  --with-apr            Specify path to APR source directory],
[
        searchfile=$withval/apr-config

	AC_MSG_CHECKING(for $searchfile)
	if test -f $searchfile ; then
	        AC_MSG_RESULT(found $searchfile)
	else
		AC_MSG_ERROR($searchfile not found.)
	fi
	
	APR_CPPFLAGS="`$searchfile --cppflags --includes`"
	APR_CFLAGS="`$searchfile --cflags`"
	APR_LDFLAGS="`$searchfile --libs --ldflags --link-libtool`"

	AC_SUBST(APR_CPPFLAGS)
	AC_SUBST(APR_CFLAGS)
	AC_SUBST(APR_LDFLAGS)
],[
	AC_MSG_ERROR(--with-apr not given)
])
])

