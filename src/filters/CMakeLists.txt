IF (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    add_definitions(-DHAVE_LOG4CXX)
    link_libraries(filters ${LOG4CXX_LIBRARIES})
    include_directories( ${LOG4CXX_INCLUDE_DIR} )
ENDIF (ENABLE_LOG4CXX AND Log4cxx_FOUND)

ADD_LIBRARY(filters filters.cpp filtermanager.cpp)

SUBDIRS(test)
