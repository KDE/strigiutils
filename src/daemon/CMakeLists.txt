include_directories( ../streamindexer ../streams )

if (FOUND_CLUCENE)
    add_definitions(-DHAVE_CLUCENE)
    set(strigidaemon_LIBS ${strigidaemon_LIBS} ${CLUCENE_LIBRARIES}
        cluceneindex)
    include_directories( ../luceneindexer )
endif (FOUND_CLUCENE)

add_executable(strigidaemon daemon.cpp server.cpp socketserver.cpp interface.cpp
	indexscheduler.cpp clientinterface.cpp)
target_link_libraries(strigidaemon streamindexer ${CMAKE_THREAD_LIBS_INIT}
	${strigidaemon_LIBS})

add_library(searchclient SHARED socketclient.cpp clientinterface.cpp 
        asyncsocket.cpp asyncsocketclient.cpp)

install(TARGETS strigidaemon searchclient
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib)
install(FILES asyncsocketclient.h asyncsocket.h clientinterface.h
	DESTINATION include)

