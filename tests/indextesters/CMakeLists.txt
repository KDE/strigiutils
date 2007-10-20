if (ENABLE_LOG4CXX AND Log4cxx_FOUND)
  set(indextesters_LIBS ${LOG4CXX_LIBRARIES})
  set(INC_DIR ${LOG4CXX_INCLUDE_DIR})
  add_definitions(-DHAVE_LOG4CXX)
endif (ENABLE_LOG4CXX AND Log4cxx_FOUND)

include_directories (.
  ${strigi_SOURCE_DIR}/src/streamanalyzer
  ${strigi_SOURCE_DIR}/src/streams
  ${strigi_BINARY_DIR}/src/streams
  ${strigi_SOURCE_DIR}/src/streams/strigi
  ../utils
)

set(indextester_SRCS indextest.cpp indexreadertester.cpp indexwritertester.cpp
    indexsearchtester.cpp)
if (CLucene_FOUND)
  set(indextester_SRCS ${indextester_SRCS} clucenetests.cpp)
  set (indextester_DEPS ${indextester_DEPS} clucene)
endif (CLucene_FOUND)
if (ENABLE_SQLITE AND SQLite_FOUND)
  set (indextester_SRCS ${indextester_SRCS} sqlitetests.cpp)
  set (indextester_DEPS sqlite)
endif (ENABLE_SQLITE AND SQLite_FOUND)
if (HyperEstraier_FOUND)
  set (indextester_SRCS ${indextester_SRCS} estraiertests.cpp)
  set (indextester_DEPS estraier)
endif (HyperEstraier_FOUND)

add_executable(indextester ${indextester_SRCS})
if (indextester_DEPS)
  ADD_DEPENDENCIES(indextester ${indextester_DEPS})
endif (indextester_DEPS)
target_link_libraries(indextester
  test_runner unittestfunctions ${indextesters_LIBS})

add_test(indextester indextester)
