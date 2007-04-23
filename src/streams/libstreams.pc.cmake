prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}/bin
libdir=${LIB_DESTINATION}
includedir=${CMAKE_INSTALL_PREFIX}/include

Name: libstreams
Description: C++ streams for reading data as streams from various file formats
Requires: libz libbz2 libxml2
Version: ${STRIGI_VERSION_MAJOR}.${STRIGI_VERSION_MINOR}.${STRIGI_VERSION_PATCH}
Libs: -L${libdir} -lstreams
Cflags: -I${includedir} -I${libdir}/include
