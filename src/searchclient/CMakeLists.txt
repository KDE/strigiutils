include_directories(../streamanalyzer ../htmlgui ../streams ../daemon
	qtdbus ${strigi_BINARY_DIR}/src/streams/compat filterwidget)
add_subdirectory(qtdbus)
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

install(TARGETS strigiclient RUNTIME DESTINATION bin)




