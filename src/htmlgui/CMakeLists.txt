include_directories( ../daemon ../streamindexer )

add_library(strigihtmlgui SHARED strigihtmlgui.cpp)
target_link_libraries(strigihtmlgui searchclient streamindexer)

install(TARGETS strigihtmlgui
	LIBRARY DESTINATION lib)
install(FILES strigihtmlgui.h
	DESTINATION include)
