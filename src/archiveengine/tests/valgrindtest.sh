#! /bin/sh
ODIR=$PWD
DIR=`dirname $0`;
cd $DIR
VALGRIND=`which valgrind`
if $ODIR/testrunner && [[ -n $VALGRIND ]]; then
	libtool --mode=execute $VALGRIND -q --leak-check=full $ODIR/testrunner;
	exit;
fi
false;
