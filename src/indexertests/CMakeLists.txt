include_directories( ../streamindexer ../streams
	${strigi_BINARY_DIR}/src/streams)

add_library(indexertests indexwritertests.cpp indexreadertests.cpp
    indexmanagertests.cpp)

target_link_libraries(indexertests streamindexer ${CMAKE_THREAD_LIBS_INIT})

