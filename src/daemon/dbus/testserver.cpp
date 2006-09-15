#include "dbusserver.h"
int
main() {
    DBusServer server(0);
    server.listen();

    return 0;
}
