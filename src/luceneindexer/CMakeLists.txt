subdirs(tests indexdump simplereader)

include_directories( ../streamindexer ../streams
	${CLUCENE_LIBRARY_DIR} ${CLUCENE_INCLUDE_DIR} ${ICONV_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams/compat
	${strigi_SOURCE_DIR}/src/streams/compat)

IF(MSVC)
    ADD_DEFINITIONS(-D_UNICODE)
    ADD_DEFINITIONS(-DUNICODE)
ENDIF(MSVC)
add_library(cluceneindex cluceneindexwriter.cpp cluceneindexmanager.cpp
        cluceneindexreader.cpp PrefixFilter.cpp)

target_link_libraries(cluceneindex streamindexer ${CLUCENE_LIBRARY})

add_executable(luceneindexer luceneindexer.cpp indexer.cpp)
target_link_libraries(luceneindexer cluceneindex)

install(TARGETS luceneindexer RUNTIME DESTINATION bin)

