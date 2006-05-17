#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <string>
#include <cstdio>

/* function prototypes and global variables */
void *do_chld(void *);

class ClientThread {
private:
	static const pthread_mutex_t lockinit;
	pthread_mutex_t lock;
	bool busy;
public:
	ClientThread() {
		lock = lockinit;
		busy = false;
		count = 0;
	}
	std::string s;
	int socket;
	int count;
	pthread_t thread;
	void setBusy(bool b) {
		pthread_mutex_lock(&lock);
		busy = b;
		pthread_mutex_unlock(&lock);
	}
	bool isBusy() {
		bool b;
		pthread_mutex_lock(&lock);
		b = busy;
		pthread_mutex_unlock(&lock);
		return b;
	}
};
const pthread_mutex_t ClientThread::lockinit = PTHREAD_MUTEX_INITIALIZER;

const static int nthreads = 5;
ClientThread threads[nthreads];

int
listenSocket(const char* socketname) {
	int sd, newSd;
	size_t len;
        unsigned int addlen;
	struct sockaddr_un sock, work;

	/* create new socket */
	unlink(socketname);
	sd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sd < 0) {
		perror("cannot open socket ");
		return -1;
	}

	/* set the address */
	len = strlen(socketname)+1;
	len = (len > sizeof(sock.sun_path)) ?sizeof(sock.sun_path) :len;
	strncpy(sock.sun_path, socketname, len);
	sock.sun_path[len] = '\0';

	/* bind server port */
	sock.sun_family = AF_UNIX;
	if (bind(sd, (struct sockaddr *)&sock, sizeof(sock))<0) {
		perror("cannot bind port ");
		return -1;
	}

	if (listen(sd, 5) < 0) {
		perror("cannot listen to port");
		return -1;
	}

	int n = 0;
	int fail = 0;
	while (n++ < 10000) {
		addlen = sizeof(work);
		newSd = accept(sd, (struct sockaddr*)&(work), &addlen);
		if(newSd < 0) {
			perror("cannot accept connection ");
			return -1;
		}
		// find a free thread object
		int i;
		for (i=0; i<nthreads; ++i) {
			int p = (i+(n%nthreads))%nthreads;
			printf("n: %i p: %i\n", n, p);
			if (!threads[p].isBusy()) {
				threads[p].setBusy(true);
				threads[p].count++;
				break;
			} else {
				fail++;
			}
		}
		if (i == nthreads) {
			// no free thread object available
			printf("no free thread object\n");
			close(newSd);
			continue;
		}
		/* create a new thread to process the incomming request */
		// signal that the thread is being initialized
		threads[i].socket = newSd;
		int r = pthread_create(&threads[i].thread, NULL, do_chld,
			(void *)(threads+i));
		if (r < 0) {
			// could not create thread
			printf("could not create thread\n");
			close(newSd);
		}

		/* the server is now free to accept another socket request */
	}
	if (close(sd) < 0) {
		perror("close socket");
	}

	printf("failed thread lookups: %i\n", fail);
	for (int i=0; i<nthreads; ++i) {
		printf("thread %i: %i\n", i, threads[i].count);
		if (threads[i].isBusy()) {
			printf("waiting for thread %i\n", i);
			int r = pthread_join(threads[i].thread, 0);
			if (r < 0) {
				perror("wait");
			}
		}
	}
	return 0;
}
int
main(int argc, char** argv) {
	listenSocket("socket");
}

/* 
	This is the routine that is executed from a new thread 
*/

void *
do_chld(void *arg) {
	ClientThread* t = (ClientThread*)arg;
//	sleep(3);

	/* close the socket and exit this thread */
	close(t->socket);
	printf("Child Done Processing...\n");

	// signal that this thread is free again
	t->setBusy(false);
	pthread_exit((void *)0);
}
