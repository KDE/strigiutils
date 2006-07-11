include_directories( ../daemon ../streamindexer )

add_library(strigihtmlgui SHARED strigihtmlgui.cpp)
set_target_properties(strigihtmlgui PROPERTIES SOVERSION ${STRIGI_VERSION})
target_link_libraries(strigihtmlgui searchclient streamindexer)

install(TARGETS strigihtmlgui
	LIBRARY DESTINATION lib)
install(FILES strigihtmlgui.h
	DESTINATION include)
