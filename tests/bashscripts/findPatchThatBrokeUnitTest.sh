#! /bin/bash
#
# (c) Jos van den Oever  <jos@vandenoever.info>
#
# This script checks out strigi and runs the given unit test. If the unit test
# fails, it goes back to the previous version, compiles and tests again.
# This goes on until the unit test is not present or runs successfully.

# current limitations:
# - only one instance of this script can be run because it relies on reusing
#   previous work to avoid expensive downloading and compiling.

# check the arguments
if (( $# != 1 && $# != 2)); then
    echo Usage: $0 testname [srcdir]
    echo Note: This script must be run from the base
    exit
fi
# if a second argument was given, go into this directory
if (( $# == 2 )); then
    DIR=$2;
    if [ ! -d $DIR ]; then
        echo Directory $DIR does not exist.
        exit
    fi
    cd $DIR
fi

# the path to the unit test executable
TESTNAME=$1

## Configuration parameters

# absolute path of the directory in which to run the tests
TESTDIR=/tmp/checksvnunittest

# the maximal number of log entries to download
MAXLOGENTRIES=300

# the maximal number of steps you wish to take
MAXSTEPS=300

# make exectable with arguments
MAKE="make -j8"

# should we do a drastic cleanup between runs or not?
FORCEFULLBUILD=0

####################

# function for testing a particular test in a particular revision
function runTest {
    REVISION=$1
    echo Testing revision $REVISION.

    # go back to the given revision
    cd $TESTDIR
    svn update $MODULE -r $REVISION
    if (( $? != 0 )); then
        # if updating failed, we have to get a fresh version
        rm -rf $TESTDIR/$MODULE
        svn checkout -r $REVISION $SVNURL
        if (( $? != 0 )); then exit; fi
    fi

    # configure the code
    # if we cannot configure the test, we continue to the next revision number
    if (( $FORCEFULLBUILD == 1 )); then
        rm -rf $TESTDIR/$MODULE/build
    fi
    mkdir $TESTDIR/$MODULE/build
    cd $TESTDIR/$MODULE/build
    cmake ..
    if (( $? != 0 )); then return; fi

    # get the name of the unit test and build it
    # if we cannot build the test, we continue to the next revision number
    echo $MAKE $TESTNAME
    $MAKE $TESTNAME
    if (( $? != 0 )); then return; fi

    # find the test executable
    TESTPATH=`find -name $TESTNAME -type f -perm -u+x`

    # run the unit test and exit if it ran without error
    $TESTPATH
    if (( $? == 0 )); then
        echo The last revision where the test $TESTNAME worked was $REVISION.
        BROKEN=`grep -B 1 $REVISION $TESTDIR/revisions |head -1`
        echo The first revision that was broken was $BROKEN:
        svn log -r $BROKEN $TESTDIR/$MODULE
        exit
    fi
}

# determine the URL of the svn repository
SVNURL=`svn info | grep -m 1 '^URL: ' | cut -b 6-`
if (( $? != 0 )); then exit; fi

# determine the module name
MODULE=`basename $SVNURL`

# initial checkout
mkdir $TESTDIR
cd $TESTDIR
svn checkout $SVNURL
if (( $? != 0 )); then
    # if checking out fails, we can try again from an empty directory
    rm -rf $TESTDIR/$MODULE
    svn checkout $SVNURL
    if (( $? != 0 )); then exit; fi
fi

# get the last 100 relevant version numbers
svn log $MODULE --limit $MAXLOGENTRIES --non-interactive \
	| grep -E '^r[0123456789]+' \
	| perl -pi -e 's/^r(\d+).*/\1/' | head -n $MAXSTEPS > revisions
if (( $? != 0 )); then exit; fi

for REVISION in `cat revisions`; do
    runTest $REVISION;
done

echo No revision was found in which the unit test worked.

