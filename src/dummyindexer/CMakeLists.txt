include_directories( ../streamindexer ../streams
	${REGEX_INCLUDE_DIR} ${strigi_BINARY_DIR}/src/streams)

add_executable(dummyindexer dummyindexer.cpp indexer.cpp)
target_link_libraries(dummyindexer streamindexer)

add_executable(deepfind deepfind.cpp indexer.cpp)
target_link_libraries(deepfind streamindexer)

add_executable(deepgrep deepgrep.cpp grepindexwriter.cpp indexer.cpp)
target_link_libraries(deepgrep streamindexer ${REGEX_LIBRARIES})

install(TARGETS deepfind deepgrep RUNTIME DESTINATION bin)

