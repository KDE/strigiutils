include_directories( ../streamindexer ../streams )

if (CLucene_FOUND)
    add_definitions(-DHAVE_CLUCENE)
    set(strigidaemon_LIBS ${strigidaemon_LIBS} cluceneindex)
    include_directories( ../luceneindexer )
endif (CLucene_FOUND)
if (HyperEstraier_FOUND)
    add_definitions(-DHAVE_ESTRAIER)
    set(strigidaemon_LIBS ${strigidaemon_LIBS} estraierindex)
    include_directories( ../estraierindexer )
endif (HyperEstraier_FOUND)

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

