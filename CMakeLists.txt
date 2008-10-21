project (strigi)

set(STRIGI_VERSION_MAJOR 0 CACHE INT "Major Strigi version number" FORCE)
set(STRIGI_VERSION_MINOR 6 CACHE INT "Minor Strigi version number" FORCE)
set(STRIGI_VERSION_PATCH 0 CACHE INT "Release Strigi version number" FORCE)
set(STRIGI_VERSION_STRING "${STRIGI_VERSION_MAJOR}.${STRIGI_VERSION_MINOR}.${STRIGI_VERSION_PATCH}" CACHE STRING "Strigi version string" FORCE)

cmake_minimum_required(VERSION 2.4.5 FATAL_ERROR)

if(WIN32)
 set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

# include specific modules
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# include general modules
include(UsePkgConfig)
include(MacroCheckGccVisibility)
include(MacroFindOptionalDep)

# compile in debug mode
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

#add extra search paths for libraries and includes
set (STRIGI_EXTRA_INC_DIRECTORY "" CACHE STRING "Extra include directories to search in")
set (STRIGI_EXTRA_LIB_DIRECTORY "" CACHE STRING "Extra library directories to search in")
set (CMAKE_INCLUDE_PATH "${STRIGI_EXTRA_INC_DIRECTORY};${CMAKE_INCLUDE_PATH}")
set (CMAKE_LIBRARY_PATH "${STRIGI_EXTRA_LIB_DIRECTORY};${CMAKE_LIBRARY_PATH}")
set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)
set (MIMEINSTALLDIR "${CMAKE_INSTALL_PREFIX}/share/mime")

# Always include srcdir and builddir in include path
# This saves typing ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY} in about every subdir
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(APPLE)
    set(CMAKE_INSTALL_NAME_DIR ${LIB_DESTINATION})
endif(APPLE)

if(MSVC)
    # msvc2005 deprecated warnings
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -Zc:wchar_t-)
    # calm mdown msvc
    add_definitions(-wd4251) # 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    add_definitions(-wd4275) # non  DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
    add_definitions(-wd4309) # 'conversion' : truncation of constant value
    add_definitions(-wd4503) # decorated name length exceeded
    add_definitions(-wd4786) # identifier was truncated to '255' characters in the debug information
else(MSVC)
    if (NOT WIN32)
       if(NOT CMAKE_SYSTEM MATCHES "SunOS-5*.")     # I suppose even GCC on SunOS doesn't like -fPIC
          add_definitions(-fPIC)
       endif(NOT CMAKE_SYSTEM MATCHES "SunOS-5*.")
       if (CMAKE_COMPILER_IS_GNUCXX)
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common -fexceptions -Wno-unused-parameter")
       endif(CMAKE_COMPILER_IS_GNUCXX)
    endif (NOT WIN32)
endif(MSVC)

option(ENABLE_INOTIFY "enable inotify support (unstable)" OFF)
if(NOT WIN32)
    option(ENABLE_FAM "enable FAM support (testing)" OFF)
endif(NOT WIN32)
option(ENABLE_DBUS "enable dbus support" ON)
option(ENABLE_LOG4CXX "enable log4cxx support for advanced logging" OFF)
option(ENABLE_CPPUNIT "enable CppUnit for unit tests" ON)
option(ENABLE_QT4 "enable Qt4 GUI" ON)
option(ENABLE_EXIV2
       "enable exiv2 support. This allows you to index EXIF/IPTC metadata." ON)

# backends

set(CLUCENE_MIN_VERSION "0.9.19")

option(ENABLE_CLUCENE "enable CLucene support (recommended)" ON)
option(ENABLE_HYPERESTRAIER "enable Hyper Estraier support(unreliable)" OFF)
option(ENABLE_SQLITE "enable SQLite support(unreliable)" OFF)

#OPTION(ENABLE_LIBXML2 "enable libxml2 support" ON)
option(ENABLE_EXPAT "enable expat support" OFF)


find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(Threads REQUIRED)
find_package(Iconv REQUIRED)

# use either expat or libxml2
find_optional_dep(Expat ENABLE_EXPAT Expat_FOUND "XML via Expat")
# libxml seems to be required regardless of what because it's used by streamanalyzer
#FIND_OPTIONAL_DEP(LibXml2 ENABLE_LIBXML2 LIBXML2_FOUND "XML via LibXml2")
find_package(LibXml2 REQUIRED)

if (NOT LIBXML2_FOUND AND NOT Expat_FOUND)
  message(FATAL_ERROR "You need libxml2 or libexpat")
