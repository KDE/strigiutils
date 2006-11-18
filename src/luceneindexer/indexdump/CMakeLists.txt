include_directories(${CLUCENE_LIBRARY_DIR} ${CLUCENE_INCLUDE_DIR}
	${ICONV_INCLUDE_DIR} ..)
add_executable(indexdump indexdump.cpp)
target_link_libraries(indexdump ${CLUCENE_LIBRARY} cluceneindex)
