include(${QT_USE_FILE})

include_directories( ../streams ../archiveengine)

# qclientold
set(qclientold_SRCS filehandler.cpp filebrowser.cpp)
set(qclientold_MOC_HDRS filebrowser.h)
qt4_wrap_cpp(qclientold_MOC_SRCS ${qclientold_MOC_HDRS})
add_executable(qclient ${qclientold_SRCS} ${qclientold_MOC_SRCS})

target_link_libraries(qclient streams archiveengine ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

# archivecat
add_executable(archivecat archivecat.cpp)

target_link_libraries(archivecat streams archiveengine ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

# archivedialog
add_executable(archivedialog archivedialog.cpp)

target_link_libraries(archivedialog streams archiveengine ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

