#ifndef SQLITESTORAGE_H
#define SQLITESTORAGE_H

#include "searchstorage.h"

class SqliteStorage;
class SqlitePropertyWriter;

struct sqlite3;
struct sqlite3_stmt;

class SqliteStorage : public SearchStorage {
private:
	char status;
	bool transaction;
	sqlite3 *db;
	SqlitePropertyWriter *writer;

	void startTransaction();
	void endTransaction();
	void handleError(int r);
	char checkForTable(const char *);
	char createPropertyTable();
	void createPropertyTable(const char *propertyname, std::string &tablename, PropertyWriter::PropertyType type, bool onetoone);
	char getPropertyTablename(const char *propertyname, std::string &tablename, PropertyWriter::PropertyType &type, bool &onetoone);
public:
	SqliteStorage(const char *filename);
	~SqliteStorage();
	void defineProperty(const char *propertyname, PropertyWriter::PropertyType type, bool onetoone);
	PropertyWriter *getPropertyWriter(const char *propertyname);
    long addFile(long parent, const char *name);
};

#endif
