/**
 *
 * Program that report the load on the computer as a value between 0 and 100
 * on socket 1500. It prints the load to every connection on port 1500 and
 * closes. This is a server agent for the Coyotepoint load balancer.
 *
 * Author: Jos van den Oever
 * last modification: 2005-06-07
 **/ 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
runDaemon() {

	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	we can exit the parent process. */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	umask(0);

	/* Open any logs here */        

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
	/* Log the failure */
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory */
	if ((chdir("/")) < 0) {
	/* Log the failure */
		exit(EXIT_FAILURE);
	}

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Daemon-specific initialization goes here */

	/* The Big Loop */
	while (1) {
	/* Do some task here ... */
		listenSocket();
		sleep(30); /* wait 30 seconds */
	}
	exit(EXIT_SUCCESS);
}

/**
 * return a value from 0 to 100 for the computer load
 **/
int
getLoad() {
	FILE *f;
	char line[128];
	char *end;
	int read, load;

	f = fopen("/proc/loadavg", "r");
	read = fread(line, 1, 128, f);
	fclose(f);
	line[read] = 0;
	load = (int)(100*strtod(line, &end));
	if (load < 0) {
		load = 0;
	} else if (load > 100) {
		load = 100;
	}
	return load;
}

#define SERVER_PORT 1500
#define ERROR 1
#define SUCCESS 0

int
listenSocket() {
	int sd, newSd, len, addlen, load;
	char msg[16];
	struct sockaddr_in sock, work;

	/* create socket */
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0) {
		perror("cannot open socket ");
		return ERROR;
	}

	/* bind server port */
	sock.sin_family = AF_INET;
	sock.sin_port = htons(SERVER_PORT);
	sock.sin_addr.s_addr = INADDR_ANY;
	if (bind(sd, (struct sockaddr *)&sock, sizeof(sock))<0) {
		perror("cannot bind port ");
		return ERROR;
	}

	if (listen(sd, 5) < 0) {
		perror("cannot listen to port");
		return ERROR;
	}

	while (1) {
		addlen = sizeof(work);
		newSd = accept(sd, (struct sockaddr*)&(work), &addlen);
		if(newSd < 0) {
			perror("cannot accept connection ");
			return ERROR;
		}
		load = getLoad();
		len = snprintf(msg, 16, "%i\n", load);
		write(newSd, msg, len);

		close(newSd);
	}	
}


int main(void) {
	runDaemon();
	return 0;
}
