add_subdirectory(throughplugins)
add_subdirectory(tests)

include_directories( ./ ../streams ${BZIP2_INCLUDE_DIR}
	${SHA_INCLUDE_DIR} ${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat ${CMAKE_BINARY_DIR} )

set(streamindexer_SRCS
        bz2endanalyzer.cpp
        digestthroughanalyzer.cpp
	textendanalyzer.cpp
	streamendanalyzer.cpp
	zipendanalyzer.cpp
	tarendanalyzer.cpp
	pngendanalyzer.cpp
	query.cpp
	mailendanalyzer.cpp
	gzipendanalyzer.cpp
	analyzerloader.cpp
	oggthroughanalyzer.cpp
	cpioendanalyzer.cpp
	arendanalyzer.cpp
	indexwriter.cpp
	rpmendanalyzer.cpp
	indexable.cpp
	helperendanalyzer.cpp
	id3v2throughanalyzer.cpp
	indexerconfiguration.cpp
	bmpendanalyzer.cpp
	cnstr.cpp
	fieldtypes.cpp
	streamindexer.cpp
)
if(NOT WIN32)

set(streamindexer_SRCS
        ${streamindexer_SRCS}
	filelister.cpp
)

endif(NOT WIN32)

if (LIBXML2_FOUND)
  set(streamindexer_SRCS ${streamindexer_SRCS} saxendanalyzer.cpp)
  set(streamindex_LIBS ${streamindex_LIBS} ${LIBXML2_LIBRARIES})
  include_directories(${LIBXML2_INCLUDE_DIR})
else (LIBXML2_FOUND)
  # we use expat
  set(streamindexer_SRCS ${streamindexer_SRCS} expatsaxendanalyzer.cpp)
  set(streamindex_LIBS ${streamindex_LIBS} ${EXPAT_LIBRARY})
  include_directories(${EXPAT_INCLUDE_DIR})
endif (LIBXML2_FOUND)

#IF(WIN32)
#	#not enough api exposed to make this a dll
#	add_library(streamindexer ifilterendanalyzer.cpp ${streamindexer_SRCS})
#    install(TARGETS streamindexer ARCHIVE DESTINATION ${LIB_DESTINATION})
#    #add ifilter library
#	set(streamindex_LIBS ${streamindex_LIBS} ntquery)
#ELSE(WIN32)
	add_library(streamindexer SHARED ${streamindexer_SRCS})
	install(TARGETS streamindexer LIBRARY DESTINATION ${LIB_DESTINATION})
#ENDIF(WIN32)
set_target_properties(streamindexer PROPERTIES
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})

set(streamindexer_libs streams ${streamindex_LIBS})
if(NOT WIN32)
        set(streamindexer_libs ${streamindexer_libs} dl)
endif(NOT WIN32)
target_link_libraries(streamindexer ${streamindexer_libs})

install(FILES indexeddocument.h indexable.h analyzerplugin.h streamindexer.h
	streamthroughanalyzer.h streamendanalyzer.h analyzerfactoryfactory.h
	indexwriter.h indexerconfiguration.h cnstr.h fieldtypes.h
	DESTINATION include/strigi)


# test executable
if(NOT WIN32)
        add_executable(filelister filelistertest.cpp)
        target_link_libraries(filelister streamindexer)
endif(NOT WIN32)
