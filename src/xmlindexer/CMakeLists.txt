include_directories( ../streamindexer ../streams ../filters)

add_executable(xmlindexer xmlindexer.cpp indexer.cpp)

target_link_libraries(xmlindexer streamindexer filters)

install(TARGETS xmlindexer RUNTIME DESTINATION bin)

add_executable(testdatacomparison testdatacomparison.cpp indexer.cpp)
target_link_libraries(testdatacomparison streamindexer filters)

ADD_TEST(XmlIndexer testdatacomparison ${CMAKE_SOURCE_DIR}/testdata/data
	${CMAKE_SOURCE_DIR}/testdata/xmlindexeroutput.bz2)
