if (ENABLE_LOG4CXX AND Log4cxx_FOUND)
  set(indextesters_LIBS ${indextesters_LIBS} ${LOG4CXX_LIBRARIES})
  set (INC_DIR ${INC_DIR} ${LOG4CXX_INCLUDE_DIR})
  add_definitions(-DHAVE_LOG4CXX)
endif (ENABLE_LOG4CXX AND Log4cxx_FOUND)

include_directories (.
  ${strigi_SOURCE_DIR}/src/streamanalyzer
  ${strigi_SOURCE_DIR}/src/streams
  ${strigi_BINARY_DIR}/src/streams
  ${strigi_SOURCE_DIR}/src/streams/strigi
  ../utils
)

add_library(indextesters indexmanagertester.cpp
                         indexreadertester.cpp
                         indexwritertester.cpp
                         indexsearchtester.cpp
)

target_link_libraries (indextesters unittestfunctions ${indextesters_LIBS})
