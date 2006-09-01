/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
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
#include <sys/stat.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#define SERVER_PORT 1500
#define ERROR 1
#define SUCCESS 0

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
int
listenSocket(const char* socketname) {
    int sd, newSd, len, load;
        unsigned int addlen;
    char msg[16];
    struct sockaddr_un sock, work;

    /* create socket */
    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sd < 0) {
        perror("cannot open socket ");
        return ERROR;
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
                // read request
                std::string req;
                char buf[1001];
                while (newSd >= 0) {
                    int r = recv(newSd, buf, 1000, 0);
                    printf("received %i\n", r);
                    if (r < 0) {
                        printf("error reading request\n");
                        close(newSd);
                        newSd = -1;
                    } else if (r == 0 || buf[r-1] == 0) {
                        buf[r] = '\0';
                        req += buf;
                        break;
                    } else {
                        buf[r] = '\0';
                        req += buf;
                    }
                }
                std::string resp = req;
                printf("before writing %s\n", req.c_str());
                // send response
                int pos = 0;
                int len = resp.length()+1;
                while (newSd >= 0) {
                    int r = send(newSd, resp.c_str()+pos, len-pos, MSG_NOSIGNAL);
                    if (r < 0) {
                        printf("error writing response\n");
                        close(newSd);
                        newSd = -1;
                    } else {
                        pos += r;
                    }
                }
                printf("after writing\n");

        close(newSd);
                printf("after closing\n");
    }
}

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
        listenSocket("socket");
        sleep(30); /* wait 30 seconds */
    }
    exit(EXIT_SUCCESS);
}




int main(void) {
    unlink("socket");
    listenSocket("socket");
    unlink("socket");
    //runDaemon();
    return 0;
}
