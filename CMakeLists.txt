PROJECT (strigi)
set(STRIGI_VERSION_MAJOR 0)
set(STRIGI_VERSION_MINOR 5)
set(STRIGI_VERSION_PATCH 5)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.5 FATAL_ERROR)

if(WIN32)
 set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

# include specific modules
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# include general modules
include(UsePkgConfig)
include(MacroCheckGccVisibility)

# compile in debug mode
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

#add extra search paths for libraries and includes
SET (STRIGI_EXTRA_INC_DIRECTORY "" CACHE STRING "Extra include directories to search in")
SET (STRIGI_EXTRA_LIB_DIRECTORY "" CACHE STRING "Extra library directories to search in")
SET (CMAKE_INCLUDE_PATH "${STRIGI_EXTRA_INC_DIRECTORY};${CMAKE_INCLUDE_PATH}")
SET (CMAKE_LIBRARY_PATH "${STRIGI_EXTRA_LIB_DIRECTORY};${CMAKE_LIBRARY_PATH}")
set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
SET (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)
# Always include srcdir and builddir in include path
# This saves typing ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY} in about every subdir
# since cmake 2.4.0
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set( KDELIBSUFF ${LIB_SUFFIX} )
set( LIBINSTALLDIR ${LIB_DESTINATION})
set( MIMEINSTALLDIR "${CMAKE_INSTALL_PREFIX}/share/mime")

IF(APPLE)
    set(CMAKE_INSTALL_NAME_DIR ${LIBINSTALLDIR})
ENDIF(APPLE)

IF(MSVC)
    # msvc2005 deprecated warnings
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
    # calm mdown msvc
    ADD_DEFINITIONS(-wd4251) # 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    ADD_DEFINITIONS(-wd4275) # non  DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
    ADD_DEFINITIONS(-wd4309) # 'conversion' : truncation of constant value
    ADD_DEFINITIONS(-wd4503) # decorated name length exceeded
    ADD_DEFINITIONS(-wd4786) # identifier was truncated to '255' characters in the debug information
ELSE(MSVC)
    IF (NOT WIN32)
      ADD_DEFINITIONS(-fPIC)
      IF (CMAKE_COMPILER_IS_GNUCXX)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common -fno-exceptions -Wno-unused-parameter")
      ENDIF(CMAKE_COMPILER_IS_GNUCXX)
    ENDIF(NOT WIN32)
ENDIF(MSVC)

IF(CMAKE_SYSTEM MATCHES "SunOS-5*.")
    # I suppose even GCC on SunOS doesn't like -fPIC
    REMOVE_DEFINITIONS(-fPIC)
ENDIF(CMAKE_SYSTEM MATCHES "SunOS-5*.")

OPTION(ENABLE_INOTIFY
  "enable inotify support (unstable)"
  OFF)

OPTION(ENABLE_DBUS
  "enable dbus support"
  ON)

OPTION(ENABLE_NEWXESAM
  "enable new xesam support"
  OFF)

OPTION(ENABLE_SQLITE "enable sqlite support" OFF)

OPTION(ENABLE_LOG4CXX
  "enable log4cxx support for advanced logging"
  OFF)

find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(Threads REQUIRED)
find_package(Iconv REQUIRED)

# use either expat or libxml2
find_package(Expat)
find_package(LibXml2 REQUIRED)
if (NOT LIBXML2_FOUND AND NOT Expat_FOUND)
  MESSAGE(FATAL "You need libxml2 or libexpat")
endif (NOT LIBXML2_FOUND AND NOT Expat_FOUND)

find_package(CLucene)
if(NOT CLucene_FOUND)
  MESSAGE("Could not find CLucene. Please install CLucene = 0.9.16a (http://clucene.sf.net)")
endif(NOT CLucene_FOUND)

find_package(HyperEstraier)
find_package(Exiv2)
if(ENABLE_SQLITE)
  find_package(SQLite)
endif(ENABLE_SQLITE)
#find_package(XAttr)
set(QT_MIN_VERSION "4.2.0")
find_package(Qt4)
if (NOT QT4_FOUND)
    MESSAGE("** Qt4 was not found. No GUI will be built.")