endif (NOT LIBXML2_FOUND AND NOT Expat_FOUND)

find_optional_dep(CLucene ENABLE_CLUCENE CLucene_FOUND "CLucene backend")
find_optional_dep(HyperEstraier ENABLE_HYPERESTRAIER HyperEstraier_FOUND "HyperEstraier backend")
find_optional_dep(SQLite ENABLE_SQLITE SQLite_FOUND "SQLite backend")

find_optional_dep(Exiv2 ENABLE_EXIV2 EXIV2_FOUND "indexing of EXIF/IPTC metadata")

#find_package(XAttr)
set(QT_MIN_VERSION "4.3.0")
find_optional_dep(Qt4 ENABLE_QT4 QT4_FOUND "Qt4 GUI client")

check_include_files(strings.h     HAVE_STRINGS_H)                      # various

if(ENABLE_DBUS)
  if(WIN32)
    file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _program_FILES_DIR)
    find_path(DBUS_INCLUDE_DIR dbus/dbus.h
        ${_program_FILES_DIR}/dbus/include ${CMAKE_INSTALL_PREFIX}/include ${CMAKE_INCLUDE_PATH}
    )
    set(DBUS_LIBRARY_DIR ${_program_FILES_DIR}/dbus/lib)
    find_library(DBUS_LIBRARY NAMES dbus dbus-1
        PATHS ${DBUS_LIBRARY_DIR} ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_LIBRARY_PATH}
    )
  else(WIN32)
    include(UsePkgConfig)
    pkgconfig(dbus-1 DBUS_INCLUDE_DIR DBUS_LIBRARY_DIR DBUS_LDFLAGS DBUS_CFLAGS)
    
    if (DBUS_INCLUDE_DIR)
      exec_program(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=1.0 dbus-1 RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )
      if(_return_VALUE STREQUAL "0")
        message(STATUS "Found dbus-1 release >= 1.0")
        set(DBUS_FOUND "ON") 
      else(_return_VALUE STREQUAL "0")
         message(STATUS "Found dbus-1 release < 1.0. Release >=1.0 is needed")
      endif(_return_VALUE STREQUAL "0")
    endif (DBUS_INCLUDE_DIR)

    report_optional_package_status(DBus-1 ENABLE_DBUS DBUS_FOUND "DBus interface in Strigi daemon")
    if(NOT DBUS_FOUND)
      set(ENABLE_DBUS "OFF")
      if(FORCE_DEPS)
        message(FATAL_ERROR "Aborting, DBus not found")
      endif(FORCE_DEPS)
    endif(NOT DBUS_FOUND)

  endif(WIN32)
endif(ENABLE_DBUS)

# Don't delete bison section, but only micron seems to need this
#find_program (BISON
#              bison
#              DOC "Path to bison command, used for xesam userlanguage parser generation")

#if (BISON)
#  MESSAGE (STATUS "Found bison: ${BISON}")
#else (BISON)
#  MESSAGE ("** GNU bison not found. This affects the xesam parser.")
#endif (BISON)

find_optional_dep(Log4cxx ENABLE_LOG4CXX LOG4CXX_FOUND "advanced logging")
if(NOT WIN32)
    find_optional_dep(FAM ENABLE_FAM FAM_FOUND "efficient file change monitoring system")
endif(NOT WIN32)

#
# AC_CHECK_LIB(dl, dlopen, DL_LIBRARY="-ldl", DL_LIBRARY="") for cmake by
# alexander neuendorf
#
# on FreeBSD dlopen is in libc, on GNU/Linux it's in libdl
if(NOT WIN32)
  include(CheckFunctionExists)
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
   message(FATAL "dlopen support is required.")
  endif (NOT HAVE_LIBDL)
endif(NOT WIN32)

# check for visibility support
macro_check_gcc_visibility(__STRIGI_HAVE_GCC_VISIBILITY)

find_optional_dep(CppUnit ENABLE_CPPUNIT CppUnit_FOUND "Strigi unit tests")

enable_testing()

include(ConfigureChecks.cmake)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory(src)
if (CppUnit_FOUND)
    add_subdirectory(tests)
endif (CppUnit_FOUND)

configure_file(
  "${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# packaging target
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Strigi Desktop Search")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
set(CPACK_PACKAGE_VERSION_MAJOR ${STRIGI_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${STRIGI_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${STRIGI_VERSION_PATCH})
include(CPack)

# code for installing an script to help cmake applications determint the Strigi
# version number
configure_file(StrigiConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/StrigiConfig.cmake @ONLY )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/StrigiConfig.cmake DESTINATION ${LIB_DESTINATION}/strigi)

