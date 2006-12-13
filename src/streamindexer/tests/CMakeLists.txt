include_directories( .. )
CREATE_TEST_SOURCELIST(Tests streamindexertests.cpp querytests.cpp)

ADD_EXECUTABLE(streamindexertest ${Tests})
target_link_libraries(streamindexertest streamindexer)

SET (TestsToRun ${Tests})
REMOVE (TestsToRun streamindexertests.cpp)

FOREACH (test ${TestsToRun})
  GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
  ADD_TEST(${TName} streamindexertest ${TName})
ENDFOREACH (test)

