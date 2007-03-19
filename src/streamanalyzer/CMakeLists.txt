add_subdirectory(throughplugins)
add_subdirectory(tests)

include_directories( ./ ../streams ${BZIP2_INCLUDE_DIR}
	${SHA_INCLUDE_DIR} ${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat ${CMAKE_BINARY_DIR} )

set(streamanalyzer_SRCS
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
	analysisresult.cpp
	helperendanalyzer.cpp
	id3v2throughanalyzer.cpp
	analyzerconfiguration.cpp
	bmpendanalyzer.cpp
	cnstr.cpp
	fieldtypes.cpp
	streamanalyzer.cpp
	filelister.cpp
)

if (LIBXML2_FOUND)
  set(streamanalyzer_SRCS ${streamanalyzer_SRCS} saxendanalyzer.cpp)
  set(streamindex_LIBS ${streamindex_LIBS} ${LIBXML2_LIBRARIES})
  include_directories(${LIBXML2_INCLUDE_DIR})
else (LIBXML2_FOUND)
  # we use expat
  set(streamanalyzer_SRCS ${streamanalyzer_SRCS} expatsaxendanalyzer.cpp)
  set(streamindex_LIBS ${streamindex_LIBS} ${EXPAT_LIBRARY})
  include_directories(${EXPAT_INCLUDE_DIR})
endif (LIBXML2_FOUND)

#IF(WIN32)
#	#not enough api exposed to make this a dll
#	add_library(streamanalyzer ifilterendanalyzer.cpp ${streamanalyzer_SRCS})
#    install(TARGETS streamanalyzer ARCHIVE DESTINATION ${LIB_DESTINATION})
#    #add ifilter library
#	set(streamindex_LIBS ${streamindex_LIBS} ntquery)
#ELSE(WIN32)
	add_library(streamanalyzer SHARED ${streamanalyzer_SRCS})
	install(TARGETS streamanalyzer LIBRARY DESTINATION ${LIB_DESTINATION})
#ENDIF(WIN32)

set_target_properties(streamanalyzer PROPERTIES
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR}
    DEFINE_SYMBOL MAKE_STREAMANALYZER_LIB
)

set(streamanalyzer_libs streams ${streamindex_LIBS})
if(HAVE_LIBDL)
        set(streamanalyzer_libs ${streamanalyzer_libs} ${LIBDL} )
endif(HAVE_LIBDL)
target_link_libraries(streamanalyzer ${streamanalyzer_libs})

install(FILES
	indexeddocument.h
	analysisresult.h
	analyzerplugin.h
	streamanalyzer.h
	streamthroughanalyzer.h
	streamendanalyzer.h
	streamsaxanalyzer.h
	streamlineanalyzer.h
	streameventanalyzer.h
	analyzerfactoryfactory.h
	indexwriter.h
	analyzerconfiguration.h
	cnstr.h
	fieldtypes.h
	DESTINATION include/strigi
)
install(TARGETS streamanalyzer
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION  ${LIB_DESTINATION}
)


# test executable
if(NOT WIN32)
        add_executable(filelister filelistertest.cpp)
        target_link_libraries(filelister streamanalyzer)
endif(NOT WIN32)
