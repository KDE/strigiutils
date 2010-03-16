project (strigiclient)


##### cmake settings #####

cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(MacroFindOptionalDep)
enable_testing()


##### global variables #####
option(ENABLE_QT4 "enable Qt4 GUI" ON)
set(QT_MIN_VERSION "4.3.0")

# definition of LIB_DESTINATION which is the path where the lib is installed
set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)


##### environment inspection #####
find_optional_dep(Qt4 ENABLE_QT4 QT4_FOUND "Qt4 GUI client")
if(QT4_FOUND)
  # properly set up compile flags (QT_DEBUG/QT_NO_DEBUG, ...)
  include(${QT_USE_FILE})
endif(QT4_FOUND)

# check for required packages
if(STRIGI_VERSION_STRING)
	# if STRIGI_VERSION_STRING is defined, we are compiling the meta-package
	set(LIBSTREAMS_INCLUDE_DIRS
		../libstreams/include
		${CMAKE_CURRENT_BINARY_DIR}/../libstreams/include)
	set(LIBSTREAMANALYZER_INCLUDE_DIRS
		../libstreamanalyzer/include
		${CMAKE_CURRENT_BINARY_DIR}/../libstreamanalyzer/include)
	set(LIBSEARCHCLIENT_INCLUDE_DIRS
		../strigidaemon/include)
	set(LIBSTREAMS_LIBRARIES streams)
	set(LIBSTREAMANALYZER_LIBRARIES streamanalyzer)
else(STRIGI_VERSION_STRING)
	find_package(LibStreams REQUIRED)
	find_package(LibStreamAnalyzer REQUIRED)
	find_package(LibSearchClient REQUIRED)
endif(STRIGI_VERSION_STRING)

##### building and testing #####


# every directory needs the headers that will be installed
include_directories(${LIBSTREAMS_INCLUDE_DIRS}
	${LIBSTREAMANALYZER_INCLUDE_DIRS}
	${LIBSEARCHCLIENT_INCLUDE_DIRS}
	lib/htmlgui
	lib/searchclient/filterwidget
	lib/searchclient/qtdbus
)

link_directories(${LIBSTREAMANALYZER_LIBRARY_DIRS})
link_directories(${LIBSEARCHCLIENT_LIBRARY_DIRS})

add_subdirectory(lib)
add_subdirectory(bin)


##### installing #####

