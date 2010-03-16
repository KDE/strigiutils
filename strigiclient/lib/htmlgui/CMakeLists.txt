add_library(strigihtmlgui SHARED strigihtmlgui.cpp)
set_target_properties(strigihtmlgui PROPERTIES
    VERSION ${STRIGI_VERSION_STRING}
    SOVERSION ${STRIGI_VERSION_MAJOR}
    DEFINE_SYMBOL MAKE_STRIGIHTMLGUI_LIB
)

target_link_libraries(strigihtmlgui searchclient streamanalyzer)

install(TARGETS strigihtmlgui DESTINATION ${LIB_DESTINATION})
install(FILES strigihtmlgui.h
	DESTINATION include/strigi)
