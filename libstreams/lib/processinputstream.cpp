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
#include <strigi/processinputstream.h>
#include <strigi/strigiconfig.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

using namespace std;
using namespace Strigi;

// define 'environ' for Mac OS X
#if defined(__APPLE__)
# include <crt_externs.h>
# define environ (*_NSGetEnviron())
#else
extern char **environ;
#endif

ProcessInputStream::ProcessInputStream(const vector<string>& a,
        InputStream* input) {
    this->input = input;
    fdin = fdout = pid = -1;

    // create an arguments structure
    char** ar = new char*[a.size()+1];
    for (uint i = 0; i < a.size(); ++i) {
        ar[i] = (char*)malloc(a[i].length()+1);
        memcpy(ar[i], a[i].c_str(), a[i].length()+1);
    }
    ar[a.size()] = 0;
    args = ar;

    if (input) {
        runCmdWithInput();
    } else {
        runCmd();
    }
}
ProcessInputStream::~ProcessInputStream() {
    if (fdin > 0) {
        close(fdin);
    }
    if (fdout > 0) {
        close(fdout);
    }
    if (pid != -1) {
        kill(SIGTERM, pid);
        int status;
        waitpid(pid, &status , 0);
    }
    const char* const* p = args;
    while (*p) {
        free((void*)*p++);
    }
    delete [] args;
}
void
ProcessInputStream::writeToPipe() {
    // read from the inputstream
    int64_t pos = input->position();
    const char* b;
    int32_t n = input->read(b, 1, 0);
    if (n <= 0 || input->status() == Eof) {
        if (input->status() == Error) {
            m_status = Error;
            m_error = input->error();
            n = 0;
        }
        // close the pipe if no more output is available
        input = 0;
        close(fdin);
        fdin = -1;
    } else {
        // write into the pipe
        ssize_t m = write(fdin, b, n);
        if (m < 0) {
            m_error = strerror(errno);
            m_status = Error;
            input = 0;
        } else if (m != n) {
            input->reset(pos+n);
        }
    }
}
int32_t
ProcessInputStream::fillBuffer(char* start, int32_t space) {
    if (fdout <= 0) return -1;
    if (input && input->status() == Ok) {
        writeToPipe();
    }
    ssize_t n = ::read(fdout, start, space);
    if (n < 0) {
        m_error = strerror(errno);
        m_status = Error;
        n = -2;
    }
    if (n <= 0) {
        close(fdout);
        fdout = 0;
    }
    return (int32_t)n;
}
void
ProcessInputStream::runCmd() {
    int p[2];
    if (pipe(p) == -1) {
        fprintf(stderr,"ProcessInputStream::runCmd: %s\n", strerror(errno));
        return;
    }
    
    if( (pid=fork()) == -1) {
        /* something went wrong */
        fprintf(stderr,"ProcessInputStream::runCmd: %s\n", strerror(errno));
        close (p[0]);
        close (p[1]);
        return;
    }
    
    if (pid == 0) {
        // define the output to be written into p[1]
        dup2(p[1], 1);
        // close p[0], because this process will not read from it
        close(0);
        close(p[0]);
        close(2);
        char *const *unconstargs = (char *const *) (args);
        execve(args[0], unconstargs, environ);
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    close(p[1]);
    fdout = p[0];
}
void
ProcessInputStream::runCmdWithInput() {
    int pin[2];
    int pout[2];
    if (pipe(pin) == -1 || pipe(pout) == -1) {
        fprintf(stderr,"ProcessInputStream::runCmd: %s\n", strerror(errno));
        return;
    }
    
    if( (pid=fork()) == -1) {
        /* something went wrong */
        fprintf(stderr,"ProcessInputStream::runCmdWithInput: fork error\n");
        close (pin[0]);
        close (pin[1]);
        close (pout[0]);
        close (pout[1]);
        return;
    }
 
    if (pid == 0) {
        // define the output to be written into p[1]
        dup2(pin[0], 0);
        dup2(pout[1], 1);
        // close p[0], because this process will not read from it
        close(pin[1]);
        close(pout[0]);
        close(2);
        char *const *unconstargs = (char *const *) (args);
        execve(args[0], unconstargs, environ);
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    close(pin[0]);
    close(pout[1]);
    fdout = pout[0];
    fdin = pin[1];
}
