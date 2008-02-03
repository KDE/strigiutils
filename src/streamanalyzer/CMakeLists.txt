add_subdirectory(throughplugins)
add_subdirectory(lineplugins)
add_subdirectory(saxplugins)
add_subdirectory(endplugins)
add_subdirectory(tests)
add_subdirectory(fieldproperties)
add_subdirectory(xesam)

include_directories( ./ ../streams ../streams/pdf ${BZIP2_INCLUDE_DIR}
	endanalyzers ${LIBXML2_INCLUDE_DIR} ${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams
	${strigi_SOURCE_DIR}/src/streams/strigi ${CMAKE_BINARY_DIR} )

set(streamanalyzer_SRCS
	analysisresult.cpp
	analyzerconfiguration.cpp
	analyzerloader.cpp
	classproperties.cpp
	digesteventanalyzer.cpp
	diranalyzer.cpp
	eventthroughanalyzer.cpp
	fieldproperties.cpp
	fieldpropertiesdb.cpp
	fieldtypes.cpp
	filelister.cpp
	htmlsaxanalyzer.cpp
	id3v2throughanalyzer.cpp
        indexreader.cpp
	indexpluginloader.cpp
	lineeventanalyzer.cpp
	m3ustreamanalyzer.cpp
	mimeeventanalyzer.cpp
	odfmimetypelineanalyzer.cpp
	odfsaxanalyzer.cpp
	oggthroughanalyzer.cpp
	query.cpp
	queryparser.cpp
	saxeventanalyzer.cpp
	sha1.cpp
	streamanalyzer.cpp
	streamanalyzerfactory.cpp
	streamsaxanalyzer.cpp
	variant.cpp
	endanalyzers/arendanalyzer.cpp
	endanalyzers/bmpendanalyzer.cpp
	endanalyzers/bz2endanalyzer.cpp
	endanalyzers/cpioendanalyzer.cpp
	endanalyzers/gzipendanalyzer.cpp
	endanalyzers/helperendanalyzer.cpp
	endanalyzers/mailendanalyzer.cpp
	endanalyzers/mpegendanalyzer.cpp   
	endanalyzers/oleendanalyzer.cpp
	endanalyzers/pdfendanalyzer.cpp
	endanalyzers/pngendanalyzer.cpp
	endanalyzers/rpmendanalyzer.cpp
	endanalyzers/sdfendanalyzer.cpp
	endanalyzers/tarendanalyzer.cpp
	endanalyzers/textendanalyzer.cpp
	endanalyzers/zipendanalyzer.cpp
)

#IF(WIN32)
#	#not enough api exposed to make this a dll
#	add_library(streamanalyzer ifilterendanalyzer.cpp ${streamanalyzer_SRCS})
#    install(TARGETS streamanalyzer ARCHIVE DESTINATION ${LIB_DESTINATION})
#    #add ifilter library
#	set(streamindex_LIBS ${streamindex_LIBS} ntquery)
#ELSE(WIN32)
	add_library(streamanalyzer SHARED ${streamanalyzer_SRCS})
	install(TARGETS streamanalyzer LIBRARY DESTINATION ${LIB_DESTINATION}
                                   RUNTIME DESTINATION ${LIB_DESTINATION})
#ENDIF(WIN32)

set_target_properties(streamanalyzer PROPERTIES
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR} DEFINE_SYMBOL MAKE_STREAMANALYZER_LIB
)

set(streamanalyzer_libs streams ${streamindex_LIBS} ${LIBXML2_LIBRARIES} )
if(HAVE_LIBDL)
        set(streamanalyzer_libs ${streamanalyzer_libs} ${LIBDL} )
endif(HAVE_LIBDL)
target_link_libraries(streamanalyzer ${streamanalyzer_libs} pdfstream
	${CMAKE_THREAD_LIBS_INIT})

if(NOT WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libstreamanalyzer.pc.cmake
  	${CMAKE_CURRENT_BINARY_DIR}/libstreamanalyzer.pc)

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libstreamanalyzer.pc
  	DESTINATION ${LIB_DESTINATION}/pkgconfig )
endif(NOT WIN32)

install(FILES
	analysisresult.h
	analyzerconfiguration.h
	analyzerplugin.h
	classproperties.h
	diranalyzer.h
	fieldproperties.h
	fieldpropertiesdb.h
	fieldtypes.h
	indexeddocument.h
	indexreader.h
	indexmanager.h
	indexplugin.h
	indexwriter.h
	variant.h
	query.h
	queryparser.h
	streamanalyzer.h
	streamanalyzerfactory.h
	streamendanalyzer.h
	streameventanalyzer.h
	streamlineanalyzer.h
	streamsaxanalyzer.h
	streamthroughanalyzer.h
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
