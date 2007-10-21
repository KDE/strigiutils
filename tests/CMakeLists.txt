set (TESTINGDIRS utils indextesters streamanalyzer daemon)
subdirs (${TESTINGDIRS})

# setting for using CPPUNIT

include_directories (. ${CPPUNIT_INCLUDE_DIR}  ${strigi_SOURCE_DIR}/src/daemon
    ${strigi_BINARY_DIR}/src/streams)

# cppunit requires exception support
IF(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
ENDIF(NOT WIN32)

# a convenience library that adds a unified way of build a test executable
# for testing
# simply link this into any unit test executable
add_library(test_runner test_runner.cpp)
target_link_libraries(test_runner ${CPPUNIT_LIBRARIES})

ENABLE_TESTING()

