include_directories(${CLUCENE_LIBRARY_DIR} ${CLUCENE_INCLUDE_DIR}
	${ICONV_INCLUDE_DIR} ..)
add_executable(indexdump indexdump.cpp ../tcharutils.cpp)
target_link_libraries(indexdump streamanalyzer ${CLUCENE_LIBRARY})
