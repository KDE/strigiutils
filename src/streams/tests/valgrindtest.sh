#! /bin/sh
VALGRIND=`which valgrind`
if [[ -n $VALGRIND ]]; then
    libtool --mode=execute valgrind -q --leak-check=full ./testrunner
fi

