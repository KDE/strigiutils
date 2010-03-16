if(NOT HAVE_REGEX_H)
  set(REGEX_INCLUDE_DIR ${strigi_SOURCE_DIR}/src/streams/strigi/regex)
  set(REGEX_SOURCES ${REGEX_INCLUDE_DIR}/regex.c)
  # TODO: install copyright file !
endif(NOT HAVE_REGEX_H)

add_executable(dummyindexer dummyindexer.cpp)
target_link_libraries(dummyindexer ${LIBSTREAMANALYZER_LIBRARIES})

add_library(libdeepfind STATIC deepfind.cpp)
target_link_libraries(libdeepfind ${LIBSTREAMANALYZER_LIBRARIES})

add_library(grepindexer STATIC grepindexreader.cpp grepindexmanager.cpp
    grepindexwriter.cpp ${REGEX_SOURCES})

add_library(libdeepgrep STATIC deepgrep.cpp)
target_link_libraries(libdeepgrep grepindexer ${LIBSTREAMANALYZER_LIBRARIES} ${REGEX_LIBRARIES})

add_executable(greptest grepindexreader.cpp)
target_link_libraries(greptest streamanalyzer)

if(NOT MSVC)
  add_executable(analyzerlatencytester analyzerlatencytester.cpp)
  target_link_libraries(analyzerlatencytester ${LIBSTREAMANALYZER_LIBRARIES})
endif(NOT MSVC)

add_library(grepindex STATIC grepindexmanager.cpp)
