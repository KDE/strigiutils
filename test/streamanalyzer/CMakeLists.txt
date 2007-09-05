if (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    set(streamanalyzertesters_LIBS ${streamanalyzertesters_LIBS} ${LOG4CXX_LIBRARIES})
    set (INC_DIR ${INC_DIR} ${LOG4CXX_INCLUDE_DIR})
    add_definitions(-DHAVE_LOG4CXX)
endif (ENABLE_LOG4CXX AND Log4cxx_FOUND)

include_directories (.
                     ${INC_DIR}
                     ${strigi_SOURCE_DIR}/src/streamanalyzer
                     ${strigi_SOURCE_DIR}/src/streams
                     ${strigi_BINARY_DIR}/src/streams
                     ${strigi_SOURCE_DIR}/src/streams/strigi
                     ../utils
)

add_library(streamanalyzertesters diranalyzertester.cpp)

target_link_libraries (streamanalyzertesters unittestfunctions ${streamanalyzertesters_LIBS})
