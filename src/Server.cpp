//
// Created by frank on 02/08/2026.
//

#include "Server.h"
#include "Parser.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

Server::Server(int port, Store& store)
    : port_(port), server_fd_(-1), store_(store), thread_()
{
}

Server::~Server()
{
    if (server_fd_ != -1)
    {
        close(server_fd_);
    }

    {
        std::lock_guard<std::mutex> lock(snapshot_mutex_);
        running_ = false;
    }
    snapshot_cv_.notify_one();

    if (snapshot_thread_.joinable())
    {
        snapshot_thread_.join();
    }

    wal_.write(store_.save());
}


void Server::run()
{
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    int opt = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "set socket failed\n";
        return;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_fd_, (sockaddr*)&address, sizeof(address)) < 0)
    {
        std::cerr << "Error binding socket" << std::endl;
        return;
    }

    if (listen(server_fd_, 5) < 0)
    {
        std::cerr << "Listen failed\n";
        return;
    }

    std::cout << "Server listening on port " << port_ << "\n";

    load();

    running_ = true;
    snapshot_thread_ = std::thread([this] { snapshot(); });

    while (!shutdown_requested)
    {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_fd_, &read_fds);
        timeval tv{1, 0}; // 1 second timeout

        int ready = select(server_fd_ + 1, &read_fds, nullptr, nullptr, &tv);
        if (ready <= 0)
        {
            continue; // timeout or error — loop back and recheck shutdown_requested
        }

        sockaddr_in client{};
        socklen_t client_len = sizeof(client);
        int client_fd = accept(server_fd_, (sockaddr*)&client, &client_len);
        if (client_fd < 0)
        {
            std::cerr << "Accept failed\n";
            continue;
        }
        thread_.enqueue([client_fd, this]()
        {
            handle_client(client_fd);
            close(client_fd);
        });
    }
}

void Server::load() const
{
    store_.load(wal_.read());
}

void Server::snapshot()
{
    std::unique_lock<std::mutex> lock(snapshot_mutex_);

    if (!running_) { return; }

    while (running_)
    {
        // Wait up to 60s, but wake immediately if notified (e.g. on shutdown)
        snapshot_cv_.wait_for(lock, std::chrono::seconds(60), [this] { return !running_; });

        if (running_)
        {
            lock.unlock();
            wal_.write(store_.save());
            lock.lock();
        }
    }
}

void Server::handle_client(int client_fd)
{
    char buffer[1024];

    while (true)
    {
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0)
        {
            break; // client disconnected or read error
        }

        std::string line(buffer, bytes_read);
        Command cmd = parse(line);
        std::string response;

        if (cmd.command == "SET")
        {
            if (cmd.arguments.size() != 2)
            {
                response = "Invalid command use of SET";
            }
            else
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
            }
            else
            {
                auto val = store_.get(cmd.arguments[0]);
                response = (val ? *val : "(nil)");
            }
        }
        else if (cmd.command == "DEL")
        {
            if (cmd.arguments.size() != 1)
            {
                response = "Invalid command use of DEL";
            }
            else
            {
                response = (store_.del(cmd.arguments[0]) ? "OK" : "(nil)");
            }
        }
        else if (cmd.command == "SUBSCRIBE")
        {
            if (cmd.arguments.size() != 1)
            {
                response = "Invalid command use of SUBSCRIBE";
            }
            else
            {
                broadcast_.subscribe(cmd.arguments[0], client_fd);
                response = "OK subscribed to " + cmd.arguments[0];
            }
        }
        else if (cmd.command == "PUBLISH")
        {
            if (cmd.arguments.size() < 2)
            {
                response = "Invalid command use of PUBLISH";
            }
            else
            {
                const std::string& channel = cmd.arguments[0];
                std::string message;
                for (size_t i = 1; i < cmd.arguments.size(); ++i)
                {
                    if (i > 1) message += ' ';
                    message += cmd.arguments[i];
                }
                int n = broadcast_.publish(channel, message);
                response = "OK delivered to " + std::to_string(n);
            }
        }
        else
        {
            response = "Invalid command";
        }

        response += '\n';
        ssize_t written = write(client_fd, response.c_str(), response.length());
        if (written <= 0)
        {
            break; // client's gone, stop trying to talk to it
        }
    }

    broadcast_.unsubscribe(client_fd); // cleanup regardless of why we exited
}
