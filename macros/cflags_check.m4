dnl
dnl PURPOSE:    COVL_CFLAGS_CHECK
dnl             check to see if certain C compiler flags are recognized
dnl	

AC_DEFUN([EK_CFLAGS_CHECK],[
	ac_save_cflags="$CFLAGS"
	CFLAGS="$CFLAGS $1"
	AC_TRY_COMPILE([], [], [], [CFLAGS="$ac_save_cflags"])
])
