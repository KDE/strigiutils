#ifndef SEARCHSTORAGE
#define SEARCHSTORAGE

#include <string>

class PropertyWriter {
public:
	enum PropertyType {STRING, INT, BINARY};
protected:
	PropertyType type;
	std::string propertyname;
public:
	virtual ~PropertyWriter() {}
	std::string getPropertyName() { return propertyname; }
	PropertyType getPropertyType() const { return type; }
	virtual char writeProperty(long fileid, const char *value, int numBytes) = 0;
	virtual char writeProperty(long fileid, const void *value, int numBytes) = 0;
	virtual char writeProperty(long fileid, int value) = 0;
};

class SearchStorage {
public:
	virtual ~SearchStorage() {}
	virtual void defineProperty(const char *propertyname, PropertyWriter::PropertyType type, bool onetoone) = 0;
	virtual PropertyWriter *getPropertyWriter(const char *propertyname) = 0;
    virtual long addFile(long parent, const char *name) = 0;
};

#endif
