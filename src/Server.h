//
// Created by frank on 02/08/2026.
//
#ifndef SERVER_H
#define SERVER_H

#include "Store.h"
#include "Thread.h"
#include "Wal.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

extern std::atomic<bool> shutdown_requested;

class Server
{
public:
    Server(int port, Store& store);
    ~Server();

    void run();
    void load() const;
    void snapshot();

private:
    int port_;
    int server_fd_;
    Store& store_;
    Thread thread_;
    Wal wal_;

    /* snapshot capture system to save around 60 seconds */
    std::atomic<bool> running_;
    std::mutex snapshot_mutex_;
    std::condition_variable snapshot_cv_;
    std::thread snapshot_thread_;

    void handle_client(int client_fd) const;
};

#endif //SERVER_H
