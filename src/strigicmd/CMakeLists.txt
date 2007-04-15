include_directories(../combinedindexer ../streamanalyzer "${CMAKE_BINARY_DIR}/src/streams/compat")

add_executable(strigicmd strigicmd.cpp)

target_link_libraries(strigicmd streamanalyzer combinedindex)

install(TARGETS strigicmd RUNTIME DESTINATION bin)




