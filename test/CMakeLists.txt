set (TESTINGDIRS ${TESTINGDIRS} daemon)
set (STRIGI_UNIT_TEST_LIBRARIES ${STRIGI_UNIT_TEST_LIBRARIES}
     daemonconfiguratortest)

# cppunit requires exception support
IF(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
ENDIF(NOT WIN32)

if (HyperEstraier_FOUND)
  set (TESTINGDIRS ${TESTINGDIRS} estraierindexer)
  set (STRIGI_UNIT_TEST_LIBRARIES ${STRIGI_UNIT_TEST_LIBRARIES} estraiertest)
endif (HyperEstraier_FOUND)

if (CLucene_FOUND)
  set(TESTINGDIRS ${TESTINGDIRS} luceneindexer)
  set (STRIGI_UNIT_TEST_LIBRARIES ${STRIGI_UNIT_TEST_LIBRARIES} lucenetest)
endif (CLucene_FOUND)

if (CLucene_FOUND OR HyperEstraier_FOUND OR SQLite_FOUND)
SUBDIRS (${TESTINGDIRS})

include_directories (. ${CPPUNIT_INCLUDE_DIR})

ADD_EXECUTABLE(strigiunittester test_runner.cpp)
target_link_libraries(strigiunittester    ${CPPUNIT_LIBRARIES}
                                    ${STRIGI_UNIT_TEST_LIBRARIES})

endif (CLucene_FOUND OR HyperEstraier_FOUND OR SQLite_FOUND)

FOREACH (dir ${TESTINGDIRS})
    FILE(GLOB_RECURSE UnitTests_SRCS ${dir} "${dir}/*test.cpp" )
    FOREACH(test ${UnitTests_SRCS})
        GET_FILENAME_COMPONENT(TestName ${test} NAME_WE)
        ADD_TEST(${TestName} UnitTester ${TestName})
        MESSAGE ("** Added testing suite " ${TestName})
    ENDFOREACH(test)
ENDFOREACH(dir)

# wait a moment before uncommenting, there's a small problem to solve
# FILE(GLOB_RECURSE UnitTests_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*test.cpp" )
# FOREACH(test ${UnitTests_SRCS})
#     GET_FILENAME_COMPONENT(TestName ${test} NAME_WE)
#     ADD_TEST(${TestName} UnitTester ${TestName})
# ENDFOREACH(test)
