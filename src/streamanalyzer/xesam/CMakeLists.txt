if (ENABLE_LOG4CXX AND LOG4CXX_FOUND)
    set(xesam_LIBS ${xesam_LIBS} ${LOG4CXX_LIBRARIES})
    set (INC_DIR ${INC_DIR} ${LOG4CXX_INCLUDE_DIR})
    add_definitions(-DHAVE_LOG4CXX)
endif (ENABLE_LOG4CXX AND LOG4CXX_FOUND)

include_directories (${LIBXML2_INCLUDE_DIR}
                     ../
                     ${strigi_SOURCE_DIR}/src/daemon
                     ${strigi_BINARY_DIR}/src/streams
                     ${INC_DIR})

#if (BISON)
#add_custom_command (OUTPUT xesam_ul_parser.cc
#                    PRE_BUILD
#                    COMMAND bison -t -o xesam_ul_parser.cc -d xesam_ul_parser.yy
#                    DEPENDS xesam_ul_parser.yy
#                    WORKING_DIRECTORY ${strigi_SOURCE_DIR}/src/streamanalyzer/xesam/
#                    )
#endif (BISON)

set (strigi_xesam_SRCS XesamQLParser.cc XesamQueryBuilder.cc
            StrigiQueryBuilder.cc xesam_ul_driver.cc
            xesam_ul_scanner.cpp xesam_ul_file_scanner.cpp xesam_ul_string_scanner.cpp 
            xesam_ul_parser.cc xesam2strigi.cpp)

add_library (strigi_xesam STATIC ${strigi_xesam_SRCS})
target_link_libraries(strigi_xesam ${LIBXML2_LIBRARIES} streamanalyzer ${xesam_LIBS})

# no getopt on win32
if(NOT WIN32)
  add_executable (xesamquerybuildertest test.cpp ../query.cpp)
  target_link_libraries(xesamquerybuildertest strigi_xesam ${xesam_LIBS})
endif(NOT WIN32)
