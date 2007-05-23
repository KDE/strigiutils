include_directories(..)

add_library(xesam xesamlivesearchinterface.cpp xesamlivesearch.cpp
	xesamsession.cpp xesamsearch.cpp)

add_executable(parsertest parsertest.cpp)
