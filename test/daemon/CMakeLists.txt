string(REGEX REPLACE /test /src INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    set(daemonconfiguratortest_LIBS ${daemonconfiguratortest_LIBS}
                                    ${LOG4CXX_LIBRARIES})
    set (INC_DIR ${INC_DIR} ${LOG4CXX_INCLUDE_DIR})
endif (ENABLE_LOG4CXX AND Log4cxx_FOUND)

include_directories (.  ${INC_DIR}
                        ${strigi_SOURCE_DIR}/src/xsd
                        ${strigi_SOURCE_DIR}/src/daemon
                        ${strigi_SOURCE_DIR}/src/streamanalyzer
                        ${strigi_SOURCE_DIR}/src/streams/strigi
                        ${strigi_BINARY_DIR}/src/streams
)

add_executable(daemonconfiguratortest daemonconfiguratortest.cpp)
target_link_libraries (daemonconfiguratortest test_runner daemonconfigurator
                       streamanalyzer ${daemonconfiguratortest_LIBS})

add_test (daemonconfiguratortest daemonconfiguratortest)
