#subdirs(tests)

include_directories( ../streamanalyzer ../streams ../streams/compat
	${SQLITE_LIBRARY_DIR} ${SQLITE_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams/compat)

add_library(sqliteindex sqliteindexwriter.cpp sqliteindexmanager.cpp
        sqliteindexreader.cpp)

target_link_libraries(sqliteindex streamanalyzer ${SQLITE_LIBRARIES})

add_executable(sqliteindexer sqliteindexer.cpp indexer.cpp)
target_link_libraries(sqliteindexer sqliteindex sqlite3)

