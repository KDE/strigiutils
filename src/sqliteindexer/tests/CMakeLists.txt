include_directories(.. ../../indexertests)

CREATE_TEST_SOURCELIST(Tests testrunner.cpp SqliteTest.cpp simpletest.cpp)

ADD_EXECUTABLE(sqlitetest ${Tests} )
target_link_libraries(sqlitetest streams indexertests)

SET (TestsToRun ${Tests})
REMOVE (TestsToRun testrunner.cpp)

FOREACH (test ${TestsToRun})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} sqlitetest ${TName} ${strigi_SOURCE_DIR}/testdata/data)
ENDFOREACH (test)

