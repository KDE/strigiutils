# - Try to find the Exiv2 library
# Once done this will define
#
#  EXIV2_FOUND - system has libexiv2
#  EXIV2_INCLUDE_DIR - the libexiv2 include directory
#  EXIV2_LIBRARIES - Link these to use libexiv2
#  EXIV2_DEFINITIONS - Compiler switches required for using libexiv2
#

if (EXIV2_INCLUDEDIR AND EXIV2_LIBRARIES)
  # in cache already
  SET(EXIV2_FOUND TRUE)
else (EXIV2_INCLUDEDIR AND EXIV2_LIBRARIES)
    if (NOT WIN32)
        PKG_CHECK_MODULES(EXIV2 exiv2>=0.12)
        if (EXIV2_FOUND)
            set(EXIV2_DEFINITIONS ${EXIV2_CFLAGS})
        endif(EXIV2_FOUND)
    else(NOT WIN32)
        #Better check
        set(EXIV2_FOUND TRUE)
    endif (NOT WIN32)
endif (EXIV2_INCLUDEDIR AND EXIV2_LIBRARIES)
