include_directories(${QT_INCLUDE_DIR})

if (QT4_FOUND)
  include(${QT_USE_FILE})

  # strigiclient
  set(strigiqtdbusclient_SRCS strigitypes.cpp strigidbus.cpp strigiclient.cpp
    strigiasyncclient.cpp)
  set(strigiqtdbusclient_MOC_HDRS strigidbus.h strigiasyncclient.h)

  qt4_wrap_cpp(strigiqtdbusclient_MOC_SRCS ${strigiqtdbusclient_MOC_HDRS})

  add_library(strigiqtdbusclient SHARED ${strigiqtdbusclient_SRCS}
    ${strigiqtdbusclient_MOC_SRCS})

  target_link_libraries(strigiqtdbusclient ${QT_QTDBUS_LIBRARY})

  install(TARGETS strigiqtdbusclient LIBRARY DESTINATION ${LIB_DESTINATION})
  install(FILES strigiasyncclient.h strigiclient.h strigidbus.h strigitypes.h
    DESTINATION include/strigi/qtdbus)

  add_executable(strigiqtdbustest strigiqtdbustest.cpp)
  target_link_libraries(strigiqtdbustest strigiqtdbusclient)

  set(strigiasynctest_SRCS strigiasynctest.cpp strigiasyncwidget.cpp)
  set(strigiasynctest_MOC_HDRS strigiasyncwidget.h)
  qt4_wrap_cpp(strigiasynctest_MOC_SRCS ${strigiasynctest_MOC_HDRS})
  add_executable(strigiasynctest ${strigiasynctest_SRCS}
    ${strigiasynctest_MOC_SRCS})
  target_link_libraries(strigiasynctest strigiqtdbusclient ${QT_QTGUI_LIBRARY})

endif (QT4_FOUND)

