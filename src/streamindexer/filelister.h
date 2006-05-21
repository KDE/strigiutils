#ifndef FILELISTER_H
#define FILELISTER_H

/**
    This class does the same as the Posix command
     find $dir -type f -mmin -$age
    The performance is equal, but there are a couple of advantages:
      - the last modification date is given instead of the age of the file in
        minutes
      - the wanted file are returned as two strings for dirpath and filename
        through a callback function instead of one concatenated string
        through a pipe.
      - no need to spawn a separate process
**/

#include <vector>
#include <string>

class FileLister {
private:
	time_t m_oldestdate;
	std::vector<const char *> m_dirnames;
	std::string m_path;
	std::vector<int> m_pathSeparators;
	bool (*m_callback)(const char *dirpath, const char *filename);

	bool walk_directory(const char *dirname);
	void expandPath(const char *);
	void shortenPath();
public:
	FileLister() {
		m_callback = 0;
	}; 
	/**
	 * Specify the callback function that reports the files found.
         **/
	void setCallbackFunction(bool (*callback)(const char *dirpath,
		const char *filename)) {
		m_callback = callback;
	}
	/**
	 * List all the files in directory dir that have been modified more
         * recently then oldestdate.
         **/
	void listFiles(const char *dir, time_t oldestdate = 0);
};

#endif
