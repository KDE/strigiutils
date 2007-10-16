#subdirs(tests)

include_directories( ../streamanalyzer ../streams ../filters ../dummyindexer
	${strigi_BINARY_DIR}/src/streams
	${strigi_SOURCE_DIR}/src/streams/strigi)

add_library(combinedindex combinedindexmanager.cpp)

target_link_libraries(combinedindex streamanalyzer grepindex
    ${combinedindex_LIBS})
