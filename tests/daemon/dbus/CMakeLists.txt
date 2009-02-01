# add a test for the daemon dbus communication

set(INC_DIR ${QT_INCLUDE_DIR})

# if we use log4cxx, include it
# ideally this should be abstracted away in something like STRIGILOG( ... )
if (ENABLE_LOG4CXX AND LOG4CXX_FOUND)
    set(daemondbustest_LIBS ${daemondbustest_LIBS} ${LOG4CXX_LIBRARIES})
    set (INC_DIR ${INC_DIR} ${LOG4CXX_INCLUDE_DIR})
    add_definitions(-DHAVE_LOG4CXX)
endif (ENABLE_LOG4CXX AND LOG4CXX_FOUND)

include_directories (.  ${INC_DIR}
                        ${strigi_SOURCE_DIR}/src/searchclient/qtdbus
                        ${strigi_BINARY_DIR}/src/streams
)

set(daemondbustest_MOC_HDRS xesam/xesamdbus.h xesamlistener.h)
qt4_wrap_cpp(daemondbustest_MOC_SRCS ${daemondbustest_MOC_HDRS})

add_executable(daemondbustest daemondbustest.cpp runner.cpp xesamdbustest.cpp
    xesam/xesamdbus.cpp xesamlistener.cpp strigidaemonunittestsession.cpp
    ${daemondbustest_MOC_SRCS})
target_link_libraries (daemondbustest strigi_test_runner ${daemondbustest_LIBS}
    strigiqtdbusclient)

add_executable(dtest test.cpp)
target_link_libraries (dtest ${daemondbustest_LIBS} strigiqtdbusclient)

add_test (daemondbustest daemondbustest)
