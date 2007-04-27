prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}/bin
libdir=${LIB_DESTINATION}
includedir=${CMAKE_INSTALL_PREFIX}/include

Name: libstreams
Description: C++ streams for reading data as streams from various file formats
Version: ${STRIGI_VERSION_MAJOR}.${STRIGI_VERSION_MINOR}.${STRIGI_VERSION_PATCH}
Libs: -L${LIB_DESTINATION} -lstreams
Cflags: -I${CMAKE_INSTALL_PREFIX}/include
