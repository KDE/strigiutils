STRING(REGEX REPLACE /test /src INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    set(estraiertest_LIBS ${estraiertest_LIBS} ${LOG4CXX_LIBRARIES})
    set (INC_DIR ${INC_DIR} ${LOG4CXX_INCLUDE_DIR})
    add_definitions(-DHAVE_LOG4CXX)
endif (ENABLE_LOG4CXX AND Log4cxx_FOUND)

include_directories (. .. ${INC_DIR}
                     ../indextests
                     ${strigi_SOURCE_DIR}/src/streamanalyzer
                     ${strigi_SOURCE_DIR}/src/streams
                     ${strigi_BINARY_DIR}/src/streams
                     ${strigi_SOURCE_DIR}/src/streams/strigi
                     ${EST_INCLUDE_DIR}
)

add_library(estraiertest SHARED   estraierindexmanagertest.cpp
                                ../indextests/indexmanagertester.cpp
                                estraierindexreadertest.cpp
                                ../indextests/indexreadertester.cpp
                                estraierindexwritertest.cpp
                                ../indextests/indexwritertester.cpp )

target_link_libraries (estraiertest estraierindex ${estraiertest_LIBS})
add_test (estraiertest estraiertest)
