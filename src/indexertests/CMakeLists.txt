include_directories( ../streamindexer ../streams)

add_library(indexertests indexwritertests.cpp indexreadertests.cpp)

target_link_libraries(streamindexer)

