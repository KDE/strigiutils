include_directories(${QT_INCLUDE_DIR})

if (QT4_FOUND)
  include(${QT_USE_FILE})

  # strigiclient
  set(strigiqtdbusclient_SRCS strigitypes.cpp strigidbus.cpp strigiclient.cpp)
  set(strigiqtdbusclient_MOC_HDRS strigidbus.h)

  qt4_wrap_cpp(strigiqtdbusclient_MOC_SRCS ${strigiqtdbusclient_MOC_HDRS})

  add_library(strigiqtdbusclient ${strigiqtdbusclient_SRCS}
    ${strigiqtdbusclient_MOC_SRCS})

  target_link_libraries(strigiqtdbusclient ${QT_QTCORE_LIBRARY} QtDBus)

  add_executable(strigiqtdbustest strigiqtdbustest.cpp)
  target_link_libraries(strigiqtdbustest strigiqtdbusclient ${QT_QTGUI_LIBRARY})

endif (QT4_FOUND)




