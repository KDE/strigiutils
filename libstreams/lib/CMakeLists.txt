# This directory builds only the library libstreams
add_definitions(-DMAKE_STREAMS_LIB)

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
#	signatureinputstream.cpp # not done yet
	skippingfileinputstream.cpp
	skippingfileinputstream2.cpp
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
	mmapfileinputstream.cpp
)
endif(NOT WIN32)

#add the compatibility files
set(streams_SRCS ${streams_SRCS} compat.cpp)

add_library(streamsstatic STATIC ${streams_SRCS})
set_target_properties(streamsstatic PROPERTIES DEFINE_SYMBOL MAKE_STREAMS_LIB)
target_link_libraries(streamsstatic ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES} ${ICONV_LIBRARIES})

add_library(streams SHARED ${streams_SRCS})
set_target_properties(streams PROPERTIES
	VERSION ${STRIGI_VERSION_STRING}
	SOVERSION ${STRIGI_VERSION_MAJOR}
	DEFINE_SYMBOL MAKE_STREAMS_LIB
)
target_link_libraries(streams ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES} ${ICONV_LIBRARIES})
install(TARGETS streams
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION ${LIB_DESTINATION}
)


if(NOT WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libstreams.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libstreams.pc)
endif(NOT WIN32)
