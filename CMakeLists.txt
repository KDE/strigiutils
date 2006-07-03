PROJECT (Strigi)

# include specific modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

SUBDIRS (src)

find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(Sha REQUIRED)
find_package(Magic REQUIRED)
find_package(Threads REQUIRED)
find_package(CLucene REQUIRED)
find_package(Libxml2)
find_package(Qt4)
check_include_files(sys/xattr.h   HAVE_SYS_XATTR_H)


# packaging target
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Strigi Desktop Search")
#SET(CPACK_PACKAGE_VENDOR "Lua")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "2")
SET(CPACK_PACKAGE_EXECUTABLES "strigi" "strigi")
INCLUDE(CPack)
