include_directories (${LIBXML2_INCLUDE_DIR}
                     ../
                     ${strigi_SOURCE_DIR}/src/daemon
                     ${strigi_BINARY_DIR}/src/streams)

add_custom_command (OUTPUT xesam_ul_parser.cc
                    COMMAND bison -t -o ${strigi_SOURCE_DIR}/src/streamanalyzer/xesam/xesam_ul_parser.cc -d ${strigi_SOURCE_DIR}/src/streamanalyzer/xesam/xesam_ul_parser.yy
                    DEPENDS ${strigi_SOURCE_DIR}/src/streamanalyzer/xesam/xesam_ul_parser.yy
                    )

set (strigi_xesam_SRCS XesamQLParser.cc XesamQueryBuilder.cc
            StrigiQueryBuilder.cc xesam_ul_driver.cc xesam_ul_file_scanner.cpp
            xesam_ul_parser.cc xesam2strigi.cpp)

add_library (strigi_xesam ${strigi_xesam_SRCS})
target_link_libraries(strigi_xesam ${LIBXML2_LIBRARIES} streamanalyzer)

# no getopt on win32
if(NOT WIN32)
  add_executable (xesamquerybuildertest test.cpp ../query.cpp)
  target_link_libraries(xesamquerybuildertest strigi_xesam)
endif(NOT WIN32)
