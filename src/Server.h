//
// Created by frank on 02/08/2026.
//

#include "Store.h"

#ifndef SERVER_H
#define SERVER_H


class Server {
public:
    Server(int port, Store& store);
    ~Server();

    void run();

private:
    int port_;
    int server_fd_;
    Store& store_;

    void handle_client(int client_fd) const;
};

#endif //SERVER_H
