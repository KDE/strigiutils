#include "server.h"

/**
 * return true if the system stopped normally
 **/
bool
Server::start() {
    return listen();
}
void
Server::stop() {
}
