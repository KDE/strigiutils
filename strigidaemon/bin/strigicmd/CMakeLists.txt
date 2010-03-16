add_executable(strigicmd strigicmd.cpp)

target_link_libraries(strigicmd ${LOG4CXX_LIBRARIES} streamanalyzer combinedindex)

install(TARGETS strigicmd RUNTIME DESTINATION bin)
