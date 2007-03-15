include_directories( ../streamanalyzer ../streams ../streams/compat
	${strigi_BINARY_DIR}/src/streams/compat)

add_library(indexertests indexwritertests.cpp indexreadertests.cpp
    indexmanagertests.cpp)

target_link_libraries(indexertests streamanalyzer ${CMAKE_THREAD_LIBS_INIT})

