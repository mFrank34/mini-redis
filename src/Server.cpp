//
// Created by frank on 02/08/2026.
//

#include "Server.h"
#include "Parser.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

Server::Server(int port, Store &store)
    : port_(port), server_fd_(-1), store_(store) {}

Server::~Server() {
    if (server_fd_ != -1) {
        close(server_fd_);
    }
}

void Server::run() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_fd_, (sockaddr*) &address, sizeof(address)) < 0)
    {
        std::cerr << "Error binding socket" << std::endl;
        return;
    }

    if (listen(server_fd_, 5) < 0) {
        std::cerr << "Listen failed\n";
        return;
    }

    std::cout << "Server listening on port " << port_ << "\n";

    while (true)
    {
        sockaddr_in client{};
        socklen_t client_len = sizeof(client);

        int client_fd = accept(server_fd_, (sockaddr*) &client, &client_len);
        if (client_fd < 0)
        {
            std::cerr << "Accept failed\n";
            continue;
        }

        // handle client
        handle_client(client_fd);
        close(client_fd);
    }

}

void Server::handle_client(int client_fd) const
{
    char buffer[1024];
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

    if (bytes_read <= 0)
    {
        return;
    }

    std::string line(buffer, bytes_read);
    Command cmd = parse(line);
    std::string response;

    if (cmd.command == "SET")
    {
        if (cmd.arguments.size() != 2)
        {
            response = "Invalid command use of SET";
        } else
        {
            store_.set(cmd.arguments[0], cmd.arguments[1]);
            response = "OK";
        }
    }
    else if (cmd.command == "GET")
    {
        if (cmd.arguments.size() != 1)
        {
            response = "Invalid command use of GET";
        } else
        {
            auto val = store_.get(cmd.arguments[0]);
            response = (val ? *val : "(nil)");
        }
    } else if (cmd.command == "DEL")
    {
        if (cmd.arguments.size() != 1)
        {
            response = "Invalid command use of DEL";
        } else
        {
            response =  (store_.del(cmd.arguments[0]) ? "OK" : "(nil)");
        }
    } else
    {
        response = "Invalid command";
    }

    // adding new line before writing
    response += '\n';
    write(client_fd, response.c_str(), response.length());
}
