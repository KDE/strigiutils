#! /bin/sh

# directory with clucene binaries and header
CLUCENESRCDIR=/tmp/clucene/src
#CLUCENESRCDIR=.

rm -rf autom4te.cache configure COPYING depcomp INSTALL install-sh Makefile.in \
	missing aclocal.m4 debug ltmain.sh config.guess config.sub config.log \
	config.status Makefile libtool 2> /dev/null
find -name Makefile.in -exec rm {} \;

if [[ -n $1 ]]; then
	TARGET=$1
	if [ $TARGET != "dist" ] && [ $TARGET != "check" ]; then
		exit;
	fi
fi

libtoolize --force --copy && \
aclocal -I m4 && \
autoconf && \
automake --add-missing || exit;

if [ test $TARGET == "dist" ]; then
	./configure && make dist-bzip2
elif [ test $TARGET == "release" ]; then
	mkdir release && \
	cd release && \
	CXXFLAGS="-Wall -O3" CPPFLAGS=-I$CLUCENESRCDIR \
	LDFLAGS=-L$CLUCENESRCDIR ../configure --enable-debug=no && \
	make
else
	mkdir debug && \
	cd debug && \
	CXXFLAGS="-Wall -O0 -g3" CPPFLAGS=-I$CLUCENESRCDIR \
	LDFLAGS=-L$CLUCENESRCDIR ../configure --enable-debug=full \
            --prefix=$HOME/testinstall && \
	make && \
	make check
fi
