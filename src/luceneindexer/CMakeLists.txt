subdirs(tests indexdump)

include_directories( ../streamindexer ../streams ../filters
	${CLUCENE_LIBRARY_DIR} ${CLUCENE_INCLUDE_DIR} ${ICONV_INCLUDE_DIR})

IF(MSVC)
    ADD_DEFINITIONS(-D_UNICODE)
    ADD_DEFINITIONS(-DUNICODE)
ENDIF(MSVC)
add_library(cluceneindex cluceneindexwriter.cpp cluceneindexmanager.cpp
        cluceneindexreader.cpp PrefixFilter.cpp)

target_link_libraries(cluceneindex streamindexer ${CLUCENE_LIBRARY})

add_executable(luceneindexer luceneindexer.cpp indexer.cpp)
target_link_libraries(luceneindexer cluceneindex filters)

install(TARGETS luceneindexer RUNTIME DESTINATION bin)

