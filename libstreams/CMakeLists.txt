project (libstreams)


##### cmake settings #####

cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(MacroCheckGccVisibility)
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

# libstreams defines the definitions shared by the Strigi projects
set(STRIGI_VERSION_MAJOR 0 CACHE INT "Major Strigi version number" FORCE)
set(STRIGI_VERSION_MINOR 7 CACHE INT "Minor Strigi version number" FORCE)
set(STRIGI_VERSION_PATCH 2 CACHE INT "Release Strigi version number" FORCE)
set(STRIGI_VERSION_STRING "${STRIGI_VERSION_MAJOR}.${STRIGI_VERSION_MINOR}.${STRIGI_VERSION_PATCH}" CACHE STRING "Strigi version string" FORCE)
set(LIBSTREAMS_VERSION ${STRIGI_VERSION_STRING})

# definition of LIB_DESTINATION which is the path where the lib is installed
set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)

# for StrigiConfig.cmake
get_filename_component(_LIBRARY_ROOT "${LIB_DESTINATION}" PATH)
file(RELATIVE_PATH LIBRARY_DESTINATION ${_LIBRARY_ROOT} ${LIB_DESTINATION})
file(RELATIVE_PATH INCLUDE_DESTINATION ${_LIBRARY_ROOT} ${CMAKE_INSTALL_PREFIX}/include)

if(NOT MSVC AND NOT CMAKE_SYSTEM MATCHES "SunOS-5*.")
	add_definitions(-fPIC)
endif(NOT MSVC AND NOT CMAKE_SYSTEM MATCHES "SunOS-5*.")

##### environment inspection #####

# check for visibility support
macro_check_gcc_visibility(__STRIGI_HAVE_GCC_VISIBILITY)

# check for required packages
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(Iconv REQUIRED)
find_package(Threads REQUIRED)

# generate the config file include/strigi/strigi-config.h and lib/config.h
include(ConfigureChecks.cmake)
configure_file(StrigiConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/StrigiConfig.cmake @ONLY )
configure_file(LibStreamsConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/LibStreamsConfig.cmake @ONLY )


##### building and testing #####

# every directory needs the headers that will be installed
include_directories(${libstreams_SOURCE_DIR}/include
	${libstreams_SOURCE_DIR}/lib
	${libstreams_BINARY_DIR}/include
	${libstreams_BINARY_DIR}/lib
        ${ZLIB_INCLUDE_DIR}
        ${BZIP2_INCLUDE_DIR}
        ${ICONV_INCLUDE_DIR}
)


add_subdirectory(lib)
add_subdirectory(tests)


##### installing #####

# all installed files are listed here
# headers
file(GLOB STRIGI_HEADERS include/strigi/*.h)
install(FILES
	${STRIGI_HEADERS}
	${strigi_config_output}
	${strigi_extra_config_output}
	DESTINATION include/strigi
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/StrigiConfig.cmake
	DESTINATION ${LIB_DESTINATION}/strigi)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LibStreamsConfig.cmake
	DESTINATION ${LIB_DESTINATION}/libstreams)

# library
if(NOT WIN32)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/libstreams.pc
		DESTINATION ${LIB_DESTINATION}/pkgconfig)
endif(NOT WIN32)
