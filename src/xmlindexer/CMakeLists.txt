include_directories( ../streamindexer ../streams ../filters)

add_executable(xmlindexer xmlindexer.cpp indexer.cpp)

target_link_libraries(xmlindexer streamindexer filters)

install(TARGETS xmlindexer RUNTIME DESTINATION bin)
