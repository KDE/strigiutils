include_directories(../combinedindexer ../streamanalyzer
    "${CMAKE_CURRENT_BINARY_DIR}/../streams/compat")

add_executable(strigicmd strigicmd.cpp indexer.cpp)

target_link_libraries(strigicmd streamanalyzer combinedindex)

install(TARGETS strigicmd RUNTIME DESTINATION bin)
