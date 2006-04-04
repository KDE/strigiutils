INCLUDES = $(all_includes) -I/usr/include
METASOURCES = AUTO
bin_PROGRAMS = sqlstorage
noinst_HEADERS = searchstorage.h sqlitestorage.h sqlitepropertywriter.h
sqlstorage_SOURCES = storagetest.cpp sqlitestorage.cpp sqlitepropertywriter.cpp
sqlstorage_LDADD = -lsqlite3
