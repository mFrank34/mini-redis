/*
 * File: Broadcast
 * Author: Michael Franks 
 * Description: system to broadcast messaged to connected clients
 */

#ifndef BROADCAST_H
#define BROADCAST_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>


class Broadcast
{
public:

    /**
     * Allows for application to subscript to channel for communications.
     * @param channel selected channel for communications.
     * @param fd id number for channel.
     */
    void subscribe(const std::string& channel, int fd);

    /**
     * unsubscribe from a channel.
     * @param fd the id for that channel.
     */
    void unsubscribe(int fd);

    /**
     * send messaged to connected clients on a channel.
     * @param channel selected peers.
     * @param message message to send.
     * @return amount of successes messages.
     */
    int publish(const std::string& channel, const std::string& message);

private:
    std::mutex mutex_;
    std::unordered_map<std::string, std::unordered_set<int>> channels_;
    std::unordered_map<int, std::shared_ptr<std::mutex>> write_mutexes_;

    /**
     * limited system to protected reads and writes on data.
     * @param fd id to free up write and reads from.
     * @return id to write too.
     */
    std::shared_ptr<std::mutex> write_mutex(int fd);
};


#endif // BROADCAST_H
