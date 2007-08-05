include_directories (${LIBXML2_INCLUDE_DIR}
                     ../
                     ${strigi_SOURCE_DIR}/src/daemon
                     ${strigi_BINARY_DIR}/src/streams)

set (strigi_xesam_SRCS XesamQLParser.cc XesamQueryBuilder.cc
             StrigiQueryBuilder.cc)

add_library (strigi_xesam ${strigi_xesam_SRCS})
target_link_libraries(strigi_xesam ${LIBXML2_LIBRARIES} streamanalyzer)

# no getopt on win32
if(NOT WIN32)
  add_executable (xesamquerybuildertest test.cpp ../query.cpp)
  target_link_libraries(xesamquerybuildertest strigi_xesam)
endif(NOT WIN32)
