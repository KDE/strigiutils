add_library(searchclient SHARED socketclient.cpp asyncsocket.cpp
	asyncsocketclient.cpp)
set_target_properties(searchclient PROPERTIES
	VERSION ${STRIGI_VERSION_STRING}
	SOVERSION ${STRIGI_VERSION_MAJOR}
	DEFINE_SYMBOL MAKE_SEARCHCLIENT_LIB
)
target_link_libraries(searchclient ${LIBSTREAMANALYZER_LIBRARIES})
install(TARGETS searchclient
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION ${LIB_DESTINATION}
)
