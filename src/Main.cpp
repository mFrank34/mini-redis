/*
 * Key store project
 */

#include <csignal>

#include "Server.h"
#include "Store.h"

std::atomic<bool> shutdown_requested(false);

static void signal_handler(int signum)
{
    shutdown_requested = true;
}

int main()
{
    std::signal(SIGINT, signal_handler);

    Store store;
    Server server(8080, store);
    server.run();
    return 0;
}