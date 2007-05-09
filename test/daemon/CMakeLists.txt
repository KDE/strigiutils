STRING(REGEX REPLACE /test /src INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

IF (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    set(daemonconfiguratortest_LIBS ${daemonconfiguratortest_LIBS}
                                    ${LOG4CXX_LIBRARIES})
    include_directories( ${LOG4CXX_INCLUDE_DIR} )
ENDIF (ENABLE_LOG4CXX AND Log4cxx_FOUND)


include_directories (.  ${INC_DIR}
                        ${strigi_SOURCE_DIR}/src/xsd
                        ${strigi_SOURCE_DIR}/src/streamanalyzer
                        ${strigi_SOURCE_DIR}/src/streams/compat 
                        ${strigi_BINARY_DIR}/src/streams/compat
)

add_library(daemonconfiguratortest SHARED daemonconfiguratortest.cpp)
target_link_libraries (daemonconfiguratortest daemonconfigurator
                       ${daemonconfiguratortest_LIBS})
