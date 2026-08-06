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
    /**
     * Application Start sections.
     * @param port application access port.
     * @param store in memory storage system.
     */
    Server(int port, Store& store);

    /**
     * Server Destroyer shuts down server safely with collecting threads
     * and shutting down server.
     */
    ~Server();

    /**
     * Main run time of serve handling server actions and calls.
     */
    void run();

    /**
     * script to hand loading from cold storage on boot.
     */
    void load() const;

    /**
     * System design to create a snapshot of memory storage container.
     */
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

    /**
     * System designed to hand a client request
     * while being able to thread across threads.
     * @param client_fd client id on request.
     */
    void handle_client(int client_fd) const;
};

#endif //SERVER_H
