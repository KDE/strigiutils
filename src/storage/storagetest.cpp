#include "sqlitestorage.h"

void
indexFile(const char *path) {
}



int
main() {
	SearchStorage *storage = new SqliteStorage("/tmp/index.db");

	int nrows = 1000;

	storage->defineProperty("md5", PropertyWriter::STRING, true);
	PropertyWriter *writer = storage->getPropertyWriter("md5");
	for (int i=0; i<nrows; i++) {
		std::string s("hiiiiiiiiiiiiiiiiiii");
		writer->writeProperty(i, s.c_str(), s.length());
		writer->writeProperty(i, s.c_str(), s.length());
	}
	storage->defineProperty("md6", PropertyWriter::STRING, true);
	writer = storage->getPropertyWriter("md6");
	for (int i=0; i<nrows; i++) {
		std::string s("hiiiiiiiiiiiiiiiiiii");
		writer->writeProperty(i, s.c_str(), s.length());
		writer->writeProperty(i, s.c_str(), s.length());
	}

	delete storage;
	return 0;
}
