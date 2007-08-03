include_directories (${LIBXML2_INCLUDE_DIR}
                     ../
                     ${strigi_SOURCE_DIR}/src/daemon
                     ${strigi_BINARY_DIR}/src/streams/compat)
add_definitions(-DSTREAMANALYZER_EXPORT=)
set (strigi_xesam_SRCS XesamQLParser.cc XesamQueryBuilder.cc
             StrigiQueryBuilder.cc ../query.cpp)

add_library (strigi_xesam ${strigi_xesam_SRCS})
target_link_libraries(strigi_xesam ${LIBXML2_LIBRARIES} )

add_executable (xesamquerybuildertest test.cpp ../query.cpp)
target_link_libraries(xesamquerybuildertest strigi_xesam)
