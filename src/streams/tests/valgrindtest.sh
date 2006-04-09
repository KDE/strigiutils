#! /bin/sh
if [[ -z $1 ]]; then
	for f in `find -type f -perm -u+x`; do  
		libtool --mode=execute valgrind -q --leak-check=full $f;
	done
else
	libtool --mode=execute valgrind -q --leak-check=full $1
fi

