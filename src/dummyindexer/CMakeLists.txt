include_directories( ../streamindexer ../streams ../filters ${REGEX_INCLUDE_DIR})

add_executable(dummyindexer dummyindexer.cpp indexer.cpp)
target_link_libraries(dummyindexer streamindexer filters)

add_executable(deepfind deepfind.cpp indexer.cpp)
target_link_libraries(deepfind streamindexer filters)

add_executable(deepgrep deepgrep.cpp grepindexwriter.cpp indexer.cpp)
target_link_libraries(deepgrep streamindexer filters ${REGEX_LIBRARIES})

install(TARGETS deepfind deepgrep RUNTIME DESTINATION bin)

