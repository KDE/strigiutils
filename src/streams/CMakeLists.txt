

include_directories(
	${ZLIB_INCLUDE_DIR}
	${BZIP2_INCLUDE_DIR}
	${OPENSSL_INCLUDE_DIR}
	${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat
)

set(streams_SRCS
	bz2inputstream.cpp
	fileinputstream.cpp
	gzipinputstream.cpp
	gzipcompressstream.cpp
	inputstreamreader.cpp
	filereader.cpp
	subinputstream.cpp
	mailinputstream.cpp
	stringterminatedsubstream.cpp
	tarinputstream.cpp
	zipinputstream.cpp
	base64inputstream.cpp
	dostime.cpp
	substreamproviderprovider.cpp
	arinputstream.cpp
	rpminputstream.cpp
	archivereader.cpp
	textutils.cpp
	kmpsearcher.cpp
	filestreamopener.cpp
	cpioinputstream.cpp
        dataeventinputstream.cpp
)

if(NOT WIN32)
set(streams_SRCS ${streams_SRCS}
	processinputstream.cpp
)
endif(NOT WIN32)

#add the compatibility files
set(streams_SRCS ${streams_SRCS} compat/compat.cpp compat/fnmatch.cpp compat/stgdirent.cpp)

add_library(streams SHARED ${streams_SRCS})

set_target_properties(streams PROPERTIES
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR}
    DEFINE_SYMBOL MAKE_STREAMS_LIB
)

target_link_libraries(streams ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES} ${OPENSSL_LIBRARIES} ${OPENSSL_EAY_LIBRARIES} ${ICONV_LIBRARIES})

install(FILES
	streambase.h
	inputstream.h
	streambuffer.h
	bufferedstream.h
	fileinputstream.h
	inputstreamreader.h
	stringstream.h
	stringreader.h
	substreamprovider.h
	substreamproviderprovider.h
	archivereader.h
	${strigi_BINARY_DIR}/src/streams/compat/jstreamsconfig.h
	DESTINATION include/strigi
)
install(TARGETS streams
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION  ${LIB_DESTINATION}
)

if(NOT WIN32)
  add_executable(testpt testpt.cpp processinputstream.cpp)
endif(NOT WIN32)

add_executable(decodebase64 decodebase64.cpp base64inputstream.cpp)
set_target_properties(decodebase64 PROPERTIES COMPILE_FLAGS "-DMAKE_STREAMS_LIB")
