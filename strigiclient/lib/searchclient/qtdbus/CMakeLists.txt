include(${QT_USE_FILE})

# strigiclient
set(strigiqtdbusclient_SRCS
	strigiasyncclient.cpp
	strigiclient.cpp
	strigidbus.cpp
	strigitypes.cpp
)
set(strigiqtdbusclient_MOC_HDRS
	strigiasyncclient.h
	strigidbus.h
)

qt4_wrap_cpp(strigiqtdbusclient_MOC_SRCS ${strigiqtdbusclient_MOC_HDRS})

add_library(strigiqtdbusclient SHARED ${strigiqtdbusclient_SRCS}
  ${strigiqtdbusclient_MOC_SRCS})

target_link_libraries(strigiqtdbusclient ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY})
set_target_properties(strigiqtdbusclient
                 PROPERTIES VERSION ${STRIGI_VERSION_STRING}
                 SOVERSION ${STRIGI_VERSION_MAJOR}
                 DEFINE_SYMBOL MAKE_STRIGI_QTDBUSCLIENT_LIB
)

install(TARGETS strigiqtdbusclient RUNTIME DESTINATION bin 
                                   LIBRARY DESTINATION ${LIB_DESTINATION} 
                                   ARCHIVE DESTINATION ${LIB_DESTINATION})
install(FILES strigiasyncclient.h strigiclient.h strigidbus.h strigitypes.h
  DESTINATION include/strigi/qtdbus)

add_executable(strigiqtdbustest strigiqtdbustest.cpp)
target_link_libraries(strigiqtdbustest strigiqtdbusclient)

set(strigiasynctest_SRCS strigiasynctest.cpp strigiasyncwidget.cpp)
set(strigiasynctest_MOC_HDRS strigiasyncwidget.h)
qt4_wrap_cpp(strigiasynctest_MOC_SRCS ${strigiasynctest_MOC_HDRS})
add_executable(strigiasynctest ${strigiasynctest_SRCS} ${strigiasynctest_MOC_SRCS})
target_link_libraries(strigiasynctest strigiqtdbusclient ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
