STRING(REGEX REPLACE /test /src INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories (. .. ${INC_DIR}
                     ../indextesters
                     ../streamanalyzer
                     ${strigi_SOURCE_DIR}/src/streamanalyzer
                     ${strigi_SOURCE_DIR}/src/luceneindexer
                     ${strigi_SOURCE_DIR}/src/streams
                     ${strigi_BINARY_DIR}/src/streams/compat
                     ${strigi_SOURCE_DIR}/src/streams/compat
                     ${CLUCENE_INCLUDE_DIR}
)

add_executable (lucenetest  luceneindexmanagertest.cpp
                            luceneindexreadertest.cpp
                            luceneindexwritertest.cpp
                            luceneindexsearchtest.cpp
                            lucenediranalyzertest.cpp
)

target_link_libraries (lucenetest   test_runner
                                    streamanalyzertesters
                                    indextesters
                                    cluceneindex )

add_test (lucenetests lucenetest)
