include_directories( ../streamanalyzer ../streams ../streams/strigi
	${strigi_BINARY_DIR}/src/streams)

add_library(indexertests STATIC
	indexmanagertests.cpp
	indexreadertests.cpp
	indexwritertests.cpp
)

target_link_libraries(indexertests streamanalyzer ${CMAKE_THREAD_LIBS_INIT})

