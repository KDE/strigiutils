prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}/bin
libdir=${LIB_DESTINATION}
includedir=${CMAKE_INSTALL_PREFIX}/include

Name: libstreamanalyzer
Description: C++ library for extracting text and metadata from files and streams
Requires: libstreams
Version: ${LIBSTREAMANALYZER_VERSION}
Libs: -L${LIB_DESTINATION} -lstreamanalyzer
Cflags: -I${CMAKE_INSTALL_PREFIX}/include
