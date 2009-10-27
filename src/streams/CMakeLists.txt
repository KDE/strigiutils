add_subdirectory(tests)
add_subdirectory(pdf)

include_directories(
	${ZLIB_INCLUDE_DIR}
	${BZIP2_INCLUDE_DIR}
	${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams
	${strigi_SOURCE_DIR}/src/streams/strigi
)

set(streams_SRCS
	archivereader.cpp
	archiveentrycache.cpp
	listinginprogress.cpp
	arinputstream.cpp
	base64inputstream.cpp
	bz2inputstream.cpp
	cpioinputstream.cpp
        dataeventinputstream.cpp
	dostime.cpp
	encodinginputstream.cpp
	fileinputstream.cpp
	filestreamopener.cpp
	gzipcompressstream.cpp
	gzipinputstream.cpp
	inputstreamreader.cpp
	kmpsearcher.cpp
        lzma/LzmaDec.c
        lzmainputstream.cpp
	mailinputstream.cpp
	oleinputstream.cpp
	rpminputstream.cpp
	sdfinputstream.cpp
	signatureinputstream.cpp
	skippingfileinputstream.cpp
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
set(streams_SRCS ${streams_SRCS} strigi/compat.cpp strigi/fnmatch.cpp strigi/stgdirent.cpp)

add_library(streams SHARED ${streams_SRCS})

set_target_properties(streams PROPERTIES
    VERSION ${STRIGI_VERSION_STRING}
    SOVERSION ${STRIGI_VERSION_MAJOR}
    DEFINE_SYMBOL MAKE_STREAMS_LIB
)

target_link_libraries(streams ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES} ${ICONV_LIBRARIES})
if(NOT WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libstreams.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libstreams.pc)
endif(NOT WIN32)
install(FILES
	archivereader.h
	bufferedstream.h
	encodinginputstream.h
	fileinputstream.h
	inputstream.h
	inputstreamreader.h
	sdfinputstream.h
	streambase.h
	streambuffer.h
	stringstream.h
	substreamprovider.h
	substreamproviderprovider.h
	stringterminatedsubstream.h
	subinputstream.h
        textutils.h
	DESTINATION include/strigi
)
install(TARGETS streams
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION  ${LIB_DESTINATION}
)

if(NOT WIN32)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libstreams.pc
          DESTINATION ${LIB_DESTINATION}/pkgconfig )
endif(NOT WIN32)

add_executable(oletest oletest.cpp)
target_link_libraries(oletest streams)

if(NOT WIN32)
  add_executable(testpt testpt.cpp processinputstream.cpp)
endif(NOT WIN32)

