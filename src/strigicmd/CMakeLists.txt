include_directories(../combinedindexer ../streamanalyzer)

add_executable(strigicmd strigicmd.cpp)

target_link_libraries(strigicmd streamanalyzer combinedindex)

install(TARGETS strigicmd RUNTIME DESTINATION bin)




