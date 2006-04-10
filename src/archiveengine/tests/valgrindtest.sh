#! /bin/sh
libtool --mode=execute valgrind -q --leak-check=full ./testrunner

