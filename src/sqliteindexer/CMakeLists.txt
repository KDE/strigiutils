include_directories( ../streamindexer ../streams ../filters
	${SQLITE_LIBRARY_DIR} ${SQLITE_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams)

add_library(sqliteindex sqliteindexwriter.cpp sqliteindexmanager.cpp
        sqliteindexreader.cpp)

target_link_libraries(sqliteindex streamindexer ${SQLITE_LIBRARY})

add_executable(sqliteindexer sqliteindexer.cpp indexer.cpp)
target_link_libraries(sqliteindexer sqliteindex sqlite3 filters)

