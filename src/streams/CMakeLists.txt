add_subdirectory(tests)
add_subdirectory(pdf)

include_directories(
	${ZLIB_INCLUDE_DIR}
	${BZIP2_INCLUDE_DIR}
	${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat
)

set(streams_SRCS
	archivereader.cpp
	arinputstream.cpp
	base64inputstream.cpp
	bz2inputstream.cpp
	cpioinputstream.cpp
        dataeventinputstream.cpp
	dostime.cpp
	fileinputstream.cpp
	filereader.cpp
	filestreamopener.cpp
	gzipcompressstream.cpp
	gzipinputstream.cpp
	inputstreamreader.cpp
	kmpsearcher.cpp
	mailinputstream.cpp
	oleinputstream.cpp
	rpminputstream.cpp
	stringterminatedsubstream.cpp
	subinputstream.cpp
	substreamproviderprovider.cpp
	tarinputstream.cpp
	textutils.cpp
	zipinputstream.cpp
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

target_link_libraries(streams ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES} ${ICONV_LIBRARIES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libstreams.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libstreams.pc)

install(FILES
	archivereader.h
	bufferedstream.h
	fileinputstream.h
	inputstream.h
	inputstreamreader.h
	streambase.h
	streambuffer.h
	stringreader.h
	stringstream.h
	substreamprovider.h
	substreamproviderprovider.h
        textutils.h
	compat/jstreamsconfig.h
	DESTINATION include/strigi
)
install(TARGETS streams
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION  ${LIB_DESTINATION}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libstreams.pc
	DESTINATION ${LIB_DESTINATION}/pkgconfig )

add_executable(oletest oletest.cpp)
target_link_libraries(oletest streams)

if(NOT WIN32)
  add_executable(testpt testpt.cpp processinputstream.cpp)
endif(NOT WIN32)

add_executable(decodebase64 decodebase64.cpp base64inputstream.cpp)
set_target_properties(decodebase64 PROPERTIES COMPILE_FLAGS "-DMAKE_STREAMS_LIB")
