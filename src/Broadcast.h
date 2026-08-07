/*
 * File: Broadcast
 * Author: Michael Franks 
 * Description:
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
     *
     * @param channel
     * @param fd
     */
    void subscribe(const std::string& channel, int fd);

    /**
     *
     * @param fd
     */
    void unsubscribe(int fd);

    /**
     *
     * @param channel
     * @param message
     * @return
     */
    int publish(const std::string& channel, const std::string& message);

private:
    std::mutex mutex_;
    std::unordered_map<std::string, std::unordered_set<int>> channels_;
    std::unordered_map<int, std::shared_ptr<std::mutex>> write_mutexes_;

    /**
     *
     * @param fd
     * @return
     */
    std::shared_ptr<std::mutex> write_mutex(int fd);
};


#endif // BROADCAST_H
