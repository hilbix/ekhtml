dnl
dnl PURPOSE:    EK_MAINTAINER_MODE
dnl             - Implement the '--enable-maintainer' configure flag, 
dnl             setting the CFLAGS for maintainence builds.
dnl 
dnl             CFLAGS is adjusted to contain flags necessary for
dnl             compiling in maintaier mode.
dnl	

AC_DEFUN([EK_MAINTAINER_MODE],[
AM_MAINTAINER_MODE
    if test ${USE_MAINTAINER_MODE}x = yesx  && test ${GCC}x = yesx ; then
	EK_CFLAGS_CHECK([-Werror])
	EK_CFLAGS_CHECK([-Wall])
	EK_CFLAGS_CHECK([-Wmissing-prototypes])
	EK_CFLAGS_CHECK([-Wmissing-declarations])
    fi
])
