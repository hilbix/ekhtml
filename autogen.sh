#!/bin/sh 

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

ORIGDIR=`pwd`
cd $srcdir
PROJECT="El-Kabong HTML"
TEST_TYPE=-f
FILE=src/ekhtml.c
COVL_MACROS=macros

DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have autoconf installed to compile $PROJECT."
	echo "Download the appropriate package for your distribution,"
	echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
	DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have automake installed to compile $PROJECT."
	echo "Get ftp://sourceware.cygnus.com/pub/automake/automake-1.4.tar.gz"
	echo "(or a newer version if it is available)"
	DIE=1
}

if test "$DIE" -eq 1; then
	exit 1
fi

test $TEST_TYPE $FILE || {
	echo "You must run this script in the top-level $PROJECT directory"
	exit 1
}

test -d $COVL_MACROS || {
	echo "$COVL_MACROS must exist to build $PROJECT"
	exit 1
}

if test -z "$*"; then
	echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

case $CC in
*xlc | *xlc\ * | *lcc | *lcc\ *) am_opt=--include-deps;;
esac

aclocal -I $COVL_MACROS

libtoolize --force
(autoheader --version)  < /dev/null > /dev/null 2>&1 && autoheader
automake -a $am_opt
autoconf
cd $ORIGDIR

$srcdir/configure "$@"

echo 
echo "Now type 'make' to compile $PROJECT."
