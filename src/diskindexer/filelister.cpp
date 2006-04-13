#include "filelister.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

char
FileLister::listFiles(const char *dir, time_t oldestdate) {
	if (m_callback == 0) return 0;
	m_oldestdate = oldestdate;
	m_dirnames.clear();
	m_dirnames.push_back(dir);
	return walk_directory(dir);
}
char
FileLister::walk_directory(const char *dirname) {
	bool expandedPath = false;
	int rc;
	DIR *dir;
	struct dirent *subdir;
	struct stat dirstat;

	// open the directory
	dir = opendir(dirname);
	if (dir == 0) {
		return -1;
	}
	// change into the directory
	rc = fchdir(dirfd(dir));
	if (rc != 0) {
		perror(dirname);
		return -1;
	}

	// append the name of the opened dir to the list of opened names
	m_dirnames.push_back(dirname);


	subdir = readdir(dir);
	while (subdir) {

		// skip the directories '.' and '..'
		char c1 = subdir->d_name[0];
		if (c1 == '.') {
			char c2 = subdir->d_name[1];
			if (c2 == '.' || c2 == '\0') {
				subdir = readdir(dir);
				continue;
			}
		}

		if (lstat(subdir->d_name, &dirstat) == 0) {
			if (S_ISREG(dirstat.st_mode)
					&& dirstat.st_mtime >= m_oldestdate) {
				// add the name of this dir to the path
				if (!expandedPath) {
					expandPath(dirname);
					expandedPath = true;
				}
				m_callback(m_path.c_str(), subdir->d_name);
			} else if (S_ISDIR(dirstat.st_mode)) {
				// add the name of this dir to the path
				if (!expandedPath) {
					expandPath(dirname);
					expandedPath = true;
				}
				walk_directory(subdir->d_name);
			}
		}
		
		subdir = readdir(dir);
	}

	// clean up
	closedir(dir);
	m_dirnames.pop_back();
	if (expandedPath) {
		shortenPath();
	}
	// go back to where we came from
	chdir("..");
	return 0;
}
void
FileLister::expandPath(const char *name) {
	m_pathSeparators.push_back(m_path.length());
	m_path += name;
	m_path += "/";
}
void
FileLister::shortenPath() {
	m_path.resize(*m_pathSeparators.rbegin());
	m_pathSeparators.pop_back();
}
