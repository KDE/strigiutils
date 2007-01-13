add_subdirectory(throughplugins)
add_subdirectory(tests)

include_directories( ./ ../streams ${BZIP2_INCLUDE_DIR}
	${SHA_INCLUDE_DIR} ${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat)

set(streamindexer_SRCS bz2endanalyzer.cpp digestthroughanalyzer.cpp
	textendanalyzer.cpp streamendanalyzer.cpp zipendanalyzer.cpp
	tarendanalyzer.cpp streamindexer.cpp pngendanalyzer.cpp query.cpp
	mailendanalyzer.cpp gzipendanalyzer.cpp filelister.cpp
	analyzerloader.cpp oggthroughanalyzer.cpp
	arendanalyzer.cpp indexwriter.cpp rpmendanalyzer.cpp indexable.cpp
	helperendanalyzer.cpp id3v2throughanalyzer.cpp indexerconfiguration.cpp)

if (Expat_FOUND)
  set(streamindexer_SRCS ${streamindexer_SRCS} expatsaxendanalyzer.cpp)
  set(streamindex_LIBS ${streamindex_LIBS} ${EXPAT_LIBRARY})
  include_directories(${EXPAT_INCLUDE_DIR})
#else (Expat_FOUND)
#  if (Libxml2_FOUND)
#    set(streamindexer_SRCS ${streamindexer_SRCS} saxendanalyzer.cpp)
#    set(streamindex_LIBS ${streamindex_LIBS} ${LIBXML2_LIBRARY})
#    include_directories(${LIBXML2_INCLUDE_DIR})
#  endif (Libxml2_FOUND)
endif(Expat_FOUND)

IF(WIN32)
	#not enough api exposed to make this a dll
	add_library(streamindexer ifilterendanalyzer.cpp ${streamindexer_SRCS})
    install(TARGETS streamindexer ARCHIVE DESTINATION ${LIB_DESTINATION})
    #add ifilter library
	set(streamindex_LIBS ${streamindex_LIBS} ntquery)
ELSE(WIN32)
	add_library(streamindexer SHARED ${streamindexer_SRCS})
	install(TARGETS streamindexer LIBRARY DESTINATION ${LIB_DESTINATION})
ENDIF(WIN32)
set_target_properties(streamindexer PROPERTIES
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})

target_link_libraries(streamindexer streams ${streamindex_LIBS})

install(FILES indexeddocument.h indexable.h analyzerplugin.h streamindexer.h
	streamthroughanalyzer.h streamendanalyzer.h analyzerfactoryfactory.h
	DESTINATION include/strigi)


# test executable
add_executable(filelister filelistertest.cpp)

target_link_libraries(filelister streamindexer)
