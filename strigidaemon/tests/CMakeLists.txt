# Not all systems have mkdtemp, and some of the tests use mkdtemp
# -- pulling in an implementation from kdefakes is an annoying workaround,
# so instead just don't do the tests here.
CHECK_FUNCTION_EXISTS(mkdtemp HAVE_MKDTEMP)

set (TESTINGDIRS utils indextesters)
# streamanalyzer

include_directories (. ../bin/daemon ${CPPUNIT_INCLUDE_DIR})

IF(BUILD_DAEMON)
    add_subdirectory(daemon)
ENDIF(BUILD_DAEMON)

IF(NOT HAVE_MKDTEMP)
    MESSAGE(WARNING " No mkdtemp found; some Strigi tests are suppressed.")
    SET(TESTINGDIRS utils daemon)
ENDIF(NOT HAVE_MKDTEMP)

subdirs (${TESTINGDIRS})

# setting for using CPPUNIT

include_directories ( ${strigi_SOURCE_DIR}/src/daemon
    ${strigi_BINARY_DIR}/src/streams)

# a convenience library that adds a unified way of build a test executable
# for testing
# simply link this into any unit test executable
add_library(strigi_test_runner STATIC test_runner.cpp)
target_link_libraries(strigi_test_runner ${CPPUNIT_LIBRARIES})

ENABLE_TESTING()

