#! /bin/sh

# directory with clucene binaries and header
#CLUCENEDIR=$HOME/testinstall
#CLUCENESRCDIR=.
PREFIX=$HOME/testinstall

rm -rf autom4te.cache configure COPYING depcomp INSTALL install-sh Makefile.in \
	missing aclocal.m4 debug ltmain.sh config.guess config.sub config.log \
	config.status Makefile libtool libltdl 2> /dev/null
find -name Makefile.in -exec rm {} \;

if [[ -n $1 ]]; then
	TARGET=$1
	if [ $TARGET != "dist" ] && [ $TARGET != "check" ]; then
		exit;
	fi
fi

aclocal -I m4 && \
libtoolize --ltdl --force --copy && \
autoconf && \
automake --add-missing || exit;

if [ test $TARGET == "dist" ]; then
	./configure && make dist-bzip2
elif [ test $TARGET == "release" ]; then
	mkdir release && \
	cd release && \
	CXXFLAGS="-Wall -O3" CPPFLAGS=-I$CLUCENEDIR/include \
	LDFLAGS=-L$CLUCENEDIR/lib ../configure --enable-debug=no && \
	make
else
	mkdir debug && \
	cd debug && \
	CXXFLAGS="-Wall -O0 -g3" ../configure --enable-debug=full \
            --prefix=$PREFIX --with-clucene=$CLUCENEDIR && \
	make && \
	make check
fi
