include_directories (${LIBXML2_INCLUDE_DIR}
                     ../
                     ${strigi_BINARY_DIR}/src/streams/compat)

set (strigi_xesam_SRCS XesamQLParser.cc XesamQueryBuilder.cc
             StrigiQueryBuilder.cc)

add_library (strigi_xesam ${strigi_xesam_SRCS})
target_link_libraries(strigi_xesam ${LIBXML2_LIBRARIES} )

# add_executable (xesamquerybuildertest test.cpp)
# target_link_libraries(xesamquerybuildertest streamanalyzer strigi_xesam)