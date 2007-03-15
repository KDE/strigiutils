include_directories( ../streamanalyzer ../streams
	${REGEX_INCLUDE_DIR} ${strigi_BINARY_DIR}/src/streams/compat)

add_executable(dummyindexer dummyindexer.cpp indexer.cpp)
target_link_libraries(dummyindexer streamanalyzer)

add_executable(deepfind deepfind.cpp indexer.cpp)
target_link_libraries(deepfind streamanalyzer)

add_executable(deepgrep deepgrep.cpp grepindexwriter.cpp indexer.cpp)
target_link_libraries(deepgrep streamanalyzer ${REGEX_LIBRARIES})

install(TARGETS deepfind deepgrep RUNTIME DESTINATION bin)

