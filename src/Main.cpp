/*
 * Key store project
 */

#include "Server.h"
#include "Store.h"

int main()
{
    Store store;
    Server server(8080, store);
    server.run();
    return 0;
}