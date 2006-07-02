include_directories( ../streamindexer ../streams )

add_executable(dummyindexer dummyindexer.cpp indexer.cpp)

target_link_libraries(dummyindexer streamindexer)

