#! /bin/bash

# this test checks if the strigicmd utility properly detect creation and
# deletion of a file

function fail() {
    echo Test failed
    exit 1
}

STRIGICMD="`find -type f -name strigicmd`"
echo Using $STRIGICMD

rm -r x y 2> /dev/null
mkdir x
touch x/y
touch x/z
touch x/zz
echo == $STRIGICMD create -t clucene -d y x ==
if ! $STRIGICMD create -j 1 -t clucene -d y x; then
    fail
fi
echo == $STRIGICMD listFiles -t clucene -d y ==
if ! $STRIGICMD listFiles -t clucene -d y; then
    fail
fi
exit
rm x/y
echo == $STRIGICMD update -t clucene -d y x ==
if ! $STRIGICMD update -t clucene -d y x; then
    fail
fi
echo == $STRIGICMD listFiles -t clucene -d y ==
if ! $STRIGICMD listFiles -t clucene -d y; then
    fail
fi
OUT=`$STRIGICMD listFiles -t clucene -d y`
if [[ $OUT == 'x/z' ]]; then
    echo Test succesfull
    exit 0
fi
fail
