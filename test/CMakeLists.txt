set (TESTINGDIRS ${TESTINGDIRS} utils indextesters streamanalyzer)

if (CLucene_FOUND OR HyperEstraier_FOUND OR SQLite_FOUND)
  set(TESTINGDIRS ${TESTINGDIRS} daemon)
endif (CLucene_FOUND OR HyperEstraier_FOUND OR SQLite_FOUND)

# cppunit requires exception support
IF(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
ENDIF(NOT WIN32)

# disable, needs some testing
#if (HyperEstraier_FOUND)
#  set (TESTINGDIRS ${TESTINGDIRS} estraierindexer)
#endif (HyperEstraier_FOUND)

if (CLucene_FOUND)
  set(TESTINGDIRS ${TESTINGDIRS} luceneindexer)
endif (CLucene_FOUND)

include_directories (. ${CPPUNIT_INCLUDE_DIR} )

add_library(test_runner test_runner.cpp)
target_link_libraries(test_runner ${CPPUNIT_LIBRARIES})

 ENABLE_TESTING()

subdirs (${TESTINGDIRS})
