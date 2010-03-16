add_subdirectory(qtdbus)
# disable searchclient for now
if(NOT WIN32)
add_subdirectory(filterwidget)
# add_subdirectory(keywordcompletion)

include(${QT_USE_FILE})

# strigiclient
set(strigiclient_SRCS
	dlgfilters.cpp
	dlglistindexedfiles.cpp
	histogram.cpp
	searchtabs.cpp
	searchview.cpp
	simplesearchgui.cpp
	strigiclient.cpp
)
set(strigiclient_MOC_HDRS
	dlgfilters.h
	dlglistindexedfiles.h
	histogram.h
	searchtabs.h
	searchview.h
	simplesearchgui.h
)

qt4_wrap_cpp(strigiclient_MOC_SRCS ${strigiclient_MOC_HDRS})

add_executable(strigiclient ${strigiclient_SRCS} ${strigiclient_MOC_SRCS})

target_link_libraries(strigiclient strigiqtdbusclient strigihtmlgui filterwidget)

add_executable(cmdlinestrigi cmdlinestrigi.cpp)
target_link_libraries(cmdlinestrigi searchclient)

install(TARGETS strigiclient RUNTIME DESTINATION bin)

endif(NOT WIN32)
