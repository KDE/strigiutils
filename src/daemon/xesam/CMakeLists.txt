include_directories(.. ../../streamanalyzer)

add_library(xesam xesamlivesearchinterface.cpp xesamlivesearch.cpp
	xesamsession.cpp xesamsearch.cpp)

add_executable(xesamparsertest xesamparsertest.cpp)
target_link_libraries(xesamparsertest streamanalyzer)

