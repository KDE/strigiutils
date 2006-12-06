include_directories( ../streamindexer ../streams
	${strigi_BINARY_DIR}/src/streams/compat)

add_executable(xmlindexer xmlindexer.cpp indexer.cpp)

target_link_libraries(xmlindexer streamindexer)

install(TARGETS xmlindexer RUNTIME DESTINATION bin)

add_executable(testdatacomparison testdatacomparison.cpp indexer.cpp)
target_link_libraries(testdatacomparison streamindexer)

ADD_TEST(XmlIndexer testdatacomparison ${strigi_SOURCE_DIR}/testdata data
	${strigi_SOURCE_DIR}/testdata/xmlindexeroutput.bz2)
