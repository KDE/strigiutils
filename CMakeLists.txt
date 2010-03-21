project (strigi)

# due to a bug in libxml2, we only get thread support if _REENTRANT is defined      
add_definitions(-D_REENTRANT)

# This is the aggregate build file. It serves only to call the build in the
# subdirectories. Each of the projects in the subdirectories is buildable
# separately.

cmake_minimum_required(VERSION 2.6)
# for testing to work in cmake, this command must be called in the root src dir
enable_testing()

add_subdirectory(libstreams)
add_subdirectory(libstreamanalyzer)
add_subdirectory(strigiutils)
add_subdirectory(strigidaemon)
add_subdirectory(strigiclient)
