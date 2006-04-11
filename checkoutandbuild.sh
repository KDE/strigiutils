#! /bin/sh
# script for checking out and testing archivereader

# change this line to contain you userid or to 'SVN=svn://anonsvn'
SVN=svn+ssh://vandenoever@svn
svn co -N $SVN.kde.org/home/kde/branches/work/kde4/playground/libs
cd libs
svn co $SVN.kde.org/home/kde/trunk/KDE/kde-common/admin
svn co $SVN.kde.org/home/kde/branches/work/kde4/playground/libs/archivereader

# fix the build system
perl -0777 -pi -e 's/A[^A]*designer plugins[^)]*\)/echo/s' admin/acinclude.m4.in

# configure and build
make -f Makefile.cvs
mkdir debug
cd debug
../configure --enable-debug=full
unsermake

# test
make check
