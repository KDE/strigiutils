include_directories( ../streamindexer ../streams
	${CLUCENE_LIBRARY_DIR} ${CLUCENE_INCLUDE_DIR} ${ICONV_INCLUDE_DIR})

add_library(cluceneindex cluceneindexwriter.cpp cluceneindexmanager.cpp
        cluceneindexreader.cpp)

target_link_libraries(cluceneindex streamindexer ${CLUCENE_LIBRARY})

add_executable(luceneindexer luceneindexer.cpp indexer.cpp)
target_link_libraries(luceneindexer cluceneindex)

