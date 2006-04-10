#! /bin/sh
ODIR=$PWD
DIR=`dirname $0`;
cd $DIR
if $ODIR/testrunner; then
	libtool --mode=execute valgrind -q --leak-check=full $ODIR/testrunner;
	exit;
fi
false;
