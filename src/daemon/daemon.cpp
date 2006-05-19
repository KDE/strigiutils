#include <pthread.h>
#include <string>
#include <stack>
#include <cstdio>
#include <signal.h>
#include "socketserver.h"

pthread_mutex_t stacklock = PTHREAD_MUTEX_INITIALIZER;
std::stack<std::string> filestack;
bool daemon_run = true;

/* function prototypes and global variables */
void *do_chld(void *);


void
quit_daemon(int) {
    daemon_run = false;
}

struct sigaction quitaction;

void
set_quit_on_signal(int signum) {
    quitaction.sa_handler = quit_daemon;
    sigaction(signum, &quitaction, 0);
}

void
shortsleep(long nanoseconds) {
    // set sleep time
    struct timespec sleeptime;
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = nanoseconds;
    nanosleep(&sleeptime, 0);
}

void *
indexloop(void *) {
    while (daemon_run) {
        shortsleep(100000000);
        pthread_mutex_lock(&stacklock);
        if (filestack.size())
        printf("doing %i file\n", filestack.size());
        while (filestack.size()) {
            filestack.pop();
        }
        pthread_mutex_unlock(&stacklock);
    }
    printf("stopping indexer\n");
    return (void*)0;
}
int
main(int argc, char** argv) {
	set_quit_on_signal(SIGINT);
	set_quit_on_signal(SIGQUIT);
	set_quit_on_signal(SIGTERM);

	// start the indexer thread
	pthread_t indexthread;
	int r = pthread_create(&indexthread, NULL, indexloop, 0);
	if (r < 0) {
		printf("cannot create thread\n");
		return 1;
	}

	// listen for requests
	SocketServer server;
	server.start();

	// wait for the indexer to finish
	pthread_join(indexthread, 0);
}

