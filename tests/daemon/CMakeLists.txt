# add a test for the daemon configuration loading classes

if (QT4_FOUND)
  subdirs(dbus)
endif (QT4_FOUND)

# if we use log4cxx, include it
# ideally this should be abstracted away in something like STRIGILOG( ... )
if (ENABLE_LOG4CXX AND LOG4CXX_FOUND)
    set(daemonconfiguratortest_LIBS ${daemonconfiguratortest_LIBS}
                                    ${LOG4CXX_LIBRARIES})
    set (INC_DIR ${INC_DIR} ${LOG4CXX_INCLUDE_DIR})
    add_definitions(-DHAVE_LOG4CXX)
endif (ENABLE_LOG4CXX AND LOG4CXX_FOUND)

include_directories (.  ${INC_DIR}
                        ${strigi_SOURCE_DIR}/src/xsd
                        ${strigi_SOURCE_DIR}/src/daemon
                        ${strigi_SOURCE_DIR}/src/streamanalyzer
                        ${strigi_SOURCE_DIR}/src/streams/strigi
                        ${strigi_BINARY_DIR}/src/streams
)

add_executable(daemonconfiguratortest daemonconfiguratortest.cpp)
target_link_libraries (daemonconfiguratortest strigi_test_runner daemonconfigurator
                       streamanalyzer ${daemonconfiguratortest_LIBS})

add_test (daemonconfiguratortest daemonconfiguratortest)
