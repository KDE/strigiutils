add_subdirectory(throughplugins)
add_subdirectory(lineplugins)
add_subdirectory(tests)

include_directories( ./ ../streams ${BZIP2_INCLUDE_DIR} endanalyzers
	${LIBXML2_INCLUDE_DIR} ${SHA_INCLUDE_DIR} ${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat ${CMAKE_BINARY_DIR} )

set(streamanalyzer_SRCS
	digestthroughanalyzer.cpp
	streamendanalyzer.cpp
	query.cpp
	analyzerloader.cpp
	oggthroughanalyzer.cpp
	indexwriter.cpp
	analysisresult.cpp
	id3v2throughanalyzer.cpp
	analyzerconfiguration.cpp
	cnstr.cpp
	fieldtypes.cpp
	streamanalyzer.cpp
	filelister.cpp
	eventthroughanalyzer.cpp
	saxeventanalyzer.cpp
	lineeventanalyzer.cpp
	htmlsaxanalyzer.cpp
	endanalyzers/bz2endanalyzer.cpp
	endanalyzers/textendanalyzer.cpp
	endanalyzers/zipendanalyzer.cpp
	endanalyzers/tarendanalyzer.cpp
	endanalyzers/pngendanalyzer.cpp
	endanalyzers/mailendanalyzer.cpp
	endanalyzers/gzipendanalyzer.cpp
	endanalyzers/cpioendanalyzer.cpp
	endanalyzers/arendanalyzer.cpp
	endanalyzers/rpmendanalyzer.cpp
	endanalyzers/helperendanalyzer.cpp
	endanalyzers/bmpendanalyzer.cpp
)

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
    SOVERSION ${STRIGI_VERSION_MAJOR} DEFINE_SYMBOL MAKE_STREAMANALYZER_LIB
)

set(streamanalyzer_libs streams ${streamindex_LIBS} ${LIBXML2_LIBRARIES})
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
