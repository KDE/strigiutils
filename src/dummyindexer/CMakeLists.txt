include_directories( ../streamindexer ../streams ../filters)

add_executable(dummyindexer dummyindexer.cpp indexer.cpp)
target_link_libraries(dummyindexer streamindexer filters)

add_executable(deepfind deepfind.cpp indexer.cpp)
target_link_libraries(deepfind streamindexer filters)

install(TARGETS deepfind
	RUNTIME DESTINATION bin)

