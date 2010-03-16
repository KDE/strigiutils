include_directories(
    ../combinedindexer
    ../streamanalyzer
    ../streamanalyzer/xesam
	${strigi_BINARY_DIR}/src/streams
    ${strigi_SOURCE_DIR}/src/streams/strigi
    ${LIBXML2_INCLUDE_DIR}
)

set(strigicmd_LIBS streamanalyzer combinedindex strigi_xesam)

IF (ENABLE_LOG4CXX AND LOG4CXX_FOUND)
    add_definitions(-DHAVE_LOG4CXX)
    set(strigicmd_LIBS ${strigicmd_LIBS} ${LOG4CXX_LIBRARIES})
    include_directories( ${LOG4CXX_INCLUDE_DIR} )
ENDIF (ENABLE_LOG4CXX AND LOG4CXX_FOUND)

add_executable(strigicmd strigicmd.cpp)

target_link_libraries(strigicmd ${strigicmd_LIBS})

install(TARGETS strigicmd RUNTIME DESTINATION bin)
