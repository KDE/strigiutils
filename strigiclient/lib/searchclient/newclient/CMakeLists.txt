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
   histogram_impl.cpp
)
set(strigiclient_MOC_HDRS
	dlgfilters.h
	dlglistindexedfiles.h
	histogram.h
	searchtabs.h
	searchview.h
	simplesearchgui.h
   histogram_impl.h
)
set(strigiclient_UIS
   searchdialog.ui
   histogramwidget.ui
)

qt4_wrap_cpp(strigiclient_MOC_SRCS ${strigiclient_MOC_HDRS})
qt4_wrap_ui(strigiclient_UIS_H ${strigiclient_UIS})


add_executable(strigiclient 
   ${strigiclient_SRCS} 
   ${strigiclient_UIS_H}
   ${strigiclient_MOC_SRCS}
)

target_link_libraries(strigiclient strigiqtdbusclient strigihtmlgui filterwidget)

add_executable(cmdlinestrigi cmdlinestrigi.cpp)
target_link_libraries(cmdlinestrigi searchclient)

install(TARGETS strigiclient RUNTIME DESTINATION bin)




