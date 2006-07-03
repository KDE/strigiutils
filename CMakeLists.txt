PROJECT (Strigi)

# include specific modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

SUBDIRS (src)

find_package(ZLIB REQUIRED)
find_package(Magic REQUIRED)
find_package(Threads REQUIRED)
find_package(CLucene)
find_package(Libxml2)
find_package(Qt4)
check_include_files(sys/xattr.h   HAVE_SYS_XATTR_H)
