if (ENABLE_LOG4CXX AND LOG4CXX_FOUND)
    set(xesamtest_LIBS ${xesamtest_LIBS} ${LOG4CXX_LIBRARIES})
    set (INC_DIR ${INC_DIR} ${LOG4CXX_INCLUDE_DIR})
    add_definitions(-DHAVE_LOG4CXX)
endif (ENABLE_LOG4CXX AND LOG4CXX_FOUND)

include_directories (.
                     ${INC_DIR}
                     ${strigi_SOURCE_DIR}/src/streamanalyzer/xesam
)

add_executable (xesam2strigitest xesam2strigitest.cpp)
target_link_libraries (xesam2strigitest strigi_test_runner strigi_xesam ${xesamtest_LIBS})

add_test (xesam2strigitest xesam2strigitest)