endif (NOT QT4_FOUND)

check_include_files(strings.h     HAVE_STRINGS_H)                      # various

IF(ENABLE_DBUS)
  IF(WIN32)
    file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _program_FILES_DIR)
    find_path(DBUS_INCLUDE_DIR dbus/dbus.h
        ${_program_FILES_DIR}/dbus/include ${CMAKE_INSTALL_PREFIX}/include
    )
    set(DBUS_LIBRARY_DIR ${_program_FILES_DIR}/dbus/lib)
    find_library(DBUS_LIBRARYS NAMES dbus
        PATHS ${DBUS_LIBRARY_DIR} ${CMAKE_INSTALL_PREFIX}/lib
    )
  ELSE(WIN32)
    include(UsePkgConfig)
    PKGCONFIG(dbus-1 DBUS_INCLUDE_DIR DBUS_LIBRARY_DIR DBUS_LDFLAGS DBUS_CFLAGS)
    if (NOT DBUS_INCLUDE_DIR)
      MESSAGE(FATAL_ERROR "Could not find DBus")
    endif (NOT DBUS_INCLUDE_DIR)
    
    EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=1.0 dbus-1 RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )
    if(_return_VALUE STREQUAL "0")
         message(STATUS "Found dbus-1 release >= 1.0")
    else(_return_VALUE STREQUAL "0")
         message(STATUS "Found dbus-1 release < 1.0 support for dbus client will be disable")
         set(ENABLE_DBUS "OFF")
    endif(_return_VALUE STREQUAL "0")

  ENDIF(WIN32)
ENDIF(ENABLE_DBUS)

find_program (BISON
              bison
              DOC "Path to bison command, used for xesam userlanguage parser generation"
)

if (BISON)
  MESSAGE (STATUS "Found bison: ${BISON}")
else (BISON)
  MESSAGE ("** GNU bison not found. This affects the xesam parser.")
endif (BISON)

if (ENABLE_LOG4CXX)
    find_package (Log4cxx)
endif (ENABLE_LOG4CXX)

#
# AC_CHECK_LIB(dl, dlopen, DL_LIBRARY="-ldl", DL_LIBRARY="") for cmake by
# alexander neuendorf
#
# on FreeBSD dlopen is in libc, on GNU/Linux it's in libdl
if(NOT WIN32)
  INCLUDE(CheckFunctionExists)
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
   MESSAGE(FATAL "dlopen support is required.")
  endif (NOT HAVE_LIBDL)
endif(NOT WIN32)

# check for visibility support
MACRO_CHECK_GCC_VISIBILITY(__STRIGI_HAVE_GCC_VISIBILITY)

SET (DIRS ${DIRS} src)

find_package(CppUnit)
if (NOT CppUnit_FOUND)
    MESSAGE("** CppUnit was not found. Strigi unit tests will not be built.")
else (NOT CppUnit_FOUND)
    MESSAGE(STATUS "CppUnit found. Strigi unit tests will be built.")
    SET (DIRS ${DIRS} test)
endif (NOT CppUnit_FOUND)

ENABLE_TESTING()
SUBDIRS (${DIRS})

include(ConfigureChecks.cmake)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

CONFIGURE_FILE(
  "${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# packaging target
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Strigi Desktop Search")
set(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_PACKAGE_VERSION_MAJOR ${STRIGI_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${STRIGI_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${STRIGI_VERSION_PATCH})
set(STRIGI_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
INCLUDE(CPack)
INCLUDE(Dart)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/htmlgui/strigihtmlgui.cpp.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/htmlgui/strigihtmlgui.cpp"
  IMMEDIATE @ONLY)

# code for installing an script to help cmake applications determint the Strigi
# version number
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/StrigiConfig.cmake" "
set(STRIGI_VERSION_MAJOR ${STRIGI_VERSION_MAJOR})
set(STRIGI_VERSION_MINOR ${STRIGI_VERSION_MINOR})
set(STRIGI_VERSION_PATCH ${STRIGI_VERSION_PATCH})
")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/StrigiConfig.cmake DESTINATION ${LIB_DESTINATION}/strigi)
    
