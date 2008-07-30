if(NOT HAVE_REGEX_H)
  set(REGEX_INCLUDE_DIR ${strigi_SOURCE_DIR}/src/streams/strigi/regex)
  set(REGEX_SOURCES ${REGEX_INCLUDE_DIR}/regex.c)
  # TODO: install copyright file !
endif(NOT HAVE_REGEX_H)

include_directories(
  ../streamanalyzer
  ../streams
  ${REGEX_INCLUDE_DIR}
  ${strigi_BINARY_DIR}/src/streams
)

add_executable(dummyindexer dummyindexer.cpp)
target_link_libraries(dummyindexer streamanalyzer)

add_executable(deepfind deepfind.cpp)
target_link_libraries(deepfind streamanalyzer)
install(TARGETS deepfind RUNTIME DESTINATION bin)

add_library(grepindexer STATIC grepindexreader.cpp grepindexmanager.cpp
    grepindexwriter.cpp ${REGEX_SOURCES})

add_executable(deepgrep deepgrep.cpp)
target_link_libraries(deepgrep grepindexer streamanalyzer ${REGEX_LIBRARIES})
install(TARGETS deepgrep RUNTIME DESTINATION bin)

add_executable(greptest grepindexreader.cpp)
target_link_libraries(greptest streamanalyzer)

if(NOT MSVC)
  add_executable(analyzerlatencytester analyzerlatencytester.cpp)
  target_link_libraries(analyzerlatencytester streamanalyzer)
endif(NOT MSVC)

add_library(grepindex STATIC grepindexmanager.cpp)
