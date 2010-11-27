project (libstreamanalyzer)


##### cmake settings #####

cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(MacroCheckGccVisibility)
include(MacroFindOptionalDep)
enable_testing()


##### global variables #####

# due to a bug in libxml2, we only get thread support if _REENTRANT is defined
add_definitions(-D_REENTRANT)

# Set up RPATH handling, so the libs are found if they are installed to a non-standard location.
# By default cmake builds the targets with full RPATH to everything in the build directory,
# but then removes the RPATH when installing.
# These two options below make it set the RPATH of the installed targets to all
# RPATH directories outside the current CMAKE_BINARY_DIR and also the library
# install directory. Alex
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH  TRUE)
set(CMAKE_INSTALL_RPATH                ${LIB_DESTINATION} )

set(LIBSTREAMANALYZER_VERSION 0.7.3)
set(LIBSTREAMANALYZER_SOVERSION 0)
option(BUILD_UTILS "build luceneindexer, xmlindexer, rdfindexer, ontoprint utilities" ON)
option(BUILD_DEEPTOOLS "build deep find and deepgrep tools" ON)
option(ENABLE_XINE
       "enable Xine support. This adds support for a larger number of media formats(highly unstable)." OFF)
option(ENABLE_FFMPEG
       "enable FFMPEG support. This adds support for a larger number of media formats(testing)." ON)
option(ENABLE_EXIV2
       "enable exiv2 support. This allows you to index EXIF/IPTC metadata." ON)
set(CLUCENE_MIN_VERSION "0.9.21")
option(ENABLE_CLUCENE "enable CLucene support (recommended)" ON)
option(ENABLE_CLUCENE_NG "enable clucene-ng (version 1+) support (testing)" ON)

# definition of LIB_DESTINATION which is the path where the lib is installed
set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)

if(NOT MSVC AND NOT CMAKE_SYSTEM MATCHES "SunOS-5*.")
	add_definitions(-fPIC)
endif(NOT MSVC AND NOT CMAKE_SYSTEM MATCHES "SunOS-5*.")

#
# AC_CHECK_LIB(dl, dlopen, DL_LIBRARY="-ldl", DL_LIBRARY="") for cmake by
# alexander neundorf
#
# on FreeBSD dlopen is in libc, on GNU/Linux it's in libdl
if(NOT WIN32)
  include(CheckFunctionExists)
  include(CheckLibraryExists)
  check_library_exists(dl dlopen ""  DLOPEN_IN_LIBDL)
  check_function_exists(dlopen DLOPEN_IN_LIBC)
  set(LIBDL)

  if (DLOPEN_IN_LIBC)
   set(HAVE_LIBDL TRUE)
  endif (DLOPEN_IN_LIBC)

  if (DLOPEN_IN_LIBDL)
   set(LIBDL dl)
   set(HAVE_LIBDL TRUE)
  endif (DLOPEN_IN_LIBDL)

  if (NOT HAVE_LIBDL)
   message(FATAL_ERROR "dlopen support is required.")
  endif (NOT HAVE_LIBDL)
endif(NOT WIN32)


##### environment inspection #####

# check for visibility support
macro_check_gcc_visibility(__STRIGI_HAVE_GCC_VISIBILITY)

# check for required packages
if(STRIGI_VERSION_STRING)
	# libstreams is being compiled now too
	set(LIBSTREAMS_INCLUDE_DIRS
		../libstreams/include
		${CMAKE_CURRENT_BINARY_DIR}/../libstreams/include)
	set(LIBSTREAMS_LIBRARIES streams)
else(STRIGI_VERSION_STRING)
	find_package(LibStreams REQUIRED)
endif(STRIGI_VERSION_STRING)
find_package(Threads REQUIRED)
find_package(LibXml2 REQUIRED)

find_optional_dep(CLucene ENABLE_CLUCENE CLucene_FOUND "CLucene backend")
find_optional_dep(CLucene1 ENABLE_CLUCENE_NG CLUCENE1_FOUND "CLucene Version 1+ backend")
find_optional_dep(Exiv2 ENABLE_EXIV2 EXIV2_FOUND "indexing of EXIF/IPTC metadata")
find_optional_dep(FFMPEG ENABLE_FFMPEG FFMPEG_FOUND "indexing FFMPEG")
find_package(XAttr)
include(ConfigureChecks.cmake)
configure_file(LibStreamAnalyzer.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/LibStreamAnalyzerConfig.cmake @ONLY )

##### building and testing #####

configure_file(${libstreamanalyzer_SOURCE_DIR}/strigi_thread.h.cmake
	${libstreamanalyzer_BINARY_DIR}/include/strigi/strigi_thread.h)

# every directory needs the headers that will be installed
include_directories(${LIBSTREAMS_INCLUDE_DIRS}
	${libstreamanalyzer_SOURCE_DIR}/include
	${libstreamanalyzer_BINARY_DIR}/include
        ${libstreamanalyzer_SOURCE_DIR}/lib
	${libstreamanalyzer_BINARY_DIR}/lib
	${ICONV_INCLUDE_DIR}
	${LIBSTREAMS_LIBRARY_DIRS}
  	${EXIV2_INCLUDEDIR}
	${XINE_INCLUDE_DIR}
	${FFMPEG_INCLUDE_DIRS}
	${MAGIC_INCLUDE_DIR}
	${LIBXML2_INCLUDE_DIR}
)
if(XATTR_FOUND)
	include_directories(${XATTR_INCLUDE_DIR})
endif(XATTR_FOUND)

add_subdirectory(lib)
add_subdirectory(plugins)
#add_subdirectory(tests)


##### installing #####

# all installed files are listed here
# headers
file(GLOB STRIGI_HEADERS include/strigi/*.h)
install(FILES
	${STRIGI_HEADERS}
	${libstreamanalyzer_BINARY_DIR}/include/strigi/strigi_thread.h
	DESTINATION include/strigi
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LibStreamAnalyzerConfig.cmake
	DESTINATION ${LIB_DESTINATION}/libstreamanalyzer)
# ontologies
file (GLOB STRIGI_ONTOLOGIES share/strigi/fieldproperties/*.rdfs)
install(FILES
	${STRIGI_ONTOLOGIES}
	DESTINATION share/strigi/fieldproperties/
)
# library
if(NOT WIN32)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/libstreamanalyzer.pc
		DESTINATION ${LIB_DESTINATION}/pkgconfig)
endif(NOT WIN32)
