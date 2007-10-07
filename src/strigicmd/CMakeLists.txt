include_directories(
    ../combinedindexer
    ../streamanalyzer
    ../streamanalyzer/xesam
	${strigi_BINARY_DIR}/src/streams
    ${strigi_SOURCE_DIR}/src/streams/strigi
    ${LIBXML2_INCLUDE_DIR}
)

add_executable(strigicmd strigicmd.cpp)

target_link_libraries(strigicmd streamanalyzer combinedindex strigi_xesam)

install(TARGETS strigicmd RUNTIME DESTINATION bin)
