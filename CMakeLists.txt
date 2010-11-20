project (strigi)

# This is the aggregate build file. It serves only to call the build in the
# subdirectories. Each of the projects in the subdirectories is buildable
# separately.

cmake_minimum_required(VERSION 2.6)
# for testing to work in cmake, this command must be called in the root src dir
enable_testing()

if(MSVC)
  add_definitions(-wd4251)
  add_definitions(-wd4355)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_USE_MATH_DEFINES)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Zc:wchar_t-")
endif(MSVC)

add_subdirectory(libstreams)
add_subdirectory(libstreamanalyzer)
add_subdirectory(strigiutils)
if(NOT MSVC)
  add_subdirectory(strigidaemon)
  add_subdirectory(strigiclient)
endif(NOT MSVC)
