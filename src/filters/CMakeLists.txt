IF (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    add_definitions(-DHAVE_LOG4CXX)
    link_libraries(filters ${LOG4CXX_LIBRARIES})
    include_directories( ${LOG4CXX_INCLUDE_DIR} )
ENDIF (ENABLE_LOG4CXX AND Log4cxx_FOUND)

include_directories( ../streams ../streamindexer )
ADD_LIBRARY(filters filters.cpp filtermanager.cpp fnmatch.cpp)
IF(WIN32)
	#todo: filters doesn't have enough api's shared to make it a library
	install(TARGETS filters ARCHIVE DESTINATION lib)
ENDIF(WIN32)

SUBDIRS(test)
