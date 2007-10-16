if (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    set(lucenetest_LIBS ${lucenetest_LIBS} ${LOG4CXX_LIBRARIES})
    set (INC_DIR ${INC_DIR} ${LOG4CXX_INCLUDE_DIR})
    add_definitions(-DHAVE_LOG4CXX)
endif (ENABLE_LOG4CXX AND Log4cxx_FOUND)

include_directories (. .. ${INC_DIR}
                     ../indextesters
                     ../streamanalyzer
                     ${strigi_SOURCE_DIR}/src/streamanalyzer
                     ${strigi_SOURCE_DIR}/src/streams
                     ${strigi_BINARY_DIR}/src/streams
                     ${strigi_SOURCE_DIR}/src/streams/strigi
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
                                    ${lucenetest_LIBS})

add_test (lucenetests lucenetest)
