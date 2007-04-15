include_directories( ../streamanalyzer ../streams
	${REGEX_INCLUDE_DIR} ${strigi_BINARY_DIR}/src/streams/compat)

add_executable(dummyindexer dummyindexer.cpp indexer.cpp)
target_link_libraries(dummyindexer streamanalyzer)

add_executable(deepfind deepfind.cpp indexer.cpp)
target_link_libraries(deepfind streamanalyzer)
install(TARGETS deepfind RUNTIME DESTINATION bin)

# no regexp.h on win32
if(NOT WIN32)
  add_executable(deepgrep deepgrep.cpp grepindexwriter.cpp indexer.cpp)
  target_link_libraries(deepgrep streamanalyzer ${REGEX_LIBRARIES})
  install(TARGETS deepgrep RUNTIME DESTINATION bin)
endif(NOT WIN32)

add_executable(greptest grepindexreader.cpp)
target_link_libraries(greptest streamanalyzer)

add_library(grepindex grepindexmanager.cpp)
