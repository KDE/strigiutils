#include "filelister.h"
#include <list>
#include <set>
#include <string>
using namespace std;

struct MTimeFile {
	std::string file;
	time_t mtime;
};
struct ltstr {
    bool operator()(const MTimeFile& m1, const MTimeFile& m2) const {
        return m1.file < m2.file;
    }
};
struct ltstrmtime {
    bool operator()(const MTimeFile& m1, const MTimeFile& m2) const {
        return (m1.mtime == m2.mtime) ?m1.file < m2.file :m1.mtime < m2.mtime;
    }
};

set<MTimeFile, ltstr> diskfiles;
set<MTimeFile, ltstr> dbfiles;

/*
    Make two sets: one with all files on disk (A) and one with all files in
    the database (B). If a file is in A but not in B, it should be (re)indexed.
    If a file is in B but not in A, it should be deleted from the database.
*/

bool
addFileCallback(const char *path, const char *filename, time_t mtime) {
    MTimeFile mt;
    mt.file = path;
    mt.file += filename;
    mt.mtime = mtime;
    printf("%s\n", mt.file.c_str());
//    diskfiles.push_back(mt);
//    dbfiles.insert(mt);
    return true;
}

int
main() {
    FileLister lister;
    lister.setCallbackFunction(&addFileCallback);
    lister.listFiles("/home/jos");
    fprintf(stderr, "%i\n", diskfiles.size());
    set<MTimeFile, ltstr> todelete;
    insert_iterator<set<MTimeFile, ltstr > > 
                 del_ins(todelete, todelete.begin());
    set_difference(dbfiles.begin(), dbfiles.end(),
        diskfiles.begin(), diskfiles.end(), del_ins);

    set<string> a;
    insert_iterator<set<string, less<string> > > ds(a, a.begin());
    set_difference(a.begin(), a.end(), a.begin(), a.end(), ds);


    return 0;
}
