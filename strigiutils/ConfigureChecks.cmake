#need to find a few default headers:
INCLUDE(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(regex.h HAVE_REGEX_H)            # src/dummyindexer/*
