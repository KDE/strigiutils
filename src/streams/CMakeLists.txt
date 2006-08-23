subdirs(tests pdf)

include_directories( ${ZLIB_INCLUDE_DIR} ${BZIP2_INCLUDE_DIR}
	${SHA_INCLUDE_DIR} ${ICONV_INCLUDE_DIR})

add_library(streams SHARED compat.cpp stgdirent.cpp bz2inputstream.cpp
	digestinputstream.cpp fileinputstream.cpp gzipinputstream.cpp 
	inputstreamreader.cpp filereader.cpp kmpsearcher.cpp 
	subinputstream.cpp mailinputstream.cpp stringterminatedsubstream.cpp 
	tarinputstream.cpp zipinputstream.cpp base64inputstream.cpp 
	dostime.cpp substreamproviderprovider.cpp arinputstream.cpp
	rpminputstream.cpp)
set_target_properties(streams PROPERTIES
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})

target_link_libraries(streams ${ZLIB_LIBRARIES} ${BZIP2_LIBRARY} ${SHA_LIBRARY} ${ICONV_LIBRARY})

install(TARGETS streams
	LIBRARY DESTINATION lib)
install(FILES fileinputstream.h substreamprovider.h substreamproviderprovider.h
	bufferedstream.h streambase.h inputstreambuffer.h inputstream.h
	jstreamsconfig.h
	DESTINATION include)
