include_directories(
    ../combinedindexer
    ../streamanalyzer
	${strigi_BINARY_DIR}/src/streams/compat
    ${strigi_SOURCE_DIR}/src/streams/compat
)

add_executable(strigicmd strigicmd.cpp)

target_link_libraries(strigicmd streamanalyzer combinedindex)

install(TARGETS strigicmd RUNTIME DESTINATION bin)
