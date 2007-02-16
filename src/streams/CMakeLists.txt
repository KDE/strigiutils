subdirs(tests pdf)

include_directories( ${ZLIB_INCLUDE_DIR} ${BZIP2_INCLUDE_DIR}
	${SHA_INCLUDE_DIR} ${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat)

set(streams_SRCS bz2inputstream.cpp
	digestinputstream.cpp fileinputstream.cpp gzipinputstream.cpp
	gzipcompressstream.cpp inputstreamreader.cpp filereader.cpp
	subinputstream.cpp mailinputstream.cpp stringterminatedsubstream.cpp
	tarinputstream.cpp zipinputstream.cpp base64inputstream.cpp
	dostime.cpp substreamproviderprovider.cpp arinputstream.cpp
	rpminputstream.cpp archivereader.cpp processinputstream.cpp
	textutils.cpp kmpsearcher.cpp filestreamopener.cpp cpioinputstream.cpp)

#add the compatibility files
set(streams_SRCS ${streams_SRCS} compat/compat.cpp compat/fnmatch.cpp compat/stgdirent.cpp)

IF(WIN32)
	add_library(streams ${streams_SRCS})
	install(TARGETS streams ARCHIVE DESTINATION ${LIB_DESTINATION})
ELSE(WIN32)
	add_library(streams SHARED ${streams_SRCS})
	install(TARGETS streams LIBRARY DESTINATION ${LIB_DESTINATION})
ENDIF(WIN32)

set_target_properties(streams PROPERTIES
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})

target_link_libraries(streams ${ZLIB_LIBRARIES} ${BZIP2_LIBRARY} ${SHA_LIBRARY} ${ICONV_LIBRARY})

install(FILES fileinputstream.h substreamprovider.h substreamproviderprovider.h
	bufferedstream.h streambase.h inputstreambuffer.h inputstream.h
	archivereader.h inputstreamreader.h stringreader.h
	${strigi_BINARY_DIR}/src/streams/compat/jstreamsconfig.h
	compat/timeofday.h compat/strigi_fnmatch.h compat/strigi_thread.h compat/stgdirent.h
	DESTINATION include/strigi)

add_executable(testpt testpt.cpp processinputstream.cpp)

add_executable(decodebase64 decodebase64.cpp base64inputstream.cpp)
