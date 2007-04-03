include_directories(
        ../streamanalyzer
        ../streams
	${strigi_BINARY_DIR}/src/streams/compat
	${BZIP2_INCLUDE_DIR}
)

add_executable(xmlindexer
        xmlindexer.cpp
        indexer.cpp
        tagmapping.cpp
	xmlindexwriter.cpp
)

target_link_libraries(xmlindexer streamanalyzer)

install(TARGETS xmlindexer RUNTIME DESTINATION bin)

add_executable(peranalyzerxml
	peranalyzerxml.cpp
	indexer.cpp
	tagmapping.cpp
	xmlindexwriter.cpp
)
target_link_libraries(peranalyzerxml streamanalyzer)

# disable test until fixed
#ADD_TEST(XmlIndexer testdatacomparison ${strigi_SOURCE_DIR}/testdata data
#	${strigi_SOURCE_DIR}/testdata/xmlindexeroutput.bz2)

if(NOT WIN32)
  add_executable(ontoprint ontoprint.cpp)
  target_link_libraries(ontoprint streamanalyzer)
endif(NOT WIN32)
