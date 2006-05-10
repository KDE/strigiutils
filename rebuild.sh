#! /bin/sh

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
automake --add-missing

if [ test $TARGET == "dist" ]; then
	./configure && make dist-bzip2
else
	mkdir debug && \
	cd debug && \
	CXXFLAGS="-Wall -O0 -g3" ../configure --enable-debug=full && \
	make check
fi
