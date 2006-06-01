#include "indexer.h"

int
main(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: %s [indexdir] [dir-to-index]\n", argv[0]);
		return -1;
	}
	std::string file = argv[1];
	file += "/estraier.db";
        Indexer indexer(file.c_str());
	indexer.index(argv[2]);
	return 0;
}
