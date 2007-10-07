#! /bin/sh
# this script will clean a given directory from extended attributes set by
# Beagle and Kat
find $1 -exec setfattr -x user.Beagle.AttrTime {} \; 2> /dev/null
find $1 -exec setfattr -x user.Beagle.Filter {} \; 2> /dev/null
find $1 -exec setfattr -x user.Beagle.Fingerprint {} \; 2> /dev/null
find $1 -exec setfattr -x user.Beagle.MTime {} \; 2> /dev/null
find $1 -exec setfattr -x user.Beagle.Uid {} \; 2> /dev/null
find $1 -exec setfattr -x user.kat.fileid {} \; 2> /dev/null
find $1 -exec setfattr -x user.kat.lastupdatedate {} \; 2> /dev/null
