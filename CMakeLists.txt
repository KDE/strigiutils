project (strigiutils)

##### cmake settings #####

cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#include(MacroCheckGccVisibility)
#include(MacroFindOptionalDep)
enable_testing()


##### global variables #####
# Set up RPATH handling, so the libs are found if they are installed to a non-standard location.
# By default cmake builds the targets with full RPATH to everything in the build directory,
# but then removes the RPATH when installing.
# These two options below make it set the RPATH of the installed targets to all
# RPATH directories outside the current CMAKE_BINARY_DIR and also the library
# install directory. Alex
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH  TRUE)
set(CMAKE_INSTALL_RPATH                ${LIB_DESTINATION} )


##### environment inspection #####

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  find_package(LibStreams REQUIRED)
  find_package(LibStreamAnalyzer REQUIRED)
else()
  set(LIBSTREAMS_INCLUDE_DIRS
    ${libstreams_SOURCE_DIR}/include
    ${libstreams_BINARY_DIR}/include
    )
  set(LIBSTREAMANALYZER_INCLUDE_DIRS
    ${libstreamanalyzer_SOURCE_DIR}/include
    ${libstreamanalyzer_BINARY_DIR}/include
    )
  set(LIBSTREAMS_LIBRARIES streams)
  set(LIBSTREAMANALYZER_LIBRARIES streamanalyzer)
endif()

# check for required packages
include(ConfigureChecks.cmake)

##### building and testing #####
include_directories(${LIBSTREAMS_INCLUDE_DIRS})
include_directories(${LIBSTREAMANALYZER_INCLUDE_DIRS})
link_directories(${LIBSTREAMANALYZER_LIBRARY_DIRS})

add_subdirectory(bin)


##### installing #####
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp "")
if(HAVE_REGEX_H)
	add_executable(deepfind dummy.cpp)	
	target_link_libraries(deepfind libdeepfind)
	install(TARGETS deepfind RUNTIME DESTINATION bin)
	add_executable(deepgrep dummy.cpp)
	target_link_libraries(deepgrep libdeepgrep)
	install(TARGETS deepgrep RUNTIME DESTINATION bin)
endif(HAVE_REGEX_H)
if(NOT MSVC)
	add_executable(xmlindexer dummy.cpp)
	target_link_libraries(xmlindexer libxmlindexer)
	install(TARGETS xmlindexer RUNTIME DESTINATION bin)
	add_executable(rdfindexer dummy.cpp)
	target_link_libraries(rdfindexer librdfindexer)
	install(TARGETS rdfindexer RUNTIME DESTINATION bin)
endif(NOT MSVC)
