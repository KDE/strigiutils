subdirs(tests pdf)

include_directories(
	${ZLIB_INCLUDE_DIR}
	${BZIP2_INCLUDE_DIR}
	${SHA_INCLUDE_DIR}
	${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat
)

set(streams_SRCS
	bz2inputstream.cpp
	digestinputstream.cpp
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
if(MINGW)
    set_target_properties(streams PROPERTIES PREFIX "mingw-")
endif(MINGW)

target_link_libraries(streams ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES} ${SHA_LIBRARIES} ${ICONV_LIBRARIES})

install(FILES
	fileinputstream.h
	substreamprovider.h
	substreamproviderprovider.h
	bufferedstream.h
	streambase.h
	inputstreambuffer.h
	inputstream.h
	archivereader.h
	inputstreamreader.h
	stringreader.h
	stringreader.h
	streams_export.h
	${strigi_BINARY_DIR}/src/streams/compat/jstreamsconfig.h
	compat/timeofday.h
	compat/strigi_fnmatch.h
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

#set_target_properties() does not work here...
add_definitions(-DMAKE_STREAMS_LIB)
add_executable(decodebase64 decodebase64.cpp base64inputstream.cpp)
