include_directories( ..  ${strigi_BINARY_DIR}/src/streams/compat )
CREATE_TEST_SOURCELIST(Tests streamanalyzertests.cpp querytests.cpp)

ADD_EXECUTABLE(streamanalyzertest ${Tests})
target_link_libraries(streamanalyzertest streamanalyzer)

SET (TestsToRun ${Tests})
REMOVE (TestsToRun streamanalyzertests.cpp)

FOREACH (test ${TestsToRun})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} streamanalyzertest ${TName})
ENDFOREACH (test)

