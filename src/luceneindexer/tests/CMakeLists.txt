include_directories(.. ../../indexertests)

CREATE_TEST_SOURCELIST(Tests testrunner.cpp CLuceneTest.cpp)

ADD_EXECUTABLE(testrunner ${Tests} )
target_link_libraries(testrunner streams cluceneindex indexertests)

SET (TestsToRun ${Tests})
REMOVE (TestsToRun testrunner.cpp)

FOREACH (test ${TestsToRun})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} testrunner ${TName} ${strigi_SOURCE_DIR}/testdata/data)
ENDFOREACH (test)

