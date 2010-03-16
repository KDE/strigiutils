add_executable (xesam2strigitest xesam2strigitest.cpp)
target_link_libraries (xesam2strigitest strigi_test_runner strigi_xesam ${LOG4CXX_LIBRARIES} ${xesamtest_LIBS})

add_test (xesam2strigitest xesam2strigitest)

